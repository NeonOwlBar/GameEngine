#include "Terrain.h"

namespace GE
{
	Terrain::Terrain(std::string heightMapName)
	{
		SDL_Surface* heightMap = IMG_Load(heightMapName.c_str());
		if (heightMap == nullptr) {
			std::cerr << "Heightmap error: " << SDL_GetError() << ". Check filepath." << std::endl;
			// allows program to run
			return;
		}

		float heightScaling = 10.0f;
		float scaling = 100.0f;

		int channels = heightMap->format->BytesPerPixel;
		unsigned char* imageData = (unsigned char*)heightMap->pixels;
		int width = heightMap->w;
		int height = heightMap->h;

		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		float x, y, z, u, v;
		for (int row = 0; row < height; row++) {
			for (int column = 0; column < width; column++) {
				unsigned char redPixel = imageData[((row * width) + column) * channels];

				x = (float)column / (float)(width - 1) * scaling;
				z = (float)row / (float)(height - 1) * scaling;
				y = redPixel / 255.0f * heightScaling;

				u = (float)column / (float)(width - 1);
				v = (float)(height - row) / (float)(height - 1);
				vertices.push_back(Vertex(x, y, z, u, v));
			}
		}

		unsigned int c, n;
		for (int row = 0; row < height - 1; row++) {
			for (int column = 0; column < width - 1; column++) {
				c = row * width + column;
				n = (row + 1) * width + column;

				// leftmost triangle
				indices.push_back(c);
				indices.push_back(n);
				indices.push_back(c + 1);

				// rightmost triangle
				indices.push_back(c + 1);
				indices.push_back(n);
				indices.push_back(n + 1);

			}
		}
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		indexCount = indices.size();
		SDL_FreeSurface(heightMap);
	}
}