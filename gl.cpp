#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "gl.h"

// To compile: g++ fuzzy.cpp -lopengl32 -lglew32 -lSDL2 -lmingw32
int main(int argc, char **argv)
{  
	SDL_Init(SDL_INIT_VIDEO);
 
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);


	SDL_Window * win = SDL_CreateWindow("OpenGL Shaders", 200, 200, SCREEN_W, SCREEN_H, SDL_WINDOW_OPENGL);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetSwapInterval(1);
	SDL_GLContext cont = SDL_GL_CreateContext(win);
	SDL_GL_MakeCurrent(win, cont);
	glewInit();  
	glEnable(GL_DEPTH_TEST);

	bool success = true;
	string vertexShader;
	string fragmentShader;
	int vertexID;
	int fragID; 
	int programID;
	success &= parseFile((char*)"vertex.vs", vertexShader);
	success &= parseFile((char*)"fragment.fs"  , fragmentShader);
	success &= compileShader(vertexShader.c_str(), GL_VERTEX_SHADER, vertexID);
	success &= compileShader(fragmentShader.c_str(), GL_FRAGMENT_SHADER, fragID);
	success &= compileProgram(vertexID, fragID, programID);

	/***************************************************************************************
	* OBJECT LOADER - Vertices, UVs, Normals, etc
	***************************************************************************************/  
	vector<material> materials;
	vector<vertexData> vertexBuffer;
	bool hasUV;
	bool hasNormal;  
	success &= getData("pot.obj", materials, vertexBuffer, hasUV, hasNormal);

	// Build out a single array of float data 
	int stride = 3 + (2*hasUV);
	int vertexBufferNumBytes = stride * vertexBuffer.size() * sizeof(float);
	float* vertexBufferData = (float*)(malloc(vertexBufferNumBytes));
	int i = 0;

	// Join data together into an interleaved buffer
	for(int vb = 0; vb < vertexBuffer.size(); vb++)
	{
		vertexBufferData[i++] = vertexBuffer[vb].vert[0];
		vertexBufferData[i++] = vertexBuffer[vb].vert[1];
		vertexBufferData[i++] = vertexBuffer[vb].vert[2];
		if(hasUV)
		{
			vertexBufferData[i++] = vertexBuffer[vb].uv[0];
			vertexBufferData[i++] = vertexBuffer[vb].uv[1];		
		}
		// Leave Normal data for later...
	}

	// Load in each texture 
	vector<int> textureIDs;	
	for(int mat = 0; mat < materials.size(); mat++)
	{
		int tmp;
		material m = materials[mat];
		success &= loadTexture(m.map_Kd, tmp);
		textureIDs.push_back(tmp);
	}
	validate(success, (char*)"Setup OpenGL Program");  


	/***************************************************************************************
	* VERTEX BUFFER OBJECT - Vertex data AND Attributes
	***************************************************************************************/  
	// Create VBO - Vertex Buffer Object - Which will hold the data (interleaved) for our vertices 
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferNumBytes, vertexBufferData, GL_STATIC_DRAW);

	
	float data[] = { -0.5, -0.5, 1.0, 
					 0.5,  -0.5, 1.0, 
					 0.0,  0.0,  1.0};
	unsigned int VBB; 
	glGenBuffers(1, &VBB);
	glBindBuffer(GL_ARRAY_BUFFER, VBB);
	glBufferData(GL_ARRAY_BUFFER, 9*4, data, GL_STATIC_DRAW);
		
	/***************************************************************************
	* ATTRIBUTE HANDLES - based on the way the shader is written; mapped to data 
	***************************************************************************/      
	int aPositionHandle = glGetAttribLocation(programID, "a_Position");
	int aUVHandle = glGetAttribLocation(programID, "a_UV");


	/***************************************************************************************
	* UNIFORM HANDLES - kept to update uniforms; mapped for each frame (could be pre-cached)
	***************************************************************************************/      
	// Establish shader-specific variables
	int uMatrixHandle = glGetUniformLocation(programID, "u_Matrix");
	int uTextureHandle = glGetUniformLocation(programID, "u_Texture");
	int uThresholdHandle = glGetUniformLocation(programID, "u_Threshold");

	// MVP Data for transforming vertices
	mat4 mvp;	  

	// Camera data 
	myCam.camX = myCam.camY = myCam.camZ = myCam.pitch = myCam.yaw = myCam.roll = 0.0;

	// Main Loop 
	int numDraw = vertexBuffer.size(); 
	bool running = true;
	printf("%s\n", glGetString(GL_VERSION));//_GetError());	
	while(running)
	{
		// Update input
		processUserInputs(running);      
		{

			// Setup Program, Attach appropriate buffer
			glUseProgram(programID);
			// Clear buffers, setup/use program 
			glClearColor(0,0,0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			/***************************************************************************
			* ATTRIBUTE HANDLES - based on the way the shader is written; mapped to data 
			***************************************************************************/     
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glEnableVertexAttribArray(aPositionHandle);
			glVertexAttribPointer(aPositionHandle, 
			3, GL_FLOAT, GL_FALSE, stride*sizeof(float), (void*)0);
			glEnableVertexAttribArray(aUVHandle);
			glVertexAttribPointer(aUVHandle, 
			2, GL_FLOAT, GL_FALSE, stride*sizeof(float), (void*)0 + 3*sizeof(float));

			// Update Texture - Assume that we want texture '0'
			glActiveTexture(GL_TEXTURE0 + 0); // + "i" to change texture chosen
			glBindTexture(GL_TEXTURE_2D, textureIDs[0]);
			// Update Threshold
			glUniform1i(uTextureHandle, 0);
			glUniform1f(uThresholdHandle, threshold);
			// Update MVP
			setupMVP(mvp);
			glUniformMatrix4fv(uMatrixHandle, 1, false, &mvp[0][0]);
			
			// Output what we have
			glDrawArrays(GL_TRIANGLES, 0, numDraw);
		}
		// Update SDL buffer
		SDL_GL_SwapWindow(win);
	}
	free(vertexBufferData);
	return 0;
}

