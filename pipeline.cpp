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
 * crossProduct
 *   By taking the two vertex point we can times them together and
 *   Perform the cross product of the two and return the float
 * ******************************************************/
float crossProduct(Vertex one, Vertex two)
{
    float crossPro = 0.0;
    crossPro = (one.x * two.y) - (one.y * two.x);
    return crossPro; 
}

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{

    //By finding the max and min of both are x and y axis we can find the outer box of the triangle. 
    int maxX = fmax((int)triangle[0].x, fmax((int)triangle[1].x, (int)triangle[2].x));
    int minX = fmin((int)triangle[0].x, fmin((int)triangle[1].x, (int)triangle[2].x));
    int maxY = fmax((int)triangle[0].y, fmax((int)triangle[1].y, (int)triangle[2].y));
    int minY = fmin((int)triangle[0].y, fmin((int)triangle[1].y, (int)triangle[2].y));

    //Finding the vertex lines between two points
    Vertex vs1;
    vs1.x = ((int)triangle[1].x - (int)triangle[0].x);
    vs1.y = ((int)triangle[1].y - (int)triangle[0].y);
    Vertex vs2;
    vs2.x = (int)triangle[2].x - (int)triangle[0].x;
    vs2.y = (int)triangle[2].y - (int)triangle[0].y;
   
   //Two for loops to go through both the y and the x 
    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            //Our original vertex that we take the  x and y positing that we are at in the box
            Vertex q;
            q.x = (x - (int)triangle[0].x);
            q.y = (y - (int)triangle[0].y);

            //We find the cross product of each part and then divid them to find were we are if we are in the triangle
            float s = (float)crossProduct(q, vs2) / crossProduct(vs1, vs2);
            float t = (float)crossProduct(vs1, q) / crossProduct(vs1, vs2);

            //If we are in the traingle these conditions will be true
            if ( (s >= 0) && (t >= 0) && (s + t <= 1))
            {
                //set the point that we are at to send to the draw point
                Vertex myPoint[1];
                myPoint[0] = { x , y };
                DrawPoint(target, myPoint, attrs, uniforms, frag);
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
        //GameOfLife(frame);
        //TestDrawPixel(frame);
        TestDrawTriangle(frame);

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
