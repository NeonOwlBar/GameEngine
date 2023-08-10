#pragma once
//#include "Terrain.h"
#include <GL/glew.h>
#include <string>

namespace GE
{
	// Represents a vertex in the engine
	struct Vertex
	{
		// Location
		float x, y, z;
		float u, v;
		float nx, ny, nz;
		// float r, g, b, a;

		// Constructors
		// Sets vertex to passed values
		Vertex(float _x, float _y, float _z, float _u, float _v, float _nx, float _ny, float _nz)
		{
			// Location
			x = _x;
			y = _y;
			z = _z;
			// Colour
			u = _u;
			v = _v;
			// Lighting
			nx = _nx;
			ny = _ny;
			nz = _nz;
		}

		Vertex(float _x, float _y, float _z, float _u, float _v)
		{
			// Location
			x = _x;
			y = _y;
			z = _z;
			// Colour
			u = _u;
			v = _v;

			nx = ny = nz = 0.0f;
		}

		// Sets vertex to origin and no colour
		Vertex()
		{
			x = y = z = 0.0f;
			u = v = 0.0f;
			nx = ny = nz = 0.0f;
			// r = g = b = a = 0.0f;
		}
	};

	// Represents a model mesh in game engine
	class Model
	{
	public:
		// Constructor
		Model()
		{
			vbo = 0;
			numVertices = 0;
		}

		// Destructor
		~Model()
		{
			glDeleteBuffers(1, &vbo);
		}

		// Help method to load vertices from a file
		bool loadFromFile(const char* filename);

		// Returns the vertices so we can create a vertex buffer
		// based on te model vertices
		GLuint getVertices()
		{
			return vbo;
		}

		// Return the number of vertices in model
		// need this for rendering as OpenGL needs to
		// know the number of vertices to render
		int getNumVertices()
		{
			return numVertices;
		}

		std::string getFileName()
		{
			return file;
		}

	private:
		//Member data
		GLuint vbo;
		int numVertices;
		std::string file;

	};
}