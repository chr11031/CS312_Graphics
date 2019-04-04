#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "gl.h"
// Compile command: g++ gl.cpp -lopengl32 -lglew32 -lSDL2

/********************************************
 * My main function
 ********************************************/
int main(int argc, char** argv)
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

    /**************************
     * Object Loader
     **************************/
    vector<material> materials;
    vector<vertexData> vertexBuffer;

    bool hasUV;
    bool hasNormal;

    success &= getObjData("bunny.obj", materials, vertexBuffer, hasUV, hasNormal);
    
    // Build a single array of floats
    int stride = 3 + (2 * hasUV) + (3 * hasNormal);
    int vertexBufferNumBytes = stride * vertexBuffer.size() * sizeof(float);
    float* vertexBufferData = (float*)(malloc(vertexBufferNumBytes));

    int i = 0;

    // Join data into an interleaved buffer
    for (int vb = 0; vb < vertexBuffer.size(); vb++)
    {
        vertexBufferData[i++] = vertexBuffer[vb].vert[0];
        vertexBufferData[i++] = vertexBuffer[vb].vert[1];
        vertexBufferData[i++] = vertexBuffer[vb].vert[2];

        if (hasUV)
        {
            vertexBufferData[i++] = vertexBuffer[vb].uv[0];
            vertexBufferData[i++] = vertexBuffer[vb].uv[1];
        }
        
        if (hasNormal)
        {
            vertexBufferData[i++] = vertexBuffer[vb].normal[0];
            vertexBufferData[i++] = vertexBuffer[vb].normal[1];
            vertexBufferData[i++] = vertexBuffer[vb].normal[2];
        }
    }

    // Materials
    vector<int> textureIDs;
    for (int mat = 0; mat < materials.size(); mat++)
    {
        int tmp;
        material m = materials[mat];
        success &= loadTexture(m.map_Kd, tmp);
        textureIDs.push_back(tmp);
    }

    validate(success, (char*)"Setup OpenGL Program");

    /**************************
     * Vertex Buffer Object
     **************************/
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferNumBytes, vertexBufferData, GL_STATIC_DRAW);

    /**************************
     * Attribute Handles
     **************************/
    int aPositionHandle = glGetAttribLocation(programHandle, "a_Position");
    int aUVHandle = glGetAttribLocation(programHandle, "a_UV");
    int aNormalHandle = glGetAttribLocation(programHandle, "a_Normal");

    /**************************
     * Uniform Handles
     **************************/
    int uModelHandle = glGetUniformLocation(programHandle, "u_Model");
    int uViewHandle = glGetUniformLocation(programHandle, "u_View");
    int uProjHandle = glGetUniformLocation(programHandle, "u_Proj");
    int uTextureHandle = glGetUniformLocation(programHandle, "u_Texture");
    int uThresholdHandle = glGetUniformLocation(programHandle, "u_Threshold");

    // Lighting uniforms
    int uAmbientHandle = glGetUniformLocation(programHandle, "ambientAmount");
    int uDiffuesHandle = glGetUniformLocation(programHandle, "diffuseAmount");
    int uSpecularHandle = glGetUniformLocation(programHandle, "specularAmount");
    int uCamHandle = glGetUniformLocation(programHandle, "u_Cam");
    int uShinyHandle = glGetUniformLocation(programHandle, "u_Shiny");

    // MVP matrices
    //mat4 mvp;
    mat4 model;
    mat4 view;
    mat4 proj;

    // set up camera
    myCam.camZ = -9;
    myCam.camX = myCam.camY = myCam.pitch = myCam.yaw = myCam.roll = 0;
    int numDraw = vertexBuffer.size();
    bool running = true;

    // Main draw loop
    while (running)
    {
        processUserInputs(running);
        {
            // Clear buffers
            glClearColor(0, 0, 0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(programHandle);

            /***************************
             * Set up attributes
             **************************/
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glVertexAttribPointer(aPositionHandle, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
            glEnableVertexAttribArray(aPositionHandle);

            glVertexAttribPointer(aUVHandle, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0 + 3 * sizeof(float));
            glEnableVertexAttribArray(aUVHandle);

            glVertexAttribPointer(aNormalHandle, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0 + 5 * sizeof(float));
            glEnableVertexAttribArray(aNormalHandle);

            /***************************
             * Set up uniforms
             **************************/
            // Update texture cache
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, textureIDs[0]);
            
            glUniform1i(uTextureHandle, 0);
            glUniform1f(uThresholdHandle, threshold);

            // Lighting
            glUniform3fv(uAmbientHandle, 1, (float*)&materials.back().Ka);
            glUniform3fv(uDiffuesHandle, 1, (float*)&materials.back().Kd);
            glUniform3fv(uSpecularHandle, 1, (float*)&materials.back().Ks);
            glUniform1f(uShinyHandle, materials.back().Ns);

            // Update camera
            glUniform3f(uCamHandle, myCam.camX, myCam.camY, myCam.camZ);

            setupMVP(model, view, proj);

            glUniformMatrix4fv(uModelHandle, 1, false, &model[0][0]);
            glUniformMatrix4fv(uViewHandle, 1, false, &view[0][0]);
            glUniformMatrix4fv(uProjHandle, 1, false, &proj[0][0]);

            glDrawArrays(GL_TRIANGLES, 0, numDraw);
        }
        
        SDL_GL_SwapWindow(win);
    }
    
    free(vertexBufferData);

    return 0;
}