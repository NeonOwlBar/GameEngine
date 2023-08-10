//#include "ShadowMap.h"
//
//namespace GE {
//	void ShadowMap::init() 
//	{
//		// Create framebuffer object for rendering depth map
//		glGenFramebuffers(1, &fboDepthMap);
//
//		// Create 2d texture for framebuffer's depth buffer
//		glGenTextures(1, &depthMap);
//		glBindTexture(GL_TEXTURE_2D, depthMap);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//		// Attach depth texture to framebuffer's depth buffer
//		glBindFramebuffer(GL_FRAMEBUFFER, fboDepthMap);
//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
//		
//		// No colour buffer required for a depth map, so need to explicitly
//		// tell OpenGL that no colour data will be used
//		glDrawBuffer(GL_NONE);
//		glReadBuffer(GL_NONE);
//
//		// Unbind framebuffer
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	}
//
//	void ShadowMap::renderToScreen()
//	{
//		//
//		glViewport(0, 0, shadowWidth, shadowHeight);
//		glBindFramebuffer(GL_FRAMEBUFFER, fboDepthMap);
//
//	}
//}