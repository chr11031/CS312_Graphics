#include "definitions.h"
#include "coursefunctions.h"
#include <iostream>

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
    //target[(int)triangle[0].y][(int)triangle[0].x] = attrs[0].color;
}

/*********************************************************************
 * CROSS_PRODUCT and MIN and MAX
 * helper functions to calculate the cross product of two vertecies,
 * min, and max
 ********************************************************************/
float crossProduct(double* v1, double* v2)
{
    return (v1[0] * v2[1]) - (v1[1] * v2[0]);
}
int min(int num1, int num2)
{
    if (num1 < num2) return num1;
    return num2;
}
int max(int num1, int num2)
{
    if (num1 > num2) return num1;
    return num2;
}
Attributes inter(Attributes* const attrs, double area0, double area1, double area2)
{
    Attributes interAtt;
    /*for(int i = 0; i < attrs[0].colorAttr.size(); i++)
    {
        interAtt.colorAttr.at(i) = (attrs[0].colorAttr[i] * area0) + (attrs[1].colorAttr[i] * area1) 
            + (attrs[2].colorAttr[i] * area2);
    }*/

    interAtt.r = (attrs[0].r * area0) + (attrs[1].r * area1) + (attrs[2].r * area2);
    interAtt.g = (attrs[0].g * area0) + (attrs[1].g * area1) + (attrs[2].g * area2);
    interAtt.b = (attrs[0].b * area0) + (attrs[1].b * area1) + (attrs[2].b * area2);
    interAtt.u = (attrs[0].u * area0) + (attrs[1].u * area1) + (attrs[2].u * area2);
    interAtt.v = (attrs[0].v * area0) + (attrs[1].v * area1) + (attrs[2].v * area2);

    return interAtt;
}

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    //DrawPoint(target, triangle, attrs, uniforms, frag);
    target[(int)triangle[0].y][(int)triangle[0].x] = attrs[0].color;
    target[(int)triangle[1].y][(int)triangle[1].x] = attrs[1].color;
    target[(int)triangle[2].y][(int)triangle[2].x] = attrs[2].color;

    //bounding box to limit the amount of checking
    int maxX = max(triangle[0].x, max(triangle[1].x, triangle[2].x));
    int minX = min(triangle[0].x, min(triangle[1].x, triangle[2].x));
    int maxY = max(triangle[0].y, max(triangle[1].y, triangle[2].y));
    int minY = min(triangle[0].y, min(triangle[1].y, triangle[2].y));
    
    /*bounding edges of the triangle
     *vectors that are a measuremtnt of the edge from triangle[0] to triangle[1] 
     *and from triangle[1] to triangle[2] and from triangle[2] to triangle[0] */
    double edge0[] = {(triangle[2].x - triangle[1].x), (triangle[2].y - triangle[1].y)};
    double edge1[] = {(triangle[0].x - triangle[2].x), (triangle[0].y - triangle[2].y)};
    double edge2[] = {(triangle[1].x - triangle[0].x), (triangle[1].y - triangle[0].y)};

    //area of the whole triangle
    double negEdge1[] = {-edge1[0], -edge1[1]};
    double area = crossProduct(negEdge1, edge0);

    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y ++)
        {
            //determinates
            double temp0[] = {triangle[2].x - x, triangle[2].y - y};
            double area0 = crossProduct(temp0, edge0);
            double temp1[] = {triangle[0].x - x, triangle[0].y - y};
            double area1 = crossProduct(temp1, edge1);
            double temp2[] = {triangle[1].x - x, triangle[1].y - y};
            double area2 = crossProduct(temp2, edge2);

            if ( (area0 >= 0) && (area1 >= 0) && (area2 >= 0) )
            {
                area0 /= area; area1 /= area; area2 /= area;
                //Interpolate attributes
                Attributes interAttr = inter(attrs, area0, area1, area2);

                //frag callback
                frag -> FragShader(target[y][x], interAttr, *uniforms);
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

        // TODO Your code goes here
            TestDrawFragments(frame);
            //TestDrawTriangle(frame);
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
