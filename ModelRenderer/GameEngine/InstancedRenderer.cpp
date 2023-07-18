#include "InstancedRenderer.h"
#include "ShaderUtils.h"
#include "Utils.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace GE {
	InstancedRenderer::InstancedRenderer() {
		instanceMatrixBuffer = -1;
	}

	InstancedRenderer::~InstancedRenderer() {
		destroy();
	}

	void InstancedRenderer::init()
	{
		// Load shader source from files.  Need the new ShaderUtils files
		std::string v_shader_source = loadShaderSourceCode("instance.vert");
		std::string f_shader_source = loadShaderSourceCode("instance.frag");

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

		vertexUVLocation = glGetAttribLocation(programId, "vUV");
		// Check for errors
		if (vertexUVLocation == -1) {
			std::cerr << "Problem getting vUV" << std::endl;
		}

		// Get the location of the instance attribute
		// which is the transformation matrix
		instanceMatLocation = glGetAttribLocation(programId, "instance_transform_mat");
		// Check for errors
		if (instanceMatLocation == -1)
		{
			std::cerr << "Problem getting instance_transform_mat" << std::endl;
		}

		// Link the uniforms to the member fields
		viewUniformId = glGetUniformLocation(programId, "view");
		projectionUniformId = glGetUniformLocation(programId, "projection");
		samplerId = glGetUniformLocation(programId, "sampler");
	}

	void InstancedRenderer::setInstanceData(const std::vector<InstanceData>& instances) {
		// Create a vector to store the transformation matrices
		std::vector<glm::mat4> instance_matrices;

		// Process the location, rotation and scales from instances vector
		// calculating the corresponding transformation matrix and insert
		// into vector
		for (auto& iprs : instances)
		{
			// Calculate the transformation matrix for the object.  Start with the identity matrix
			glm::mat4 instanceMat = glm::mat4(1.0f);

			// Current instance position, rotation and scale
			instanceMat = glm::translate(instanceMat, glm::vec3(iprs.posX, iprs.posY, iprs.posZ));
			instanceMat = glm::rotate(instanceMat, glm::radians(iprs.rotX), glm::vec3(1.0f, 0.0f, 0.0f));
			instanceMat = glm::rotate(instanceMat, glm::radians(iprs.rotY), glm::vec3(0.0f, 1.0f, 0.0f));
			instanceMat = glm::rotate(instanceMat, glm::radians(iprs.rotZ), glm::vec3(0.0f, 0.0f, 1.0f));
			instanceMat = glm::scale(instanceMat, glm::vec3(iprs.scaleX, iprs.scaleY, iprs.scaleZ));

			instance_matrices.push_back(instanceMat);
		}

		// Store the number of instances as need that number in
		// draw method
		numInstances = instance_matrices.size();

		// if instance matrix buffer == -1 {create}
		// else {delete current buffer data, then create new}
		// set to -1 in constructor
		if (!instanceMatrixBuffer == -1) {
			glDeleteBuffers(1, &instanceMatrixBuffer);
		}
		// Setup input of an array of matrix which define the position of each instance
		glGenBuffers(1, &instanceMatrixBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, instanceMatrixBuffer);
		glBufferData(GL_ARRAY_BUFFER, instance_matrices.size() * sizeof(glm::mat4), instance_matrices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);	
		// matrices have been passed to an OpenGL buffer references by the variable instanceMatrixBuffer
	}

	// Draw objects using instancing
	void InstancedRenderer::drawInstanced(Camera* cam, Model* m, Texture* tex) {
		// Enable back face culling by enabling face culling
		// Note that back face culling is the default culling
		// Change cull type with glCullFace() and pass either
		// GL_FRONT, GL_BACK or GL_FRONT_AND_BACK
		// glEnable(GL_FRONT_AND_BACK);
		glEnable(GL_CULL_FACE);

		// Get the view and projection matrices
		glm::mat4 viewMat = cam->getViewMatrix();
		glm::mat4 projectionMat = cam->getProjectionMatrix();

		// Select the program into the rendering context
		glUseProgram(programId);

		glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projectionMat));

		// Select the vertex buffer object into the context
		glBindBuffer(GL_ARRAY_BUFFER, m->getVertices());

		// Enable the attribute to be passed vertices from the vertex buffer object
		glEnableVertexAttribArray(vertexLocation);

		// Define the structure of a vertex for OpenGL to select values from vertex buffer
		// and store in vertexPos2DLocation attribute
		glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));

		// Enable the attribute to be passed vertices from the vertex buffer object
		glEnableVertexAttribArray(vertexUVLocation);

		// Define where the vertex specification will find the colour data and how much
		// Colour data is four float values, located at where the r member is.  Stride is a vertex apart
		glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));

		// Unbind the vertex buffer before defining the input for the
		// transformation matrices instance buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Now bind the matrix to the input specification which starts at instance_transform_mat
		glBindBuffer(GL_ARRAY_BUFFER, instanceMatrixBuffer);

		// Define the layout of the instance matrix input attribute in terms of
		// four vec4 values.  Reason is can't define an input in more than a vec4 so
		// matrix has to be broken down into four vec4s
		// Structure occupies more than one location (multiple vec4's), and locations are consecutive
		glEnableVertexAttribArray(instanceMatLocation);
		glVertexAttribPointer(instanceMatLocation, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(instanceMatLocation + 1);
		glVertexAttribPointer(instanceMatLocation + 1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(instanceMatLocation + 2);
		glVertexAttribPointer(instanceMatLocation + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(instanceMatLocation + 3);
		glVertexAttribPointer(instanceMatLocation + 3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));


		// Define how the instance data is used in terms of how many instances
		// pass before the transformation matrix is updated.  1 means one instance
		// or one pass of all of the vertices in the model.  2 means after two
		// passes and so on.  0 means every vertex which is normal behaviour
		glVertexAttribDivisor(instanceMatLocation, 1);
		glVertexAttribDivisor(instanceMatLocation + 1, 1);
		glVertexAttribDivisor(instanceMatLocation + 2, 1);
		glVertexAttribDivisor(instanceMatLocation + 3, 1);


		// Unbind the instance buffer as may want to define more inputs
		// based on another buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		// Select the texture
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerId, 0);
		glBindTexture(GL_TEXTURE_2D, tex->getTextureName());

		// Draw the instances
		glDrawArraysInstanced(GL_TRIANGLES, 0, m->getNumVertices(), numInstances);

		glVertexAttribDivisor(instanceMatLocation, 0);
		glVertexAttribDivisor(instanceMatLocation + 1, 0);
		glVertexAttribDivisor(instanceMatLocation + 2, 0);
		glVertexAttribDivisor(instanceMatLocation + 3, 0);

		glDisableVertexAttribArray(vertexLocation);
		glDisableVertexAttribArray(vertexUVLocation);
		// Unselect the attribute from the context
		glDisableVertexAttribArray(instanceMatLocation);
		glDisableVertexAttribArray(instanceMatLocation + 1);
		glDisableVertexAttribArray(instanceMatLocation + 2);
		glDisableVertexAttribArray(instanceMatLocation + 3);

		// Unselect the program from the context
		glUseProgram(0);

		glDisable(GL_CULL_FACE);
		//glDisable(GL_FRONT_AND_BACK);
	}

	// Release objects allocated for program and vertex buffer object
	void InstancedRenderer::destroy()
	{
		// Delete the instance buffer when finished
		glDeleteBuffers(1, &instanceMatrixBuffer);

		glDeleteProgram(programId);
	}

}


