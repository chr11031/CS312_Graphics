// To compile: g++ file.cpp -lopengl32 -lglew32 -lSDL2 -lmingw32
// For setup download the following libraries:
// 1) glm headers folder
// 2) glew

// Essentials
#define SDL_MAIN_HANDLED
#define SCREEN_W 640.0f
#define SCREEN_H 480.0f
#include "SDL2/SDL.h"
#include "glm/glm.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
using glm::mat4;
using glm::vec3;
using glm::vec2;

#include "gl/glew.h"
#include "SDL2/SDL_opengl.h"

// Friendly, Other libraries
#include "stdio.h"
#include "stdlib.h"
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

struct userCamera
{
    float mX;
    float mY;
    float mZ;
    
    float rX;
    float rY;
    float rZ;
    
    float SLIDER;
} gCam;

bool getObjVao(char * objFile, float *& buf, int & buffLen)
{
    buf = NULL;
    buffLen = 0;
    std::vector<float> vao;
    
    std::vector<vec3> vertexIndex;
    std::vector<vec2> uvIndex;
    std::vector<vec3> normalIndex;
    
    FILE* fp;
    fp = fopen(objFile, "r");
    
    if(fp == NULL)
    {
        std::cout << "OBJ: " << objFile << " could not be opened\n";
        return false;
    }
    
    for(;;)
    {
        char lineHeader[256];
        int res = fscanf(fp, "%s", lineHeader);
        
        if(res == EOF) // Check EOF
        {
            break;
        }
        if(strcmp(lineHeader, "v") == 0) // Check for "V" Vertex data
        {
            vec3 vertex;
            fscanf(fp, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            vertexIndex.push_back(vertex);
        }
        else if(strcmp(lineHeader, "vt") == 0) // Check for "VT" UV Data
        {
            vec2 uv;
            fscanf(fp, "%f %f\n", &uv.x, &uv.y);
            uvIndex.push_back(uv);
        }
        else if(strcmp(lineHeader, "vn") == 0) // Check for "VN" Normal data
        {
            vec3 normal;
            fscanf(fp, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            normalIndex.push_back(normal);
        }
        else if(strcmp(lineHeader, "f") == 0) // Generate faces "F" to VAO buff
        {
            unsigned int vertexI[3], uvI[3], normalI[3];
            
            int matches = fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                                    &vertexI[0], &uvI[0], &normalI[0],
                                    &vertexI[1], &uvI[1], &normalI[1],
                                    &vertexI[2], &uvI[2], &normalI[2]);
            if(matches != 9)
            {
                std::cout << "File can't be read by our simple parser.... :(\n";
                return false;
            }
            
            // Generate 3 triangles at a time... with 3XYZ 2UV values
            for(int i = 0; i < 3; i++)
            {
                vao.push_back(vertexIndex[vertexI[i]-1].x);
                vao.push_back(vertexIndex[vertexI[i]-1].y);
                vao.push_back(vertexIndex[vertexI[i]-1].z);
                vao.push_back(1.0); // "W" Variable added by 4x4 convention - always 1.0
                vao.push_back(uvIndex[uvI[i]-1].x);
                vao.push_back(uvIndex[uvI[i]-1].y);
                buffLen++;
            }
        }
    }
    int a = vertexIndex.size();
    int b = uvIndex.size();
    int c = normalIndex.size();
    
    // Generate buffer
    int len = vao.size();
    buf = (float*)malloc(len * sizeof(float));
    for(int i = 0; i < len; i++)
    {
        buf[i] = vao[i];
    }
    
    fclose(fp);
    return true;
}
bool input(bool & ongoing)
{
    float stepSize = 0.1;
    
    // Increase rotation values based on Mouse
    int mouseX, mouseY;
    SDL_GetRelativeMouseState(&mouseX, &mouseY);
    gCam.rX -= mouseX * stepSize * stepSize;
    gCam.rY += mouseY * stepSize * stepSize;
    while(gCam.rX >= (M_PI*2.0))
    {
        gCam.rX -= (M_PI*2.0);
    }
    while(gCam.rX <= -(M_PI*2.0))
    {
        gCam.rX += (M_PI*2.0);
    }
    while(gCam.rY >= (M_PI*2.0))
    {
        gCam.rY -= (M_PI*2.0);
    }
    while(gCam.rY <= -(M_PI*2.0))
    {
        gCam.rY += (M_PI*2.0);
    }
    // Update other values based on keyboard
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            ongoing = false;
        }
        
        if(event.type == SDL_KEYDOWN)
        {
            switch(event.key.keysym.sym)
            {
                // HACK
                case SDLK_q:
                gCam.SLIDER += 0.01;
                break;
                case SDLK_z:
                gCam.SLIDER -= 0.01;
                break;
                
                case SDLK_a:
                gCam.mX -= stepSize;
                break;
                case SDLK_d:
                gCam.mX += stepSize;
                break;
                
                case SDLK_w:
                gCam.mY -= stepSize;
                break;
                case SDLK_s:
                gCam.mY += stepSize;
                break;
                
                case SDLK_f:
                gCam.mZ -= stepSize;
                break;

                case SDLK_r:
                gCam.mZ += stepSize;
                break;
            }
        }
    }
}

bool parseFile(char* ref, std::string & out)
{
    // Check for read success
    std::ifstream fin(ref);
    if(fin.fail())
    {
        std::cout << "Failed to read file " << ref << std::endl;
        return false;
    }
    
    // Dump in file
    std::string tmp;
    while(getline(fin, tmp))
    {
        out += tmp;
        out += "\n";
    }
    fin.close();
    return true;
}

bool compileShader(std::string shader, int type, int & handle)
{
    int shaderID = glCreateShader(type);
    if(shaderID == 0)
    {
        std::cout << "Could not create shader of type " << type << std::endl;
        return false;
    }
    
    const char* src = shader.c_str();
    glShaderSource(shaderID, 1, &src, NULL);
    glCompileShader(shaderID);
    
    int status;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
    if(status == 0)
    {
        std::cout << "Error Compiling shader of type " << type << std::endl;
        return false;
    }

    handle = shaderID;
    return true;
}

bool compileProgram(int vertexID, int fragID, int & handle)
{
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexID);
    glAttachShader(shaderProgram, fragID);
    glLinkProgram(shaderProgram);
    
    int status;
    SDL_Delay(10);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if(status == 0)
    {
        std::cout << "Error Linking shaders" << std::endl;
        return false;
    }
    handle = shaderProgram;
    return true;
}

void validate(bool success, char * message)
{
    //glGetProgramInfoLog(program, 1024, &log_length, message);COULD BE USEFUL LATER
    success |= glGetError();
    if(success)
    {
        std::cout << "SUCCESS: " << message << std::endl;
    }
    else
    {
        std::cout << "FAILURE: " << message << std::endl;
        exit(1);
    }
}

bool loadTexture(char * fileName, int & handle)
{
    // SDL-Specific part
    SDL_Surface* loadedImg = SDL_LoadBMP(fileName);
    if(loadedImg == NULL)
    {
        std::cout << "FAILURE TO LOAD FILE THROUGH SDL2 " << fileName << std::endl;
        return false;
    }
    loadedImg = SDL_ConvertSurfaceFormat(loadedImg, SDL_PIXELFORMAT_RGB24, 0);
    
    // OpenGL bindings
    int textureID;
    glGenTextures(1, (GLuint*)&textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, loadedImg->w, loadedImg->h, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)loadedImg->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind from current call
    int err = glGetError();
    if(err != GL_NO_ERROR)
    {
        std::cout << "FAILURE TO INITIALIZE IMAGE IN OPENGL. Returned: " << err << std::endl;
        return false;
    }
    
    handle = textureID;
    return true;
}

void displayMat(mat4 mat)
{
    for(int x = 0; x < 4; x++)
    {
        for(int y = 0; y < 4; y++)
        {
            std::cout << mat[y][x] << "\t";
        }
        std::cout << std::endl;
    }
}

void setupViewTransform(mat4 & mvp)
{
    // Identity Matrix
    
    mat4 identity(1.0); // Identity matrix
    mat4 proj = glm::perspective(glm::radians(60.0f), SCREEN_W / SCREEN_H, 0.1f, 100.0f); // Perspective matrix
    
    mat4 transMat = glm::translate(glm::mat4(), glm::vec3(gCam.mX, gCam.mY, gCam.mZ));
    mat4 rotMat;
    rotMat = glm::rotate(glm::mat4(), gCam.rY/*glm::radians(-60.0f)*/, glm::vec3(1.0f, 0.0f, 0.0f));
    rotMat = glm::rotate(rotMat, gCam.rX, glm::vec3(0.0f, 0.0f, 1.0f));
    
    // M*V*P is in reverse order in Matrix Math
    //mvp = proj * identity;
    mvp = proj * rotMat * transMat;
    return;
    
    // TRANS, ROT, MAT Examples
    // mat4 scalingMat = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f)); // Scale *2 in all dimensions
    // mat4 transMat   = glm::translate(glm::mat4(), glm::vec3(10.0f, 0.0f, 0.0f));
    // mat4 rotMat     = glm::rotate(30.0f, glm::vec3(1.0, 0.0, 0.0));
}

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * win = SDL_CreateWindow("OpenGL Shaders", 200, 200, SCREEN_W, SCREEN_H, SDL_WINDOW_OPENGL);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetSwapInterval(1);
    SDL_GLContext cont = SDL_GL_CreateContext(win);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    glewInit(); // >:( 
    glEnable(GL_DEPTH_TEST);
    
    // Enable AA
    glEnable(GL_MULTISAMPLE_ARB);
    glEnable(GL_MULTISAMPLE);
    
    // Setup camera variable
    gCam.SLIDER = gCam.mX = gCam.mY = gCam.mZ = gCam.rX = gCam.rY = gCam.rZ = 0.0;
    gCam.SLIDER = 1.0;
    bool success = true;
    std::string vertexShader;
    std::string fragmentShader;
    int vertexID;
    int fragID;
    int programID;
    int textureID;
    int textureOtherID;
    /*success &= loadTexture("Rust.bmp", textureID);
    success &= loadTexture("MASK.bmp", textureOtherID);
    success &= parseFile((char*)"vertex.vs", vertexShader);
    success &= parseFile((char*)"fragment.fs" , fragmentShader);
    */
    success &= compileShader(vertexShader.c_str(), GL_VERTEX_SHADER, vertexID);
    success &= compileShader(fragmentShader.c_str(), GL_FRAGMENT_SHADER, fragID);
    success &= compileProgram(vertexID, fragID, programID);
    
    // Establish shader-specific variables
    int uMatrixLocation = glGetUniformLocation(programID, "u_Matrix");
    int uTextureUnitLocation = glGetUniformLocation(programID, "u_TextureUnit");
    int uTextureUnitOtherLocation = glGetUniformLocation(programID, "u_TextureUnitOther");
    int aPositionLocation = glGetAttribLocation(programID, "a_Position");
    int aTextureCoordinatesLocation = glGetAttribLocation(programID, "a_TextureCoordinates");
    
    // HACK
    int uSLIDER = glGetUniformLocation(programID, "u_SLIDER");
    // Cube data
    float *cubeData;
    int cubeSize;
    success &= getObjVao("batman.obj", cubeData, cubeSize);
    
    // Table Data as a VAO
    float tableData[] = 
    {
        // X, Y, Z, W, S, T
        -0.5, 0.5, 0.0, 1.0, 0.0, 0.0,
        -0.5, -0.5, 0.0, 1.0, 1.0, 0.0,
        0.5, -0.5, 0.0, 1.0, 1.0, 1.0,
        
        0.5, -0.5, 0.0, 1.0, 1.0, 1.0,
        0.5, 0.5, 0.0, 1.0, 0.0, 1.0,
        -0.5, 0.5, 0.0, 1.0, 0.0, 0.0
        };
        
        // Validate and Start use of this shader program
        validate(success, (char*)"Generate GLSL Program");
        bool ongoing = true;
        while(ongoing)
        {
            // Update input
            input(ongoing);
            
            // OPENGL
            {
                glClearColor(0,0,0, 1.0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glUseProgram(programID);
                // Set Attribute data
                int POSITION_COMPONENT_COUNT = 4;
                int TEXTURE_COORDINATES_COMPONENT_COUNT = 2;
                int STRIDE = (POSITION_COMPONENT_COUNT + TEXTURE_COORDINATES_COMPONENT_COUNT) * sizeof(float);
                glVertexAttribPointer(aPositionLocation, POSITION_COMPONENT_COUNT, GL_FLOAT,
                                        false, STRIDE, cubeData/*tableData*/);
                glEnableVertexAttribArray(aPositionLocation);
                glVertexAttribPointer(aTextureCoordinatesLocation, TEXTURE_COORDINATES_COMPONENT_COUNT, GL_FLOAT,
                                        false, STRIDE, /*tableData*/cubeData + POSITION_COMPONENT_COUNT);
                glEnableVertexAttribArray(aTextureCoordinatesLocation);
                
                // Set Uniform data
                // Setup texture
                glActiveTexture(GL_TEXTURE0); // + "i" to change texture chosen
                glBindTexture(GL_TEXTURE_2D, textureID);
                glUniform1i(uTextureUnitLocation, 0);
                
                glActiveTexture(GL_TEXTURE0 + 1);
                glBindTexture(GL_TEXTURE_2D, textureOtherID);
                glUniform1i(uTextureUnitOtherLocation, 0 + 1);
                
                glUniform1f(uSLIDER, gCam.SLIDER);
                
                // Update matrix
                mat4 mvp;
                setupViewTransform(mvp);
                glUniformMatrix4fv(uMatrixLocation, 1, false, &mvp[0][0]);
                
                // Output what we have
                int totalVertices = cubeSize;
                glDrawArrays(GL_TRIANGLES, 0, totalVertices);
            }

            // Update SDL buffer
            SDL_GL_SwapWindow(win);
        }
        return 0;
    }
    
    // FRAG SHADER (seperate file)
    precision mediump float;
    uniform sampler2D u_TextureUnit;
    uniform sampler2D u_TextureUnitOther;
    
    uniform float u_SLIDER;
    
    varying vec2 v_TextureCoordinates;
    
    void main()
    {
        vec4 sample = texture2D(u_TextureUnit, v_TextureCoordinates);
        vec4 mask = texture2D(u_TextureUnitOther, v_TextureCoordinates);
        if(sample.r >= u_SLIDER)
        {
            sample.g = (sample.b + sample.r + sample.g) / 3.0;
            sample.b = 0.0; 511   sample.r = 0.0;
            //discard;
        }
        gl_FragColor = sample;
    }
    
    // FRAG SHADER (seperate file)
    // VERTEX SHADER
    uniform mat4 u_Matrix;
    
    attribute vec4 a_Position;
    attribute vec2 a_TextureCoordinates;
    varying vec2 v_TextureCoordinates;
    
    void main()
    {
        v_TextureCoordinates = a_TextureCoordinates;
        gl_Position = u_Matrix * a_Position;
    }