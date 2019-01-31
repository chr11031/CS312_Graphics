#include "definitions.h"
#include "coursefunctions.h"
#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

/***********************************************
 * CLEAR_SCREEN
 * Sets the screen to the indicated color value.
 **********************************************/
void clearScreen(Buffer2D<PIXEL> &frame, PIXEL color = 0xff000000)
{
    int h = frame.height();
    int w = frame.width();
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            frame[y][x] = color;
        }
    }
}

/************************************************************
 * UPDATE_SCREEN
 * Blits pixels from RAM to VRAM for rendering.
 ***********************************************************/
void SendFrame(SDL_Texture *GPU_OUTPUT, SDL_Renderer *ren, SDL_Surface *frameBuf)
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
void processUserInputs(bool &running)
{
    SDL_Event e;
    int mouseX;
    int mouseY;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
        {
            running = false;
        }
        if (e.key.keysym.sym == 'q' && e.type == SDL_KEYDOWN)
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
void DrawPoint(Buffer2D<PIXEL> &target, Vertex *v, Attributes *attrs, Attributes *const uniforms, FragmentShader *const frag)
{
    // Set our pixel according to the attribute value!
    target[(int)v[0].y][(int)v[0].x] = attrs[0].color;
}

/****************************************
 * DRAW_TRIANGLE
 * Renders a line to the screen.
 ***************************************/
void DrawLine(Buffer2D<PIXEL> &target, Vertex *const triangle, Attributes *const attrs, Attributes *const uniforms, FragmentShader *const frag)
{
    // Your code goes here
    for (float t = 0; t < 1; t += .01)
    {
        int x = ((int)triangle[0].x) * (1.0 - t) + ((int)triangle[1].x) * t;
        int y = ((int)triangle[0].y) * (1.0 - t) + ((int)triangle[1].y) * t;
        target[y][x] = attrs[0].color;
    }

    for (float t = 1; t < 2; t += .01)
    {
        int x = ((int)triangle[1].x) * (1.0 - t) + ((int)triangle[2].x) * t;
        int y = ((int)triangle[1].y) * (1.0 - t) + ((int)triangle[2].y) * t;
        target[y][x] = attrs[1].color;
    }

    for (float t = 2; t < 3; t += .01)
    {
        int x = ((int)triangle[2].x) * (1.0 - t) + ((int)triangle[0].x) * t;
        int y = ((int)triangle[2].y) * (1.0 - t) + ((int)triangle[0].y) * t;
        target[y][x] = attrs[2].color;
    }
}

/*******************************************************
* crossProduct
* Calculate the float by a cross product formula of the vertex
* Returns crossPro
* ******************************************************/
float crossProduct(Vertex one, Vertex two)
{
    float crossPro = 0.0;
    crossPro = (one.x * two.y) - (one.y * two.x);
    return crossPro;
}

/*************************************************************
* INTERPOLATE
* This function will x and y xoordinates the required pixel and weight to the
* of the triangle 
************************************************************/
double interoplate(double y, double x, Vertex* const triangle, double attr1, double attr2, double attr3)
{
	 // calculating how much color goies in the first point of the triangle
	double i = ((triangle[1].y - triangle[2].y) * (x - triangle[2].x) + (triangle[2].x - triangle[1].x) * (y - triangle[2].y)) / ((triangle[1].y - triangle[2].y) * (triangle[0].x - triangle[2].x) + (triangle[2].x - triangle[1].x) * (triangle[0].y - triangle[2].y));
    // calculating how much color goies in the second point of the triangle
    double j = ((triangle[2].y - triangle[0].y) * (x - triangle[2].x) + (triangle[0].x - triangle[2].x) * (y - triangle[2].y)) / ((triangle[1].y - triangle[2].y) * (triangle[0].x - triangle[2].x) + (triangle[2].x - triangle[1].x) * (triangle[0].y - triangle[2].y));
    // calculating how much color goies in the third point of the triangle which is 100% - i and j
    double k = 1 - i - j;

    //get the three attributes and return the color interpolated attributes
    return ((i * attr1) + (j * attr2) + (k * attr3));
}
	
/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> &target, Vertex *const triangle, Attributes *const attrs, Attributes *const uniforms, FragmentShader *const frag)
{
    // Your code goes here
    int maxX = MAX3((int)triangle[0].x, (int)triangle[1].x, (int)triangle[2].x);
    int minX = MIN3((int)triangle[0].x, (int)triangle[1].x, (int)triangle[2].x);
    int maxY = MAX3((int)triangle[0].y, (int)triangle[1].y, (int)triangle[2].y);
    int minY = MIN3((int)triangle[0].y, (int)triangle[1].y, (int)triangle[2].y);

    //Vertex lines points of verOne
    Vertex verOne;
    verOne.x = ((int)triangle[1].x - (int)triangle[0].x);
    verOne.y = ((int)triangle[1].y - (int)triangle[0].y);

    //Vertex lines points of verTwo
    Vertex verTwo;
    verTwo.x = (int)triangle[2].x - (int)triangle[0].x;
    verTwo.y = (int)triangle[2].y - (int)triangle[0].y;

    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            Vertex vertQ;
            vertQ.x = (x - (int)triangle[0].x);
            vertQ.y = (y - (int)triangle[0].y);

            //Making sure if we lie inside the trangle by finding the cross product
            float s = (float)crossProduct(vertQ, verTwo) / crossProduct(verOne, verTwo);
            float t = (float)crossProduct(verOne, vertQ) / crossProduct(verOne, verTwo);

            //true only when in triangle
            if ((s >= 0) && (t >= 0) && (s + t <= 1))
            {
                Attributes interpolatedAttribs;

                // Interpolate Attributes for this pixel - In this case the R,G,B values as attr 0,1,2 respeectively
	            interpolatedAttribs.attr[0] = interoplate(y, x, triangle, attrs[0].attr[0],attrs[1].attr[0],attrs[2].attr[0]);
	            interpolatedAttribs.attr[1] = interoplate(y, x, triangle, attrs[0].attr[1],attrs[1].attr[1],attrs[2].attr[1]);
	            interpolatedAttribs.attr[2] = interoplate(y, x, triangle, attrs[0].attr[2],attrs[1].attr[2],attrs[2].attr[2]);

                //Interpolate Attributes for this pixel - in this case U and V coordinate as attr 3, 4 respectively
	            interpolatedAttribs.attr[3] = interoplate(y, x, triangle, attrs[0].attr[0],attrs[1].attr[0], attrs[2].attr[0]);
	            interpolatedAttribs.attr[4] = interoplate(y, x, triangle, attrs[0].attr[1],attrs[1].attr[1], attrs[2].attr[1]);
                    
                //calling the shader callback
                frag->FragShader(target[y][x],interpolatedAttribs, *uniforms);
            }
        }
    }
}

/**************************************************************
 * VERTEX_SHADER_EXECUTE_VERTICES
 * Executes the vertex shader on inputs, yielding transformed
 * outputs. 
 *************************************************************/
void VertexShaderExecuteVertices(const VertexShader *vert, Vertex const inputVerts[], Attributes const inputAttrs[], const int &numIn,
                                 Attributes *const uniforms, Vertex transformedVerts[], Attributes transformedAttrs[])
{
    // Defaults to pass-through behavior
    if (vert == NULL)
    {
        for (int i = 0; i < numIn; i++)
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
                   Buffer2D<PIXEL> &target,
                   const Vertex inputVerts[],
                   const Attributes inputAttrs[],
                   Attributes *const uniforms,
                   FragmentShader *const frag,
                   VertexShader *const vert,
                   Buffer2D<double> *zBuf)
{
    // Setup count for vertices & attributes
    int numIn = 0;
    switch (prim)
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
    switch (prim)
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
    SDL_Window *WIN;         // Our Window
    SDL_Renderer *REN;       // Interfaces CPU with GPU
    SDL_Texture *GPU_OUTPUT; // GPU buffer image (GPU Memory)
    SDL_Surface *FRAME_BUF;  // CPU buffer image (Main Memory)

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
    while (running)
    {
        // Handle user inputs
        processUserInputs(running);

        // Refresh Screen
        clearScreen(frame);

        //TestDrawPixel(frame);
        //TestDrawTriangle(frame);
        TestDrawFragments(frame);

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
