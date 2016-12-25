#include "relwarb_renderer.h"

#include "relwarb.h"
#include "relwarb_opengl.h"
#include "relwarb_debug.h"

global_variable GLuint g_bitmapProg;
global_variable GLuint g_colorProg;

global_variable const char* bitmapVert = R"(
#version 330

layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_uv;

uniform mat4 u_proj;
uniform mat4 u_world;
uniform mat4 u_model;

out vec2 uv;

void main()
{
    gl_Position = u_proj * u_world * u_model * vec4(in_pos, 0, 1);
    uv = in_uv;
}
)";

global_variable const char* bitmapFrag = R"(
#version 330

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

global_variable GLfloat vertices[] =
{
    -.5f, -.5f, 0.f, 0.f,
     .5f, -.5f, 1.f, 0.f,
     .5f,  .5f, 1.f, 1.f,
    -.5f,  .5f, 0.f, 1.f,
};

global_variable GLuint indices[] =
{
    0, 1, 2,
    0, 2, 3,
};

global_variable GLuint vao, vbo, ibo;

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

void InitializeRenderer()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    Assert(vao);
    Assert(vbo);
    Assert(ibo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

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

void RenderPattern(RenderingPattern* pattern, Transform* transform, z::vec2 size)
{
    switch (pattern->patternType)
    {
        case RenderingPattern_Unique:
        {
            RenderBitmap(GetSpriteBitmap(pattern->unique), transform);
        } break;
        case RenderingPattern_Fill:
        {
            RenderFillPattern(pattern, transform, size);
        } break;

        default:
        {
            // Unknown render pattern type
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
            currentTransform.scale = z::vec2(1);
            currentTransform.position += z::vec2(i, j);
            RenderBitmap(pattern->tiles[indexY * uint32(pattern->size.x()) + indexX], &currentTransform);

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

void RenderBitmap(Bitmap* bitmap, const Transform* transform)
{
    glDisable(GL_DEPTH_TEST);

    // TODO(Charly): Should this be done in an Init step ?
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

    z::mat4 T = z::Translation(transform->position.x(), transform->position.y(), 0);
    z::mat4 S = z::Scale(transform->scale.x(), transform->scale.y(), 0);
    z::mat4 model = S * T;

    glUseProgram(g_bitmapProg);
    glBindTexture(GL_TEXTURE_2D, bitmap->texture);
    glActiveTexture(GL_TEXTURE0);

    glUniform1i(glGetUniformLocation(g_bitmapProg, "u_tex"), 0);
    glUniformMatrix4fv(glGetUniformLocation(g_bitmapProg, "u_proj"), 1, GL_FALSE, transform->proj.data);
    glUniformMatrix4fv(glGetUniformLocation(g_bitmapProg, "u_world"), 1, GL_FALSE, transform->world.data);
    glUniformMatrix4fv(glGetUniformLocation(g_bitmapProg, "u_model"), 1, GL_FALSE, model.data);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.f);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.f, -1.1f);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_DEPTH_TEST);
}

void ReleaseBitmap(Bitmap* bitmap)
{
    if (glIsTexture(bitmap->texture))
    {
        glDeleteTextures(1, &bitmap->texture);
    }
}
