#include "relwarb_renderer.h"

#include "relwarb.h"
#include "relwarb_opengl.h"
#include "relwarb_debug.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <stdio.h>
#include <vector> // TODO(Charly): Remove this
#include <algorithm>

#if defined(RELWARB_DEBUG)
#define GLAssert(x)                                 \
    do {                                            \
        x;                                          \
        GLenum err;                                 \
        while ((err = glGetError()) != GL_NO_ERROR) \
        {                                           \
            Assert(!"OpenGL Error");                \
        }                                           \
    } while(0)
#else
#define GLAssert(x) x
#endif

constexpr size_t MAX_PARTICLES = 100000;
global_variable size_t g_renderPeak;

global_variable const char* colorFrag = R"(
#version 450

out vec4 color;
uniform vec4 u_color;

void main()
{
    color = u_color;
}
)";

GLuint CompileProgram(GLenum shaderType, const char* shaderFile)
{
    GLuint result;

    FILE* file = fopen(shaderFile, "r");
    if (file)
    {
        fseek(file, 0, SEEK_END);
        size_t length = ftell(file);
        fseek(file, 0, SEEK_SET);

        char* shaderSource = (char*)malloc((length + 1) * sizeof(char));

        if (shaderSource)
        {
            length = fread(shaderSource, 1, length, file);
            shaderSource[length] = '\0';

            //Log(Log_Info, "File %s has %u characters.\n%s", shaderFile, length, shaderSource);

            result = glCreateShaderProgramv(shaderType, 1, &shaderSource);

            GLint linked;
            glGetProgramiv(result, GL_LINK_STATUS, &linked);
            if (linked != GL_TRUE)
            {
                GLsizei logLength;
                GLchar message[1024];
                glGetProgramInfoLog(result, 1024, &logLength, message);
                Log(Log_Error, "Shader program %s did not compile: %s", shaderFile, message);

                result = 0;
            }

            free(shaderSource);
        }
        else
        {
            Log(Log_Error, "Could not allocate memory to read program %s", shaderFile);
            result = 0;
        }

        fclose(file);
    }
    else
    {
        Log(Log_Error, "Could not open file %s", shaderFile);
        result = 0;
    }

    return result;
}

void InitializeRenderer(GameState* gameState)
{
    Renderer* renderer = &gameState->renderer;

    renderer->defaultVertexProg     = CompileProgram(GL_VERTEX_SHADER, "assets/shaders/default.vert.glsl");
    renderer->instancedVertexProg   = CompileProgram(GL_VERTEX_SHADER, "assets/shaders/instanced.vert.glsl");

    renderer->bitmapFragmentProg    = CompileProgram(GL_FRAGMENT_SHADER, "assets/shaders/bitmap.frag.glsl");
    renderer->colorFragmentProg     = CompileProgram(GL_FRAGMENT_SHADER, "assets/shaders/color.frag.glsl");
    renderer->textFragmentProg      = CompileProgram(GL_FRAGMENT_SHADER, "assets/shaders/text.frag.glsl");
    renderer->particlesFragmentProg = CompileProgram(GL_FRAGMENT_SHADER, "assets/shaders/particles.frag.glsl");

    Assert(renderer->defaultVertexProg);
    Assert(renderer->instancedVertexProg);
    Assert(renderer->bitmapFragmentProg);
    Assert(renderer->textFragmentProg);
    Assert(renderer->particlesFragmentProg);

    glGenProgramPipelines(RendererPipeline_Count, renderer->pipelines);
    glUseProgramStages(renderer->pipelines[RendererPipeline_Bitmap], GL_VERTEX_SHADER_BIT, renderer->defaultVertexProg);
    glUseProgramStages(renderer->pipelines[RendererPipeline_Bitmap], GL_FRAGMENT_SHADER_BIT, renderer->bitmapFragmentProg);

    glUseProgramStages(renderer->pipelines[RendererPipeline_Color], GL_VERTEX_SHADER_BIT, renderer->defaultVertexProg);
    glUseProgramStages(renderer->pipelines[RendererPipeline_Color], GL_FRAGMENT_SHADER_BIT, renderer->colorFragmentProg);

    glUseProgramStages(renderer->pipelines[RendererPipeline_Text], GL_VERTEX_SHADER_BIT, renderer->defaultVertexProg);
    glUseProgramStages(renderer->pipelines[RendererPipeline_Text], GL_FRAGMENT_SHADER_BIT, renderer->textFragmentProg);

    glUseProgramStages(renderer->pipelines[RendererPipeline_Particles], GL_VERTEX_SHADER_BIT, renderer->instancedVertexProg);
    glUseProgramStages(renderer->pipelines[RendererPipeline_Particles], GL_FRAGMENT_SHADER_BIT, renderer->particlesFragmentProg);

    ///////////////////////////////////////////////////////////////
    GLfloat billboard[] =
    {
        -0.5f, -0.5f, 0.f, 0.f,
        +0.5f, -0.5f, 1.f, 0.f,
        -0.5f,  0.5f, 0.f, 1.f,
        +0.5f,  0.5f, 1.f, 1.f,
    };

    glGenVertexArrays(1, &renderer->particlesVAO);
    glBindVertexArray(renderer->particlesVAO);

    glGenBuffers(ParticleBuffer_Count, renderer->particlesVBOs);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->particlesVBOs[ParticleBuffer_Billboard]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(billboard), billboard, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->particlesVBOs[ParticleBuffer_PosSize]);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(z::vec4), nullptr, GL_STREAM_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->particlesVBOs[ParticleBuffer_Color]);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(z::vec4), nullptr, GL_STREAM_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
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

GameUI* CreateTextualGameUI(GameState* gameState, char* text, z::vec4 color, z::vec2 pos, real32 duration)
{
    ComponentID id = gameState->nbGameUIs++;
    Assert(id < WORLD_SIZE);

    GameUI* result = &gameState->gameUIs[id];
    result->uiType = GameUIType_Text;
    result->text = new char[strlen(text)];
    result->initialColor = color;
    memcpy(result->text, text, strlen(text)/* * sizeof(char)*/);
    result->duration = duration;
    result->elapsed = 0.0;

    return result;
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

void UpdateGameUITime(GameUI* gameUI, real32 dt)
{
    if (gameUI->duration > 0.0)
    {
        gameUI->elapsed += dt;
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



internal void FlushRenderQueue(RenderQueue* renderQueue, GameState* gameState)
{
    // Avoid state changes as much as possible
    std::sort(renderQueue->begin(), renderQueue->end(),
              [](const Mesh& a, const Mesh& b)
              {
                  bool result =
                      a.pipeline < b.pipeline ? true
                      : a.pipeline > b.pipeline ? false
                      : a.renderMode < b.renderMode ? true
                      : a.renderMode > b.renderMode ? false
                      : a.texture < b.texture ? true : false;

                  return result;
              });

    int start = 0;

    while (start < renderQueue->size())
    {
        int end = start + 1;
        RenderMode currMode = (*renderQueue)[start].renderMode;
        GLuint currTexture = (*renderQueue)[start].texture;
        z::vec4 currColor = (*renderQueue)[start].color;

        // NOTE(Charly): Find all meshes that share a texture
        while (end < renderQueue->size() &&
               (*renderQueue)[end].renderMode == currMode &&
               (*renderQueue)[end].texture == currTexture &&
               (*renderQueue)[end].color == currColor)
        {
            ++end;
        }

        Mesh mesh;
        mesh.pipeline = (*renderQueue)[start].pipeline;
        mesh.vertexProgram = (*renderQueue)[start].vertexProgram;
        mesh.fragmentProgram = (*renderQueue)[start].fragmentProgram;
        mesh.texture = currTexture;
        mesh.color = currColor;

        z::mat3 projMatrix = GetProjectionMatrix(currMode, gameState);

        GLuint startIdx = 0;
        for (int i = start; i < end; ++i)
        {
            Mesh* currMesh = &(*renderQueue)[i];
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

    renderQueue->clear();
}

void FlushRenderQueue(GameState* gameState)
{
    if (gameState->renderer.defaultRenderQueue.size() > g_renderPeak)
    {
        g_renderPeak = gameState->renderer.defaultRenderQueue.size();
        Log(Log_Info, "New render count peak: %zu", g_renderPeak);
    }

    glViewport(0, 0, gameState->viewportSize.x(), gameState->viewportSize.y());
    glClearColor(0.3f, 0.8f, 0.7f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    FlushRenderQueue(&gameState->renderer.defaultRenderQueue, gameState);

    RenderParticles(gameState);

    glDisable(GL_DEPTH_TEST);

    FlushRenderQueue(&gameState->renderer.uiRenderQueue, gameState);
    FlushRenderQueue(&gameState->renderer.debugRenderQueue, gameState);
}

void RenderPattern(GameState* gameState, RenderingPattern* pattern, Transform* transform, z::vec2 size)
{
    switch (pattern->patternType)
    {
        case RenderingPattern_Unique:
        {
            RenderBitmap(gameState, GetSpriteBitmap(pattern->unique), RenderMode_World, transform);
        } break;
        case RenderingPattern_Fill:
        {
            RenderFillPattern(gameState, pattern, transform, size);
        } break;

        default:
        {
            // Unknown render pattern type
            Assert(!"Wrong code path");
        };
    }
}

void RenderFillPattern(GameState* gameState, RenderingPattern* pattern, Transform* transform, z::vec2 size)
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
            RenderBitmap(gameState, pattern->tiles[indexY * uint32(pattern->size.x()) + indexX],
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

void InsertMesh(GameState* gameState, const Mesh& mesh, ObjectType type)
{
    switch (type)
    {
        case ObjectType_Default:
        {
            gameState->renderer.defaultRenderQueue.push_back(mesh);
        } break;

        case ObjectType_UI:
        {
            gameState->renderer.uiRenderQueue.push_back(mesh);
        } break;

        case ObjectType_Debug:
        {
            gameState->renderer.debugRenderQueue.push_back(mesh);
        } break;

        default:
        {
            Assert(!"Wrong code path");
        }
    }
}

void RenderGameUI(GameState* gameState, const GameUI* gameUI)
{
    switch (gameUI->uiType)
    {
        case GameUIType_Text:
        {
            RenderText(gameUI->text, gameUI->initialPos, gameUI->initialColor, gameState, ObjectType_UI);
        } break;

        default:
        {
            // Unknown game UI type
            Assert(!"Wrong code path");
        };
    }
}

void RenderBitmap(GameState* gameState, Bitmap* bitmap, RenderMode mode, Transform* transform, z::vec4 color)
{
    glDisable(GL_DEPTH_TEST);

    Mesh mesh;
    mesh.renderMode = mode;
    mesh.pipeline = gameState->renderer.pipelines[RendererPipeline_Bitmap];
    mesh.vertexProgram = gameState->renderer.defaultVertexProg;
    mesh.fragmentProgram = gameState->renderer.bitmapFragmentProg;
    mesh.texture = bitmap->texture;
    mesh.worldTransform = GetTransformMatrix(mode, transform);
    mesh.color = z::Saturate(color);

    mesh.vertices.push_back({z::vec2(0, 0), z::vec2(0, 1)});
    mesh.vertices.push_back({z::vec2(1, 0), z::vec2(1, 1)});
    mesh.vertices.push_back({z::vec2(1, 1), z::vec2(1, 0)});
    mesh.vertices.push_back({z::vec2(0, 1), z::vec2(0, 0)});

    mesh.indices = std::vector<GLuint>({0, 1, 2, 0, 2, 3});

    gameState->renderer.defaultRenderQueue.push_back(mesh);
}

void RenderParticles(GameState* gameState)
{
    // 0: Billboard -> vec4
    // 1: PosSize -> vec4
    // 2: Color -> vec4

    GLsizei particleCount = 0;

    local_persist std::vector<z::vec4> positionsSizes;
    local_persist std::vector<z::vec4> colors;
    local_persist bool32 firstCall = true;

    if (firstCall)
    {
        positionsSizes.reserve(MAX_PARTICLES);
        colors.reserve(MAX_PARTICLES);
        firstCall = false;
    }

    z::mat3 projMatrix = GetProjectionMatrix(RenderMode_World, gameState);
    for (int systemIdx = 0; systemIdx < MAX_PARTICLE_SYSTEMS; ++systemIdx)
    {
        ParticleSystem* system = gameState->particleSystems + systemIdx;
        for (const auto& particle : system->particles)
        {
            z::mat3 worldMatrix = z::Translation(particle.p);
            z::mat3 transformMatrix = projMatrix * worldMatrix;

            z::vec3 pos = transformMatrix * z::vec3(0, 0, 1);
            z::vec3 size = transformMatrix * z::vec3(0.5, 0.5, 0);
            z::vec4 ps(pos.x(), pos.y(), size.x(), size.y());
            positionsSizes.push_back(ps);
            colors.push_back(particle.color);
        }
    }

    particleCount = (GLsizei)positionsSizes.size();
    Log(Log_Info, "Particle count : %u", particleCount);

    if (particleCount > MAX_PARTICLES)
    {
        Log(Log_Warning, "Too much particles, need a bit of shrinking");
        positionsSizes.resize(MAX_PARTICLES);
        colors.resize(MAX_PARTICLES);
        particleCount = (GLsizei)positionsSizes.size();
    }

    glBindVertexArray(gameState->renderer.particlesVAO);

    glBindBuffer(GL_ARRAY_BUFFER, gameState->renderer.particlesVBOs[ParticleBuffer_PosSize]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, positionsSizes.size() * sizeof(z::vec4), positionsSizes.data());

    glBindBuffer(GL_ARRAY_BUFFER, gameState->renderer.particlesVBOs[ParticleBuffer_Color]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(z::vec4), colors.data());

    glBindProgramPipeline(gameState->renderer.pipelines[RendererPipeline_Particles]);
    glBindTextureUnit(0, gameState->particleBitmap.texture);

    glProgramUniform2f(gameState->renderer.instancedVertexProg,
                       glGetUniformLocation(gameState->renderer.instancedVertexProg, "u_worldSize"),
                       gameState->worldSize.x(), gameState->worldSize.y());
    glProgramUniform1i(gameState->renderer.particlesFragmentProg,
                       glGetUniformLocation(gameState->renderer.particlesFragmentProg, "u_tex"), 0);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particleCount);

    positionsSizes.clear();
    colors.clear();
}

void LoadTexture(Bitmap* bitmap)
{
    if (!glIsTexture(bitmap->texture))
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &bitmap->texture);
        glTextureStorage2D(bitmap->texture, 1, GL_RGBA8, bitmap->width, bitmap->height);
        glTextureSubImage2D(bitmap->texture, 0, 0, 0,
                            bitmap->width, bitmap->height, GL_RGBA,
                            GL_UNSIGNED_BYTE, bitmap->data);

        glTextureParameteri(bitmap->texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(bitmap->texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(bitmap->texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(bitmap->texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glGenerateTextureMipmap(bitmap->texture);
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
        GLAssert(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, tmpBitmap));
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

void RenderText(char* text, z::vec2 pos, z::vec4 color, GameState* gameState, ObjectType type)
{
    // FIXME(Charly): The font rendering is terribly hacky, this needs to be cleaned up
    if (fontTexture == 0)
    {
        LoadFont("assets/fonts/Righteous-Regular.ttf");
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
        v.position /= gameState->viewportSize;
    }

    Transform t;
    t.position = pos;
    mesh.worldTransform = GetTransformMatrix(RenderMode_ScreenRelative, &t);
    mesh.renderMode = RenderMode_ScreenRelative;
    mesh.pipeline = gameState->renderer.pipelines[RendererPipeline_Text];
    mesh.vertexProgram = gameState->renderer.defaultVertexProg;
    mesh.fragmentProgram = gameState->renderer.textFragmentProg;
    mesh.texture = fontTexture;
    mesh.color = color;

    InsertMesh(gameState, mesh, type);
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

    glBindProgramPipeline(mesh->pipeline);
    glBindTextureUnit(0, mesh->texture);

    Assert(glIsProgram(mesh->fragmentProgram));
    glProgramUniform1i(mesh->fragmentProgram, glGetUniformLocation(mesh->fragmentProgram, "u_tex"), 0);
    glProgramUniform4fv(mesh->fragmentProgram, glGetUniformLocation(mesh->fragmentProgram, "u_color"), 1, mesh->color.data);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, (GLuint)mesh->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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
