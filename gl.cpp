#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "gl.h"

// this goes in the tasks.json file for vscode
// gg++ gl.cpp -lopengl32 -lglew32 -lSDL2
int main(int agrc, char ** argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow("OpenGL Program", 200, 200, SCREEN_W, SCREEN_H, SDL_WINDOW_OPENGL);
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetSwapInterval(1);
    SDL_GLContext cont = SDL_GL_CreateContext(win);

    glewInit();
    glEnable(GL_DEPTH_TEST);

    bool success = true;
    string vertexShader;
    string fragmentShader;
    int vertexHandle;
    int fragmentHandle;
    int programHandle;

    success &= parseFile((char*)"vertex.vs", vertexShader);
    success &= parseFile((char*)"fragment.fs", fragmentShader);

    success &= compileShader(vertexShader.c_str(), GL_VERTEX_SHADER, vertexHandle);
    success &= compileShader(fragmentShader.c_str(), GL_FRAGMENT_SHADER, fragmentHandle);
    success &= compileProgram(vertexHandle, fragmentHandle, programHandle);

    /********************************
     * Object Loader
     ***************************/
    vector<material> materials;
    vector<vertexData> vertexBuffer;

    bool hasUV;
    bool hasNormal;

    success &= getObjData("pot.obj", materials, vertexBuffer, hasUV, hasNormal);

    // Build out a single array of floats
    int stride = 3 + (2 * hasUV);
    int vertexBufferNumBytes = stride * vertexBuffer.size() * sizeof(float);
    float *vertexBufferData = (float*)(malloc(vertexBufferNumBytes));

    int i = 0;
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

    }

    vector<int> textureIDs;
    for(int mat = 0; mat < materials.size(); mat++)
    {
        int temp;
        material m = materials[mat];
        success &= loadTexture(m.map_Kd, temp);
        textureIDs.push_back(temp);
    }

    validate(success, (char*)"Setup OpenGL Program");

    /************
     * Vertex Buffer Object
     * ******************/
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferNumBytes, vertexBufferData, GL_STATIC_DRAW);
    
    /******************
     * Attribute Handles
     * **************************/

    int aPositionHandle = glGetAttribLocation(programHandle, "a_Position");
    int aUVHandle = glGetAttribLocation(programHandle, "a_UV");

    /***************
     * Uniform Handles
     * *******************/
    int uMatrixHandle = glGetUniformLocation(programHandle, "u_Matrix");
    int uTextureHandle = glGetUniformLocation(programHandle, "u_Texture");
    int uThresholdHandle = glGetUniformLocation(programHandle, "u_Threshold");

    /**************
     * MVP Matrix
     * ********/
    mat4 mvp;

    /*********************
    * Setup Camera Data
    * ***********************/
    myCam.camX = myCam.camY = myCam.camZ = myCam.pitch = myCam.yaw = myCam.roll = 0.0;

    int numDraw = vertexBuffer.size();
    bool running = true;

    // Main Draw Loop 
    while(running)
    {
        processUserInputs(running);
        {
            glUseProgram(programHandle);
            //Clear buffers
            glClearColor(0,0,0,1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            /***********
             * Setup Attributes
             * ************/
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glEnableVertexAttribArray(aPositionHandle);
            glVertexAttribPointer(aPositionHandle, 3,GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

            glEnableVertexAttribArray(aUVHandle);
            glVertexAttribPointer(aUVHandle, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

            /**************
             * Setup Uniforms
             * **********/
            // Update Texture
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, textureIDs[0]);

            glUniform1i(uTextureHandle, 0);
            glUniform1f(uThresholdHandle, threshold);

            setupMVP(mvp);
            glUniformMatrix4fv(uMatrixHandle, 1, false, &mvp[0][0]);

            glDrawArrays(GL_TRIANGLES, 0, numDraw);
        }
        SDL_GL_SwapWindow(win);
    }
    free(vertexBufferData);
    return 0;
}