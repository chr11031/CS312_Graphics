#include "definitions.h"
#include "coursefunctions.h"
#include <iostream>

/*********************************************************
 * FIND_DETERMINANT
 * INPUTS: Vertex, Vertex
 * OUTPUTS: double
 * Finds and returns the determinant of the two given vertices.
 * *******************************************************/
double determinant(const double &A, const double &B, const double &C, const double &D)
{
    return ((A * D) - (B * C));
}

double interpolateZ(const double & area, const double & det1, const double & det2, const double & det3,
                  const Vertex vertices[3])
{
    double w1 = det1 / area;
    double w2 = det2 / area;
    double w3 = 1 - w1 - w2;

    double fractionalZ = vertices[0].w * w2 +
                         vertices[1].w * w3 +
                         vertices[2].w * w1;
    return 1 / fractionalZ;
}

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

//double interpolation(const double &w1, const double &w2)

/****************************************
 * DRAW_POINT
 * Renders a point to the screen with the
 * appropriate coloring.
 ***************************************/
void DrawPoint(Buffer2D<PIXEL> & target, Vertex* v, Attributes* attrs, Attributes * const uniforms, FragmentShader* const frag)
{
    frag->FragShader(target[(int)v[0].y][(int)v[0].x], *attrs, *uniforms);
}

/****************************************
 * DRAW_TRIANGLE
 * Renders a line to the screen.
 ***************************************/
void DrawLine(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    // Your code goes here
}

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    Vertex boxMax;
    Vertex boxMin;

    boxMax = {
        MAX3(triangle[0].x, triangle[1].x, triangle[2].x),
        MAX3(triangle[0].y, triangle[1].y, triangle[2].y)
    };

    boxMin = {
        MIN3(triangle[0].x, triangle[1].x, triangle[2].x),
        MIN3(triangle[0].y, triangle[1].y, triangle[2].y)
    };


    // vectors necessary to get interpolation weights 
    Vertex vector01 = {triangle[1].x - triangle[0].x,
                      triangle[1].y - triangle[0].y};
    Vertex vector12 = {triangle[2].x - triangle[1].x,
                      triangle[2].y - triangle[1].y};
    Vertex vector20 = {triangle[0].x - triangle[2].x,
                      triangle[0].y - triangle[2].y};

    double area = determinant(vector01.x, -vector20.x, vector01.y, -vector20.y);

    // Obtained from the point we are looking at in the loop
    Vertex pointVector;

    // Variables to hold the weights
    double det1;
    double det2;
    double det3;
    for (int y = boxMin.y; y <= boxMax.y; y++)
    {
        for (int x = boxMin.x; x <= boxMax.x; x++)
        {
            // Obtain the first determinant
            pointVector = {x - triangle[0].x, y - triangle[0].y};
            det1 = determinant(vector01.x, pointVector.x, vector01.y, pointVector.y);

            // Obtain the second determinant
            pointVector = {x - triangle[1].x, y - triangle[1].y};
            det2 = determinant(vector12.x, pointVector.x, vector12.y, pointVector.y);

            // Obatin the third determinant
            pointVector = {x - triangle[2].x, y - triangle[2].y};
            det3 = determinant(vector20.x, pointVector.x, vector20.y, pointVector.y);


            if ((det1 >= 0.0) && (det2 >= 0.0) && (det3 >= 0.0))
            {
                double correctedZ = 0.0;
                Attributes interpolatedAttrs;
                interpolatedAttrs.valuesToInterpolate = attrs[0].valuesToInterpolate;

                correctedZ = interpolateZ(area, det1, det2, det3, triangle);
                interpolatedAttrs.interpolateValues(area, det1, det2, det3, attrs);
                interpolatedAttrs.correctPerspective(correctedZ);

                frag->FragShader(target[y][x], interpolatedAttrs, *uniforms);
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

    BufferImage bmpImage("c.bmp");

    // Draw loop 
    bool running = true;
    while(running) 
    {           
        //GameOfLife(frame);
        
        // Handle user inputs
        processUserInputs(running);

        // Refresh Screen
        clearScreen(frame);

        // Your code goes here
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
