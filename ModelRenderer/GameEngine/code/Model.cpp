#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include "Model.h"

namespace GE {
	bool Model::loadFromFile(const char *filename)
	{
		// Temporay vector for storing model vertices loaded from file
		std::vector<Vertex> loadedVertices;
		file = filename;

		// Create importer object
		Assimp::Importer imp;

		// Load model into scene object
		const aiScene* pScene = imp.ReadFile(file, aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs);

		// Check file was opened successfully
		if (!pScene)
		{
			return false;
		}

		// Get the vertices from the model object and put them in the temporary vector
		for (int MeshIdx = 0; MeshIdx < pScene->mNumMeshes; MeshIdx++)
		{
			const aiMesh* mesh = pScene->mMeshes[MeshIdx];

			for (int faceIdx = 0; faceIdx < mesh->mNumFaces; faceIdx++)
			{
				const aiFace& face = mesh->mFaces[faceIdx];

				// Extract a vertex from the mesh's main vertex array
				// for each point in the face. 3 as there are three vertices
				// in a face (triangulated)
				for (int vertIdx = 0; vertIdx < 3; vertIdx++)
				{
					// Extract pos and tex co-ord based on its index number
					// not directly from mesh arrays
					const aiVector3D* pos = &mesh->mVertices[face.mIndices[vertIdx]];

					// Get uvs for vertex. This code assumes there are uvs
					// defined in the model vertices. If not, then code will 
					// crash
					const aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[vertIdx]];

					// This will cause a crash if a model mesh has no normals
					const aiVector3D* norm = &mesh->mNormals[face.mIndices[vertIdx]];

					// Create a new object in the shape array based on the extracted vertex
					// This shape array will be used to create the vertex buffer
					loadedVertices.push_back(Vertex(pos->x, pos->y, pos->z, uv.x, uv.y, norm->x, norm->y, norm->z));
				}
			}
		}

		// Number of vertices is derived from number of items in the temporary vector
		numVertices = loadedVertices.size();

		// Copy vertices into a memory buffer in order to
		// transfer to a vbo later
		glGenBuffers(1, &vbo);

		// Create the buffer
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// Copy vertex data from vector to buffer
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), loadedVertices.data(), GL_STATIC_DRAW);

		// Remove the buffer from the pipeline
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return true;
	}
}