#pragma once

#include "..\..\Alien Engine\Alien.h"
#include "Macros/AlienScripts.h"

class ALIEN_ENGINE_API SoundMaterial : public Alien {

public:
	enum (SoundMaterialType,
		MUD,
		SAND,
		STONE,
		VEGETATION,
		WOOD
		);

	SoundMaterial();
	virtual ~SoundMaterial();

	std::string GetMaterialName();

public:
	SoundMaterialType material = SoundMaterialType::MUD;
};

ALIEN_FACTORY SoundMaterial* CreateSoundMaterial() {
	SoundMaterial* alien = new SoundMaterial();
	// To show in inspector here

	SHOW_IN_INSPECTOR_AS_ENUM(SoundMaterial::SoundMaterialType, alien->material);

	return alien;
}
