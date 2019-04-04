#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "gl.h"

// compile command
// g++ gl.cpp -lopengl -lglew -lSDL2

/********************************************
 * My main function
 ********************************************/
int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * win = SDL_CreateWindow("OpenGL Program", 200, 200, SCREEN_W, SCREEN_H, SDL_WINDOW_OPENGL);

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

    /***************************************************
     * OBJECT LOADER
     **************************************************/
    vector <material> materials;
    vector <vertexData> vertexBuffer;

    bool hasUV;
    bool hasNormal;

    success &= getObjData("bunny.obj", materials, vertexBuffer, hasUV, hasNormal);

    // build out a single array of floats
    int stride = 3 + (2 * hasUV) + (3 * hasNormal);
    int vertexBufferNumBytes = stride * vertexBuffer.size() * sizeof(float);
    float * vertexBufferData = (float*)(malloc(vertexBufferNumBytes));

    int i = 0;
    // join data into an interleaved buffer
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

    vector <int> textureIDs;
    for (int mat = 0; mat < materials.size(); mat++)
    {
        int tmp;
        material m = materials[mat]; // material m;
        success &= loadTexture(m.map_Kd, tmp);
        textureIDs.push_back(tmp);
    }

    validate(success, (char*)"Setup OpenGL Program");

    /***************************************************
     * VERTEX BUFFER OBJECT
     **************************************************/
    int VBO; // vertex buffer obejct
    glGenBuffers(1, (GLuint*)&VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferNumBytes, vertexBufferData, GL_STATIC_DRAW);

    /***************************************************
     * ATTRIBUTE HANDLES
     **************************************************/
    int aPositionHandle = glGetAttribLocation(programHandle, "a_Position");
    int aUVHandle       = glGetAttribLocation(programHandle, "a_UV");
    int aNormHandle     = glGetAttribLocation(programHandle, "a_Normal");

    /***************************************************
     * UNIFORMS HANDLES
     **************************************************/
    // MVP Maxtrix stuff
    int uModelHandle      = glGetUniformLocation(programHandle, "u_Model");
    int uViewHandle       = glGetUniformLocation(programHandle, "u_View");
    int uProjHandle       = glGetUniformLocation(programHandle, "u_Proj");

    // Texture and Threshold info
    int uTextureHandle     = glGetUniformLocation(programHandle, "u_Texture");
    int uThresholdHandle   = glGetUniformLocation(programHandle, "u_Threshold");

    // Lighting Information
    int uLightPosHandle    = glGetUniformLocation(programHandle, "u_LightPos");
    int uLightColorHandle  = glGetUniformLocation(programHandle, "u_LightColor");
    int uLightNormalHandle = glGetUniformLocation(programHandle, "u_LightNormal");

    // Phong Shading stuff
    int uAmbienceHandle  = glGetUniformLocation(programHandle, "u_Ambience");
    int uDiffuseHandle  = glGetUniformLocation(programHandle, "u_Diffuse");
    int uSpecularHandle  = glGetUniformLocation(programHandle, "u_Specular");
    int uCameraHandle = glGetUniformLocation(programHandle, "u_CameraPos");

    // MVP matrix
    mat4 model;
    mat4 view;
    mat4 proj;

    // Lighting information
    float lightPos[]    = {0, 10, 0};
    float lightColor[]  = {1.0, 1.0, 1.0};
    float lightNormal[] = {0, -1, -1};

    // camera data
    myCam.camX = myCam.camY = myCam.camZ = myCam.pitch = myCam.yaw = myCam.roll = 0.0;

    int numDraw = vertexBuffer.size();
    bool running = true;

    // Main Draw Loop
    while (running)
    {
        processUserInputs(running);
        {
            // isolate the gl functions
            // clear buffers
            glClearColor(0, 0, 0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(programHandle);

            /***************************************************
             * SET UP ATTRIBUTES
             **************************************************/
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glVertexAttribPointer(aPositionHandle, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
            glEnableVertexAttribArray(aPositionHandle);

            glVertexAttribPointer(aUVHandle, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(0 + 3 * sizeof(float)));
            glEnableVertexAttribArray(aUVHandle);

            glVertexAttribPointer(aNormHandle, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(0 + 5 * sizeof(float)));
            glEnableVertexAttribArray(aNormHandle);

            /***************************************************
             * SET UP UNIFORMS
             **************************************************/
            float camPositions[] = {myCam.camX, myCam.camY, myCam.camZ};

            // Update texture
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, textureIDs[0]);

            // Texture/Threshold stuff
            glUniform1i(uTextureHandle, 0);
            glUniform1f(uThresholdHandle, threshold);

            // Settings up lighting
            glUniform3fv(uLightPosHandle, 1, lightPos);
            glUniform3fv(uLightColorHandle, 1, lightColor);
            glUniform3fv(uLightNormalHandle, 1, lightNormal);

            // Phong Shading info
            glUniform3fv(uAmbienceHandle,  1, materials[0].Ka);
            glUniform3fv(uDiffuseHandle,  1, materials[0].Kd);
            glUniform3fv(uSpecularHandle,  1, materials[0].Ks);
            glUniform3fv(uCameraHandle, 1, camPositions);

            // MVP Stuff
            setupMVP(model, view, proj);
            glUniformMatrix4fv(uModelHandle, 1, false, &model[0][0]);
            glUniformMatrix4fv(uViewHandle,  1, false, &view[0][0]);
            glUniformMatrix4fv(uProjHandle,  1, false, &proj[0][0]);

            glDrawArrays(GL_TRIANGLES, 0, numDraw);
        }

        SDL_GL_SwapWindow(win);
    }

    free(vertexBufferData);
    return 0;
}