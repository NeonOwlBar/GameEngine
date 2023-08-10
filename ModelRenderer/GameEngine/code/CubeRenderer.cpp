#include "CubeRenderer.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace GE {

	// Represents a vertex in the engine
	struct CubeVertex
	{
		// Location
		float x, y, z;
		// Colours
		float r, g, b, a;

		// Constructors
		// Sets vertex to passed values
		CubeVertex(float _x, float _y, float _z, float _r, float _g, float _b, float _a)
		{
			// Location
			x = _x;
			y = _y;
			z = _z;

			// Colour
			r = _r;
			g = _g;
			b = _b;
			a = _a;
		}

		// Sets vertex to origin
		CubeVertex()
		{
			x = y = z = 0.0f;
			r = g = b = a = 0.0f;
		}
	};


	// Define the cube's vertices
	CubeVertex cubeData[] = {
	CubeVertex(-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f),
	CubeVertex(-0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f),
	CubeVertex(0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f),

	CubeVertex(-0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f),
	CubeVertex(0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f),
	CubeVertex(0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f),

	// Right side face
	CubeVertex(0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f),
	CubeVertex(0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f),
	CubeVertex(0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f),

	CubeVertex(0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f),
	CubeVertex(0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f),
	CubeVertex(0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f),

	//// Back face, note that points are in counter clockwise order
	CubeVertex(-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f),
	CubeVertex(0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f),
	CubeVertex(0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f),

	CubeVertex(0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f),
	CubeVertex(-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f),
	CubeVertex(-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f),

	//// Left side face, note that points are in counter clockwise order
	CubeVertex(-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f),
	CubeVertex(-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f),
	CubeVertex(-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f),

	CubeVertex(-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f),
	CubeVertex(-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f),
	CubeVertex(-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f),

	//// Top face
	CubeVertex(-0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f),
	CubeVertex(-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f),
	CubeVertex(0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f),

	CubeVertex(0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f),
	CubeVertex(0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f),
	CubeVertex(-0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f),

	//// Bottom face, note that points are in counter clockwise order
	CubeVertex(-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f),
	CubeVertex(0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f),
	CubeVertex(0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f),

	CubeVertex(0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f),
	CubeVertex(-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f),
	CubeVertex(-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f),
	};

	// Constructor.  Initialises transformation variables
	// Should also initialise the other member variables to 0
	// to get rid of the green line under CubeRenderer
	CubeRenderer::CubeRenderer()
	{
		// Initialise location, rotation and scale to default values
		pos_x = pos_y = pos_z = 0.0f;
		rot_x = rot_y = rot_z = 0.0f;
		scale_x = scale_y = scale_z = 1.0f;
	}

	// Destructor.  Does nothing
	CubeRenderer::~CubeRenderer()
	{
	}

	// This is a helper function that allows us to see
	// shader compiler error messages should our shaders not compile okay
	//void displayShaderCompilerError(GLuint shaderId) {
	//	// First, get the length of the error message string
	//	GLint MsgLen = 0;

	//	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &MsgLen);

	//	// Only display the message is more than 1 character was returned
	//	if (MsgLen > 1) {
	//		// Create a character buffer to store the error message characters
	//		GLchar* Msg = new GLchar[MsgLen + 1];

	//		// Get the error message characters from the OpenGL internal log
	//		// into the Msg buffer
	//		glGetShaderInfoLog(shaderId, MsgLen, NULL, Msg);

	//		// Display the error message so we can see what the problem
	//		// was with the shader
	//		std::cerr << "Error compiling shader" << Msg << std::endl;

	//		// Release the memory allocated to the string
	//		delete[] Msg;
	//	}
	//}

	// Creates and compiles the shaders, creates the project and links it,
	// and creates the vertex buffer object.  Could be argued that method
	// is not too cohesive
	void CubeRenderer::init()
	{
		// Create the vertex shader first.
		// Order doesn't matter but shaders must be created and compiled before
		// attaching to program
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// The shader code as described in lecture
		const GLchar* V_ShaderCode[] =
		{
			"#version 140\n"
			"in vec3 vertexPos3D;\n"
			"in vec4 vColour;\n"
			"out vec4 fColour;\n"
			"uniform mat4 viewMat;\n"
			"uniform mat4 projMat;\n"
			"uniform mat4 transformMat;\n"
			"void main() {\n"
			"gl_Position = projMat * viewMat * transformMat * vec4(vertexPos3D.x, vertexPos3D.y, vertexPos3D.z, 1);\n"
			"fColour = vColour;\n"
			"}\n"
		};

		// Copy the source to OpenGL ready for compilation
		glShaderSource(vertexShader, 1, V_ShaderCode, NULL);

		// Compile the code
		glCompileShader(vertexShader);

		// Check for compiler errors
		// Presume shader didn't compile
		GLint isShaderCompiledOK = GL_FALSE;

		// Get the compile status from OpenGL
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isShaderCompiledOK);

		// Has the shader failed to compile?
		if (isShaderCompiledOK != GL_TRUE) {
			// Yes, so display an error message
			std::cerr << "unable to compile vertex shader" << std::endl;
			// displayShaderCompilerError(vertexShader);
			return;
		}

		// Do the same for the fragment shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Fragment shader source code
		const GLchar* F_ShaderCode[] = {
			"#version 140\n"
			"in vec4 fColour;\n"
			"out vec4 fragmentColour;\n"
			"void main()\n"
			"{\n"
			"fragmentColour = fColour;\n"
			"}\n"
		};

		// Transfer the shader code
		glShaderSource(fragmentShader, 1, F_ShaderCode, NULL);

		// Compile it
		glCompileShader(fragmentShader);

		// Check for errors.  Code is same as above of getting status
		// and displaying error message, if necessary
		isShaderCompiledOK = GL_FALSE;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isShaderCompiledOK);

		if (isShaderCompiledOK != GL_TRUE) {
			std::cerr << "unable to compile fragment shader" << std::endl;
			// displayShaderCompilerError(fragmentShader);
			return;
		}

		// Create the program object
		programId = glCreateProgram();

		// Attach shaders to the program object
		glAttachShader(programId, vertexShader);
		glAttachShader(programId, fragmentShader);


		// Now link the program to create an executable program we
		// and use to render the object
		// Program executable will exist in graphics memory
		glLinkProgram(programId);

		// Check for linking errors
		GLint isProgramLinked = GL_FALSE;
		glGetProgramiv(programId, GL_LINK_STATUS, &isProgramLinked);
		if (isProgramLinked != GL_TRUE) {
			std::cerr << "Failed to link program" << std::endl;
		}


		// Now get a link to the vertexPos2D so we can link the attribute
		// to our vertices when rendering
		vertexPos3DLocation = glGetAttribLocation(programId, "vertexPos3D");

		// Check for errors
		if (vertexPos3DLocation == -1) {
			std::cerr << "Problem getting vertexPos3D" << std::endl;
		}

		// Now get a link to the vColour attribute in the vertex shader
		// so we cn indicate to the pipeline where the colour data is to be
		// inserted
		vertexColourLocation = glGetAttribLocation(programId, "vColour");

		// Check for errors
		if (vertexColourLocation == -1)
		{
			std::cerr << "problem getting vColour" << std::endl;
		}

		// Get uniform id in shader so C++ program can send data to it
		viewUniformId = glGetUniformLocation(programId, "viewMat");
		projectionUniformId = glGetUniformLocation(programId, "projMat");
		// Transformation matrix uniform
		transformUniformId = glGetUniformLocation(programId, "transformMat");

		// Create the vertex buffer object
		glGenBuffers(1, &vboCube);
		glBindBuffer(GL_ARRAY_BUFFER, vboCube);

		// Transfer vertices to graphics memory
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeData), cubeData, GL_STATIC_DRAW);

		// Tidy up after setting up buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// Update does nothing for this example
	void CubeRenderer::update()
	{


	}

	// Draw renders the cubes from the buffer object
	void CubeRenderer::draw(Camera* cam)
	{
		glm::mat4 transformationMat = glm::mat4(1.0f);

		transformationMat = glm::translate(transformationMat, glm::vec3(pos_x, pos_y, pos_z));
		transformationMat = glm::rotate(transformationMat, glm::radians(rot_x), glm::vec3(1.0f, 0.0f, 0.0f));
		transformationMat = glm::rotate(transformationMat, glm::radians(rot_y), glm::vec3(0.0f, 1.0f, 0.0f));
		transformationMat = glm::rotate(transformationMat, glm::radians(rot_z), glm::vec3(0.0f, 0.0f, 1.0f));
		transformationMat = glm::scale(transformationMat, glm::vec3(scale_x, scale_y, scale_z));

		glm::mat4 viewMat = cam->getViewMatrix();
		glm::mat4 projectionMat = cam->getProjectionMatrix();


		// Select the program into the rendering context
		glUseProgram(programId);

		// Set the uniforms in the shader
		glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, glm::value_ptr(transformationMat));
		glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projectionMat));

		// Select the vertex buffer object into the context
		glBindBuffer(GL_ARRAY_BUFFER, vboCube);

		// Enable the attribute to be passed vertices from the vertex buffer object
		glEnableVertexAttribArray(vertexPos3DLocation);

		// Define the structure of a vertex for OpenGL to select values from vertex buffer
		// and store in vertexPos2DLocation attribute
		// Remember: Vertex is a STRUCT that has been added manually
		glVertexAttribPointer(vertexPos3DLocation, 3, GL_FLOAT, GL_FALSE, sizeof(CubeVertex), (void*)offsetof(CubeVertex, x));

		// Enable the attribute to be passed colours form the vertex buffer object
		glEnableVertexAttribArray(vertexColourLocation);

		// Define the structure of a vertex for OpenGL to select values from vertex buffer
		// and store in vColour attribute
		glVertexAttribPointer(vertexColourLocation, 4, GL_FLOAT, GL_FALSE, sizeof(CubeVertex), (void*)offsetof(CubeVertex, r));

		// Draw the cube
		glDrawArrays(GL_POLYGON, 0, sizeof(cubeData) / sizeof(CubeVertex));

		// Unselect the attribute from the context
		glDisableVertexAttribArray(vertexPos3DLocation);

		// Unselect colour attribute form pipeline
		glDisableVertexAttribArray(vertexColourLocation);

		// Unselect vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Unselect the program from the context
		glUseProgram(0);
	}

	// Release objects allocated for program and vertex buffer object
	void CubeRenderer::destroy()
	{
		glDeleteProgram(programId);

		glDeleteBuffers(1, &vboCube);
	}

}

