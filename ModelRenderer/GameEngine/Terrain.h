#pragma once
// #include "ModelRenderer.h"
#include "Model.h"
#include <SDL_image.h>
#include <SDL.h>
#include <iostream>
#include <vector>

namespace GE
{
	class Terrain /* : public Model /* could inherit from something like ": public Model" so that
			  you wouldn't have to define getters or setters to access things like Texture or buffer objects...*/
	{
	public:
		Terrain(std::string heightMapName);

		GLuint getVbo() { return vbo; }
		GLuint getIbo() { return ibo; }
		GLuint getIndexCount() { return indexCount; }

	private:
		GLuint vbo, ibo, indexCount;
	};
}