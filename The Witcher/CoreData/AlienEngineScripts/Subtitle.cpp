#include "Subtitle.h"

Subtitle::Subtitle() : Alien()
{
}

Subtitle::~Subtitle()
{
}

void Subtitle::Start()
{
	text = game_object->GetComponent<ComponentText>();
	if (!text)
	{
		LOG("Text not found");
		return;
	}
		
	audio = GetComponent<ComponentAudioEmitter>();
	if (!audio)
	{
		LOG("Audio component not found");
		return;
	}
		
	if (songBard)
	{
		song = songBard->GetComponent<ComponentAudioEmitter>();
	}


	std::string json_path = std::string("GameData/Intro.json");
	JSONfilepack* jsonDoc = JSONfilepack::GetJSON(json_path.c_str());
	if (jsonDoc)
	{
		LOG("READING Intro named %s", json_path.data());
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
	else
	{
		LOG("Can't read this json");
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