#include <glm/glm.hpp>
#include <iostream>
#include "BillboardRenderer.h"
#include "ShaderUtils.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace GE {
	// Represents a vertex in the engine
	// This is a copy of the model vertex so is unnecessary
	// duplication.  Think about how you can combine the similar
	// renderers into one type of scene object
	struct Vertex {
		// Location
		float x, y, z;
		float u, v;

		// Constructors
		// Sets vertex to passed values
		Vertex(float _x, float _y, float _z, float _u, float _v) {
			// Location
			x = _x;
			y = _y;
			z = _z;

			// Colour
			u = _u;
			v = _v;
		}

		// Default, set to 0.0
		Vertex() {
			x = y = z = 0.0f;
			u = v = 0.0f;
		}
	};

	// Billboard quad.  Quad is a square so use scale
	// to get correct dimensions.  Is on 0,0,0
	Vertex billboard[] = {
		Vertex(0.5f, 1.0f, 0.0f, 1.0f, 1.0f),
		Vertex(-0.5f, 1.0f, 0.0f, 0.0f, 1.0f),
		Vertex(-0.5f, 0.0f, 0.0f, 0.0f, 0.0f),

		Vertex(-0.5f, 0.0f, 0.0f, 0.0f, 0.0f),
		Vertex(0.5f, 0.0f, 0.0f, 1.0f, 0.0f),
		Vertex(0.5f, 1.0f, 0.0f, 1.0f, 1.0f)
	};

	BillboardRenderer::BillboardRenderer(float _fogDens)
	{
		fogDensBill = _fogDens;
	}

	void BillboardRenderer::init()
	{
		// Load shader source from files.  Need the new ShaderUtils files
		std::string v_shader_source = loadShaderSourceCode("assets/billboards/billboard.vert");
		std::string f_shader_source = loadShaderSourceCode("assets/billboards/billboard.frag");

		// Due to the unique way OpenGL handles shader source, OpenGL expects
		// an array of strings.  In this case, create an array of the
		// loaded source code strings and pass to compileProgram for compilation
		const GLchar* v_source_array[] = { v_shader_source.c_str() };
		const GLchar* f_source_array[] = { f_shader_source.c_str() };

		// Compile shaders into a program
		if (!compileProgram(v_source_array, f_source_array, &programId)) {
			std::cerr << "Problem building billboard program.  Check console log for more information." << std::endl;
		}

		// Now get a link to the vertexPos3D so we can link the attribute
		// to our vertices when rendering
		vertexLocation = glGetAttribLocation(programId, "vertexPos3D");

		// Check for errors
		if (vertexLocation == -1) {
			std::cerr << "Problem getting vertex3DPos" << std::endl;
		}

		// Now link to the vUV attribute
		vertexUVLocation = glGetAttribLocation(programId, "vUV");

		// Check for errors
		if (vertexUVLocation == -1) {
			std::cerr << "Problem getting vUV" << std::endl;
		}

		// Link the uniforms to the member fields
		transformUniformId = glGetUniformLocation(programId, "transform");
		viewUniformId = glGetUniformLocation(programId, "view");
		projectionUniformId = glGetUniformLocation(programId, "projection");
		samplerId = glGetUniformLocation(programId, "sampler");

		// Link to fog uniforms
		fogColourId = glGetUniformLocation(programId, "fog_colour");
		// Exponential fog
		fogDensityId = glGetUniformLocation(programId, "fog_density");

		// Setting fog values in shader in init means
		// the fog doesn't change for the run of the game
		// Do this if you want constant fog
		glUseProgram(programId);
		glUniform1f(fogDensityId, fogDensBill);
		glm::vec3 fog_colour = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(fogColourId, 1, glm::value_ptr(fog_colour));
		// Select the program into the rendering context
		glUseProgram(0);

		// Create the vertex buffer for the quad
		glGenBuffers(1, &vboQuad);
		glBindBuffer(GL_ARRAY_BUFFER, vboQuad);

		// Transfer vertices to graphics memory
		glBufferData(GL_ARRAY_BUFFER, sizeof(billboard), billboard, GL_STATIC_DRAW);

		// Lot of duplication going on here, so think about how you could reduce this
		// duplication.  Don't forget, billboard is a quad so doesn't need a model to
		// be loaded from a file
	}

	void BillboardRenderer::draw(Billboard* b, Camera* cam)
	{
		// Allows transparency values to be used
		glEnable(GL_BLEND);
		// sourceFactor, destinationFactor
		// GL_SRC_ALPHA is 5th of 9 source factors
		// GL_ONE_MINUS_SRC_ALPHA is 6th of 8 destination factors
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Calculate the transformation matrix for the object.  Start with the identity matrix
		glm::mat4 transformationMat = glm::mat4(1.0f);
		
		glm::vec3 dirBill = glm::normalize(cam->getPos() - b->getPos());
		// atan2 produces result in radians, so no conversion required
		rotAngle = atan2(dirBill.x, dirBill.z);

		// Position in world
		transformationMat = glm::translate(transformationMat, glm::vec3(b->getX(), b->getY(), b->getZ()));

		// Rotation, on y axis to face direction of camera
		transformationMat = glm::rotate(transformationMat, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));

		// Scale, only need x and y
		transformationMat = glm::scale(transformationMat, glm::vec3(b->getScaleX(), b->getScaleY(), 0.0f));

		// Render object in the same way as a regular model
		// Get the view and projection matrices
		glm::mat4 viewMat = cam->getViewMatrix();
		glm::mat4 projectionMat = cam->getProjectionMatrix();

		// Select the program into the rendering context
		glUseProgram(programId);

		// Set the uniforms in the shader
		glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, glm::value_ptr(transformationMat));
		glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projectionMat));

		// Enable the attribute to be passed vertices from the vertex buffer object
		glEnableVertexAttribArray(vertexLocation);
		// Select the vertex buffer object into the context
		glBindBuffer(GL_ARRAY_BUFFER, vboQuad);

		// Define the structure of a vertex for OpenGL to select values from vertex buffer
		// and store in vertexPos2DLocation attribute
		glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));

		// Enable the attribute to be passed vertices from the vertex buffer object
		glEnableVertexAttribArray(vertexUVLocation);
		// Define where the vertex specification will find the colour data and how much
		// Colour data is four float values, located at where the r member is.  Stride is a vertex apart
		glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerId, 0);
		glBindTexture(GL_TEXTURE_2D, b->getTexture()->getTextureName());

		// Draw the model
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Unselect the attribute from the context
		glDisableVertexAttribArray(vertexLocation);
		glDisableVertexAttribArray(vertexUVLocation);

		// Unselect the program from the context
		glUseProgram(0);

		glDisable(GL_BLEND);
	}

}