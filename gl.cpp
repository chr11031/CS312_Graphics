#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "gl.h"

//g++ gl.cpp -lopengl32 -lglew32 -lSDL2

int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow("OpenGL Program", 200, 200,
                                      SCREEN_W, SCREEN_H, SDL_WINDOW_OPENGL);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    SDL_GL_SetSwapInterval(1);
    SDL_GLContext cont = SDL_GL_CreateContext(win);

    glewInit();
    glEnable(GL_DEPTH_TEST);

    bool success = true;
    string vertexShader;
    string fragShader;
    int vertexHandle;
    int fragHandle;
    int programHandle;

    success &= parseFile((char*)"vertex.vs", vertexShader);
    success &= parseFile((char*)"fragment.fs", fragShader); 
    success &= compileShader(vertexShader.c_str(), GL_VERTEX_SHADER, vertexHandle);
    success &= compileShader(fragShader.c_str(), GL_FRAGMENT_SHADER, fragHandle);
    success &= compileProgram(vertexHandle, fragHandle, programHandle);

    /********************************************************************
    * Object Loader
    * *******************************************************************/
   vector<material> materials;
   vector<vertexData> vertexBuffer;

   bool hasUV;
   bool hasNormal;

   success &= getObjData("bunny.obj", materials, vertexBuffer, hasUV, hasNormal);

   // Build out a signle array of floats
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

    vector<int> textureIDs;
    for(int mat = 0; mat < materials.size(); mat++)
    {
        int tmp;
        material m = materials[mat];
        success &= loadTexture(m.map_Kd, tmp);
        textureIDs.push_back(tmp);
    }

    validate(success, (char*)"Setup OpenGL Program\n");


    /**************************************************************
     *  Vertex Buffer Object
     * ***********************************************************/
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertexBufferNumBytes, vertexBufferData, GL_STATIC_DRAW);

    /**************************************************************
     *  Attributes Handles
     * ***********************************************************/
    int aPositionHandle = glGetAttribLocation(programHandle, "a_Position");
    int aUVHandle = glGetAttribLocation(programHandle, "a_UV");
    int aNormHandle = glGetAttribLocation(programHandle, "a_Norm");

    /**************************************************************
     *  Uniforms Handle
     * ***********************************************************/
    int uModelHandle = glGetUniformLocation(programHandle, "u_Model");
    int uViewHandle = glGetUniformLocation(programHandle, "u_View");
    int uProjHandle = glGetUniformLocation(programHandle, "u_Proj");
    int uTextureHandle = glGetUniformLocation(programHandle, "u_Texture");
    int uThresholdHandle = glGetUniformLocation(programHandle, "u_Threshold");
    int uLightPositionHandle = glGetUniformLocation(programHandle, "u_LightPosition");
    int uLightColorHandle = glGetUniformLocation(programHandle, "u_LightColor");
    int uKaHandle = glGetUniformLocation(programHandle, "u_Ka");
    int uKdHandle = glGetUniformLocation(programHandle, "u_Kd");
    int uKsHandle = glGetUniformLocation(programHandle, "u_Ks");
    int uCamHandle= glGetUniformLocation(programHandle, "u_Camera");

    // MVP MAtrix
    mat4 model;
    mat4 proj;
    mat4 view;

    float lightPos[] = {0.0, 10.0, 30.0};
    float lightColor[] = {1.0, 1.0, 1.0};

    // Setup Camera Data
    myCam.camX = myCam.camY = myCam.camZ = myCam.pitch = myCam.yaw = myCam.roll = 0.0;

    int numDraw = vertexBuffer.size();
    bool running = true;

    // Main Draw Loop
    while(running)
    {
        processUserInputs(running);
        {
            // Clear buffers
            glClearColor(0, 0, 0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(programHandle);

            /**********************************
             *  Setup Attributes
             * *******************************/
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glVertexAttribPointer(aPositionHandle, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
            glEnableVertexAttribArray(aPositionHandle);

            glVertexAttribPointer(aUVHandle, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0 + 3*sizeof(float));
            glEnableVertexAttribArray(aUVHandle);

            glVertexAttribPointer(aNormHandle, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0 + 5*sizeof(float));
            glEnableVertexAttribArray(aNormHandle);


            /**********************************
             *  Setup Uniforms
             * *******************************/
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, textureIDs[0]);

            glUniform1i(uTextureHandle, 0);
            glUniform1f(uThresholdHandle, threshold);

            glUniform3fv(uLightPositionHandle, 1, lightPos);
            glUniform3fv(uLightColorHandle, 1, lightColor);
            glUniform3fv(uKaHandle, 1, (float*)&materials[0].Ka);
            glUniform3fv(uKdHandle, 1, (float*)&materials[0].Kd);
            glUniform3fv(uKsHandle, 1, (float*)&materials[0].Ks);

            float cam[] = {myCam.camX, myCam.camY, myCam.camZ};
            glUniform3fv(uCamHandle, 1, cam);

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