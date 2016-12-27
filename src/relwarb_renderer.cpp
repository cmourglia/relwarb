#include "relwarb_renderer.h"

#include "relwarb.h"
#include "relwarb_opengl.h"
#include "relwarb_debug.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <stdio.h>
#include <vector> // TODO(Charly): Remove this
#include <algorithm>

global_variable std::vector<Mesh> g_renderQueue;

global_variable GLuint g_bitmapProg;
global_variable GLuint g_colorProg;

global_variable const char* bitmapVert = R"(
#version 330

layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_uv;

uniform mat3 u_proj;

out vec2 uv;
out vec2 pos;

void main()
{
    //vec3 p = u_proj * vec3(in_pos, 1);
    gl_Position = vec4(in_pos, 0, 1);
    uv = in_uv;
    pos = gl_Position.xy;
}
)";

global_variable const char* bitmapFrag = R"(
#version 330

in vec2 pos;
in vec2 uv;
out vec4 color;

uniform sampler2D u_tex;

void main()
{
    color = texture(u_tex, uv);
    if (color.a < 0.1)
    {
        discard;
    }
}
)";

global_variable const char* colorFrag = R"(
#version 330

out vec4 color;
uniform vec3 u_color;

void main()
{
    color.rgb = u_color;
    color.a = 1.0;
}
)";

internal GLuint CompileShader(const char* src, GLenum type)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint compiled = GL_TRUE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE)
    {
        GLsizei logLength;
        GLchar message[1024];
        glGetShaderInfoLog(shader, 1024, &logLength, message);

        // TODO(Charly): Proper logging
        Log(Log_Error, "Shader not compiled (%d): %s\n", logLength, message);

        glDeleteShader(shader);
        shader = 0;
    }

    return shader;
}


Sprite* CreateStillSprite(GameState* gameState, Bitmap* bitmap)
{
    ComponentID id = gameState->nbSprites++;
    Assert(id < WORLD_SIZE);

    Sprite* result = &gameState->sprites[id];
    result->spriteType = SpriteType_Still;
    result->stillSprite = bitmap;

    return result;
}

Sprite* CreateTimeSprite(GameState* gameState, uint32 nbBitmaps, Bitmap** bitmaps, real32 stepTime, bool32 active)
{
    ComponentID id = gameState->nbSprites++;
    Assert(id < WORLD_SIZE);

    Sprite* result = &gameState->sprites[id];
    result->spriteType = SpriteType_Timed;
    result->nbSteps = nbBitmaps;
    result->steps = new Bitmap*[nbBitmaps];
    memcpy(result->steps, bitmaps, nbBitmaps * sizeof(Bitmap*));
    result->currentStep = 0;
    result->stepTime = stepTime;
    result->active = active;

    return result;
}

Bitmap* GetSpriteBitmap(const Sprite* sprite)
{
    switch (sprite->spriteType)
    {
        case SpriteType_Still:
        {
            return sprite->stillSprite;
        } break;
        case SpriteType_Timed:
        {
            return sprite->steps[sprite->currentStep];
        } break;
        default:
            Assert(false);
            return nullptr;
    }
}

RenderingPattern* CreateUniqueRenderingPattern( GameState* gameState,
                                                Sprite* sprite)
{
    ComponentID id = gameState->nbPatterns++;
    Assert(id < WORLD_SIZE);

    RenderingPattern* result = &gameState->patterns[id];
    result->patternType = RenderingPattern_Unique;
    result->unique = sprite;

    return result;
}

void UpdateSpriteTime(Sprite* sprite, real32 dt)
{
    if (sprite->spriteType == SpriteType_Timed && sprite->active)
    {
        sprite->elapsed += dt;
        if (sprite->elapsed > sprite->stepTime)
        {
            sprite->currentStep++;
            sprite->elapsed -= sprite->stepTime;
            if (sprite->currentStep >= sprite->nbSteps)
            {
                sprite->currentStep = 0;
            }
        }
    }
}

RenderingPattern* CreateFillRenderingPattern(GameState* gameState,
                                         z::vec2 size,
                                         uint8* pattern,
                                         uint8 nbBitmaps,
                                         Bitmap** bitmaps)
{
    ComponentID id = gameState->nbPatterns++;
    Assert(id < WORLD_SIZE);

    RenderingPattern* result = &gameState->patterns[id];
    result->size = size;
    result->patternType = RenderingPattern_Fill;
    result->pattern = new uint8[size.x() * size.y()];
    memcpy(result->pattern, pattern, size.x() * size.y() * sizeof(uint8));
    result->tiles = new Bitmap*[nbBitmaps];
    memcpy(result->tiles, bitmaps, nbBitmaps * sizeof(Bitmap*));

    return result;
}

void AddRenderingPatternToEntity(Entity* entity, RenderingPattern* pattern)
{
    entity->pattern = pattern;
    SetEntityComponent(entity, ComponentFlag_Renderable);
}

void InitializeRenderer(GameState* gameState)
{
    GLuint vshader = CompileShader(bitmapVert, GL_VERTEX_SHADER);
    GLuint fshader = CompileShader(bitmapFrag, GL_FRAGMENT_SHADER);

    Assert(vshader);
    Assert(fshader);

    g_bitmapProg = glCreateProgram();
    glAttachShader(g_bitmapProg, vshader);
    glAttachShader(g_bitmapProg, fshader);
    glLinkProgram(g_bitmapProg);

    GLint linked;
    glGetProgramiv(g_bitmapProg, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE)
    {
        GLsizei length;
        GLchar message[1024];
        glGetProgramInfoLog(g_bitmapProg, 1024, &length, message);
        Log(Log_Error, "Program not linked : %s", message);
    }

    fshader = CompileShader(colorFrag, GL_FRAGMENT_SHADER);
    Assert(fshader);

    g_colorProg = glCreateProgram();
    glAttachShader(g_colorProg, vshader);
    glAttachShader(g_colorProg, fshader);
    glLinkProgram(g_colorProg);

    glGetProgramiv(g_colorProg, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE)
    {
        GLsizei length;
        GLchar message[1024];
        glGetProgramInfoLog(g_colorProg, 1024, &length, message);
        Log(Log_Error, "Program not linked : %s", message);
    }
}

void FlushRenderQueue(GameState* gameState)
{
    // Avoid state changes as much as possible
    std::sort(g_renderQueue.begin(), g_renderQueue.end(),
              [](const Mesh& a, const Mesh& b)
              {
                  bool result =
                      a.program < b.program ? true
                      : a.program > b.program ? false
                      : a.renderMode < b.renderMode ? true
                      : a.renderMode > b.renderMode ? false
                      : a.texture < b.texture ? true : false;

                  return result;
              });

    int start = 0;

#if 1
    while (start < g_renderQueue.size())
    {
        int end = start + 1;
        RenderMode currMode = g_renderQueue[start].renderMode;
        GLuint currTexture = g_renderQueue[start].texture;

        // NOTE(Charly): Find all meshes that share a texture
        while (end < g_renderQueue.size() &&
               g_renderQueue[end].renderMode == currMode &&
               g_renderQueue[end].texture == currTexture)
        {
            ++end;
        }

        Mesh mesh;
        mesh.program = g_renderQueue[start].program;
        mesh.texture = currTexture;

        z::mat3 projMatrix = GetProjectionMatrix(currMode, gameState);

        GLuint startIdx = 0;
        for (int i = start; i < end; ++i)
        {
            Mesh* currMesh = &g_renderQueue[i];
            for (const auto& vert : currMesh->vertices)
            {
                z::vec2 p = projMatrix * currMesh->worldTransform * vert.position;
                Vertex v = {p, vert.texcoord};
                mesh.vertices.push_back(v);
            }

            for (const auto& idx : currMesh->indices)
            {
                mesh.indices.push_back(startIdx + idx);
            }
            startIdx += (GLuint)currMesh->vertices.size();
        }

        RenderMesh(&mesh, projMatrix);

        start = end;
    }
#else
    for (auto& mesh : g_renderQueue)
    {
        z::mat3 projMatrix = GetProjectionMatrix(mesh.renderMode, gameState);
        for (auto& vert : mesh.vertices)
        {
            vert.position = projMatrix * mesh.worldTransform * vert.position;
        }
        RenderMesh(&mesh, projMatrix);
    }
#endif

    g_renderQueue.clear();
}

void RenderPattern(RenderingPattern* pattern, Transform* transform, z::vec2 size)
{
    switch (pattern->patternType)
    {
        case RenderingPattern_Unique:
        {
            RenderBitmap(GetSpriteBitmap(pattern->unique), RenderMode_World, transform);
        } break;
        case RenderingPattern_Fill:
        {
            RenderFillPattern(pattern, transform, size);
        } break;

        default:
        {
            // Unknown render pattern type
            Assert(!"Wrong code path");
        };
    }
}

void RenderFillPattern(RenderingPattern* pattern, Transform* transform, z::vec2 size)
{
    uint32 middleTileX = pattern->size.x() * 0.5f;
    uint32 middleTileY = pattern->size.y() * 0.5f;
    uint32 deltaX = size.x() - pattern->size.x();
    uint32 deltaY = size.y() - pattern->size.y();
    real32 halfSizeX = size.x() * 0.5f - 0.5f;
    real32 halfSizeY = size.y() * 0.5f - 0.5f;
    Assert(deltaX >= 0 && deltaY >= 0);

    uint32 indexX = 0, indexY;
    for (real32 i = -halfSizeX; i <= halfSizeX; i += 1, ++indexX)
    {
        indexY = 0;
        for (real32 j = -halfSizeY; j <= halfSizeY; j += 1, ++indexY)
        {
            Transform currentTransform = *transform;
            // TODO(Thomas): Do properly.
            currentTransform.size = z::vec2(1);
            currentTransform.position += z::vec2(i, j);
            RenderBitmap(pattern->tiles[indexY * uint32(pattern->size.x()) + indexX],
                         RenderMode_World, &currentTransform);

            if (indexY == middleTileY && deltaY > 0)
            {
                --indexY;
                --deltaY;
            }
        }

        if (indexX == middleTileX && deltaX > 0)
        {
            --indexX;
            --deltaX;
        }
    }
}

void RenderBitmap(Bitmap* bitmap, RenderMode mode, Transform* transform)
{
    glDisable(GL_DEPTH_TEST);

    Mesh mesh;
    mesh.renderMode = mode;
    mesh.program = g_bitmapProg;
    mesh.texture = bitmap->texture;
    mesh.worldTransform = GetTransformMatrix(mode, transform);

    mesh.vertices.push_back({z::vec2(0, 0), z::vec2(0, 1)});
    mesh.vertices.push_back({z::vec2(1, 0), z::vec2(1, 1)});
    mesh.vertices.push_back({z::vec2(1, 1), z::vec2(1, 0)});
    mesh.vertices.push_back({z::vec2(0, 1), z::vec2(0, 0)});

    mesh.indices = std::vector<GLuint>({0, 1, 2, 0, 2, 3});

    g_renderQueue.push_back(mesh);
}

void LoadTexture(Bitmap* bitmap)
{
    if (!glIsTexture(bitmap->texture))
    {
        glGenTextures(1, &bitmap->texture);
        glBindTexture(GL_TEXTURE_2D, bitmap->texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                     bitmap->width, bitmap->height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE,
                     bitmap->data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void ReleaseTexture(Bitmap* bitmap)
{
    if (glIsTexture(bitmap->texture))
    {
        glDeleteTextures(1, &bitmap->texture);
    }
}

static stbtt_bakedchar cdata[96]; // NOTE(Charly): ASCII 32..126
static GLuint fontTexture;

void LoadFont(const char* font)
{
    FILE* fontFile;
    fopen_s(&fontFile, font, "rb");
    if (fontFile != nullptr)
    {
        unsigned char* ttfBuffer = new unsigned char[1 << 20];
        unsigned char* tmpBitmap = new unsigned char[512 * 512];

        fread(ttfBuffer, 1, 1 << 20, fontFile);
        // NOTE(Charly): No guarantee this fits !
        stbtt_BakeFontBitmap(ttfBuffer, 0, 32.0, tmpBitmap, 512, 512, 32, 96, cdata);

        glGenTextures(1, &fontTexture);
        glBindTexture(GL_TEXTURE_2D, fontTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, tmpBitmap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);

        delete[] tmpBitmap;
        delete[] ttfBuffer;
    }
    else
    {
        Log(Log_Error, "Could not find font %s\n", font);
    }
}

void RenderText(char* text, z::vec2 pos, GameState* state)
{
    // FIXME(Charly): The font rendering is terribly hacky, this needs to be cleaned up
    if (fontTexture == 0)
    {
        LoadFont("c:/windows/fonts/times.ttf");
    }

    glBindTexture(GL_TEXTURE_2D, fontTexture);

    GLuint idx = 0;
    Mesh mesh;

    real32 x = 0, y = 0;
    real miny = 0;

    while (*text)
    {
        if (*text >= 32 && *text < 128)
        {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(cdata, 512, 512, *text - 32, &x, &y, &q, 1);

            miny = z::Min(q.y0, miny);

            mesh.vertices.push_back({z::vec2(q.x0, q.y0), z::vec2(q.s0, q.t0)});
            mesh.vertices.push_back({z::vec2(q.x1, q.y0), z::vec2(q.s1, q.t0)});
            mesh.vertices.push_back({z::vec2(q.x1, q.y1), z::vec2(q.s1, q.t1)});
            mesh.vertices.push_back({z::vec2(q.x0, q.y1), z::vec2(q.s0, q.t1)});

            mesh.indices.push_back(idx + 0);
            mesh.indices.push_back(idx + 1);
            mesh.indices.push_back(idx + 2);

            mesh.indices.push_back(idx + 0);
            mesh.indices.push_back(idx + 2);
            mesh.indices.push_back(idx + 3);

            idx += 4;
        }
        ++text;
    }

    for (auto& v : mesh.vertices)
    {
        v.position.y() -= miny;
        v.position /= state->viewportSize;
    }

    Transform t;
    t.position = pos;
    mesh.worldTransform = GetTransformMatrix(RenderMode_ScreenRelative,Back to a stable state &t);
    mesh.renderMode = RenderMode_ScreenRelative;
    mesh.program = g_bitmapProg;
    mesh.texture = fontTexture;

    g_renderQueue.push_back(mesh);
}

void RenderMesh(const Mesh* mesh, z::mat3 proj)
{
    GLuint vao = 0, vbo = 0, ibo = 0;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Vertex),
                 mesh->vertices.data(), GL_STATIC_DRAW);

    GLint64 ptr = 0;
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)ptr);
    glEnableVertexAttribArray(0);
    ptr += 2 * sizeof(GLfloat);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)ptr);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(GLuint),
                 mesh->indices.data(), GL_STATIC_DRAW);

    glDisable(GL_DEPTH_TEST);

    glUseProgram(mesh->program);
    glBindTexture(GL_TEXTURE_2D, mesh->texture);
    glActiveTexture(GL_TEXTURE0);

    glUniform1i(glGetUniformLocation(mesh->program, "u_tex"), 0);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, (GLuint)mesh->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.f);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.f, -1.1f);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, (GLuint)mesh->indices.size() , GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_DEPTH_TEST);

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);
}

z::mat3 GetTransformMatrix(RenderMode renderMode, Transform* transform)
{
#if 1
    // TODO(Charly): origin or -origin ?
    z::mat3 O = z::Translation(-transform->origin);
    z::mat3 S = z::Scale(transform->size);
    z::mat3 R = z::Rotation(transform->rotation);
    z::mat3 T = z::Translation(transform->position);

    if (transform->orientation < 0)
    {
        S[0][0] *= -1;
    }

    // NOTE(Charly): Not sure if this is a hack or not ...
    if (renderMode == RenderMode_World)
    {
        S[1][1] *= -1;
    }

    // TODO(Charly): Check multiplication order
    z::mat3 result = T * R * S * O;
#else
    z::mat3 result;
    real32 angle = -transform->rotation;
    real32 cosine = z::Cos(angle);
    real32 sine = z::Sin(angle);
    real32 sxc = transform->size.x() * cosine;
    real32 syc = transform->size.y() * cosine;
    real32 sxs = transform->size.x() * sine;
    real32 sys = transform->size.y() * sine;
    real32 tx = -transform->origin.x() * sxc - transform->origin.y() * sys + transform->position.x();
    real32 ty = transform->origin.x() * sxs - transform->origin.y() * syc + transform->position.y();

    result[0][0] = sxc;
    result[0][1] = sys;
    result[0][2] = tx;
    result[1][0] = -sxs;
    result[1][1] = syc;
    result[1][2] = ty;
    result[2][0] = 0;
    result[2][1] = 0;
    result[2][2] = 1;

#endif
    return result;
}

z::mat3 GetProjectionMatrix(RenderMode renderMode, GameState* gameState)
{
    z::mat3 result(1);

    switch (renderMode)
    {
        case RenderMode_ScreenAbsolute:
        {
            result[0][0] = 2 / gameState->viewportSize.x();
            result[0][2] = -1;
            result[1][1] = -2 / gameState->viewportSize.y();
            result[1][2] = 1;
        } break;

        case RenderMode_ScreenRelative:
        {
            result[0][0] = 2;
            result[0][2] = -1;
            result[1][1] = -2;
            result[1][2] = 1;
        } break;

        case RenderMode_World:
        {
            result[0][0] = 2 / gameState->worldSize.x();
            result[1][1] = 2 / gameState->worldSize.y();
        } break;

        default:
        {
            Assert(!"Wrong code path");
        }
    }

    return result;
}
