#include "SkyboxRenderer.h"
#include "ShaderUtils.h"
#include <SDL_image.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace GE
{
	// Note that the cube for skybox has its own vertex struct
	// Major difference is there is no colour or uv
	// This is because cubemap is sampled from cube vertices
	struct CubeVertex
	{
		float x, y, z;

		CubeVertex()
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
		}

		CubeVertex(float _x, float _y, float _z)
		{
			x = _x;
			y = _y;
			z = _z;
		}
	};

	const float SIDE = 1.0f;

	CubeVertex cube[] = {
		// Front face
		CubeVertex(-SIDE,  SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE, -SIDE),
		CubeVertex( SIDE, -SIDE, -SIDE),

		CubeVertex( SIDE, -SIDE, -SIDE),
		CubeVertex( SIDE,  SIDE, -SIDE),
		CubeVertex(-SIDE, SIDE, -SIDE),

		// Back face
		CubeVertex(-SIDE,  SIDE, SIDE),
		CubeVertex(-SIDE, -SIDE, SIDE),
		CubeVertex(SIDE, -SIDE, SIDE),

		CubeVertex(SIDE, -SIDE, SIDE),
		CubeVertex(SIDE,  SIDE, SIDE),
		CubeVertex(-SIDE, SIDE, SIDE),

		// Left face
		CubeVertex(-SIDE, -SIDE, SIDE),
		CubeVertex(-SIDE,  SIDE, SIDE),
		CubeVertex(-SIDE,  SIDE, -SIDE),

		CubeVertex(-SIDE,  SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE,  SIDE),

		// Right face
		CubeVertex(SIDE, -SIDE, SIDE),
		CubeVertex(SIDE,  SIDE, SIDE),
		CubeVertex(SIDE,  SIDE, -SIDE),

		CubeVertex(SIDE,  SIDE, -SIDE),
		CubeVertex(SIDE, -SIDE, -SIDE),
		CubeVertex(SIDE, -SIDE,  SIDE),

		// Top face
		CubeVertex(-SIDE, SIDE,  SIDE),
		CubeVertex( SIDE, SIDE,  SIDE),
		CubeVertex( SIDE, SIDE, -SIDE),

		CubeVertex(SIDE,  SIDE, -SIDE),
		CubeVertex(-SIDE, SIDE, -SIDE),
		CubeVertex(-SIDE, SIDE,  SIDE),

		// Bottom face
		CubeVertex(-SIDE, -SIDE,  SIDE),
		CubeVertex( SIDE, -SIDE,  SIDE),
		CubeVertex( SIDE, -SIDE, -SIDE),

		CubeVertex( SIDE, -SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE,  SIDE),
	};

	void SkyboxRenderer::createCubemap(std::vector<std::string> filenames)
	{
		// Create a name for the cubemap
		glGenTextures(1, &skyboxCubeMapName);

		// Select the cubemap as the target to subsequent texture operations
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubeMapName);

		// Load texture data from fules into cubemap
		for (int faceNum = 0; faceNum < 6; faceNum++)
		{
			// Load texture data from file
			SDL_Surface* surfaceImage = IMG_Load(filenames[faceNum].c_str());

			// Check it was loaded okay
			if (surfaceImage == nullptr)
			{
				return;
			}

			// Get the format from the SDL object, will be in SDL Image's format
			GLenum format = surfaceImage->format->format;

			// Determine OpenGL format from SDL format
			switch (format)
			{
				// RGBA where there are 8 bits per pixel, 32 bits in total
			case SDL_PIXELFORMAT_RGBA32: format = GL_RGBA;
				break;

				// RGB where there are 8 bits per pixel, 24 bits in total
			case SDL_PIXELFORMAT_RGB24: format = GL_RGB;
				break;

				// Would have more formats here. There are alternative
				// ways to achieve the same mapping. Left as an exercise 
				// to find out.
			default:
				format = GL_RGB;
				break;
			}

			// Copy the pixel data from the SDL_Surface object to the OpenGL texture
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceNum, 0,
				format, surfaceImage->w, surfaceImage->h, 0,
				format, GL_UNSIGNED_BYTE, surfaceImage->pixels);

			// Release the surface object and memory associated with it
			// as no longer need it
			SDL_FreeSurface(surfaceImage);
		}

		// Configure how the texture will be manipulated when it needs to be reduced or increased (magnified)
		// when rendering onto an object. GL_LINEAR is weighted average of the colours around the texture co-ords
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Clamp the text edge to deal with inaccuracies. If the xyz co-ords go over
		// under 0/1, then sampler will wrap texture resulting in artefacting at the edges
		// This sets the cubemap to use clamping to repeat the last line of the texture
		// to deal with inaccuracies
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void SkyboxRenderer::createCubeVBO()
	{
		// Create the vertex buffer object
		glGenBuffers(1, &vboSkybox);
		glBindBuffer(GL_ARRAY_BUFFER, vboSkybox);

		// Transfer vertices to graphics memory
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

		// Release from pipeline
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void SkyboxRenderer::createSkyboxProgram()
	{
		// The shader code as described in lecture
		const GLchar* V_ShaderCode[] = {
			"#version 140\n"
			"in vec3 vertexPos3D;\n"
			"out vec3 texCoord;\n"
			"uniform mat4 view;\n"
			"uniform mat4 projection;\n"
			"void main() {\n"
			"vec4 v = vec4(vertexPos3D.xyz, 1);\n"
			"v = projection * view * v;\n"
			"gl_Position = v;\n"
			"texCoord = vertexPos3D;\n"
			"}\n"
		};

		// Fragment shader source code
		const GLchar* F_ShaderCode[] = {
			"#version 140\n"
			"in vec3 texCoord;\n"
			"uniform samplerCube sampler;\n"
			"out vec4 fragmentColour;\n"
			"void main() {\n"
			"fragmentColour = vec4(texture(sampler, texCoord).rgb, 1.0f);\n"
			"}\n"
		};

		bool result = compileProgram(V_ShaderCode, F_ShaderCode, &skyboxProgramId);

		// Check result
		if (!result)
		{
			std::cerr << "Failed to create SkyboxRenderer program. Check console for errors." << std::endl;
			return;
		}

		// Now get a link to the vertexPos3D so we can link the attribute
		// to our vertices when rendering
		vertexLocation = glGetAttribLocation(skyboxProgramId, "vertexPos3D");

		// Check for errors
		if (vertexLocation == -1)
		{
			std::cerr << "problem getting vertexPos3D" << std::endl;
		}

		// Link the uniforms to the member fields
		viewUniformId = glGetUniformLocation(skyboxProgramId, "view");
		projectionUniformId = glGetUniformLocation(skyboxProgramId, "projection");
		samplerId = glGetUniformLocation(skyboxProgramId, "sampler");
	}

	void SkyboxRenderer::draw(Camera* cam)
	{
		bool isDepthTestEnable = glIsEnabled(GL_DEPTH_TEST);

		glDisable(GL_DEPTH_TEST);

		glm::mat4 camView = cam->getViewMatrix();
		glm::mat4 projection = cam->getProjectionMatrix();

		// Zero out translation as just interested in rotation to make
		// sure skybox is oriented around the origin
		camView[3][0] = 0.0f;
		camView[3][1] = 0.0f;
		camView[3][2] = 0.0f;

		// Select the program into the rendering context
		glUseProgram(skyboxProgramId);

		//Select the vertex buffer object into the context
		glBindBuffer(GL_ARRAY_BUFFER, vboSkybox);

		// Set the uniforms in the shader
		glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(camView));
		glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projection));

		// eenable the attribut to be passed vertices from the vertex buffer object
		glEnableVertexAttribArray(vertexLocation);

		// Define the structure of a vertex for OpenGL to select values from vertex buffer
		// and store in vertexLocation attribute. Note, there is no attribute pointer for UVs
		// as for a skybox the texture coordinates are teaken from the vertex as using a cube map
		glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(CubeVertex), (void*)offsetof(CubeVertex, x));

		// Select the texture
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerId, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubeMapName);

		// Draw the model
		glDrawArrays(GL_TRIANGLES, 0, sizeof(cube) / sizeof(CubeVertex));

		// Unselect the attribute from the context
		glDisableVertexAttribArray(vertexLocation);

		// Unselect the program from the context
		glUseProgram(0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		if (isDepthTestEnable)
		{
			glEnable(GL_DEPTH_TEST);
		}
	}

	void SkyboxRenderer::destroy()
	{
		glDeleteProgram(skyboxProgramId);

		glDeleteBuffers(1, &vboSkybox);

		glDeleteTextures(1, &skyboxCubeMapName);
	}
}