#include "relwarb_renderer.h"

#include "relwarb.h"
#include "relwarb_opengl.h"
#include "relwarb_debug.h"

global_variable GLuint g_bitmapProg;

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

RenderingPattern* CreateRenderingPattern(GameState* gameState, 
                                         Vec2 size, 
                                         uint8* pattern, 
                                         Bitmap** bitmaps, 
                                         uint8 nbBitmaps,
                                         RenderingPatternType type)
{
	ComponentID id = gameState->nbPatterns++;
	Assert(id < WORLD_SIZE);
	RenderingPattern* result = &gameState->patterns[id];
	result->size = size;
    result->patternType = type;

	result->pattern = new uint8[size.x * size.y];
	memcpy(result->pattern, pattern, size.x * size.y * sizeof(uint8));

	result->bitmaps = new Bitmap*[nbBitmaps];
	memcpy(result->bitmaps, bitmaps, nbBitmaps * sizeof(Bitmap *));

	return result;
}

void AddRenderingPatternToEntity(Entity* entity, RenderingPattern* pattern)
{
	entity->pattern = pattern;
	SetEntityFlag(entity, ComponentFlag_Renderable);
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
}

void RenderPattern(RenderingPattern* pattern, Transform* transform, Vec2 size)
{
    switch (pattern->patternType)
    {
        case RenderingPattern_Unique:
        {
            RenderBitmap(pattern->bitmaps[0], transform);
        } break;

        default:
        {
            // Unknown render pattern type
        }; 
    }
	// uint32 deltaX = size.x - pattern->size.x;
	// uint32 deltaY = size.y - pattern->size.y;
	// uint32 halfSizeX = size.x * 0.5;
	// uint32 halfSizeY = size.y * 0.5;
	// Assert(deltaX >= 0 && deltaY >= 0);
	// for (uint32 i = -halfSizeX; i < halfSizeX; ++i)
	// {
	// 	uint32 indexX = i ;
	// 	if (deltaX > 0 && i > 0)
	// 	{
	// 		indexX--;
	// 		deltaX--;
	// 	}
	// 	for (uint32 j = 0; j < size.y; ++j)
	// 	{
	// 		uint32 indexY = j;
	// 		if (deltaY > 0 && j > 0)
	// 		{
	// 			indexY--;
	// 			deltaY--;
	// 		}
	// 		Transform currentTransform = *transform;
	// 		currentTransform.position += Vec2();
	// 		RenderBitmap(pattern->bitmaps[indexY * uint32(pattern->size.x) + indexX], &currentTransform);
	// 	}
	// }
}

void RenderBitmap(Bitmap* bitmap, Transform* transform)
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

    Mat4 pos = Translation(transform->position.x, transform->position.y, 0);

    glUseProgram(g_bitmapProg);
    glBindTexture(GL_TEXTURE_2D, bitmap->texture);
    glActiveTexture(GL_TEXTURE0);

    glUniform1i(glGetUniformLocation(g_bitmapProg, "u_tex"), 0);
    glUniformMatrix4fv(glGetUniformLocation(g_bitmapProg, "u_proj"), 1, GL_FALSE, transform->proj.data);
    glUniformMatrix4fv(glGetUniformLocation(g_bitmapProg, "u_world"), 1, GL_FALSE, transform->world.data);
    glUniformMatrix4fv(glGetUniformLocation(g_bitmapProg, "u_model"), 1, GL_FALSE, pos.data);

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