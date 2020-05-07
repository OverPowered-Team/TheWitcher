#include "VagoneteMove.h"
#include "VagoneteDirection.h"
#include "Wagonnete_UI.h"

Quat VagoneteInputs::playerRotation = Quat::identity();
float VagoneteInputs::inclination4player = 0.0F;
float VagoneteInputs::globalInclination = 0.0F;
float VagoneteInputs::globalInclinationY = 0.0F;
float VagoneteInputs::speedInclination = 0.0F;
VagoneteInputs::State VagoneteInputs::globalState = VagoneteInputs::State::IDLE;

VagoneteMove::VagoneteMove()
{
}

VagoneteMove::~VagoneteMove()
{
	for (auto item = players.begin(); item != players.end(); ++item) {
		delete* item;
		*item = nullptr;
	}
	players.clear();
}

void VagoneteMove::Start()
{
	curve = GameObject::FindWithName("Curve")->GetComponent<ComponentCurve>();
	rigid_body = GetComponent<ComponentRigidBody>();
	players.push_back(new VagoneteInputs(PlayerController::PlayerType::GERALT));
	players.push_back(new VagoneteInputs(PlayerController::PlayerType::YENNEFER));
	max_life = vagonete_life;
	HUD = GameObject::FindWithName("Wagonnette_UI")->GetComponent<Wagonnete_UI>();
}

void VagoneteMove::Update()
{
	VagoneteInputs::playerRotation = Quat::identity();
	VagoneteInputs::globalInclination = 0;
	VagoneteInputs::globalInclinationY = 0;

	for (auto item = players.begin(); item != players.end(); ++item) {
		(*item)->Update();
	}

	FollowCurve();

	if (Input::GetKeyDown(SDL_SCANCODE_1)) {
		actual_pos = 0;
	}
}

void VagoneteMove::OnTriggerEnter(ComponentCollider* col)
{
	if (strcmp("VagoneteDirection", col->game_object_attached->GetTag()) == 0) {
		VagoneteDirection* direction = col->game_object_attached->GetComponent<VagoneteDirection>();
		if (direction != nullptr) {
			if (VagoneteInputs::globalInclination == 0) {
				if (direction->default_right) {
					curve = direction->curve_right->GetComponent<ComponentCurve>();
				}
				else {
					curve = direction->curve_left->GetComponent<ComponentCurve>();
				}
			}
			else {
				if (VagoneteInputs::globalInclination > 0) {
					curve = direction->curve_left->GetComponent<ComponentCurve>();
				}
				else {
					curve = direction->curve_right->GetComponent<ComponentCurve>();
				}
			}
			actual_pos = 0.0F;
		}
	}
	else if (strcmp("VagoneteCover", col->game_object_attached->GetTag()) == 0) {
		for (auto item = players.begin(); item != players.end(); ++item) {
			if ((*item)->state != VagoneteInputs::State::COVER) 
			{
				vagonete_life -= 20;
				HUD->UpdateLifebar(vagonete_life, max_life);
			}
		}
	}
}

void VagoneteMove::FollowCurve()
{
	float3 currentPos = curve->curve.ValueAt(actual_pos);
	float3 nextPos = curve->curve.ValueAt(actual_pos + speed * Time::GetDT() * 5);

	//Pitch (slope)
	float3 railVector = (currentPos - nextPos).Normalized();
	Quat rot = Quat::LookAt(float3::unitX(), railVector, float3::unitY(), float3::unitY());

	//Inclination (normals + players)
	float3 inclinationVector = curve->curve.NormalAt(actual_pos).Normalized();
	Quat inclinationRot = Quat::RotateFromTo(float3::unitY(), inclinationVector);
	inclinationRot.Inverse();
	rot = rot * inclinationRot;

	rigid_body->SetRotation(rot * VagoneteInputs::playerRotation);
	rigid_body->SetPosition(currentPos + float3{ 0, VagoneteInputs::globalInclinationY, 0 });

	actual_pos += speed * Time::GetDT();
}

VagoneteInputs::VagoneteInputs(PlayerController::PlayerType type)
{
	globalState = State::IDLE;

	switch (type)
	{
	case PlayerController::PlayerType::GERALT: {
		controllerIndex = 1;
		keyboardInput.inclinationLeft = SDL_SCANCODE_A;
		keyboardInput.inclinationRight = SDL_SCANCODE_D;
		keyboardInput.cover = SDL_SCANCODE_S;
		keyboardInput.jump = SDL_SCANCODE_W;
		player = GameObject::FindWithName("Geralt_Prefab"); // TODO: Change this to only find in children of GameObject
		break; }
	case PlayerController::PlayerType::YENNEFER: {
		controllerIndex = 2;
		keyboardInput.inclinationLeft = SDL_SCANCODE_LEFT;
		keyboardInput.inclinationRight = SDL_SCANCODE_RIGHT;
		keyboardInput.cover = SDL_SCANCODE_DOWN;
		keyboardInput.jump = SDL_SCANCODE_UP;
		player = GameObject::FindWithName("Yenn_Ready");
		break; }
	default: {
		break; }
	}

	state = State::IDLE;
}

void VagoneteInputs::Update()
{
	UpdateInputs();
	DoAction();
}

void VagoneteInputs::UpdateInputs()
{
	switch (globalState) {
	case VagoneteInputs::State::IDLE: {
		bool rightInclinationInput = Input::GetKeyRepeat(keyboardInput.inclinationRight) || Input::GetControllerJoystickLeft(controllerIndex, Input::JOYSTICK_RIGHT) == Input::KEY_REPEAT;;
		bool leftInclinationInput = Input::GetKeyRepeat(keyboardInput.inclinationLeft) || Input::GetControllerJoystickLeft(controllerIndex, Input::JOYSTICK_LEFT) == Input::KEY_REPEAT;
		bool jumpInput = Input::GetKeyRepeat(keyboardInput.jump) || Input::GetControllerButtonDown(controllerIndex, Input::CONTROLLER_BUTTON_A);
		bool coverInput = Input::GetKeyRepeat(keyboardInput.cover) || Input::GetControllerJoystickLeft(controllerIndex,Input::JOYSTICK_DOWN) == Input::KEY_REPEAT;

		if (state == State::COVER) {
			if (!coverInput) {
				state = State::IDLE;
				player->transform->SetLocalPosition(player->transform->GetLocalPosition().x, 0.5f, player->transform->GetLocalPosition().z);
			}
		}
		else {
			if (rightInclinationInput || leftInclinationInput) {
				inclinationZone = (rightInclinationInput) ? -1 : 1;
				state = State::INCLINATION;
				globalState = State::INCLINATION;
			}
			else if (jumpInput) {
				state = State::JUMP;
				globalState = State::JUMP;
			}
			else if (coverInput) {
				state = State::COVER;
			}
			else {
				state = State::IDLE;
			}
		}

		break; }
	case VagoneteInputs::State::JUMPING: {

		break; }
	case VagoneteInputs::State::INCLINATION: {
		bool rightInclinationInput = Input::GetKeyRepeat(keyboardInput.inclinationRight) || Input::GetControllerJoystickLeft(controllerIndex, Input::JOYSTICK_RIGHT) == Input::KEY_REPEAT;;
		bool leftInclinationInput = Input::GetKeyRepeat(keyboardInput.inclinationLeft) || Input::GetControllerJoystickLeft(controllerIndex, Input::JOYSTICK_LEFT) == Input::KEY_REPEAT;
		bool coverInput = Input::GetKeyRepeat(keyboardInput.cover) || Input::GetControllerJoystickLeft(controllerIndex, Input::JOYSTICK_DOWN) == Input::KEY_REPEAT;

		if (state == State::INCLINATION) {
			if (currentInclination != 0 || (rightInclinationInput || leftInclinationInput)) {
				if (rightInclinationInput || leftInclinationInput) {
					inclinationZone = (rightInclinationInput) ? -1 : 1;
				}
				else {
					inclinationZone = 0;
				}
			}
			else {
				if (globalInclination == 0) {
					globalState = State::IDLE;
				}
				state = State::IDLE;
			}
		}
		else if (state == State::COVER) {
			if (!coverInput) {
				player->transform->SetLocalPosition(player->transform->GetLocalPosition().x, 0.5f, player->transform->GetLocalPosition().z);
				state = State::IDLE;
			}
		}
		else {
			if (rightInclinationInput || leftInclinationInput) {
				state = State::INCLINATION;
				inclinationZone = (rightInclinationInput) ? -1 : 1;
			}
			else if (coverInput) {
				state = State::COVER;
				player->transform->SetLocalPosition(player->transform->GetLocalPosition().x, 0.3f, player->transform->GetLocalPosition().z);
			}
		}
		break; }
	}
}

void VagoneteInputs::DoAction()
{
	switch (state)
	{
	case VagoneteInputs::State::JUMP: {
		state = State::IDLE;
		break; }
	case VagoneteInputs::State::JUMPING: {
		state = State::IDLE;
		break; }
	case VagoneteInputs::State::INCLINATION: {
		Inclination();
		break; }
	case VagoneteInputs::State::COVER: {
		player->transform->SetLocalPosition(player->transform->GetLocalPosition().x, 0.3f, player->transform->GetLocalPosition().z);
		break; }
	}
}

void VagoneteInputs::Inclination()
{
	if (inclinationZone != 0) {
		currentInclination += speedInclination * Time::GetDT() * inclinationZone;
	}
	else {
		if (currentInclination < 0) {
			currentInclination += speedInclination * Time::GetDT();
			if (currentInclination > 0) {
				currentInclination = 0;
			}
		}
		else if (currentInclination > 0) {
			currentInclination -= speedInclination * Time::GetDT();
			if (currentInclination < 0) {
				currentInclination = 0;
			}
		}
	}
	currentYInclination = tan(currentInclination * Maths::Deg2Rad()) * 0.7F;
	if (currentInclination != 0) {
		globalInclinationY += currentYInclination;
		globalInclination += currentInclination;
		currentInclination = Maths::Clamp(currentInclination, -inclination4player, inclination4player);
		playerRotation = playerRotation * Quat::RotateX(currentInclination * Maths::Deg2Rad());
	}
}
