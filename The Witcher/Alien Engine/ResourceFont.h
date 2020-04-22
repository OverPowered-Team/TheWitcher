#pragma once

#include "Resource_.h"
#include "MathGeoLib/include/Math/float2.h"
#include <map>

typedef unsigned char uchar;
class ResourceShader;

struct FontImportSettings {
	std::string fontPath;
	std::vector<uint> sizes;
};

struct Character {
	uint    textureID;  // ID handle of the glyph texture
	float2	size;       // Size of glyph
	float2	bearing;    // Offset from baseline to left/top of glyph
	uint    advance;    // Offset to advance to next glyph
};

struct ResourceFontData {
	uint fontSize;
	uint maxCharHeight;
	std::map<char, Character> charactersMap;
	std::vector<uint8_t*> fontBuffer;
};

class ResourceFont : public Resource {

public:
	ResourceFont(ResourceFontData fontData);
	virtual ~ResourceFont();

	void CreateMeta();
	static ResourceFont* ImportFile(const char* file, u64 forced_id = 0);
	
	static ResourceFont* ImportFont(const char* file, u64 forced_id);
	static ResourceFont* LoadFile(const char* file, u64 forced_id);

private:
	static uint LoadTextureCharacter(uint width, uint height, uchar* buffer);
	static uint SaveFile(ResourceFontData& fontData, const char* exported_path, u64 ortho, u64 text);

	bool LoadMemory();
	void FreeMemory();
	static ResourceShader* SetShader(const char* path);
	//bool ReadBaseInfo(const char* assets_path);
	//void ReadLibrary(const char* meta_data);
	//bool DeleteMetaData();

public:
	FontImportSettings importSettings;
	ResourceFontData fontData;

	ResourceShader* text_shader = nullptr;
	ResourceShader* background_shader = nullptr;
};
