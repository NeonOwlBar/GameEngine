#include "ModelRenderer.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace GE 
{
	// Constructor.  Initialises transformation variables
	// Should also initialise the other member variables to 0
	// to get rid of the green line under TriangleRenderer
	ModelRenderer::ModelRenderer(float _fogDens)
	{
		// Initialise location, rotation and scale to default values
		pos_x = pos_y = pos_z = 0.0f;
		rot_x = rot_y = rot_z = 0.0f;

		scale_x = scale_y = scale_z = 1.0f;

		programId = 0;

		vertexPos3DLocation = 0;

		vertexUVLocation = 0;

		transformUniformId = 0;
		viewUniformId = 0;
		projectionUniformId = 0;
		fogDensMod = _fogDens;

		setLightColour(1.0f, 1.0f, 1.0f);
	}

	// Destructor.  Does nothing
	ModelRenderer::~ModelRenderer()
	{
		destroy();
	}

	// Creates and compiles the shaders, creates the project and links it,
	// and creates the vertex buffer object.  Could be argued that method
	// is not too cohesive
	void ModelRenderer::init()
	{
		// The shader code as described in lecture
		const GLchar* V_ShaderCode[] =
		{
			"#version 140\n"

			"in vec3 vertexPos3D;\n"
			// vertex normals
			"in vec3 vertexNormal;\n"
			"in vec2 vUV;\n"

			"out vec2 uv;\n"
			"out vec3 posW;\n"
			"out vec3 outNormal;\n"

			"uniform mat4 viewMat;\n"
			"uniform mat4 projMat;\n"
			"uniform mat4 transformMat;\n"

			"out float fog_amount;\n"

			"uniform float fog_density;\n"

			"void main() {\n"
				"vec4 v = vec4(vertexPos3D.xyz, 1);\n"
				"vec4 posInWorld = transformMat * v;\n"
				"v = projMat * viewMat * posInWorld;\n"

				// calculate where vertex is in view space solely. To determine light direction
				"posW = vec3(viewMat * transformMat * v);\n"

				// Transform the normal so it reflects orientation of model
				"outNormal = vec3(viewMat * transformMat * vec4(vertexNormal, 0.0f));\n"

				"gl_Position = v;\n"
				// "fColour = vColour;\n"
				"uv = vUV;\n"
				"vec4 pos_rel_eye = viewMat * posInWorld;\n"
				"float distance = length(pos_rel_eye.xyz);\n"
				// "fog_amount = (distance - fog_start) / fog_range;\n"
				// "fog_amount = clamp(fog_amount, 0.0f, 1.0f);\n"
				// exponential fog
				"fog_amount = 1.0 - exp(-pow(distance * fog_density, 2.0f));\n"
				"fog_amount = clamp(fog_amount, 0.0f, 1.0f);\n"
			"}\n" 
		};


		// Fragment shader source code
		const GLchar* F_ShaderCode[] = {
			"#version 140\n"

			"in vec2 uv;\n"
			"in float fog_amount;\n"
			"in vec3 outNormal;\n"
			"in vec3 posW;\n"

			"uniform vec3 fog_colour;\n"

			// Light parameters
			"uniform vec3 lightColour;\n"
			"uniform vec3 ambient;\n"
			"uniform vec3 lightPos;\n"
			"uniform float shininess;"
			// Intensity of the specular light
			"uniform float specularStrength;\n"

			"uniform sampler2D sampler;\n"

			// location of viewer, e.g. camera
			"uniform vec3 viewPos;\n"

			"out vec4 fragmentColour;\n"

			"void main()\n"
			"{\n"
				"vec4 colour = texture(sampler, uv).rgba;\n"

				// Normalise the transformed nnormal so calculations
				// are performed on unit vectors to determine cosine
				// of the angle and resulting proportion of light
				"vec3 normalizedNormal = normalize(outNormal);\n"
	
			// Diffuse light
				// Determine the direction of the light with respect to position of vertex
				"vec3 lightDirection = normalize(lightPos - posW);\n"
				// Determine the amount of diffuse light from source
				"float diffIllum = max(dot(normalizedNormal, lightDirection), 0.0f);\n"
				// Calculate the diffuse component of the light
				"vec3 diffuse = diffIllum * lightColour;\n"
	
			// Specular light
				// Specular light is with respect to the viewer (camera)
				"vec3 viewDir = normalize(viewPos - posW);\n"
	
				// Determine direction of relected light with respect to the light direction
				// and normal
				"vec3 reflectDirection = reflect(-lightDirection, normalizedNormal); \n"
	
				// Calculate the angle between reflect light direction and camera
				// This is raised to a power to determine the shininess of the specular light.
				// The larger the power, te shiner the specular light is.
				// Shininess value could be a uniform so it can be set by the program as it runs
				 // rather than being fixed in the shader
				"float spec = pow(max(dot(viewDir, reflectDirection), 0.0f), shininess); \n"
	
				// Calculate final specular amount 
				"vec3 specular = specularStrength * spec * lightColour; \n"

				"vec3 finalColour = (ambient + diffuse) * colour.rgb + specular;\n"

				"fragmentColour = mix(vec4(finalColour, colour.a), vec4(fog_colour, 1.0f), fog_amount);\n"
			"}\n"
		};

		bool result = compileProgram(V_ShaderCode, F_ShaderCode, &programId);


		// Now get a link to the vertexPos3D so we can link the attribute
		// to our vertices when rendering
		vertexPos3DLocation = glGetAttribLocation(programId, "vertexPos3D");

		// Check for errors
		if (vertexPos3DLocation == -1) {
			std::cerr << "Problem getting vertexPos3D" << std::endl;
		}

		// Now get a link to the vColour attribute in the vertex shader
		// so we can indicate to the pipeline where the colour data is to be
		// inserted
		vertexUVLocation = glGetAttribLocation(programId, "vUV");
		
		// Check for errors
		if (vertexUVLocation == -1)
		{
			std::cerr << "Problem getting vUV" << std::endl;
		}

		vertexNormal = glGetAttribLocation(programId, "vertexNormal");
		if (vertexNormal == -1) 
		{
			std::cerr << "Problem getting vertexNormal" << std::endl;
		}

		// Get uniform id in shader so C++ program can send data to it
		viewUniformId = glGetUniformLocation(programId, "viewMat");
		projectionUniformId = glGetUniformLocation(programId, "projMat");
		// Transformation matrix uniform
		transformUniformId = glGetUniformLocation(programId, "transformMat");
		samplerId = glGetUniformLocation(programId, "sampler");
		// For lighting
		viewPosId = glGetUniformLocation(programId, "viewPos");
		lightColourId = glGetUniformLocation(programId, "lightColour");
		lightPosId = glGetUniformLocation(programId, "lightPos");
		lightAmbienceId = glGetUniformLocation(programId, "ambient");
		lightShineId = glGetUniformLocation(programId, "shininess");
		lightSpecStrId = glGetUniformLocation(programId, "specularStrength");

		// Link to fog uniforms
		fogColourId = glGetUniformLocation(programId, "fog_colour");
		fogDensityId = glGetUniformLocation(programId, "fog_density");
		// fogStartId = glGetUniformLocation(programId, "fog_start");
		// fogRangeId = glGetUniformLocation(programId, "fog_range");

		// Set fog values in shader in init means fog doesn't
		// change for the run of the game
		// Do this is constant fog wanted
		glUseProgram(programId);
		// glUniform1f(fogStartId, 50.0f);		// fog applied at 50 units
		// glUniform1f(fogRangeId, 200.0f);	// max fog at 200 units away
		// Exponential fog 
		// Default value: 0.0035f
		glUniform1f(fogDensityId, fogDensMod);
		glm::vec3 fog_colour = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(fogColourId, 1, glm::value_ptr(fog_colour));
		// Select the program into the rendering context
		glUseProgram(0);


		//// Tidy up after setting up buffer
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// Update does nothing for this example
	void ModelRenderer::update() {}

	// Draw renders the triangles from the buffer object
	void ModelRenderer::draw(Camera* cam, Model* model)
	{
		glEnable(GL_CULL_FACE);

		glm::mat4 transformationMat = glm::mat4(1.0f);

		transformationMat = glm::rotate(transformationMat, glm::radians(rot_x), glm::vec3(1.0f, 0.0f, 0.0f));
		transformationMat = glm::rotate(transformationMat, glm::radians(rot_y), glm::vec3(0.0f, 1.0f, 0.0f));
		transformationMat = glm::rotate(transformationMat, glm::radians(rot_z), glm::vec3(0.0f, 0.0f, 1.0f));
		transformationMat = glm::translate(transformationMat, glm::vec3(pos_x, pos_y, pos_z));
		transformationMat = glm::scale(transformationMat, glm::vec3(scale_x, scale_y, scale_z));

		glm::mat4 viewMat = cam->getViewMatrix();
		glm::mat4 projectionMat = cam->getProjectionMatrix();


		// Select the program into the rendering context
		glUseProgram(programId);

		// Set the uniforms in the shader
		glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, glm::value_ptr(transformationMat));
		glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projectionMat));

		// Pass the camera position to fragment shader viewPos
		glUniform3f(viewPosId, cam->getPosX(), cam->getPosY(), cam->getPosZ());

		// Pass the light variables to the fragment shader
		// glUniform3f(lightColourId, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightColourId, getLightColourR(), getLightColourG(), getLightColourB());
		glUniform3f(lightAmbienceId, 0.0f, 0.0f, 0.0f);
		glUniform3f(lightPosId, -50.0f, 200.0f, 0.0f);
		glUniform1f(lightShineId, 16.0f);
		glUniform1f(lightSpecStrId, 0.2f);

		// Select the vertex buffer object into the context
		glBindBuffer(GL_ARRAY_BUFFER, model->getVertices());

		// Enable the attribute to be passed vertices from the vertex buffer object
		glEnableVertexAttribArray(vertexPos3DLocation);

		// Define the structure of a vertex for OpenGL to select values from vertex buffer
		// and store in vertexPos3DLocation attribute
		// Remember: Vertex is a STRUCT that has been added manually
		glVertexAttribPointer(vertexPos3DLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, x));

		// Enable the attribute to be passed colours form the vertex buffer object
		glEnableVertexAttribArray(vertexUVLocation);

		// Define the structure of a vertex for OpenGL to select values from vertex buffer
		// and store in vColour attribute
		glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, u));


		glEnableVertexAttribArray(vertexNormal);

		// Define where pipeline will find vertex normal in a vertex in vertex buffer
		// to put in vertexNormal attribute
		glVertexAttribPointer(vertexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));

		// Select the texture
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerId, 0);
		glBindTexture(GL_TEXTURE_2D, tex->getTextureName());
		
		// Draw the triangle
		glDrawArrays(GL_TRIANGLES, 0, model->getNumVertices());

		// Unselect the attribute from the context
		glDisableVertexAttribArray(vertexPos3DLocation);

		// Unselect colour attribute form pipeline
		glDisableVertexAttribArray(vertexUVLocation);

		// Unselect normals attribute from pipeline
		glDisableVertexAttribArray(vertexNormal);

		// Unselect vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Unselect the program from the context
		glUseProgram(0);

		glDisable(GL_CULL_FACE);
	}

	void ModelRenderer::drawTerrain(Camera* cam, Terrain* terrain)
	{
		glEnable(GL_CULL_FACE);

		glUseProgram(programId);

		glm::vec3 rotation(getRotX(), getRotY(), getRotZ()), translation(getPosX(), getPosY(), getPosZ()), scale(getScaleX(), getScaleY(), getScaleZ());

		// Create and manipulate the Model Matrix
		glm::mat4 modelMatrix = glm::mat4(1.0f); // Identity matrix
		// modelMatrix = glm::rotate(modelMatrix, rotation.x * 180.0f / pi, glm::vec3(1.0f, 0.0f, 0.0f));
		// modelMatrix = glm::rotate(modelMatrix, rotation.y * 180.0f / pi, glm::vec3(0.0f, 1.0f, 0.0f));
		// modelMatrix = glm::rotate(modelMatrix, rotation.z * 180.0f / pi, glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix = glm::translate(modelMatrix, translation);
		modelMatrix = glm::scale(modelMatrix, scale);

		// Set Uniforms/Attributes
		glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(cam->getViewMatrix()));
		glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(cam->getProjectionMatrix()));

		// Vertex positions
		glBindBuffer(GL_ARRAY_BUFFER, terrain->getVbo());
		//
		glEnableVertexAttribArray(vertexPos3DLocation);
		glVertexAttribPointer(vertexPos3DLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
		// UV coordinates
		glEnableVertexAttribArray(vertexUVLocation);
		glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
		// glm::vec3 lightColourVal(1.0f, 1.0f, 1.0f);
		// glUniform3f(lightColourId,lightColourVal.x, lightColourVal.y, lightColourVal.z);
		glUniform3f(lightColourId, getLightColourR(), getLightColourG(), getLightColourB());
		glUniform3f(lightAmbienceId, 0.0f, 0.0f, 0.0f);
		glUniform3f(lightPosId, 150.0f, 200.0f, 0.0f);
		glUniform1f(lightShineId, 16.0f);
		glUniform1f(lightSpecStrId, 0.2f);

		// Texture
		glBindTexture(GL_TEXTURE_2D, tex->getTextureName());
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerId, 0);

		// Indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain->getIbo());

		// Drawing Function
		glDrawElements(GL_TRIANGLES, terrain->getIndexCount(), GL_UNSIGNED_INT, nullptr);

		// Remember to clean up and deselect everything! 
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glBindTexture(GL_TEXTURE_2D, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
		glUseProgram(NULL);

		glDisable(GL_CULL_FACE);
	}

	// Release objects allocated for program and vertex buffer object
	void ModelRenderer::destroy()
	{
		glDeleteProgram(programId);

		glDeleteBuffers(1, &vboTriangle);
	}

}

