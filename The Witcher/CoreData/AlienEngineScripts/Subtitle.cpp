#include "Subtitle.h"

Subtitle::Subtitle() : Alien()
{
}

Subtitle::~Subtitle()
{
}

void Subtitle::Start()
{
	text = (ComponentText*)GetComponent(ComponentType::UI_TEXT);
	if (!text)
		return;
	audio = (ComponentAudioEmitter*)GetComponent(ComponentType::A_EMITTER);
	if (!audio)
		return;
	if (songBard)
	{
		song = (ComponentAudioEmitter*)songBard->GetComponent(ComponentType::A_EMITTER);
	}


	std::string json_path = std::string("Configuration/Subtitles/Project_1.json");
	JSONfilepack* jsonDoc = JSONfilepack::GetJSON(json_path.c_str());
	if (jsonDoc)
	{
		LOG("READING ENEMY STAT GAME JSON WITH NAME %s", json_path.data());
		JSONArraypack* titles = jsonDoc->GetArray("titles");
		if (titles == nullptr)
			return;
		do 
		{
			subtitle new_subtitle;
			new_subtitle.start = titles->GetNumber("s")/1000;
			new_subtitle.end = titles->GetNumber("e")/1000;
			new_subtitle.text = titles->GetString("t");
			subtitles.push_back(new_subtitle);
			
		} while (titles->GetAnotherNode());
	}
	
	JSONfilepack::FreeJSON(jsonDoc);
	start_time = Time::GetGameTime();
}

void Subtitle::Update()
{
	if (!text || !audio)
		return;
	
	current_time = Time::GetGameTime() - start_time;
	if (subtitles.size() > 0 && subtitles.size() > current_sub)
	{
		if (current_time >= subtitles[current_sub].start)
		{
			if (first_entered)
			{
				text->SetText(subtitles[current_sub].text.c_str());
				first_entered = false;
			}
			else if (current_time > subtitles[current_sub].end && !first_entered)
			{
				text->SetText("");
				first_entered = true;
				++current_sub;
			}
		}
	}
	if (current_time > end_seconds || Input::GetControllerButton(1,Input::CONTROLLER_BUTTON_START) || Input::GetKeyDown(SDL_SCANCODE_A))
	{
		LOG("ENTERED");
		if(audio)
			audio->Mute(true);
		if(song)
			song->Mute(true);
		SceneManager::LoadScene("newTRIGGER");
	}
}