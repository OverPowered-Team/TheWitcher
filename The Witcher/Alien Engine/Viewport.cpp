#include "Globals.h"
#include "Application.h"
#include "Viewport.h"
#include "GameObject.h"
#include "ComponentCamera.h"
#include "ModuleObjects.h"
#include "ModuleResources.h"
#include "ResourceShader.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "glew/include/glew.h"

#include "Optick/include/optick.h"

// FBO ========================================================================

FBO::FBO()
{
	for (int i = 0; i < BufferType::MAX; ++i)
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
	OPTICK_EVENT();

	glBindFramebuffer(GL_FRAMEBUFFER, ID[MULTISAMPLING_FBO]);
	glClearColor(color.r, color.g, color.b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	float black[] = { 0.0f, 0.0f, 0.0f ,0.0f };
	glClearBufferfv(GL_COLOR, 1, black);

}

void FBO::EndFBO()
{
	OPTICK_EVENT();

	// Blit Frame buffer -------------------------------------------------------

	glBindFramebuffer(GL_READ_FRAMEBUFFER, ID[MULTISAMPLING_FBO]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ID[NORMAL_FBO]);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	glReadBuffer(GL_COLOR_ATTACHMENT1);

	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// Generate Mipmap --------------------------------------------------------

	glBindTexture(GL_TEXTURE_2D, ID[NORMAL_TEXTURE]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glBindTexture(GL_TEXTURE_2D, ID[BLOOM_TEXTURE]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Copy depht component ---------------------------------------------------

	//glBindTexture(GL_TEXTURE_2D, ID[DEPTH_TEAXTURE]);
	//glReadBuffer(GL_BACK); // Ensure we are reading from the back buffer.
	//glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 0, 0, width, height, 0);

}

void FBO::UpdateFBO(float width, float height)
{
	OPTICK_EVENT();

	this->width = width;
	this->height = height;
	bool fboUsed = true;
	// Normal =====================================================================

	// Textures ---------------------------------------------

	// NORMAL TEXTURE
	glBindTexture(GL_TEXTURE_2D, ID[NORMAL_TEXTURE]);

	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	// NORMAL TEXTURE 2nd Attachment
	glBindTexture(GL_TEXTURE_2D, ID[BLOOM_TEXTURE]);

	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	// HDR TEXTURE (Final Texture)
	glBindTexture(GL_TEXTURE_2D, ID[POST_PROC_TEXTURE]);

	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);


	
	// ----------------------------------------------- NORMAL FBO -----------------------------------------------

	// Depth & Stencil -------------------------------------

	glBindRenderbuffer(GL_RENDERBUFFER, ID[NORMAL_DEPTH_RBO]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, ID[NORMAL_FBO]);

	// Attachment ------------------------------------------

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID[NORMAL_TEXTURE], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, ID[BLOOM_TEXTURE], 0); // Bloom attachment
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID[NORMAL_DEPTH_RBO]);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		fboUsed = false;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//----------------------------------------------- HDR MULTISAMPLE FBO ---------------------------------------------


	// Multisampling HDR -------------------------------------

	glBindRenderbuffer(GL_RENDERBUFFER, ID[POST_PROC_MULTISAMPLING_COLOR]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_RGBA16F, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, ID[POST_PROC_MULTISAMPLING_DEPTH]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	glBindFramebuffer(GL_FRAMEBUFFER, ID[POST_PROC_MULTISAMPLING_FBO]);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ID[POST_PROC_MULTISAMPLING_COLOR]);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID[POST_PROC_MULTISAMPLING_DEPTH]);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	 
	//----------------------------------------------- HDR Normal FBO ------------------------------------------

	// Depth & Stencil -------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, ID[POST_PROC_FBO]);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID[POST_PROC_TEXTURE], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//----------------------------------------------- PING PONG BLOOM FBOs & Texs ------------------------------------------
	
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingPongTex[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingPongTex[i], 0
		);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//----------------------------------------------- Multisample FBO -----------------------------------------------

	// Color -----------------------------------------------

	glBindRenderbuffer(GL_RENDERBUFFER, ID[MULTISAMPLING_COLOR_RBO1]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_RGBA16F, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, ID[MULTISAMPLING_COLOR_RBO2]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_RGBA16F, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Depth & Stencil -------------------------------------

	glBindRenderbuffer(GL_RENDERBUFFER, ID[MULTISAMPLING_DEPTH_RBO]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	// Frame -----------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, ID[MULTISAMPLING_FBO]);

	// Attachment ------------------------------------------

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ID[MULTISAMPLING_COLOR_RBO1]);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, ID[MULTISAMPLING_COLOR_RBO2]);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID[MULTISAMPLING_DEPTH_RBO]);

	uint bloom_attach[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, bloom_attach);

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
	glGenTextures(1, &ID[BLOOM_TEXTURE]);
	glGenTextures(1, &ID[DEPTH_TEAXTURE]);
	glGenTextures(1, &ID[POST_PROC_TEXTURE]);
	glGenTextures(2, pingPongTex);

	glGenFramebuffers(1, &ID[NORMAL_FBO]);
	glGenFramebuffers(1, &ID[MULTISAMPLING_FBO]);
	glGenFramebuffers(1, &ID[POST_PROC_FBO]);
	glGenFramebuffers(1, &ID[POST_PROC_MULTISAMPLING_FBO]);
	glGenFramebuffers(2, pingPongFBO);

	glGenRenderbuffers(1, &ID[NORMAL_DEPTH_RBO]);
	glGenRenderbuffers(1, &ID[MULTISAMPLING_COLOR_RBO1]);
	glGenRenderbuffers(1, &ID[MULTISAMPLING_COLOR_RBO2]);
	glGenRenderbuffers(1, &ID[MULTISAMPLING_DEPTH_RBO]);
	glGenRenderbuffers(1, &ID[POST_PROC_MULTISAMPLING_COLOR]);
	glGenRenderbuffers(1, &ID[POST_PROC_MULTISAMPLING_DEPTH]);
}

void FBO::DeleteFBO()
{
	glDeleteTextures(1, &ID[NORMAL_TEXTURE]);
	glDeleteTextures(1, &ID[BLOOM_TEXTURE]);
	glDeleteTextures(1, &ID[DEPTH_TEAXTURE]);
	glDeleteTextures(1, &ID[POST_PROC_TEXTURE]);
	glDeleteTextures(2, pingPongTex);

	glDeleteFramebuffers(1, &ID[NORMAL_FBO]);
	glDeleteFramebuffers(1, &ID[MULTISAMPLING_FBO]);
	glDeleteFramebuffers(1, &ID[POST_PROC_FBO]);
	glDeleteFramebuffers(1, &ID[POST_PROC_MULTISAMPLING_FBO]);
	glDeleteFramebuffers(2, pingPongFBO);

	glDeleteRenderbuffers(1, &ID[NORMAL_DEPTH_RBO]);
	glDeleteRenderbuffers(1, &ID[MULTISAMPLING_COLOR_RBO1]);
	glDeleteRenderbuffers(1, &ID[MULTISAMPLING_COLOR_RBO2]);
	glDeleteRenderbuffers(1, &ID[MULTISAMPLING_DEPTH_RBO]);
	glDeleteRenderbuffers(1, &ID[POST_PROC_MULTISAMPLING_COLOR]);
	glDeleteRenderbuffers(1, &ID[POST_PROC_MULTISAMPLING_DEPTH]);

	for (int i = 0; i < BufferType::MAX; ++i)
	{
		ID[i] = 0;
	}
}

uint FBO::GetFBOTexture()
{
	return ID[NORMAL_TEXTURE];
}

uint FBO::GetSecondTextureAttachment()
{
	return ID[BLOOM_TEXTURE];
}

uint FBO::GetPostProcTexture()
{
	return ID[POST_PROC_TEXTURE];
}

uint FBO::GetPostProcFinalFBO()
{
	return ID[POST_PROC_FBO];
}

uint FBO::GetPostProcMSAAFBO()
{
	return ID[POST_PROC_MULTISAMPLING_FBO];
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
	//OPTICK_EVENT(); <- this breaks all (?)

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
	OPTICK_EVENT();

	// Disables --------------------------------------------
	glDisable(GL_LIGHTING);
	glDisable(GL_POLYGON_SMOOTH);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	fbo->EndFBO();
}

void Viewport::ApplyPostProcessing()
{
	// --------------------- Blur Bright Pixels Image (2nd Attachment) --------------------------------------
	OPTICK_EVENT();

	BlurImage();

	// First copy depth and stencil from Normal MSAA FBO onto Post processing MSAA FBO 

	BlitFboToFbo(GetFBO(), GetPostProcFBO(), false, true, true);

	// ---------------------  Then draw Plane with the HDR Mixing Both Images--------------------- 

	// This plane doesnt need to be "placed" in the space so no depth testing
	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, GetPostProcFBO());
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);

	// ========== HDR PASS ============== 

	// HDR Shader
	App->resources->hdr_shader->Bind();

	// Get Rendered Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GetTexture());
	App->resources->hdr_shader->SetUniform1i("hdrBuffer", 0);

	// Get blurred image
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fbo->pingPongTex[0]);
	App->resources->hdr_shader->SetUniform1i("bloomBlur", 1);

	// Set Uniforms
	App->resources->hdr_shader->SetUniform1i("bloom", camera->bloom);
	App->resources->hdr_shader->SetUniform1i("hdr", camera->hdr);
	App->resources->hdr_shader->SetUniform1f("exposure", camera->exposure);
	App->resources->hdr_shader->SetUniform1f("gamma", camera->gamma);

	// Bind and draw Screen Quad with HDR pass 

	glBindVertexArray(App->renderer3D->screen_quad_VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Unbinds
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	App->resources->hdr_shader->Unbind();
}

void Viewport::BlurImage()
{
	OPTICK_EVENT();

	bool first_iteration = true; 
	bool horizontal = true; 
	App->resources->blur_shader->Bind(); 

	for (uint i = 0; i < camera->blur_iters; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo->pingPongFBO[horizontal]);
		App->resources->blur_shader->SetUniform1i("horizontal", horizontal);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? fbo->GetSecondTextureAttachment() : fbo->pingPongTex[!horizontal]);

		// Render quad
		glBindVertexArray(App->renderer3D->screen_quad_VAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}


	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	App->resources->blur_shader->Unbind();
}

void Viewport::FinalPass()
{
	OPTICK_EVENT();
	// Copy only color from PostProcMSAA FBO to the final PostProc texture
	BlitFboToFbo(GetPostProcFBO(), GetPostProcFinalFBO());

	// Lastly create the texture correctly
	glBindTexture(GL_TEXTURE_2D, GetPostProcTexture());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

uint Viewport::GetFBO()
{
	return fbo->GetFBO();
}

uint Viewport::GetPostProcFinalFBO()
{
	return fbo->GetPostProcFinalFBO();
}

uint Viewport::GetPostProcFBO()
{
	return fbo->GetPostProcMSAAFBO();
}

uint Viewport::GetTexture()
{
	return fbo->GetFBOTexture();
}

uint Viewport::GetBlurredTexture()
{
	return fbo->pingPongTex[0];
}

uint Viewport::GetPostProcTexture()
{
	return fbo->GetPostProcTexture();
}

void Viewport::BlitFboToFbo(uint from, uint to, bool color, bool depth, bool stencil)
{
	OPTICK_EVENT();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, from);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to);

	if(color)
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	if(depth)
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	if(stencil)
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_STENCIL_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
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