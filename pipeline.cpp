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
 * determinant
 * Multiplies the two vertex points
 * Performs cross product
 * return float
 * ******************************************************/
double determinant(const double & v1x, const double & v2x, const double & v1y, const double & v2y)
{
    double determin;
    determin = (v1x * v2y) - (v2x * v1y);
    determin = ((v1x * v2y) - (v1y * v2x));

    return determin; 
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
* This function calculate interpolate by 3 times the area and then half the determinate
************************************************************/
double interoplate(double areaTriangle, double x, double y, double z, double attr1, double attr2, double attr3)
{
    double interpolat = 0.0;
	interpolat = (x/areaTriangle * attr3) + (y/areaTriangle * attr1) + (z/areaTriangle * attr2);
	
    return interpolat;
}
	
/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> &target, Vertex *const triangle, Attributes *const attrs, Attributes *const uniforms, FragmentShader *const frag)
{
     //finding the max and min of both are x and y axis 
     //finding the outer border of the triangle. 
    int maxX = MAX3((int)triangle[0].x, (int)triangle[1].x, (int)triangle[2].x);
    int minX = MIN3((int)triangle[0].x, (int)triangle[1].x, (int)triangle[2].x);
    int maxY = MAX3((int)triangle[0].y, (int)triangle[1].y, (int)triangle[2].y);
    int minY = MIN3((int)triangle[0].y, (int)triangle[1].y, (int)triangle[2].y);

    //Finding the vertices of the triangles
    double verOne[] = {triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y};
    double verTwo[] = {triangle[2].x -  triangle[1].x, triangle[2].y - triangle[1].y};
    double verThree[] = {triangle[0].x - triangle[2].x, triangle[0].y - triangle[2].y};

    //Finding the area of the triangle
    double areaTriangle = determinant(verOne[X_Key], -verThree[X_Key], verOne[Y_Key], -verThree[Y_Key]);

    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
          //Taking the determinant of the differnt vertexes 
          double DetOne = determinant(verOne[X_Key], x - triangle[0].x, verOne[Y_Key], y - triangle[0].y);
          double DetTwo = determinant(verTwo[X_Key], x - triangle[1].x, verTwo[Y_Key], y - triangle[1].y);
          double DetThree = determinant(verThree[X_Key], x - triangle[2].x, verThree[Y_Key], y - triangle[2].y);

            //true only when in triangle
            if (DetOne >= 0 && DetTwo >= 0 && DetThree >= 0)
            {
                Attributes interpolatedAttribs;
                double zt = 1/interoplate(areaTriangle, DetOne, DetTwo, DetThree, triangle[0].w, triangle[1].w, triangle[2].w);
                int size = (sizeof(interpolatedAttribs.attrs)/sizeof(*interpolatedAttribs.attrs));
	              for (int i = 0; i <= size; i++)
	              {
	                  interpolatedAttribs.attrs[i] = zt *interoplate(areaTriangle, DetOne, DetTwo, DetThree, attrs[0].attrs[i], attrs[1].attrs[i], attrs[2].attrs[i]);
	              }

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

        TestDrawPerspectiveCorrect(frame);

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
