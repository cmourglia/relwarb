#include "relwarb.h"

#include <ctime>

#include "relwarb_math.h"
#include "relwarb_renderer.h"
#include "relwarb_opengl.h"
#include "relwarb_debug.h"
#include "relwarb_parser.h"
#include "relwarb_editor.h"

// TODO(Charly): This should go somewhere else.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


GameState* state = nullptr;

internal void ConfigureControllers();
void InitGame()
{
	z::SeedRNG((unsigned)time(nullptr));
	LoadBitmapData("assets/sprites/health_full.png", &state->hudHealth[0]);
	LoadBitmapData("assets/sprites/health_mid.png", &state->hudHealth[1]);
	LoadBitmapData("assets/sprites/health_none.png", &state->hudHealth[2]);
	LoadBitmapData("assets/sprites/mana_full.png", &state->hudMana[0]);
	LoadBitmapData("assets/sprites/mana_none.png", &state->hudMana[1]);
	// LoadBitmapData("assets/sprites/smiley.png", &state->particleBitmap);
	LoadBitmapData("assets/sprites/particle.png", &state->particleBitmap);

	InitializeRenderer();
	state->projMatrix = z::Ortho(-state->viewportSize.x / 2,
	                             state->viewportSize.x / 2,
	                             -state->viewportSize.y / 2,
	                             state->viewportSize.y / 2);
	real32 ratio      = state->viewportSize.x / state->viewportSize.y;

	// NOTE(Thomas): Seems like worldSize should be the one we define, and windows size/viewport
	// size are computed accordingly.
	state->worldSize = z::Vec2(48, 24);

	z::mat4 worldMat(1);
	worldMat[0][0]     = (state->viewportSize.x / 2.f) / (state->worldSize.x / 2.f);
	worldMat[1][1]     = (state->viewportSize.y / 2.f) / (state->worldSize.y / 2.f);
	state->worldMatrix = worldMat;

	state->world = new b2World(b2Vec2(0, -10.0f));

	// NOTE(Thomas): Must be before any other data is created, as indices are hardcoded in the file
	LoadMapFile("config/base_map.ini");

	Bitmap* bitmap_p1[2];
	bitmap_p1[0] = CreateBitmap();
	LoadBitmapData("assets/sprites/p1_stand.png", bitmap_p1[0]);
	bitmap_p1[1] = CreateBitmap();
	LoadBitmapData("assets/sprites/p1_stand2.png", bitmap_p1[1]);

	ComponentID sprite_p1 = CreateTimeSprite(2, bitmap_p1, 0.5f);

	Bitmap* bitmap_p2[2];
	bitmap_p2[0] = CreateBitmap();
	LoadBitmapData("assets/sprites/p2_stand.png", bitmap_p2[0]);
	bitmap_p2[1] = CreateBitmap();
	LoadBitmapData("assets/sprites/p2_stand2.png", bitmap_p2[1]);

	ComponentID sprite_p2 = CreateTimeSprite(2, bitmap_p2, 0.5f);

	uint8 tiles_indices[] = {1};

	auto heroShape = CreateShape(z::Vec2(1.f, 1.5f));

	auto heroPattern1 = CreateUniqueRenderingPattern(sprite_p1);
	CreatePlayerEntity(z::Vec2(-2, -2), heroPattern1, heroShape, 0);

	auto heroPattern2 = CreateUniqueRenderingPattern(sprite_p2);
	CreatePlayerEntity(z::Vec2(2, -2), heroPattern2, heroShape, 1);

	ConfigureControllers();

	LoadBitmapData("assets/sprites/corner_topleft.png", CreateBitmap());
	LoadBitmapData("assets/sprites/horizontal_up.png", CreateBitmap());
	LoadBitmapData("assets/sprites/corner_topright.png", CreateBitmap());
	LoadBitmapData("assets/sprites/vertical_left.png", CreateBitmap());
	LoadBitmapData("assets/sprites/vertical_right.png", CreateBitmap());
	LoadBitmapData("assets/sprites/corner_bottomleft.png", CreateBitmap());
	LoadBitmapData("assets/sprites/horizontal_down.png", CreateBitmap());
	LoadBitmapData("assets/sprites/corner_bottomright.png", CreateBitmap());
	LoadBitmapData("assets/sprites/horizontal_up.png", CreateBitmap());
}

void UpdateGame(real32 dt)
{
	// NOTE(Charly): Toggle game mode on presses
	if (IsKeyRisingEdge(Key_F1))
	{
		state->mode = state->mode == GameMode_Game ? GameMode_Editor : GameMode_Game;
	}

	local_persist bool32 slowDownTime = false;
	if (IsKeyRisingEdge(Key_T))
	{
		slowDownTime ^= true;
	}

	if (slowDownTime) dt *= 0.1f;

	switch (state->mode)
	{
		case GameMode_Game:
		{
			if (IsMouseButtonRisingEdge(MouseButton_Left))
			{
				SpawnParticleSystem(GetCursorWorldPosition());
			}

			UpdateGameLogic(dt);
			UpdateWorld(dt);
		}
		break;

		case GameMode_Editor:
		{
			UpdateEditor();
		}
		break;

		default:
		{
			Assert(!"Wrong code path");
		}
	}
}

// TODO(Charly): Move this in renderer ?
void RenderGame(real32 dt)
{
	switch (state->mode)
	{
		case GameMode_Game:
		{
			for (uint32 elementIdx = 0; elementIdx < state->nbEntities; ++elementIdx)
			{
				Entity* entity = &state->entities[elementIdx];
				if (EntityHasComponent(entity, ComponentFlag_Renderable))
				{
					Transform transform = GetWorldTransform(entity->p);

					// TODO(Thomas): Handle drawing size with a drawing size
					if (EntityHasComponent(entity, ComponentFlag_Collidable))
					{
						transform.size = GetShape(entity)->size;
						transform.origin += GetShape(entity)->offset;
					}

					if (entity->entityType == EntityType_Player)
					{
						transform.orientation = entity->orientation < 0.f ? -1 : 1;
					}

					RenderPattern(entity->pattern, &transform, GetShape(entity)->size);
				}
			}

			RenderHUD();
			RenderText("Hello, World", z::Vec2(0.0, 0.0), z::Vec4(1, 0, 0, 1), ObjectType_Debug);
			RenderText("I am another test text !",
			           z::Vec2(0.0, 0.1),
			           z::Vec4(0, 1, 0, 1),
			           ObjectType_Debug);
			RenderText("abcdefghijklmnopqrstuvwxyz 0123456789",
			           z::Vec2(0.0, 0.2),
			           z::Vec4(0, 0, 1, 1),
			           ObjectType_Debug);

			char fps[128];
			snprintf(fps, 128, "dt: %.3f, fps: %.3f", dt, 1 / dt);
			RenderText(fps, z::Vec2(0.8, 0), z::Vec4(0, 0, 0, 1), ObjectType_Debug);

			state->world->DrawDebugData();

			FlushRenderQueue();
		}
		break;

		case GameMode_Editor:
		{
			RenderEditor();
		}
		break;

		default:
		{
			Assert(!"Wrong code path");
		}
	}
}

void RenderHUD()
{
	real32 ratio = state->viewportSize.x / state->viewportSize.y;

	Transform transform;

	z::vec2 onScreenPos = z::Vec2(0.04, 0.04);
	for (uint32 i = 0; i < state->nbPlayers; ++i)
	{
		Entity* player = GetPlayerEntity(i);

		transform.size = z::Vec2(0.0625, 0.0625 * ratio);

		// Avatar
		transform.position = onScreenPos;
		RenderBitmap(player->avatar, RenderMode_ScreenRelative, &transform);

		// Health
		transform.size    = z::Vec2(0.025f, 0.025f * ratio);
		z::vec2 healthPos = onScreenPos + z::Vec2(0.075f, 0.f);
		for (uint32 hp = 0; hp < player->max_health; hp += 2)
		{
			transform.position = healthPos;
			if (hp < player->health)
			{
				if (hp + 1 < player->health)
				{
					RenderBitmap(&state->hudHealth[0], RenderMode_ScreenRelative, &transform);
				}
				else
				{
					RenderBitmap(&state->hudHealth[1], RenderMode_ScreenRelative, &transform);
				}
			}
			else
			{
				RenderBitmap(&state->hudHealth[2], RenderMode_ScreenRelative, &transform);
			}

			healthPos.x += 0.0255f;
		}

		// Mana
		z::vec2 manaPos = onScreenPos + z::Vec2(0.075f, 0.0375f * ratio);
		for (uint32 mp = 0; mp < player->max_mana; ++mp)
		{
			transform.position = manaPos;
			if (mp < player->mana)
			{
				RenderBitmap(&state->hudMana[0], RenderMode_ScreenRelative, &transform);
			}
			else
			{
				RenderBitmap(&state->hudMana[1], RenderMode_ScreenRelative, &transform);
			}
			manaPos.x += 0.0255f;
		}

		onScreenPos.x += 0.24;
	}
}

void LoadBitmapData(const char* filename, Bitmap* bitmap)
{
	// NOTE(Charly): Get images bottom-up
	stbi_set_flip_vertically_on_load(true);
	int n;
	bitmap->data = stbi_load(filename, &bitmap->width, &bitmap->height, &n, 4);
	Assert(bitmap->data);

	LoadTexture(bitmap);
}

void ReleaseBitmapData(Bitmap* bitmap)
{
	ReleaseTexture(bitmap);
	stbi_image_free(bitmap->data);
}

Entity* CreateEntity(EntityType type, z::vec2 p, z::vec2 dp, z::vec2 ddp)
{
	EntityID id = state->nbEntities++;
	Assert(id < WORLD_SIZE);

	static Entity empty  = {};
	Entity*       result = &state->entities[id];
	*result              = empty;

	result->id = id;

	result->p   = p;
	result->dp  = dp;
	result->ddp = ddp;

	result->entityType = type;

	return result;
}

Bitmap* CreateBitmap()
{
	ComponentID id = state->nbBitmaps++;
	Assert(id < WORLD_SIZE);
	Bitmap* result = &state->bitmaps[id];

	return result;
}

z::vec2 ViewportToWorld(z::vec2 in)
{
	// [0, viewport] -> [0, 1], origin top left
	z::vec2 result = in / state->viewportSize;
	// [0, 1] -> [0, 1] origin bot left
	result.y = 1 - result.y;
	// [0, 1] -> [-0.5, 0.5]
	result = result - z::Vec2(0.5);
	// [-0.5, 0.5] -> [-world / 2, world / 2]
	result = result * state->worldSize;

	return result;
}

Transform GetWorldTransform(z::vec2 pos)
{
	Transform result;
	result.position = pos;
	result.origin   = z::Vec2(0.5);

	return result;
}

internal void ConfigureControllers()
{
	ConfigureController(0, ControllerType_Keyboard);
	MapActionToInput(0, Action_Left, Key_Left);
	MapActionToInput(0, Action_Right, Key_Right);
	MapActionToInput(0, Action_Up, Key_Up);
	MapActionToInput(0, Action_Down, Key_Down);
	MapActionToInput(0, Action_Jump, Key_Up);
	MapActionToInput(0, Action_Skill1, Key_Space);
	MapActionToInput(0, Action_Skill2, Key_LShift);

	ConfigureController(1, ControllerType_Gamepad, 0);
	MapActionToInput(1, Action_Left, GamepadButton_PadLeft);
	MapActionToInput(1, Action_Right, GamepadButton_PadRight);
	MapActionToInput(1, Action_Up, GamepadButton_PadUp);
	MapActionToInput(1, Action_Down, GamepadButton_PadDown);
	MapActionToInput(1, Action_Jump, GamepadButton_A);
	MapActionToInput(1, Action_Skill1, GamepadButton_X);
	MapActionToInput(1, Action_Skill2, GamepadButton_Y);
}