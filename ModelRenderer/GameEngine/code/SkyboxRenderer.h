#pragma once
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <vector>
#include <string>
#include "Camera.h"

namespace GE
{
	class SkyboxRenderer
	{
	public:
		SkyboxRenderer(std::string front_fname, std::string back_fname, 
			std::string right_fname, std::string left_fname, 
			std::string top_fname, std::string bottom_fname)
		{
			std::vector<std::string> filenames;

			filenames.push_back(right_fname);
			filenames.push_back(left_fname);
			filenames.push_back(top_fname);
			filenames.push_back(bottom_fname);
			filenames.push_back(front_fname);
			filenames.push_back(back_fname);

			createCubemap(filenames);
			createCubeVBO();
			createSkyboxProgram();
		}

		~SkyboxRenderer() {}

		void draw(Camera* cam);

		void destroy();

	private:
		void createCubemap(std::vector<std::string>fiilenames);
		void createCubeVBO();
		void createSkyboxProgram();

		GLuint skyboxCubeMapName;

		// Member fields
		// This member stores the program object that contains the shaders
		GLuint skyboxProgramId;

		// This member stores the attribute to select into the pipeline
		// to link the triangle vertices to the pipeline
		GLint vertexLocation;

		// This member stores the triangle vertex buffer object containing the vertices
		// transferred from this code to the graphics memory
		GLuint vboSkybox;

		// GLSL uniform variables for the transformation, view and projection matrices
		GLuint viewUniformId;
		GLuint projectionUniformId;
		GLuint samplerId;
	};
}