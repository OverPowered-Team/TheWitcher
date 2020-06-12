#ifndef _VIEWPORT_H__
#define _VIEWPORT_H__

#include "Color.h"
#include "MathGeoLib/include/Math/float2.h"

class FBO
{
public:

	enum BufferType
	{
		DEPTH_TEAXTURE,
		NORMAL_FBO,
		NORMAL_TEXTURE,
		NORMAL_DEPTH_RBO,
		MULTISAMPLING_FBO,
		MULTISAMPLING_COLOR_RBO,
		MULTISAMPLING_DEPTH_RBO,
		POST_PROC_FBO,
		POST_PROC_TEXTURE,
		POST_PROC_MULTISAMPLING_FBO,
		POST_PROC_MULTISAMPLING_COLOR,
		POST_PROC_MULTISAMPLING_DEPTH,
		MAX
	};

	FBO();

	~FBO();

	void BeginFBO(const Color& color);

	void EndFBO();

	void GenerateFBO();

	void UpdateFBO(float width, float height);

	void DeleteFBO();

	uint GetFBOTexture();

	uint GetPostProcTexture(); 

	uint GetPostProcFinalFBO();
	uint GetPostProcMSAAFBO();

	uint GetFBO();

	// Only multiple of 2 values // 0 : MSAA disabled // 2-16 : MSAA enabled
	void SetMSAA(uint MSAA)
	{
		if (MSAA % 2 != 0 || MSAA > 0 || MSAA <= 16)
		{
			msaa = MSAA;
		}
	}

	uint GetMSAA()
	{
		return msaa;
	}

private:

	bool z_buffer_mode = true;
	uint ID[BufferType::MAX];
	uint msaa = 4;
	float2 position;

	float width = 0, height = 0;
};


class ComponentCamera;

class Viewport
{
public:

	Viewport(ComponentCamera* camera);

	~Viewport();

	void SetCamera(ComponentCamera* camera_go);

	ComponentCamera* GetCamera();

	void BeginViewport();

	void EndViewport();

	void ApplyPostProcessing();

	void FinalPass();

	void SetPos(float2 position);

	void SetSize(float width, float height);

	void SetMSAA(uint mssa);

	uint GetMSAA();

	uint GetFBO();

	// PostProc FBO with MSAA, depth buffer
	uint GetPostProcFBO();

	// Final fbo with only a colored mesh, that is what the player sees
	uint GetPostProcFinalFBO();

	uint GetTexture();

	uint GetPostProcTexture(); 

	void BlitFboToFbo(uint from, uint to, bool color = true, bool depth = false, bool stencil = false);
	
	bool ScreenPointToViewport(float2& screen_point);

	bool CanRender();

	float2 GetPos() const;

	float2 GetSize() const;

public:

	bool active = false;

private:

	FBO* fbo = nullptr;
	bool to_update = true;
	ComponentCamera* camera = nullptr;

	float2 position;
	float width = 640.f, height = 360.f;
};

#endif // !_VIEWPORT_H__
