#pragma once

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "Terrain.h"
#include "ShaderUtils.h"

namespace GE {

	class ModelRenderer {
	public:
		ModelRenderer(float _fogDens);
		/*virtual*/ ~ModelRenderer();

		// init method to create the shaders and vertex buffer object
		void init();

		// update method to update the state of the object
		// For example, in other types of object may have some animation
		// which needs updating every frame
		void update();

		// draw method to render the triangle
		void draw(Camera* cam, Model* model);
		void drawTerrain(Camera* cam, Terrain* terrain);

		// Release method to release the objects such as vertex buffer object
		void destroy();

		// Accessors
		// Position
		float getPosX()
		{
			return pos_x;
		}
		float getPosY()
		{
			return pos_y;
		}
		float getPosZ()
		{
			return pos_z;
		}

		// Rotation
		float getRotX()
		{
			return rot_x;
		}
		float getRotY()
		{
			return rot_y;
		}
		float getRotZ()
		{
			return rot_z;
		}

		// Scale
		float getScaleX()
		{
			return scale_x;
		}
		float getScaleY()
		{
			return scale_y;
		}
		float getScaleZ()
		{
			return scale_z;
		}

		// Light uniform values
		float getLightColourR()
		{
			return lightColourVal.x;
		}
		float getLightColourG()
		{
			return lightColourVal.y;
		}
		float getLightColourB()
		{
			return lightColourVal.z;
		}

		glm::vec3 getLightPos() 
		{
			return lightPosVal;
		}

		glm::vec3 getLightAmbience()
		{
			return lightAmbienceVal;
		}

		float getLightShine()
		{
			return lightShineVal;
		}

		float getLightSpecStrength()
		{
			return lightSpecStrVal;
		}

		// Mutator methods
		void setPos(float x, float y, float z)
		{
			pos_x = x;
			pos_y = y;
			pos_z = z;
		}
		void setRotation(float rx, float ry, float rz)
		{
			rot_x = rx;
			rot_y = ry;
			rot_z = rz;
		}
		void setScale(float sx, float sy, float sz)
		{
			scale_x = sx;
			scale_y = sy;
			scale_z = sz;
		}

		void setLightColour(float r, float g, float b)
		{
			lightColourVal.x = r;
			lightColourVal.y = g;
			lightColourVal.z = b;
		}
		void setLightPos(glm::vec3 value)
		{
			lightPosVal = value;
		}
		void setLightAmbience(glm::vec3 value)
		{
			lightAmbienceVal = value;
		}
		void setLightShine(float value)
		{
			lightShineVal = value;
		}
		void setLightSpecStrength(float value)
		{
			lightSpecStrVal = value;
		}

		// Texture methods
		void setTexture(Texture* _tex)
		{
			tex = _tex;
		}


	private:
		// Member fields
		// This member stores the program object that contains the shaders
		GLuint programId;

		// This member stores the attribute to select into the pipeline
		// to link the triangle vertices to the pipeline
		GLint vertexPos3DLocation;

		// Link to vColour attribute which receives the colour (rgba) value
		// in a vertex
		GLint vertexUVLocation;

		// Link to normal attribute
		GLint vertexNormal;

		// This member stores the triangle vertex buffer object containing the vertices
		// transferred from this code to the graphics memory
		GLuint vboTriangle;

		// Location, rotation and scale variables
		float pos_x, pos_y, pos_z;
		float rot_x, rot_y, rot_z;
		float scale_x, scale_y, scale_z;

		// GLSL uniform variables for the transformation, view and projection matrices
		GLuint transformUniformId;
		GLuint viewUniformId;
		GLuint projectionUniformId;
		GLuint samplerId;

		// Fog uniform link variables
		GLuint fogColourId;
		GLuint fogDensityId;
		float fogDensMod;
		// GLuint fogStartId;
		// GLuint fogRangeId;

		// Texture to apply to the model
		Texture* tex;

		// For camera position
		GLuint viewPosId;
		// Light colour
		GLuint lightColourId;
		glm::vec3 lightColourVal;
		// Light Position
		GLuint lightPosId;
		glm::vec3 lightPosVal;
		// Light ambience
		GLuint lightAmbienceId;
		glm::vec3 lightAmbienceVal;
		// light shininess
		GLuint lightShineId;
		float lightShineVal;
		// light spectral strength
		GLuint lightSpecStrId;
		float lightSpecStrVal;
	};

}
