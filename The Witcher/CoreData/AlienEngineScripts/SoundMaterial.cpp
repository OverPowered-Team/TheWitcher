#include "SoundMaterial.h"

SoundMaterial::SoundMaterial()
{
}

SoundMaterial::~SoundMaterial()
{
}

std::string SoundMaterial::GetMaterialName()
{
	std::string name = "";
	switch (material)
	{
	case SoundMaterialType::MUD:
		return name = "Mud";
		break;
	case SoundMaterialType::SAND:
		return name = "Sand";
		break;
	case SoundMaterialType::STONE:
		return name = "Stone";
		break;
	case SoundMaterialType::VEGETATION:
		return name = "Vegetation";
		break;
	case SoundMaterialType::WOOD:
		return name = "Wood";
		break;
	}

	return name;
}
