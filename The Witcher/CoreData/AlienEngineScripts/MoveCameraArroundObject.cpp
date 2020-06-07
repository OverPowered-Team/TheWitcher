#include "MoveCameraArroundObject.h"

MoveCameraArroundObject::MoveCameraArroundObject() : Alien()
{
}

MoveCameraArroundObject::~MoveCameraArroundObject()
{
}

void MoveCameraArroundObject::Start()
{
	start_pos = game_object->transform->GetGlobalPosition();

	if (path)
	{
		curve = path->GetComponent<ComponentCurve>();
		if (curve != nullptr)
		{
			LOG("Curve found!");
			camera = GetComponent<ComponentCamera>();
			if (camera)
			{
				start_curve = curve->curve.ValueAt(0);
				end_curve = curve->curve.ValueAt(1);
				start_front = camera->frustum.front;
				end_front = target->transform->GetGlobalPosition() - start_curve;
			}

		}

	}
}

void MoveCameraArroundObject::Update()
{

	if (!ended_intro && curve && camera)
	{
		float3 currentPos = target->transform->GetGlobalPosition();
		float3 front_vector = camera->frustum.front;

		if (start_transition)
		{

			current_time_transition += Time::GetDT();
			if (current_time_transition >= transition_duration)
			{
				start_transition = false;
				current_time_transition = transition_duration;
			}

			float percentatge_lerp = current_time_transition / transition_duration;
			currentPos = math::Lerp(start_pos, start_curve, percentatge_lerp);
			front_vector = math::Lerp(start_front, end_front, percentatge_lerp);
			//front_vector = target->transform->GetGlobalPosition() - currentPos;
			front_vector.Normalize();
		}
		else if (go_back)
		{
			current_time_transition += Time::GetDT();
			if (entered_go_back)
			{
				front_end = camera->frustum.front;
				entered_go_back = false;
			}
			if (current_time_transition >= transition_duration)
			{
				current_time_transition = transition_duration;
				go_back = false;
				ended_intro = true;
			}


			float percentatge_lerp = current_time_transition / transition_duration;
			currentPos = math::Lerp(end_curve, start_pos, percentatge_lerp);
			front_vector = math::Lerp(front_end, start_front, percentatge_lerp);
			front_vector.Normalize();
			LOG("%f", percentatge_lerp);
		}
		else
		{

			currentPos = curve->curve.ValueAt(current_pos);
			current_pos += speed * Time::GetDT();
			if (current_pos > 1)
			{
				go_back = true;
				current_time_transition = 0;

			}
			front_vector = target->transform->GetGlobalPosition() - currentPos;
			front_vector.Normalize();


		}
		camera->frustum.front = front_vector;
		game_object->transform->SetGlobalPosition(currentPos);

		camera->frustum.up = float3::unitY();

	}
}