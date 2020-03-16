#include "Application.h"
#include "ResourceFont.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "ModuleImporter.h"
#include "FreeType/include/freetype/freetype.h"
#include "glew/include/glew.h"
#include "mmgr/mmgr.h"

ResourceFont::ResourceFont(ResourceFontData fontData):Resource(), fontData(fontData)
{
	type = ResourceType::RESOURCE_FONT;
}

ResourceFont::~ResourceFont()
{
	for (auto itBuff = fontData.fontBuffer.begin(); itBuff != fontData.fontBuffer.end(); itBuff++)
	{
		delete[](*itBuff);
	}

	fontData.fontBuffer.clear();

}

void ResourceFont::CreateMeta()
{
	JSON_Value* value = json_value_init_object();
	JSON_Object* json_object = json_value_get_object(value);

	std::string meta_path = std::string(App->file_system->GetPathWithoutExtension(path) + "_meta.alien");

	json_serialize_to_file_pretty(value, meta_path.data());

	if (value != nullptr && json_object != nullptr) {

		JSONfilepack* file = new JSONfilepack(meta_path, json_object, value);
		file->StartSave();
		file->SetString("Meta.ID", std::to_string(ID));
		file->SetString("Meta.name", name);
		file->FinishSave();
		delete file;
	}
}

ResourceFont* ResourceFont::ImportFile(const char* file, u64 forced_id)
{
	u64 new_id = 0;
	ResourceFont* font = nullptr;

	if (forced_id == 0)
	{
		new_id = App->resources->GetRandomID();
		font = ImportFont(file, new_id);
	}
	else
	{
		new_id = forced_id;
		std::string path(LIBRARY_FONTS_FOLDER + std::to_string(new_id) + ".fnt");

		if (App->file_system->Exists(path.c_str())) {
			LOG_ENGINE("Own file founded!", std::to_string(new_id));
			font = LoadFile(path.c_str(), new_id);
			font->path = file;
			font->name = App->file_system->GetBaseFileName(file);
		}
		else
		{
			LOG_ENGINE("Own file NOT founded! Reloading font with same ID: %s", std::to_string(new_id));
			font = ImportFont(file, new_id);
			font->path = file;
			font->name = App->file_system->GetBaseFileName(file);
		}
	}

	App->resources->AddResource(font);

	return font;
}

ResourceFont* ResourceFont::ImportFont(const char* file, u64 forced_id)
{
	ResourceFont* font = nullptr;
	uint maxHeight = 0;
	std::map<char, Character> charactersBitmap;

	FT_Face face;
	if (FT_New_Face(App->importer->library, file, 0, &face))
	{
		LOG_ENGINE("The font file can't be opened, read or unsopported format!");
	}
	else
	{
		ResourceFontData fontData;
		FT_Set_Pixel_Sizes(face, 0, 350);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		for (uint c = 32; c < 128; c++) {
			//Load character gylph
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				LOG_ENGINE("Failed Glyph from freetype the character %c", c);
				continue;
			}
			uint texture = LoadTextureCharacter(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);

			// Now store character in the char map
			Character character = {
				texture,
				float2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				float2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x / 64
			};
			charactersBitmap.insert(std::pair<char, Character>(c, character));
			if (face->glyph->bitmap.rows > maxHeight)
				maxHeight = face->glyph->bitmap.rows;

			//Save buffer for next loads
			uint bufferSize = face->glyph->bitmap.width * face->glyph->bitmap.rows;
			uint8_t* characterBuffer = new uint8_t[bufferSize];
			uint bytes = sizeof(uint8_t) * bufferSize;
			memcpy(characterBuffer, (uint8_t*)face->glyph->bitmap.buffer, bytes);

			fontData.fontBuffer.push_back(characterBuffer);

		}
		glBindTexture(GL_TEXTURE_2D, 0);
		FT_Done_Face(face);

		fontData.charactersMap = charactersBitmap;
		fontData.fontSize = 350;
		fontData.maxCharHeight = maxHeight;

		font = new ResourceFont(fontData);
		font->ID = forced_id;
		font->path = file;
		font->name = App->file_system->GetBaseFileName(file);
		font->meta_data_path = LIBRARY_FONTS_FOLDER + std::to_string(font->GetID()) + ".fnt";

		ResourceFont::SaveFile(fontData, font->meta_data_path.c_str());
		font->CreateMeta();
	}

	return font;
}

ResourceFont* ResourceFont::LoadFile(const char* file, u64 forced_id)
{
	ResourceFont* res = nullptr;

	char* buffer;
	uint size = App->file_system->Load(file, &buffer);
	if (size > 0) {
		//Get UID

		ResourceFontData fontData;
		char* cursor = (char*)buffer;

		uint bytes = sizeof(uint);
		memcpy(&fontData.fontSize, cursor, bytes);
		cursor += bytes;

		memcpy(&fontData.maxCharHeight, cursor, bytes);
		cursor += bytes;

		uint charactersSize = 0;
		bytes = sizeof(uint);
		memcpy(&charactersSize, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(Character);
		for (uint i = 0; i < charactersSize; ++i)
		{
			Character character;
			memcpy(&character, cursor, bytes);
			fontData.charactersMap.insert(std::pair<char, Character>(i + 32, character));

			cursor += bytes;
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		fontData.fontBuffer.resize(charactersSize);
		for (uint i = 0; i < charactersSize; ++i)
		{
 			uint width = fontData.charactersMap[i + 32].size.x;
			uint height = fontData.charactersMap[i + 32].size.y;

			bytes = sizeof(uint8_t) * width * height;
			uint8_t* buffer = new uint8_t[width * height];
			memcpy(buffer, cursor, bytes);
			fontData.fontBuffer[i] = buffer;
			fontData.charactersMap[i + 32].textureID = ResourceFont::LoadTextureCharacter(width, height, fontData.fontBuffer[i]);

			cursor += bytes;
			
		}

		res = new ResourceFont(fontData);
		res->ID = forced_id;
		res->meta_data_path = LIBRARY_FONTS_FOLDER + std::to_string(res->GetID()) + ".fnt";

		RELEASE_ARRAY(buffer);
	
	}

	return res;
}

uint ResourceFont::LoadTextureCharacter(uint width, uint height, uchar* buffer)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, buffer);

	// Set texture options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	return texture;
}

uint ResourceFont::SaveFile(ResourceFontData& fontData, const char* exported_path)
{
	uint sizeBuffer = 0;
	for (uint i = 0; i < fontData.charactersMap.size(); ++i)
	{
		sizeBuffer += (fontData.charactersMap[i + 32].size.x * fontData.charactersMap[i + 32].size.y);
	}

	uint size = sizeof(uint) * 3 + sizeBuffer +
		sizeof(Character) * fontData.charactersMap.size();

	char* buffer = new char[size];
	char* cursor = buffer;

	uint bytes = sizeof(uint);
	memcpy(cursor, &fontData.fontSize, bytes);
	cursor += bytes;

	memcpy(cursor, &fontData.maxCharHeight, bytes);
	cursor += bytes;

	uint listSize = fontData.charactersMap.size();
	memcpy(cursor, &listSize, bytes);
	cursor += bytes;

	bytes = sizeof(Character);
	for (std::map<char, Character>::iterator it = fontData.charactersMap.begin(); it != fontData.charactersMap.end(); ++it)
	{
		memcpy(cursor, &(*it).second, bytes);
		cursor += bytes;
	}

	for (uint i = 0; i < listSize; ++i)
	{
		bytes = (fontData.charactersMap[i + 32].size.x * fontData.charactersMap[i + 32].size.y);
		memcpy(cursor, fontData.fontBuffer[i], bytes);
		cursor += bytes;
	}
	// --------------------------------------------------

	// Save the file
	uint ret = App->file_system->Save(exported_path, buffer, size);

	if (ret > 0)
	{
		LOG_ENGINE("Resource Font: Successfully saved Font '%s'", exported_path);
	}
	else
		LOG_ENGINE("Resource Font: Could not save Font '%s'", exported_path);

	RELEASE_ARRAY(buffer);

	return ret;
}
