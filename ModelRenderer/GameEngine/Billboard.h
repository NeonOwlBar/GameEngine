#pragma once
#include "Texture.h"

namespace GE 
{
	class Billboard
	{
	public:
		// Constructor
		Billboard(Texture* t)
		{
			billTex = t;

			x = y = z = 0.0f;

			scaleX = scaleY = scaleZ = 1.0f;
		}
		
		// Destructor, does nothing
		~Billboard() {
		}

		// Accessor methods to allow access to position and scale
		// values for rendering
		float getX()
		{
			return x;
		}
		float getY()
		{
			return y;
		}
		float getZ()
		{
			return z;
		}

		glm::vec3 getPos()
		{
			return glm::vec3(x, y, z);
		}

		void setX(float newX)
		{
			x = newX;
		}
		void setY(float newY)
		{
			y = newY;
		}
		void setZ(float newZ)
		{
			z = newZ;
		}

		// Return the texture
		Texture* getTexture()
		{
			return billTex;
		}

		void setScaleX(float newX)
		{
			scaleX = newX;
		}
		void setScaleY(float newY)
		{
			scaleY = newY;
		}

		float getScaleX()
		{
			return scaleX;
		}
		float getScaleY()
		{
			return scaleY;
		}

	private:
		// Position
		float x, y, z;

		// Scale
		float scaleX, scaleY, scaleZ;

		// Texture
		Texture* billTex;
	};
}