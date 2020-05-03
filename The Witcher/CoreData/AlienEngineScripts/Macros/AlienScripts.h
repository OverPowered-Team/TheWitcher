#pragma once

#include "..\..\..\Alien Engine\GameObject.h"
/*-----------------COMPONENTS-------------------*/
#include "..\..\..\Alien Engine\ComponentScript.h"
#include "..\..\..\Alien Engine\ComponentTransform.h"
#include "..\..\..\Alien Engine\ComponentCamera.h"
#include "..\..\..\Alien Engine\ComponentLightDirectional.h"
#include "..\..\..\Alien Engine\ComponentLightSpot.h"
#include "..\..\..\Alien Engine\ComponentMaterial.h"
#include "..\..\..\Alien Engine\ComponentMesh.h"
#include "../../../Alien Engine/ComponentAudioEmitter.h"
#include "..\..\..\Alien Engine\ComponentParticleSystem.h"
#include "..\..\..\Alien Engine\ComponentAnimator.h"
#include "..\..\..\Alien Engine\ParticleSystem.h"
#include "..\..\..\Alien Engine\ParticleEmitter.h"
#include "..\..\..\Alien Engine\ComponentDeformableMesh.h"
// physix ----
#include "..\..\..\Alien Engine\ComponentRigidBody.h"
#include "..\..\..\Alien Engine\ComponentCharacterController.h"
#include "..\..\..\Alien Engine\ComponentCollider.h"
#include "..\..\..\Alien Engine\ComponentBoxCollider.h"
#include "..\..\..\Alien Engine\ComponentCapsuleCollider.h"
#include "..\..\..\Alien Engine\ComponentBasePhysic.h"
// -----------
#include "..\..\..\Alien Engine\ComponentSphereCollider.h"
#include "..\..\..\Alien Engine\ComponentLightDirectional.h"
#include "..\..\..\Alien Engine\ComponentLightPoint.h"
#include "..\..\..\Alien Engine\ComponentLightSpot.h"
#include "..\..\..\Alien Engine\ComponentUI.h"
#include "..\..\..\Alien Engine\ComponentButton.h"
#include "..\..\..\Alien Engine\ComponentText.h"
#include "..\..\..\Alien Engine\ComponentCurve.h"
#include "..\..\..\Alien Engine\ComponentImage.h"
/*-----------------COMPONENTS-------------------*/

/*-----------------MATH-------------------*/
#include "..\..\..\Alien Engine\MathGeoLib\include\Math\float3.h"
#include "..\..\..\Alien Engine\MathGeoLib\include\Math\Quat.h"
#include "..\..\..\Alien Engine\MathGeoLib\include\Math\float4x4.h"
/*-----------------MATH-------------------*/

/*-----------------HELPERS-------------------*/
#include "..\..\..\Alien Engine\StaticInput.h"
#include "..\..\..\Alien Engine\StaticTween.h"
#include "..\..\..\Alien Engine\Maths.h"
#include "..\..\..\Alien Engine\Physics.h"
#include "..\..\..\Alien Engine\Debug.h"
#include "..\..\..\Alien Engine\RandomHelper.h"
#include "..\..\..\Alien Engine\Time.h"
#include "..\..\..\Alien Engine\Physics.h"
#include "..\..\..\Alien Engine\Camera.h"
#include "..\..\..\Alien Engine\Gizmos.h"
#include "..\..\..\Alien Engine\Color.h"
#include "..\..\..\Alien Engine\Prefab.h"
#include "..\..\..\Alien Engine\SceneManager.h"
#include "..\..\..\Alien Engine\AlienEngine.h"
#include "..\..\..\Alien Engine\RayCreator.h"
#include "..\..\..\Alien Engine\Screen.h"
#include "..\\..\..\\Alien Engine\JSONfilepack.h"
#include "../../../Alien Engine/WwiseT.h"
/*-----------------HELPERS-------------------*/

#ifdef ALIENENGINESCRIPTS_EXPORTS
#define ALIEN_ENGINE_API __declspec(dllexport)
#else
#define ALIEN_ENGINE_API __declspec(dllimport)
#endif

// define it next to the CreateClass/StructFunct to be able to use the class/struct
#define ALIEN_FACTORY extern "C" ALIEN_ENGINE_API

ALIEN_FACTORY void ChangeString(std::string* pointer, const char* newString) {
	pointer->assign(newString);
}

// ------------INSPECTOR MACROS----------------\\
/*--------------------int--------------------*/
#define SHOW_IN_INSPECTOR_AS_INPUT_INT(INT_) ComponentScript::InspectorInputInt(&INT_, #INT_)
#define SHOW_IN_INSPECTOR_AS_DRAGABLE_INT(INT_) ComponentScript::InspectorDragableInt(&INT_, #INT_)
#define SHOW_IN_INSPECTOR_AS_SLIDER_INT(INT_, MIN_, MAX_) ComponentScript::InspectorSliderInt(&INT_, #INT_, MIN_, MAX_)
/*--------------------float--------------------*/
#define SHOW_IN_INSPECTOR_AS_INPUT_FLOAT(FLOAT_) ComponentScript::InspectorInputFloat(&FLOAT_, #FLOAT_)
#define SHOW_IN_INSPECTOR_AS_DRAGABLE_FLOAT(FLOAT_) ComponentScript::InspectorDragableFloat(&FLOAT_, #FLOAT_)
#define SHOW_IN_INSPECTOR_AS_SLIDER_FLOAT(FLOAT_, MIN_, MAX_) ComponentScript::InspectorSliderFloat(&FLOAT_, #FLOAT_, MIN_, MAX_)
/*--------------------bool--------------------*/
#define SHOW_IN_INSPECTOR_AS_CHECKBOX_BOOL(BOOL_) ComponentScript::InspectorBool(&BOOL_, #BOOL_)
/*--------------------string--------------------*/
#define SHOW_IN_INSPECTOR_AS_STRING(STRING) ComponentScript::InspectorString(&STRING, #STRING)
/*--------------------enum--------------------*/
#define SHOW_IN_INSPECTOR_AS_ENUM(ENUM_TYPE, ENUM_VALUE) ComponentScript::InspectorEnum((int*)(void*)&ENUM_VALUE, #ENUM_VALUE, ENUM_TYPE##EnumNames)
/*--------------------prefab--------------------*/
#define SHOW_IN_INSPECTOR_AS_PREFAB(PREFAB_) ComponentScript::InspectorPrefab(&PREFAB_, #PREFAB_)
/*--------------------gameobject--------------------*/
#define SHOW_IN_INSPECTOR_AS_GAMEOBJECT(GAMEOBJECT_) ComponentScript::InspectorGameObject(&GAMEOBJECT_, #GAMEOBJECT_)
/*--------------------function--------------------*/
#define SHOW_VOID_FUNCTION(FUNCTION, CONTEXT_) ComponentScript::ShowVoidFunction(std::bind(&FUNCTION, CONTEXT_), #FUNCTION)
// more info about LAMBDA, allow us to convert functions with arguments to void funtion pointer. https://thispointer.com/c11-lambda-how-to-capture-local-variables-inside-lambda/
#define SHOW_LAMBDA_FUNCTION(FUNCTION, NAME, CONTEXT_, ...) ComponentScript::ShowVoidFunction([CONTEXT_]() -> void { FUNCTION(__VA_ARGS__); }, NAME)

// -------------HELPERS INSPECTOR----------------- \\
/*--------------------text help--------------------*/
#define SHOW_TEXT(TEXT_) ComponentScript::InspectorText(TEXT_)
/*--------------------separator help--------------------*/
#define SHOW_SEPARATOR() ComponentScript::InspectorSeparator()
/*--------------------spacing help--------------------*/
#define SHOW_SPACING() ComponentScript::InspectorSpacing()
/*--------------------tooltip help--------------------*/
#define SHOW_TOOLTIP(TOOLTIP_) ComponentScript::InspectorToolTip(TOOLTIP_)
// -------------HELPERS INSPECTOR----------------- \\
// ------------INSPECTOR MACROS----------------\\

#define VARAIBLE_TO_STRING(VAR_) #VAR_

#define LOG(format, ...) Debug::Log(__FILE__, __LINE__, format, __VA_ARGS__);

#define enum(ENUM, ...) enum class ENUM {\
	__VA_ARGS__\
	};\
	inline static const char* ENUM##EnumNames = #__VA_ARGS__;