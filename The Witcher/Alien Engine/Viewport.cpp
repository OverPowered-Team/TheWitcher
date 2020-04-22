#include "Globals.h"
#include "Application.h"
#include "Viewport.h"
#include "GameObject.h"
#include "ComponentCamera.h"
#include "ModuleObjects.h"

#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "glew/include/glew.h"

// FBO ========================================================================

FBO::FBO()
{
	for (int i = 0; i < 8; ++i)
	{
		ID[i] = 0;
	}

	GenerateFBO();
}

FBO::~FBO()
{
	DeleteFBO();
}

void FBO::BeginFBO(const Color& color)
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID[MULTISAMPLING_FBO]);
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void FBO::EndFBO()
{
	// Blit Frame buffer -------------------------------------------------------

	glBindFramebuffer(GL_READ_FRAMEBUFFER, ID[MULTISAMPLING_FBO]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ID[NORMAL_FBO]);

	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// Generate Mipmap --------------------------------------------------------

	glBindTexture(GL_TEXTURE_2D, ID[NORMAL_TEXTURE]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Copy depht component ---------------------------------------------------

	//glBindTexture(GL_TEXTURE_2D, ID[DEPTH_TEAXTURE]);
	//glReadBuffer(GL_BACK); // Ensure we are reading from the back buffer.
	//glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 0, 0, width, height, 0);

}

void FBO::UpdateFBO(float width, float height)
{
	this->width = width;
	this->height = height;
	bool fboUsed = true;
	// Normal =====================================================================

	// Texture ---------------------------------------------

	glBindTexture(GL_TEXTURE_2D, ID[NORMAL_TEXTURE]);

	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	// Depth & Stencil -------------------------------------

	glBindRenderbuffer(GL_RENDERBUFFER, ID[NORMAL_DEPTH_RBO]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Frame -----------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, ID[NORMAL_FBO]);

	// Attachment ------------------------------------------

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID[NORMAL_TEXTURE], 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID[NORMAL_DEPTH_RBO]);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		fboUsed = false;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Multisample ================================================================

	// Color -----------------------------------------------

	glBindRenderbuffer(GL_RENDERBUFFER, ID[MULTISAMPLING_COLOR_RBO]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_RGBA8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Depth & Stencil -------------------------------------

	glBindRenderbuffer(GL_RENDERBUFFER, ID[MULTISAMPLING_DEPTH_RBO]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	// Frame -----------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, ID[MULTISAMPLING_FBO]);

	// Attachment ------------------------------------------

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ID[MULTISAMPLING_COLOR_RBO]);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID[MULTISAMPLING_DEPTH_RBO]);


	GLenum status_2 = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status_2 != GL_FRAMEBUFFER_COMPLETE)
		fboUsed *= false;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

//if (z_buffer_mode)
//{
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_FLOAT, 0);
//}
//else

void FBO::GenerateFBO()
{
	glGenTextures(1, &ID[NORMAL_TEXTURE]);
	glGenTextures(1, &ID[DEPTH_TEAXTURE]);

	glGenFramebuffers(1, &ID[NORMAL_FBO]);
	glGenFramebuffers(1, &ID[MULTISAMPLING_FBO]);

	glGenRenderbuffers(1, &ID[NORMAL_DEPTH_RBO]);
	glGenRenderbuffers(1, &ID[MULTISAMPLING_COLOR_RBO]);
	glGenRenderbuffers(1, &ID[MULTISAMPLING_DEPTH_RBO]);

}

void FBO::DeleteFBO()
{
	glDeleteTextures(1, &ID[NORMAL_TEXTURE]);
	glDeleteTextures(1, &ID[DEPTH_TEAXTURE]);

	glDeleteFramebuffers(1, &ID[NORMAL_FBO]);
	glDeleteFramebuffers(1, &ID[MULTISAMPLING_FBO]);

	glDeleteRenderbuffers(1, &ID[NORMAL_DEPTH_RBO]);
	glDeleteRenderbuffers(1, &ID[MULTISAMPLING_COLOR_RBO]);
	glDeleteRenderbuffers(1, &ID[MULTISAMPLING_DEPTH_RBO]);

	for (int i = 0; i < 8; ++i)
	{
		ID[i] = 0;
	}
}

uint FBO::GetFBOTexture()
{
	return ID[NORMAL_TEXTURE];
}

uint FBO::GetFBO()
{
	return ID[MULTISAMPLING_FBO];
}

// Viewport ========================================================================

Viewport::Viewport(ComponentCamera* camera_go)
{
	App->objects->viewports.push_back(this);
	SetCamera(camera_go);
	fbo = new FBO();
}

Viewport::~Viewport()
{
	camera = nullptr;
	delete fbo;
}

void Viewport::SetCamera(ComponentCamera* camera_go)
{
	if (camera_go != nullptr) {
		camera_go->AspectRatio(width, height);
		to_update = true;
	}
	camera = camera_go;
}

ComponentCamera* Viewport::GetCamera()
{
	return camera;
}

void Viewport::BeginViewport()
{
	if (to_update)
	{
		fbo->UpdateFBO(width, height);
		to_update = false;
	}

	// Start Buffer Frame ----------------------------------

	fbo->BeginFBO(camera->camera_color_background);

	// Load matrices ----------------------------------------

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(camera->GetProjectionMatrix());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(camera->GetViewMatrix());

	// Enbales ----------------------------------------------

	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

	// Default Stencil Settings ----------------------------

	glStencilFunc(GL_ALWAYS, 1, 0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	// Default Depth Settings ----------------------------
	glDepthMask(true);
	glDepthRange(0.f, 1.f);

}

void Viewport::EndViewport()
{
	// Disables --------------------------------------------
	glDisable(GL_LIGHTING);
	glDisable(GL_POLYGON_SMOOTH);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);


	fbo->EndFBO();
}

void Viewport::SetPos(float2 position)
{
	this->position = position;
}

void Viewport::SetSize(float width, float height)
{
	this->width = width;
	this->height = height;

	camera->AspectRatio(width, height);

	to_update = true;
}

void Viewport::SetMSAA(uint mssa)
{
	fbo->SetMSAA(mssa);
	to_update = true;
}

uint Viewport::GetMSAA()
{
	return fbo->GetMSAA();
}

uint Viewport::GetTexture()
{
	return fbo->GetFBOTexture();
}

uint Viewport::GetFBO()
{
	return fbo->GetFBO();
}

bool Viewport::CanRender()
{
	return camera != nullptr;
}

float2 Viewport::GetPos() const
{
	return position;
}

float2 Viewport::GetSize() const
{
	float2 size = float2(width, height);
	return size;
}

// Return if screen point is inside viewport

bool Viewport::ScreenPointToViewport(float2& input_output)
{
	float2 screen_point = input_output;

	if (!(screen_point.x > position.x&& screen_point.x < position.x + width && screen_point.y > position.y&& screen_point.y < position.y + height))
	{
		return false;
	}

	float normalized_x = -(1.0f - ((screen_point.x - position.x) * 2.0f) / width);
	float normalized_y = 1.0f - ((screen_point.y - position.y) * 2.0f) / height;

	input_output = { normalized_x , normalized_y };

	return true;
}