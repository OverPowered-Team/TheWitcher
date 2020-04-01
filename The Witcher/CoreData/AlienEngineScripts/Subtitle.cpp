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

	std::string json_path = std::string("Configuration/Subtitles/Project_1.json");
	LOG("READING ENEMY STAT GAME JSON WITH NAME %s", json_path.data());
	JSONfilepack* jsonDoc = JSONfilepack::GetJSON(json_path.c_str());
	if (jsonDoc)
	{
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
}

void Subtitle::Update()
{
	if (!text)
		return;
	current_time += Time::GetDT();
	if (subtitles.size() > 0 && subtitles.size() > current_sub)
	{
		if (current_time > subtitles[current_sub].start && current_time < subtitles[current_sub].end)
		{
			text->SetText(subtitles[current_sub].text.c_str());
		}
		else if (current_time > subtitles[current_sub].end)
		{
			++current_sub;
			text->SetText("");
		}
	}
}