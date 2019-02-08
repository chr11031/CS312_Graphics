#include "definitions.h"
#include "coursefunctions.h"
#include <vector>
#include <iostream>
#include <math.h>

using namespace std;

/***********************************************
 * CLEAR_SCREEN
 * Sets the screen to the indicated color value.
 **********************************************/
void clearScreen(Buffer2D<PIXEL> & frame, PIXEL color = 0xff000000)
{
    int h = frame.height();
    int w = frame.width();
    for(int y = 0; y < h; y++)
    {
        for(int x = 0; x < w; x++)
        {
            frame[y][x] = color;
        }
    }
}

/************************************************************
 * UPDATE_SCREEN
 * Blits pixels from RAM to VRAM for rendering.
 ***********************************************************/
void SendFrame(SDL_Texture* GPU_OUTPUT, SDL_Renderer * ren, SDL_Surface* frameBuf) 
{
    SDL_UpdateTexture(GPU_OUTPUT, NULL, frameBuf->pixels, frameBuf->pitch);
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, GPU_OUTPUT, NULL, NULL);
    SDL_RenderPresent(ren);
}

/*************************************************************
 * POLL_CONTROLS
 * Updates the state of the application based on:
 * keyboard, mouse, touch screen, gamepad inputs. 
 ************************************************************/
void processUserInputs(bool & running)
{
    SDL_Event e;
    int mouseX;
    int mouseY;
    while(SDL_PollEvent(&e)) 
    {
        if(e.type == SDL_QUIT) 
        {
            running = false;
        }
        if(e.key.keysym.sym == 'q' && e.type == SDL_KEYDOWN) 
        {
            running = false;
        }
    }
}

/****************************************
 * DRAW_POINT
 * Renders a point to the screen with the
 * appropriate coloring.
 ***************************************/
void DrawPoint(Buffer2D<PIXEL> & target, Vertex* v, Attributes* attrs, Attributes * const uniforms, FragmentShader* const frag)
{
    // Your code goes here
    target[(int)v[0].y][(int)v[0].x] = attrs[0].color;
}

/****************************************
 * DRAW_TRIANGLE
 * Renders a line to the screen.
 ***************************************/
void DrawLine(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    // Your code goes here
    for (float t = 0; t < 1; t += .01)
    {
        int x = ((int)triangle[0].x)*(1.0-t) + ((int)triangle[1].x)*t;
        int y = ((int)triangle[0].y)*(1.0-t) + ((int)triangle[1].y)*t;
        target[y][x] = attrs[0].color;
    }

    for (float t = 1; t < 2; t += .01)
    {
        int x = ((int)triangle[1].x)*(1.0-t) + ((int)triangle[2].x)*t;
        int y = ((int)triangle[1].y)*(1.0-t) + ((int)triangle[2].y)*t;
        target[y][x] = attrs[1].color;
    }

    for (float t = 2; t < 3; t += .01)
    {
        int x = ((int)triangle[2].x)*(1.0-t) + ((int)triangle[0].x)*t;
        int y = ((int)triangle[2].y)*(1.0-t) + ((int)triangle[0].y)*t;
        target[y][x] = attrs[2].color;
    }

}

/*******************************************************
 * determinant
 *   By taking the two vertex point we can times them together and
 *   Perform the cross product of the two and return the float
 * ******************************************************/
double determinant(const double & v1x, const double & v2x, const double & v1y, const double & v2y)
{
    double determin;
    determin = ((v1x * v2y) - (v1y * v2x));// /2;
    return determin; 
}

/****************************************************
 * Interpolation
 *   Area = Area + area + area
 *   Area = 1/2 Determinate
 * *****************************************************/
double interp(double areaTriangle, double firstDet, double secndDet, double thirdDet, double attrs1, double attrs2, double attrs3)
{
    //Finding where the point is in the traingle and how much color is in each part
    double interpolated = 0.0;
    interpolated = (firstDet/areaTriangle * attrs3) + (secndDet/areaTriangle * attrs1) + (thirdDet/areaTriangle * attrs2);

    return interpolated;
}

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{

    //By finding the max and min of both are x and y axis we can find the outer box of the triangle. 
    int maxX = MAX3((int)triangle[0].x, (int)triangle[1].x, (int)triangle[2].x);
    int minX = MIN3((int)triangle[0].x, (int)triangle[1].x, (int)triangle[2].x);
    int maxY = MAX3((int)triangle[0].y, (int)triangle[1].y, (int)triangle[2].y);
    int minY = MIN3((int)triangle[0].y, (int)triangle[1].y, (int)triangle[2].y);

    //Finding the vertexis of the triangles
    double firstVec[] = {triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y};
    double secndVec[] = {triangle[2].x -  triangle[1].x, triangle[2].y - triangle[1].y};
    double thirdVec[] = {triangle[0].x - triangle[2].x, triangle[0].y - triangle[2].y};

    //Find the area of the triangle that you want
    double areaTriangle = determinant(firstVec[X_Key], -thirdVec[X_Key], firstVec[Y_Key], -thirdVec[Y_Key]);
   
   //Two for loops to go through both the y and the x 
    for (int y = minY; y < maxY; y++)
    {
        for (int x = minX; x < maxX; x++)
        {

          //Take the determinant of the differnt vertexes 
          double firstDet = determinant(firstVec[X_Key], x - triangle[0].x, firstVec[Y_Key], y - triangle[0].y);
          double secndDet = determinant(secndVec[X_Key], x - triangle[1].x, secndVec[Y_Key], y - triangle[1].y);
          double thirdDet = determinant(thirdVec[X_Key], x - triangle[2].x, thirdVec[Y_Key], y - triangle[2].y);

          //IF the determinates are in the triangle
          if(firstDet >= 0 && secndDet >= 0 && thirdDet >= 0)
          {

              double zt = 1/interp(areaTriangle, firstDet, secndDet, thirdDet, triangle[0].w, triangle[1].w, triangle[2].w);

              Attributes interpolatedAttribs;
              int size = (sizeof(interpolatedAttribs.newColor)/sizeof(*interpolatedAttribs.newColor));
              for (int i = 0; i <= size; i++)
              {
                  interpolatedAttribs.newColor[i] = zt *interp(areaTriangle, firstDet, secndDet, thirdDet, attrs[0].newColor[i], attrs[1].newColor[i], attrs[2].newColor[i]);
              }
              frag->FragShader(target[y][x], interpolatedAttribs, *uniforms);

          }
        }
    }
    
}

/**************************************************************
 * VERTEX_SHADER_EXECUTE_VERTICES
 * Executes the vertex shader on inputs, yielding transformed
 * outputs. 
 *************************************************************/
void VertexShaderExecuteVertices(const VertexShader* vert, Vertex const inputVerts[], Attributes const inputAttrs[], const int& numIn, 
                                 Attributes* const uniforms, Vertex transformedVerts[], Attributes transformedAttrs[])
{
    // Defaults to pass-through behavior
    if(vert == NULL)
    {
        for(int i = 0; i < numIn; i++)
        {
            transformedVerts[i] = inputVerts[i];
            transformedAttrs[i] = inputAttrs[i];
        }
    }
}

/***************************************************************************
 * DRAW_PRIMITIVE
 * Processes the indicated PRIMITIVES type through pipeline stages of:
 *  1) Vertex Transformation
 *  2) Clipping
 *  3) Normalization
 *  4) ViewPort transform
 *  5) Rasterization & Fragment Shading
 **************************************************************************/
void DrawPrimitive(PRIMITIVES prim, 
                   Buffer2D<PIXEL>& target,
                   const Vertex inputVerts[], 
                   const Attributes inputAttrs[],
                   Attributes* const uniforms,
                   FragmentShader* const frag,                   
                   VertexShader* const vert,
                   Buffer2D<double>* zBuf)
{
    // Setup count for vertices & attributes
    int numIn = 0;
    switch(prim)
    {
        case POINT:
            numIn = 1;
            break;
        case LINE:
            numIn = 2;
            break;
        case TRIANGLE:
            numIn = 3;
            break;
    }

    // Vertex shader 
    Vertex transformedVerts[MAX_VERTICES];
    Attributes transformedAttrs[MAX_VERTICES];
    VertexShaderExecuteVertices(vert, inputVerts, inputAttrs, numIn, uniforms, transformedVerts, transformedAttrs);

    // Vertex Interpolation & Fragment Drawing
    switch(prim)
    {
        case POINT:
            DrawPoint(target, transformedVerts, transformedAttrs, uniforms, frag);
            break;
        case LINE:
            DrawLine(target, transformedVerts, transformedAttrs, uniforms, frag);
            break;
        case TRIANGLE:
            DrawTriangle(target, transformedVerts, transformedAttrs, uniforms, frag);
    }
}

/*************************************************************
 * MAIN:
 * Main game loop, initialization, memory management
 ************************************************************/
int main()
{
    // -----------------------DATA TYPES----------------------
    SDL_Window* WIN;               // Our Window
    SDL_Renderer* REN;             // Interfaces CPU with GPU
    SDL_Texture* GPU_OUTPUT;       // GPU buffer image (GPU Memory)
    SDL_Surface* FRAME_BUF;        // CPU buffer image (Main Memory) 

    // ------------------------INITIALIZATION-------------------
    SDL_Init(SDL_INIT_EVERYTHING);
    WIN = SDL_CreateWindow(WINDOW_NAME, 200, 200, S_WIDTH, S_HEIGHT, 0);
    REN = SDL_CreateRenderer(WIN, -1, SDL_RENDERER_SOFTWARE);
    FRAME_BUF = SDL_CreateRGBSurface(0, S_WIDTH, S_HEIGHT, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    FRAME_BUF = SDL_ConvertSurface(SDL_GetWindowSurface(WIN), SDL_GetWindowSurface(WIN)->format, 0);
    GPU_OUTPUT = SDL_CreateTextureFromSurface(REN, FRAME_BUF);
    BufferImage frame(FRAME_BUF);

    // Draw loop 
    bool running = true;
    while(running) 
    {           
        // Handle user inputs
        processUserInputs(running);

        // Refresh Screen
        clearScreen(frame);

        // Your code goes here
          TestDrawPerspectiveCorrect(frame);
        //TestDrawFragments(frame);
        // Push to the GPU
        SendFrame(GPU_OUTPUT, REN, FRAME_BUF);
    }

    // Cleanup
    SDL_FreeSurface(FRAME_BUF);
    SDL_DestroyTexture(GPU_OUTPUT);
    SDL_DestroyRenderer(REN);
    SDL_DestroyWindow(WIN);
    SDL_Quit();
    return 0;
}
