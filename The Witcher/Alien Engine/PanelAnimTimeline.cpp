#include "Application.h"
#include "ModuleObjects.h"
#include "ComponentAnimator.h"
#include "Time.h"
#include "ComponentTransform.h"
#include "PanelAnimTimeline.h"
#include "ResourceScript.h"
#include "ComponentScript.h"
#include "ResourceAudio.h"
#include "ComponentAudioEmitter.h"
#include "ModuleAudio.h"
#include "ShortCutManager.h"

#include "mmgr/mmgr.h"
#include "Optick/include/optick.h"

PanelAnimTimeline::PanelAnimTimeline(const std::string& panel_name, const SDL_Scancode& key1_down, const SDL_Scancode& key2_repeat, const SDL_Scancode& key3_repeat_extra)
	: Panel(panel_name, key1_down, key2_repeat, key3_repeat_extra)
{
	shortcut = App->shortcut_manager->AddShortCut("Animation Timeline", key1_down, std::bind(&Panel::ChangeEnable, this), key2_repeat, key3_repeat_extra);
}

PanelAnimTimeline::~PanelAnimTimeline()
{
}

void PanelAnimTimeline::CleanUp()
{
	animations.clear();
	current_animation = nullptr;
	component_animator = nullptr;
	animator = nullptr;
	channel = nullptr;
}

bool PanelAnimTimeline::FillInfo()
{
	OPTICK_EVENT();
	bool ret = false;

	CleanUp();

	if (App->objects->GetSelectedObjects().size() > 0)
	{

		std::list<GameObject*> selected = App->objects->GetSelectedObjects();
		auto go = selected.begin();

		for (; go != selected.end(); ++go)
		{
			if (*go != nullptr && (*go)->GetComponent(ComponentType::ANIMATOR))
			{
				component_animator = (ComponentAnimator*)(*go)->GetComponent(ComponentType::ANIMATOR);

				if (component_animator->GetResourceAnimatorController() != nullptr)
				{
					animator = component_animator->GetResourceAnimatorController();
					animator->AddAnimGameObject((*go));

					for (uint i = 0; i < animator->GetNumStates(); ++i)
					{
						if (animator->GetStates()[i]->GetClip())
						{
							animations.push_back(animator->GetStates()[i]->GetClip());
							ret = true;
							changed = false;
						}
					}
				}
			}
		}
	}
	if (ret)
	{
		current_animation = animations[0];
		channel = &current_animation->channels[0];
		num_frames = current_animation->end_tick - current_animation->start_tick;
	}
	return ret;
}

void PanelAnimTimeline::Play()
{
	if (!pause)	aux_time = Time::GetTimeSinceStart();
	else aux_time = Time::GetTimeSinceStart() - animation_time;
	play = true;
	pause = false;
	button_position = 0.0f;
}

void PanelAnimTimeline::Stop()
{
	play = false;
	animation_time = 0.0f;
	pause = false;
	stop = true;
	button_position = 0.0f;
}

void PanelAnimTimeline::OnObjectSelect()
{
	changed = true;
}

void PanelAnimTimeline::OnObjectDelete()
{
	changed = true;
}

void PanelAnimTimeline::MoveBones(GameObject* go)
{
	OPTICK_EVENT();
	if (go)
	{
		uint channel_index = current_animation->GetChannelIndex(go->GetName());
		if (channel_index < current_animation->num_channels)
		{
			// Position
			if (current_animation->channels[channel_index].position_keys[key].time == key)
				go->transform->SetLocalPosition(current_animation->channels[channel_index].position_keys[key].value);

			// Rotation
			if (current_animation->channels[channel_index].rotation_keys[key].time == key)
				go->transform->SetLocalRotation(current_animation->channels[channel_index].rotation_keys[key].value);

			// Scale
			if (current_animation->channels[channel_index].scale_keys[key].time == key)
				go->transform->SetLocalScale(current_animation->channels[channel_index].scale_keys[key].value);
		}

		for (int i = 0; i < go->GetChildren().size(); i++)
		{
			MoveBones(go->GetChildren()[i]);
		}
	}
}

void PanelAnimTimeline::PanelLogic()
{
	OPTICK_EVENT();
	ImGuiWindowFlags aboutFlags = 0;
	aboutFlags |= ImGuiWindowFlags_HorizontalScrollbar;
	ImGui::Begin("Animation Timeline", &enabled, aboutFlags);

	if ((current_animation && !current_animation->channels) || !animator || !component_animator)
		changed = true;

	if (changed)
	{
		FillInfo();
		ImGui::Text("ANIMATION NOT SELECTED");
	}
	else
	{
		
		// Motor Buttons Play, Pause, Stop
		if (Time::IsPlaying() && !in_game)
		{
			Play();
			animator = component_animator->GetCurrentAnimatorController();
			in_game = true;
		}
		else if (Time::IsPaused())
		{
			pause = true;
			in_game = false;
			aux_time = animation_time;
		}
		else if (!Time::IsInGameState() && in_game)
		{
			Stop();
			in_game = false;
		}
		else
		{
			// My Buttons Play, Pause, Stop
			if (ImGui::Button("Play"))
			{
				if (!play)
					Play();
			}
			ImGui::SameLine();
			if (ImGui::Button("Pause") && progress != 0.0f)
			{
				pause = !pause;
				play = !play;
				button_position = progress;
				if (!pause)	aux_time = Time::GetTimeSinceStart() - animation_time;
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop"))
			{
				Stop();
			}

			ImGui::SameLine();

			//Check Events 
			// Audio
			if (!animator->GetEmitter() && component_animator->game_object_attached->GetComponent(ComponentType::A_EMITTER))
				animator->SetEmitter((ComponentAudioEmitter*)component_animator->game_object_attached->GetComponent(ComponentType::A_EMITTER));
			else if (!component_animator->game_object_attached->GetComponent(ComponentType::A_EMITTER))
				animator->SetEmitter(nullptr);
			// Scripts
			if (animator->GetScripts().size() < 1 && component_animator->game_object_attached->GetComponent(ComponentType::SCRIPT))
				animator->SetScripts(component_animator->game_object_attached->GetComponents<ComponentScript>());
			else if (!component_animator->game_object_attached->GetComponent(ComponentType::SCRIPT))
				animator->GetScripts().clear();
		}

		//Animation bar Progress
		ImGui::SetCursorPosX(165);
		ImGui::ProgressBar(animation_time / current_animation->GetDuration(), { windows_size,0 });


		//Choose Animations
		ImGui::BeginGroup();

		ImGui::BeginChild("All Animations", ImVec2(150, 140), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);


		for (int i = 0; i < animations.size(); i++)
		{
			if (ImGui::Button(animations[i]->name.c_str()))
			{
				current_animation = animations[i];
				num_frames = current_animation->end_tick - current_animation->start_tick;
			}
		}

		ImGui::EndChild();

		ImGui::BeginChild("Selected Animation", ImVec2(150, 30), true);

		if (current_animation != nullptr) ImGui::Text(current_animation->name.c_str());

		ImGui::EndChild();
		ImGui::EndGroup();

		ImGui::SameLine();

		//ALL TIMELINE
		ImGui::BeginChild("Timeline", ImVec2(windows_size, 170), true);

		//Animation types of Keys
		ImGui::BeginGroup();
		ImGui::SetCursorPosY(50);

		ImGui::Text("Position");

		ImGui::SetCursorPosY(85);

		ImGui::Text("Rotation");

		ImGui::SetCursorPosY(120);

		ImGui::Text("Scale");

		ImGui::EndGroup();

		ImGui::SameLine();

		//Animation TimeLine
		ImGui::BeginChild("TimeLine", ImVec2(windows_size - 80, 150), true, ImGuiWindowFlags_HorizontalScrollbar);
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImVec2 redbar = ImGui::GetCursorScreenPos();
		ImGui::InvisibleButton("scrollbar", { num_frames * zoom + zoom,1 });
		ImGui::SetCursorScreenPos(p);

		for (int i = 0; i < animator->GetNumAnimEvents(); i++)
		{
			if (animator->GetAnimEvents()[i]->animation_id == current_animation->GetID())
			{
				ImGui::BeginGroup();

				ImGui::GetWindowDrawList()->AddTriangleFilled(ImVec2((p.x + (animator->GetAnimEvents()[i]->frame * zoom)), p.y),
					ImVec2((p.x + (animator->GetAnimEvents()[i]->frame * zoom)) - 5, p.y + 5),
					ImVec2((p.x + (animator->GetAnimEvents()[i]->frame * zoom)) + 5, p.y + 5),
					ImColor(1.0f, 0.0f, 0.0f, 0.5f));

				ImGui::EndGroup();
				ImGui::SameLine();
			}
		}

		for (int i = 0; i <= num_frames; i++)
		{
			ImGui::BeginGroup();

			ImGui::GetWindowDrawList()->AddLine({ p.x,p.y + 20 }, ImVec2(p.x, p.y + 135), IM_COL32(100, 100, 100, 255), 1.0f);
			char frame[8];
			sprintf(frame, "%01d", i);
			ImVec2 aux = { p.x - 4,p.y + 5};
			ImGui::GetWindowDrawList()->AddText(aux, ImColor(1.0f, 1.0f, 1.0f, 1.0f), frame);

			if (current_animation != nullptr && channel != nullptr)
			{
				if (channel->position_keys[i].time == i)
					ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 1, p.y + 45), 6.0f, ImColor(1.0f, 0.5f, 0.0f, 0.5f));

				if (channel->rotation_keys[i].time == i)
					ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 1, p.y + 80), 6.0f, ImColor(0.0f, 1.0f, 0.0f, 0.5f));

				if (channel->scale_keys[i].time == i)
					ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 1, p.y + 115), 6.0f, ImColor(0.0f, 0.0f, 1.0f, 0.5f));
			}

			p = { p.x + zoom,p.y };

			ImGui::EndGroup();

			ImGui::SameLine();
		}

		//RedLine 
		if (play && !in_game)
		{
			ImGui::GetWindowDrawList()->AddLine({ redbar.x + progress,redbar.y - 10 }, ImVec2(redbar.x + progress, redbar.y + 135), IM_COL32(255, 0, 0, 255), 1.0f);

			if (!pause)
			{
				animation_time = Time::GetTimeSinceStart() - aux_time;
				progress = (animation_time * current_animation->ticks_per_second) * zoom;
			}

			if (progress != 0 && progress > windows_size - margin + ImGui::GetScrollX())
				ImGui::SetScrollX(10 + ImGui::GetScrollX());

			if (animation_time > current_animation->GetDuration())
			{
				if (current_animation->loops)
				{
					progress = 0.0f;
					ImGui::SetScrollX(0);
					aux_time = Time::GetTimeSinceStart();
				}
				else
					Stop();
			}

			// Execute Event
			if (animator->GetNumAnimEvents() > 0)
			{
				auto aux = animator->GetAnimEvents();
				for (auto it = aux.begin(); it != aux.end(); ++it)
				{
					if ((*it)->frame == key && (*it)->animation_id == current_animation->GetID())
					{
						// Audio
						if ((*it)->type == EventAnimType::EVENT_AUDIO && animator->GetEmitter() != nullptr)
						{
							animator->GetEmitter()->StartSound(std::stoull((*it)->event_id.c_str()));
						}
						
						// Script
						if ((*it)->type == EventAnimType::EVENT_SCRIPT && animator->GetScripts().size() > 0)
						{
							auto scripts = animator->GetScripts();
							for (auto item = scripts.begin(); item != scripts.end(); ++item)
							{
								if (*item != nullptr && (*item)->data_ptr != nullptr && !(*item)->functionMap.empty())
								{
									for (auto j = (*item)->functionMap.begin(); j != (*item)->functionMap.end(); ++j) {
										if (strcmp((*j).first.data(),(*it)->event_id.c_str()) == 0)
										{
											std::function<void()> functEvent = (*j).second;
											functEvent();
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
			if (stop)
			{
				progress = 0.0f;
				ImGui::SetScrollX(0);
				stop = false;
			}

			ImGui::GetWindowDrawList()->AddLine({ redbar.x + progress,redbar.y - 10 }, ImVec2(redbar.x + progress, redbar.y + 135), IM_COL32(255, 0, 0, 255), 1.0f);

			ImGui::SetCursorPos({ button_position,ImGui::GetCursorPosY() + 5});
			ImGui::PushID("scrollButton");
			ImGui::Button("", { 20, 15 });
			ImGui::PopID();

			if (ImGui::IsItemClicked(0) && dragging == false)
			{
				dragging = true;
			}

			if (dragging && ImGui::IsMouseDown(0))
			{
				button_position = ImGui::GetMousePos().x - ImGui::GetWindowPos().x + ImGui::GetScrollX();
				if (button_position < 0)
					button_position = 0;
				if (button_position > num_frames* zoom)
					button_position = num_frames * zoom;

				if (button_position > windows_size - margin + ImGui::GetScrollX())
					ImGui::SetScrollX(10 + ImGui::GetScrollX());
				else if (button_position < ImGui::GetScrollX() + margin)
					ImGui::SetScrollX(ImGui::GetScrollX() - 10);

				progress = button_position;
				animation_time = progress / (current_animation->ticks_per_second * zoom);

			}
			else
			{
				dragging = false;
			}

			ImGui::GetWindowDrawList()->AddLine({ redbar.x + progress,redbar.y - 10 }, ImVec2(redbar.x + progress, redbar.y + 165), IM_COL32(255, 0, 0, 255), 1.0f);

			//Events--
			ShowNewEventPopUp();
		}

		ImGui::EndChild();
		ImGui::EndChild();

		ImGui::SameLine();
		 // Bones
		ImGui::BeginGroup();

		ImGui::BeginChild("All Bones", ImVec2(150, 140), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

		if (current_animation != nullptr)
		{
			for (int i = 0; i < current_animation->num_channels; i++)
			{
				if (ImGui::Button(current_animation->channels[i].name.c_str()))
					channel = &current_animation->channels[i];
			}
		}

		ImGui::EndChild();

		ImGui::BeginChild("Selected Bone", ImVec2(150, 30), true);

		if (channel != nullptr)	ImGui::Text(channel->name.c_str());

		ImGui::EndChild();
		ImGui::EndGroup();

		ImGui::NewLine();

		// Move Bones
		if (!in_game && progress > 0)
		{
			key = (int)progress / zoom;
			MoveBones(component_animator->game_object_attached);

			if (key != previous_key)
			{
				previous_key = key;
			}
			else if(play)
				key = 0;
		}
	}

	ImGui::End();
}


// EVENTS-----------------------------------------------------------------------
void PanelAnimTimeline::ShowNewEventPopUp()
{
	if (ImGui::BeginPopupContextItem("")) {

		if (animator->GetAnimEvents().size() > 0)
		{
			auto aux = animator->GetAnimEvents();
			for (auto it = aux.begin(); it != aux.end(); ++it)
			{
				if ((*it)->frame == key && (*it)->animation_id == current_animation->GetID())
				{
					event_created = true;
					// Audio
					if ((*it)->type == EventAnimType::EVENT_AUDIO)
					{
						ImGui::Text(App->audio->GetEventNameByID(std::stoull((*it)->event_id.c_str())));
						event_audio_created = true;
					}
					ImGui::Separator();

					// Script
					if ((*it)->type == EventAnimType::EVENT_SCRIPT)
					{
						ImGui::Text((*it)->event_id.c_str());
						event_script_created = true;
					}
					ImGui::Separator();
				}
			}
		}
		else
			event_created = false;

		if (event_created)
		{
			event_created = false;

			ImGui::Separator();

			if (ImGui::BeginMenu("Delete Animation Event"))
			{
				// Audio
				if (event_audio_created && ImGui::BeginMenu("AUDIO EVENT"))
				{
					auto aux = animator->GetAnimEvents();
					for (auto it = aux.begin(); it != aux.end(); ++it)
					{
						if ((*it)->frame == key && (*it)->animation_id == current_animation->GetID() && (*it)->type == EventAnimType::EVENT_AUDIO)
						{
							if (ImGui::MenuItem(App->audio->GetEventNameByID(std::stoull((*it)->event_id.c_str()))))
							{
								animator->RemoveAnimEvent((*it));
								break;
							}
						}
					}
					ImGui::EndMenu();
				}
				
				if (event_particle_created && ImGui::MenuItem("PARTICLE EVENT"))
				{
				}

				// Script
				if (event_script_created && ImGui::BeginMenu("SCRIPT EVENT"))
				{
					auto aux = animator->GetAnimEvents();
					for (auto it = aux.begin(); it != aux.end(); ++it)
					{
						if ((*it)->frame == key && (*it)->animation_id == current_animation->GetID() && (*it)->type == EventAnimType::EVENT_SCRIPT)
						{
							if (ImGui::MenuItem((*it)->event_id.c_str()))
							{
								animator->RemoveAnimEvent((*it));
								break;
							}
						}
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
		}
		
		ShowOptionsToCreate();

		ImGui::EndPopup();
	}
}

void PanelAnimTimeline::ShowOptionsToCreate()
{
	if (ImGui::BeginMenu("Create Animation Event"))
	{
		// Audio
		if (!event_audio_created)
		{
			if (component_animator->game_object_attached->GetComponent(ComponentType::A_EMITTER))
			{
				if (ImGui::BeginMenu("Audio List"))
				{
					//For with banks
					animator->SetEmitter((ComponentAudioEmitter*)component_animator->game_object_attached->GetComponent(ComponentType::A_EMITTER));
					auto banks = App->audio->GetBanks();
					if (banks.size() > 0)
					{
						for (auto bk = banks.begin(); bk != banks.end(); ++bk)
						{
							if (ImGui::BeginMenu((*bk)->name.c_str()))
							{
								//For with audios
								for (auto j = (*bk)->events.begin(); j != (*bk)->events.end(); ++j)
								{
									if (ImGui::MenuItem((*j).second.c_str()))
									{
										animator->AddAnimEvent(new AnimEvent(std::to_string((*j).first), current_animation->GetID(), key, EventAnimType::EVENT_AUDIO));
									}
								}
								ImGui::EndMenu();
							}
						}
					}
					ImGui::EndMenu();
				}
			}
			else
				ImGui::Text("No Component Audio Emitter Created");

			ImGui::Separator();
		}


		// Particles
		if (component_animator->game_object_attached->GetComponent(ComponentType::PARTICLES))
		{
			// TODO WITH PARTICLES
		}
		else
			ImGui::Text("No Component Particle Created");

		ImGui::Separator();


		// Scripts
		if (!event_script_created)
		{
			if (component_animator->game_object_attached->GetComponent(ComponentType::SCRIPT))
			{
				if (ImGui::BeginMenu("Scripts List"))
				{
					animator->SetScripts(component_animator->game_object_attached->GetComponents<ComponentScript>());
					auto scripts = animator->GetScripts();
					for (auto item = scripts.begin(); item != scripts.end(); ++item)
					{
						if (*item != nullptr && (*item)->data_ptr != nullptr)
						{
							if (ImGui::BeginMenu((*item)->data_name.data())) 
							{
								if (!(*item)->functionMap.empty()) 
								{
									for (auto j = (*item)->functionMap.begin(); j != (*item)->functionMap.end(); ++j) {
										if (ImGui::MenuItem((*j).first.data()))
										{
											animator->AddAnimEvent(new AnimEvent((*j).first, current_animation->GetID(), key, EventAnimType::EVENT_SCRIPT));
										}
									}
								}
								else 
									ImGui::Text("No exported functions");

								ImGui::EndMenu();
							}
						}
					}
					ImGui::EndMenu();
				}
			}
			else
				ImGui::Text("No Component Script Created");

			ImGui::Separator();
		}
		ImGui::EndMenu();
	}

	event_audio_created = false;
	event_script_created = false;
}
