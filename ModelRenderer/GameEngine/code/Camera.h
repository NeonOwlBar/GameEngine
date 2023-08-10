#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace GE
{
	class Camera
	{
	public:
		Camera(glm::vec3 cam_pos, glm::vec3 cam_target, glm::vec3 cam_upDir, float fov, float ar, float near, float far)
		{
			// Initiliase the camera variables to the values passed on the parameters
			pos = cam_pos;
			target = cam_target;
			up = cam_upDir;

			fovy = fov;
			aspectR = ar;
			nearClip = near;
			farClip = far;


			updateCamMatrices();

			// Get the active GL window
			SDL_Window* window = SDL_GL_GetCurrentWindow();

			// Get the width and height, in pixels, of the window
			int w, h;

			SDL_GetWindowSize(window, &w, &h);

			// Set the mouse to the centre of screen
			SDL_WarpMouseInWindow(SDL_GL_GetCurrentWindow(), w / 2, h / 2);
		}

		~Camera()
		{
		}

		// Accessor methods
		// Return the position of the camera in terms of x, y, z
		float getPosX()
		{
			return pos.x;
		}
		float getPosY()
		{
			return pos.y;
		}

		float getPosZ()
		{
			return pos.z;
		}

		// Target or what location the camera is looking at
		glm::vec3 getTarget()
		{
			return target;
		}

		// Up direction
		glm::vec3 getUpDir()
		{
			return up;
		}

		// Get pitch and yaw
		float getPitch()
		{
			return pitch;
		}
		float getYaw()
		{
			return yaw;
		}

		// Return the camera's view matrix. 
		// Used by draw() to send view matrix to vertex shader
		glm::mat4 getViewMatrix()
		{
			return viewMat;
		}

		// Return camera's projection matrix. Same reason as above
		glm::mat4 getProjectionMatrix()
		{
			return projectionMat;
		}


		// Mutator methods
		// Set position
		void setPosX(float newX)
		{
			pos = glm::vec3(newX, pos.y, pos.z);

			updateCamMatrices();
		}

		void setPosY(float newY)
		{
			pos = glm::vec3(pos.x, newY, pos.z);

			updateCamMatrices();
		}

		void setPosZ(float newZ)
		{
			pos = glm::vec3(pos.x, pos.y, newZ);

			updateCamMatrices();
		}

		glm::vec3 getPos() {
			return pos;
		}

		// Set position for all axes in one method
		void setPos(float newX, float newY, float newZ)
		{
			pos = glm::vec3(newX, newY, newZ);
			updateCamMatrices();
		}
		void setPos(glm::vec3 newPos)
		{
			pos = newPos;
			updateCamMatrices();
		}
		
		// Set new target
		void setTarget(glm::vec3 newTarget)
		{
			target = newTarget;
			updateCamMatrices();
		}

		// Set the up direction
		void setUpDir(glm::vec3 newUp)
		{
			up = newUp;
			updateCamMatrices();
		}

		// Set pitch and yaw
		void setPitch(float newPitch)
		{
			pitch = newPitch;
			// Clamp the pitch values so we can't point down at our feet or look straight up
			if (pitch > 85.0f) pitch = 85.0f;
			if (pitch < -85.0f) pitch = -85.0f;
		}

		void setYaw(float newYaw)
		{
			yaw = newYaw;
		}

		// Set the fov
		void setFov(float newFov)
		{
			fovy = newFov;
			updateCamMatrices();
		}

		// Set the aspect ratio
		// You might use this method if the program's window
		// can be resized meaning the width and height change
		void setAspectRatio(float newAR)
		{
			aspectR = newAR;
			updateCamMatrices();
		}

		// Set clipping panes
		void setNearClip(float newNearClip)
		{
			nearClip = newNearClip;
			updateCamMatrices();
		}

		void setFarClip(float newFarClip)
		{
			farClip = newFarClip;
			updateCamMatrices();
		}

		// Update the camera matrices done in repsonse to
		// a member variable being updated
		void updateCamMatrices()
		{
			viewMat = glm::lookAt(pos, pos + target, up);
			projectionMat = glm::perspective(glm::radians(fovy), aspectR, nearClip, farClip);
		}

	private:
		// Member variables
		// Camera view variables
		glm::vec3 pos;
		glm::vec3 target;
		glm::vec3 up;

		// Camera projection variables
		float fovy;
		float aspectR;
		float nearClip;
		float farClip;

		// Camera pitch and yaw values
		float pitch = 0.0f;
		float yaw = -90.0f;	// To ensure the camera starts pointing along the -z axis

		// View and projection matrices
		glm::mat4 viewMat;
		glm::mat4 projectionMat;

	};
}