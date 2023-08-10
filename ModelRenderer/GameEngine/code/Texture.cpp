#include "Texture.h"

namespace GE
{
	void Texture::loadTexture(std::string filename)
	{
		// load texture data from file
		SDL_Surface* surfaceImage = IMG_Load(filename.c_str());

		// Check it was loaded okay
		if (surfaceImage == nullptr)
		{
			std::cerr << "Error finding image: " << SDL_GetError() << std::endl;
			return;
		}

		// Get the dimensions, need for OpenGL
		width = surfaceImage->w;
		height = surfaceImage->h;

		// Get the format from the SDL object, will be in SDL Image's format
		format = surfaceImage->format->format;

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

		// Create the texture name for the texture
		glGenTextures(1, &textureName);

		// Select created texture for subsequent texture operations
		// to setup the texture for OpenGL
		glBindTexture(GL_TEXTURE_2D, textureName);

		// Copy the pixel data from the SDL_Surface object to the OpenGL texture
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, 
			GL_UNSIGNED_BYTE, surfaceImage->pixels);

		// Configure how the texture will be manipulated when it needs to be
		// reduced or increased (magnified) when rendering onto an object.
		// GL_LINEAR is weighted average of the colours around the texture co-ords
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Release the surface object and memory associated with it
		// as no longer need it
		SDL_FreeSurface(surfaceImage);

		return;
	}
}