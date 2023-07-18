#pragma once

#include <iostream>
#include <GL/glew.h>


#if _DEBUG
#define GLCALL(call) utils::clearGLError(); call; { if (!utils::printGLError(#call, __FILE__, __LINE__)) __debugbreak(); };
#else 
#define GLCALL(call) call;
#endif



namespace utils {

	inline void clearGLError() { while (GLenum error = glGetError()); }

	inline bool printGLError(const char* call, const char* file, int line) {
		while (GLenum error = glGetError()) {
			std::cout << "Error in function: " << call << std::endl;
			std::cout << "in file: " << file << std::endl;
			std::cout << "in line: " << line << std::endl;
			switch (error) {
			case GL_INVALID_OPERATION:
				std::cout << "OPENGL Invalid operation!: " << error << std::endl;
				break;
			case GL_INVALID_ENUM:
				std::cout << "OPENGL Invalid enum!: " << error << std::endl;
				break;
			case GL_INVALID_VALUE:
				std::cout << "OPENGL Invalid value!: " << error << std::endl;
				break;
			case GL_STACK_OVERFLOW:
				std::cout << "OPENGL Stack overflow!: " << error << std::endl;

				break;
			case GL_STACK_UNDERFLOW:
				std::cout << "OPENGL Stack underflow!: " << error << std::endl;
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				std::cout << "OPENGL Invalid framebuffer operation!: " << error << std::endl;
				break;
			}
			return false;
		}
		return true;
	}
}