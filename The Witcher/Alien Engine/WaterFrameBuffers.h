#ifndef __WATER_FRAME_BUFFERS_H__
#define __WATER_FRAME_BUFFERS_H__

#include "glew/include/glew.h"

#include "Globals.h"

class ResourceTexture;

class WaterFrameBuffers
{
public:
	WaterFrameBuffers();
	~WaterFrameBuffers();

public:
	void BindReflectionFrameBuffer();
	void BindRefractionFrameBuffer();
	void UnbindCurrentFrameBuffer(); // Note: width and height of the current viewport

	uint GetReflectionTexture();
	uint GetRefractionTexture();
	uint GetRefractionDepthTexture();

private:
	void InitialiseReflectionFrameBuffer();
	void InitialiseRefractionFrameBuffer();

	void BindFrameBuffer(const uint& frameBuffer, const int& width, const int& height);
	uint CreateFrameBuffer();

	uint CreateTextureAttachment(const int& width, const int& height);
	uint CreateDepthTextureAttachment(const int& width, const int& height);
	uint CreateDepthBufferAttachment(const int& width, const int& height);

public:
	static int reflection_width;
	static int reflection_height;

	static int refraction_width;
	static int refraction_height;

	ResourceTexture* dvud_tex = nullptr;

private:
	uint reflection_frame_buffer;
	uint reflection_texture;
	uint reflection_depth_buffer;
	
	uint refraction_frame_buffer;
	uint refraction_texture;
	uint refraction_depth_texture;
};

#endif /* __WATER_FRAME_BUFFERS_H__ */