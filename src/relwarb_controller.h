#ifndef RELWARB_CONTROLLER_H
#define RELWARB_CONTROLLER_H

#include "relwarb_defines.h"

enum ControllerType
{
	ControllerType_Keyboard = 0,
	ControllerType_Gamepad,
};

enum Action
{
	Action_Left = 0,
	Action_Right,
	Action_Up,
	Action_Down,
	Action_Jump,
	Action_Skill1,
	Action_Skill2,
	Action_Skill3,
	Action_Skill4,
	Action_Count,
};

struct Controller
{
	ControllerType controllerType = ControllerType_Keyboard;
	int32          actionToInput[Action_Count];

	// Used only for gamepad controller
	int32 gamepadId;
};

struct GameState;

// TODO(Charly): Save and load user defined mappings (https://github.com/Zouch/relwarb/issues/26)
// TODO(Charly): Secondary
void           SetControllerType(int32 id, ControllerType type);
ControllerType GetControllerType(int32 id);
void           SetControllerGamepadId(int32 id, int32 gid);
void           ConfigureController(int32 id, ControllerType type, int32 gid = -1);

void MapActionToInput(int32 id, int32 action, int32 input);

bool32 IsActionPressed(int32 id, int32 action);
bool32 IsActionRisingEdge(int32 id, int32 action);
bool32 IsActionFallingEdge(int32 id, int32 action);

#endif // RELWARB_CONTROLLER_H