#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "imgui/examples/imgui_impl_sdl.h"
#include "ModuleFileSystem.h"
#include "ModuleRenderer3D.h"
#include "mmgr/mmgr.h"
#include "Optick/include/optick.h"

#include "Maths.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(bool start_enabled) : Module(start_enabled)
{
	name.assign("Input");

	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init()
{
	LOG_ENGINE("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG_ENGINE("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0)
	{
		LOG_ENGINE("SDL_INIT_GAMECONTROLLER could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	if (SDL_InitSubSystem(SDL_INIT_HAPTIC) < 0)
	{
		LOG_ENGINE("SDL_GAMECONTROLLER HAPTIC could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	OPTICK_EVENT();
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if (keyboard[i] == KEY_IDLE) {
				keyboard[i] = KEY_DOWN;
				AddInputBuff(i, KEY_DOWN);
			}
			else {
				keyboard[i] = KEY_REPEAT;
				AddInputBuff(i, KEY_REPEAT);
			}
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN) {
				keyboard[i] = KEY_UP;
				AddInputBuff(i, KEY_UP);
			}
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if (mouse_buttons[i] == KEY_IDLE) {
				mouse_buttons[i] = KEY_DOWN;
				AddInputBuff(i, KEY_DOWN, true);
			}
			else {
				mouse_buttons[i] = KEY_REPEAT;
				AddInputBuff(i, KEY_REPEAT, true);
			}
		}
		else
		{
			if (mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN) {
				mouse_buttons[i] = KEY_UP;
				AddInputBuff(i, KEY_UP, true);
			}
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}
	
	auto item = game_pads.begin();
	for (; item != game_pads.end(); ++item) {
		if ((*item).second != nullptr) {
			for (uint i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i)
			{
				Uint8 state = SDL_GameControllerGetButton((*item).second->controller, (SDL_GameControllerButton)i);
				if (state == 1)
				{
					if ((*item).second->controller_buttons[i] == KEY_IDLE) {
						(*item).second->controller_buttons[i] = KEY_DOWN;
					}
					else {
						(*item).second->controller_buttons[i] = KEY_REPEAT;
					}
				}
				else
				{
					if ((*item).second->controller_buttons[i] == KEY_REPEAT || (*item).second->controller_buttons[i] == KEY_DOWN) {
						(*item).second->controller_buttons[i] = KEY_UP;
					}
					else
						(*item).second->controller_buttons[i] = KEY_IDLE;
				}
			}
		}


		(*item).second->joystick_left.valueX = GetControllerAxis((*item).second, SDL_CONTROLLER_AXIS_LEFTX);
		(*item).second->joystick_left.valueY = GetControllerAxis((*item).second, SDL_CONTROLLER_AXIS_LEFTY);
		(*item).second->joystick_right.valueX = GetControllerAxis((*item).second, SDL_CONTROLLER_AXIS_RIGHTX);
		(*item).second->joystick_right.valueY = GetControllerAxis((*item).second, SDL_CONTROLLER_AXIS_RIGHTY);

		SetJoystickState((*item).second->joystick_left.valueX, &(*item).second->joystick_left.joystick_state_right, Input::JOYSTICK_BUTTONS::JOYSTICK_RIGHT);
		SetJoystickState((*item).second->joystick_left.valueX, &(*item).second->joystick_left.joystick_state_left, Input::JOYSTICK_BUTTONS::JOYSTICK_LEFT);
		SetJoystickState((*item).second->joystick_left.valueY, &(*item).second->joystick_left.joystick_state_up, Input::JOYSTICK_BUTTONS::JOYSTICK_UP);
		SetJoystickState((*item).second->joystick_left.valueY, &(*item).second->joystick_left.joystick_state_down, Input::JOYSTICK_BUTTONS::JOYSTICK_DOWN);

		SetJoystickState((*item).second->joystick_right.valueX, &(*item).second->joystick_right.joystick_state_right, Input::JOYSTICK_BUTTONS::JOYSTICK_RIGHT);
		SetJoystickState((*item).second->joystick_right.valueX, &(*item).second->joystick_right.joystick_state_left, Input::JOYSTICK_BUTTONS::JOYSTICK_LEFT);
		SetJoystickState((*item).second->joystick_right.valueY, &(*item).second->joystick_right.joystick_state_up, Input::JOYSTICK_BUTTONS::JOYSTICK_UP);
		SetJoystickState((*item).second->joystick_right.valueY, &(*item).second->joystick_right.joystick_state_down, Input::JOYSTICK_BUTTONS::JOYSTICK_DOWN);


	}

	mouse_x_motion = mouse_y_motion = 0;

	bool first_key = true;
	mouse_pressed = false;
	first_key_pressed = SDL_SCANCODE_UNKNOWN;
	bool quit = false;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
#ifndef GAME_VERSION
		ImGui_ImplSDL2_ProcessEvent(&e);
#endif
		switch(e.type)
		{
		case SDL_MOUSEWHEEL: {
			mouse_z = e.wheel.y;
			break; }
		case SDL_KEYDOWN: {
			if (first_key) {
				first_key = false;
				first_key_pressed = e.key.keysym.scancode;
			}
			break; }
		case SDL_MOUSEMOTION: {
			mouse_x = e.motion.x / SCREEN_SIZE;
			mouse_y = e.motion.y / SCREEN_SIZE;

			mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = e.motion.yrel / SCREEN_SIZE;
			break; }
		case SDL_MOUSEBUTTONDOWN: {
			mouse_pressed = true;
			break; }
		case SDL_QUIT: {
			quit = true;
			App->QuitApp();
			break; }
		case SDL_DROPFILE: {
			App->file_system->ManageNewDropFile(e.drop.file);
			SDL_free(e.drop.file);
			break; }
		case SDL_WINDOWEVENT: {
			if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				App->renderer3D->OnResize(e.window.data1, e.window.data2);
			break; }
		case SDL_CONTROLLERDEVICEADDED: {
			SDL_GameController* controller = SDL_GameControllerOpen(e.cdevice.which);
			if (controller != nullptr) {
				if (SDL_JoystickIsHaptic(SDL_GameControllerGetJoystick(controller)) > 0)
				{
					SDL_Haptic* haptic = SDL_HapticOpenFromJoystick(SDL_GameControllerGetJoystick(controller));

					if (haptic != nullptr)
					{
						if (SDL_HapticRumbleInit(haptic) < 0)
						{
							LOG_ENGINE("Warning: Unable to initialize rumble! SDL Error: %s\n", SDL_GetError());
						}

						if (SDL_HapticRumblePlay(haptic, 0.3f, 1000) < 0)
						{
							LOG_ENGINE("Error when rumbing the controller number %i", e.cdevice.which);
						}
					}
					GamePad* pad = new GamePad();
					pad->controller = controller;
					pad->haptic = haptic;
					pad->number = e.cdevice.which + 1;
					memset(pad->controller_buttons, KEY_IDLE, sizeof(KEY_STATE) * SDL_CONTROLLER_BUTTON_MAX);
					game_pads.emplace(pad->number, pad);
					LOG_ENGINE("Controller %i loaded correctly", pad->number);
				}
				else
				{
					LOG_ENGINE("haptic error! SDL_Error: %s\n", SDL_GetError());
					LOG_ENGINE("haptic error! SDL_Error: %i\n", SDL_JoystickIsHaptic(SDL_GameControllerGetJoystick(controller)));
				}
			}
			else {
				LOG_ENGINE("Error when trying to open the controller number %i", e.cdevice.which);
			}
			break; }
		case SDL_CONTROLLERDEVICEREMOVED: {
			auto item = game_pads.begin();
			for (; item != game_pads.end(); ++item) {
				if ((*item).second != nullptr && SDL_GameControllerGetAttached((*item).second->controller) == SDL_FALSE) {
					SDL_HapticClose((*item).second->haptic);
					(*item).second->haptic = nullptr;
					SDL_GameControllerClose((*item).second->controller);
					(*item).second->controller = nullptr;
					delete (*item).second;
					(*item).second = nullptr;
					game_pads.erase(item);
					LOG_ENGINE("CONTROLLER ERASED");
					break;
				}
			}
			break; }
		}
	}

	for (; item != game_pads.end(); ++item) {
		if ((*item).second != nullptr) {

		}
	}

	if (quit == true) {
		return UPDATE_STOP;
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG_ENGINE("Quitting SDL input event subsystem");

	for (auto gpads = game_pads.begin(); gpads != game_pads.end(); ++gpads)
		delete (*gpads).second;

	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

bool ModuleInput::IsControllerActive(int controller_index)
{
	auto item = game_pads.begin();
	for (; item != game_pads.end(); ++item) {
		if ((*item).second != nullptr && (*item).second->number == controller_index) {
			return true;
		}
	}
	return false;
}

float3 ModuleInput::GetMousePosition()
{
	return float3(mouse_x, mouse_y, mouse_z);
}

void ModuleInput::AddInputBuff(const uint& key, const uint& state, const bool& is_mouse)
{
	static char text[60];
	const char* state_string = nullptr;
	switch (state) {
	case KEY_DOWN:
		state_string = "KEY_DOWN";
		break;
	case KEY_UP:
		state_string = "KEY_UP";
		break;
	case KEY_REPEAT:
		state_string = "KEY_REPEAT";
		break;
	}

	if (is_mouse)
		sprintf_s(text, 60, "MOUSE: %i - %s \n", SDL_GetScancodeName((SDL_Scancode)key), state_string);
	else
		sprintf_s(text, 60, "KEYBOARD: %s - %s \n", SDL_GetScancodeName((SDL_Scancode)key), state_string);
	
	static char repeat[60] = "";

	if (strcmp(repeat, text))
		input.appendf(text);

	strcpy(repeat, text);
}

void ModuleInput::SetJoystickState(float value, KEY_STATE* state, Input::JOYSTICK_BUTTONS joystick)
{

	switch (joystick)
	{
	case Input::JOYSTICK_UP: {

		if (*state == KEY_IDLE && value < 0)
			*state = KEY_DOWN;

		else if (*state == KEY_DOWN && value < 0)
			*state = KEY_REPEAT;

		else if (*state == KEY_DOWN && value == 0)
			*state = KEY_IDLE;

		else if (*state == KEY_REPEAT && value == 0)
			*state = KEY_IDLE;
		break; }

	case Input::JOYSTICK_DOWN: {

		if (*state == KEY_IDLE && value > 0)
			*state = KEY_DOWN;

		else if (*state == KEY_DOWN && value > 0)
			*state = KEY_REPEAT;

		else if (*state == KEY_DOWN && value == 0)
			*state = KEY_IDLE;

		else if (*state == KEY_REPEAT && value == 0)
			*state = KEY_IDLE;
		break; }

	case Input::JOYSTICK_RIGHT: {

		if (*state == KEY_IDLE && value > 0)
			*state = KEY_DOWN;

		else if (*state == KEY_DOWN && value > 0)
			*state = KEY_REPEAT;

		else if (*state == KEY_DOWN && value == 0)
			*state = KEY_IDLE;

		else if (*state == KEY_REPEAT && value == 0)
			*state = KEY_IDLE;
		break; }

	case Input::JOYSTICK_LEFT: {

		if (*state == KEY_IDLE && value < 0)
			*state = KEY_DOWN;

		else if (*state == KEY_DOWN && value < 0)
			*state = KEY_REPEAT;

		else if (*state == KEY_DOWN && value == 0)
			*state = KEY_IDLE;

		else if (*state == KEY_REPEAT && value == 0)
			*state = KEY_IDLE;
		break; }

	default: {
		break; }
	}
	
}

float ModuleInput::GetControllerAxis(GamePad* gp , SDL_GameControllerAxis axis) {
	Sint16 axisValue = SDL_GameControllerGetAxis(gp->controller, axis);
	//Avoid clunky zone for older or used gamepads precision
	if (axisValue < -DEAD_ZONE || axisValue > DEAD_ZONE) {
		return Maths::Map(axisValue, SDL_MIN_SINT16, SDL_MAX_SINT16, -1.0f, 1.0f);
	}
	return 0;
}