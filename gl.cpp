#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "gl.h"

// g++ gl.cpp -lopengl32 -lglew32 -lSDL2 
int main(int argc, char ** argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* win = SDL_CreateWindow("OpenGL Program", 200, 200, SCREEN_W, SCREEN_H,
    SDL_WINDOW_OPENGL);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetSwapInterval(1);
	SDL_GLContext cont = SDL_GL_CreateContext(win);

	glewInit();
	glEnable(GL_DEPTH_TEST);

	bool success = true;
	string vertexShader;
	string fragmentShader;
	int vertexHandle;
	int fragHandle;
	int programHandle;

	success &= parseFile((char*)"phongVertex.vs", vertexShader);
	success &= parseFile((char*)"phongFrag.fs", fragmentShader);
	success &= compileShader(vertexShader.c_str(), GL_VERTEX_SHADER, vertexHandle);
	success &= compileShader(fragmentShader.c_str(), GL_FRAGMENT_SHADER, fragHandle);
	success &= compileProgram(vertexHandle, fragHandle, programHandle);

    /****************************
    * Object Loader
    ****************************/
	vector<material> materials;
	vector<vertexData> vertexBuffer;

	bool hasUV;
	bool hasNormal;

	success &= getObjData("bunny.obj", materials, vertexBuffer, hasUV, hasNormal);

	// Build out a single array of floatsF
	int stride = 3 + (2*hasUV) + (3*hasNormal);
	int vertexBufferNumBytes = stride * vertexBuffer.size() * sizeof(float);
	float *vertexBufferData = (float*)(malloc(vertexBufferNumBytes));

	int i = 0;
	// Join data into an interleaved buffer
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
		// Leave Normal Data for later...

		if(hasNormal)
		{
			vertexBufferData[i++] = vertexBuffer[vb].normal[0];
			vertexBufferData[i++] = vertexBuffer[vb].normal[1];
			vertexBufferData[i++] = vertexBuffer[vb].normal[2];
		}
		
	}

	vector<int> textureIDs;
	for(int mat = 0; mat < materials.size(); mat++)
	{
		int tmp;
		material m = materials[mat];
		success &= loadTexture(m.map_Kd, tmp);
		textureIDs.push_back(tmp);
	}

	validate(success, (char*)"Setup OpenGL Program");

	
	/****************************
    * Vertex Buffer Object
	****************************/
	int VBO;
	glGenBuffers(1, (GLuint*) &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferNumBytes, vertexBufferData, GL_STATIC_DRAW);

	/****************************
    * Attribute Handles
	****************************/
	int aPositionHandle = glGetAttribLocation(programHandle, "a_Position");
	int aUVHandle = glGetAttribLocation(programHandle, "a_UV");
	int aNormalHandle = glGetAttribLocation(programHandle, "a_Normal");

	/****************************
    * UniformHandles
	****************************/
	int uMatrixHandle = glGetUniformLocation(programHandle, "u_Matrix");
	int uTextureHandle = glGetUniformLocation(programHandle, "u_Texture");
	int uThresholdHandle = glGetUniformLocation(programHandle, "u_Threshold");
	int uCamHandle = glGetUniformLocation(programHandle, "u_Camera");

	int uMaterialA = glGetUniformLocation(programHandle, "u_MaterialAmbient");
    int uMaterialD = glGetUniformLocation(programHandle, "u_MaterialDiffuse");
    int uMaterialS = glGetUniformLocation(programHandle, "u_MaterialSpecular");
    int uMaterialShine = glGetUniformLocation(programHandle, "u_MaterialShine");

	// MVP Matrix
	mat4 mvp;

	// Setup Camera Data
	myCam.camX = myCam.camY = myCam.camZ = myCam.pitch = myCam.yaw = myCam.roll = 0.0;

	int numDraw = vertexBuffer.size();
	bool running = true;

	//Main Draw Loop
	while(running)
	{
		processUserInputs(running);
		{
			//clear buffers
			glClearColor(0,0,0,1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(programHandle);
			
			/*************************
			Setup Attributes
			*************************/
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glVertexAttribPointer(aPositionHandle, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
			glEnableVertexAttribArray(aPositionHandle);

			glVertexAttribPointer(aUVHandle, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0 + 3*sizeof(float));
			glEnableVertexAttribArray(aUVHandle);

			glVertexAttribPointer(aNormalHandle, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0 + 5*sizeof(float));
			glEnableVertexAttribArray(aNormalHandle);

			/******************************************
			 * setup uniforms
			 * ***********************************/
			//update Texture
			glActiveTexture(GL_TEXTURE0 +0);
			glBindTexture(GL_TEXTURE_2D, textureIDs[0]);

			glUniform1i(uTextureHandle, 0);
			glUniform1f(uThresholdHandle, threshold);
			glUniform3f(uCamHandle, myCam.camX, myCam.camY, myCam.camZ);

			glUniform3f(uMaterialA, materials[0].Ka[0], materials[0].Ka[1], materials[0].Ka[2]);
            glUniform3f(uMaterialD, materials[0].Kd[0], materials[0].Kd[1], materials[0].Kd[2]);
            glUniform3f(uMaterialS, materials[0].Ks[0], materials[0].Ks[1], materials[0].Ks[2]);
            glUniform1f(uMaterialShine, materials[0].Ns);

			setupMVP(mvp);
			glUniformMatrix4fv(uMatrixHandle, 1, false, &mvp[0][0]);

			glDrawArrays(GL_TRIANGLES, 0, numDraw);
		}
		SDL_GL_SwapWindow(win);
	}
	free(vertexBufferData);
	return 0;
}
