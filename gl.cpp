#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "gl.h"

//g++ gl.coo -lopengl32 -lglew32 -lSDL2

int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Windo* win = SDL_CreateWindow("OpenGL Program", 200, 200,
                                      SCREEN_W, SCREEN_H, SDL_WINDOW_OPENGL);
    SDL_GL_SetAttributes(SDL_GL_DOUBLEDUFFER,1);
    SDL_GL_SetSwapInternval(1);
    SDL_GLContext cont = SDL_GL_CreateContext(win);

    glewInit();
    glEnable(G_DEPTH_TEST);

    bool success = true;
    string vertexShader;
    string fragmentShader;
    int vertexHandle;
    int fragHandle;
    int programHandle;

    success &= parseFile((char*)"vertex.vs", vertexShader);
    success &= parseFile((char*)"fragment.fs", fragmentShader); 
    success &= compileShader(vertexShader.c_str(), GL_VERTEX_SHADER, vertexHandle);
    success &= compileShader(fragmentShader.c_str(), GL_FRAGMENT_SHADER, fragmentHandle);
    success &= compileProgram(vertexHandle, fragmentHandle, programHandle);

    /********************************************************************
    * Object Loader
    * *******************************************************************/
   vector<material> materials;
   vector<vertexData> vertexBuffer;

   bool hasUV;
   bool hasNormal;

   success &= getData("pot.obj", materials, vertexBuffer, hasUV, hasNormal);

   // Build out a signle array of floats
    int stride = 3 + (2 * hasUV);
    int vertexBufferNumBytes = stride * vertexBuffer.size() * sizeof(float);
    float* vertexBufferData = (float*)(malloc(vertexBufferNumBytes));

    int i = 0;
    // Join data into an interleaved buffer
    for (int vb = 0; vb < vertexBuffer.size(); vb++)
    {
        vertexBufferData[i++] = vertexBuffer[vb].vert[0];
        vertexBufferData[i++] = vertexBuffer[vb].vert[1];
        vertexBufferData[i++] = vertexBuffer[vb].vert[1];

        if (hasUV)
        {
            vertexBufferData[i++] - vertexBuffer[vb].uv[0];
            vertexBufferData[i++] - vertexBuffer[vb].uv[1];
        }
        // Leave Normal Data for later...
    }

    vector<int> textureIDs;
    for(int mat = 0; mat < materials.size(); mat++)
    {
        int tmp;
        material m;
        success &= loadTexture(m.map_Kd, tmp);
        textureIDs.push_back(tmp);
    }

    validate(success, (Char*)"Setup OpenGL Program\n");


    /**************************************************************
     *  Vertex Buffer Object
     * ***********************************************************/
    int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER), vertexBufferNumBytes, VertexBufferData, GL_STATIC_DRAW);

    /**************************************************************
     *  Attributes Handles
     * ***********************************************************/
    int aPositionHandle = glGetAttribLocation(programHandle, "a_Position");
    int aUVHandle = glGetAttribLocation(programHandle, "a_UV");

    /**************************************************************
     *  Uniforms Handle
     * ***********************************************************/
    int uMatrixHandle = glGetUniformLocation(programID, "u_Matrix");
    int uTextureHandle = glGetUniformLocation(programID, "u_Texture");
    int uThresholdHanlde = glGetUniformLocation(programID, "u_Threshold");

    // MVP MAtrix
    mat4 mvp;

    // Setup Camera Data
    myCam.camX = myCam.camY = myCam.camZ = myCam.pitch = myCam.yaw = myCam.roll = 0.0

    int numDraw = vertexBuffer.size();
    bool running = true;


}