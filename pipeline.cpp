#include "definitions.h"
#include "coursefunctions.h"
#include <cmath>

#include <iostream>

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
    // Set the dot to our given attribute    
    //target[(int)v[0].y][(int)v[0].x] = attrs[0].color; 
}

/****************************************
 * DRAW_TRIANGLE
 * Renders a line to the screen.
 ***************************************/
void DrawLine(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    // Your code goes here
}

/**************************************************************
 * CROSS_PRODUCT
 * Calculates the cross product of two vertecies.
 *************************************************************/
float crossProduct(Vertex a, Vertex b)
{
    return (a.x * b.y) - (a.y * b.x);
}

/**************************************************************
 * DETERMINANT
 * Calculates the determinant with the four parameters.
 *************************************************************/
double determinant(double ax, double bx, double ay, double by) 
{
    // Find the area of the triangle
    return ((ax * by) - (bx * ay));
}

/**************************************************************
 * INTERP
 * Interpolates the attributes.
 *************************************************************/
double interp(double area, double det1, double det2, double det3, double attr1, double attr2, double attr3) 
{
    // Calculate the weights of each point (percentage)
    double weight1 = det1 / area;
    double weight2 = det2 / area;
    double weight3 = 1 - weight1 - weight2;
    
    // Apply the weights and return the final value
    return ((weight1 * attr1) + (weight2 * attr2) + (weight3 * attr3));
}

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
   // Bounding box
   int xMax = MAX3(triangle[0].x, triangle[1].x, triangle[2].x);
   int yMax = MAX3(triangle[0].y, triangle[1].y, triangle[2].y);
   int xMin = MIN3(triangle[0].x, triangle[1].x, triangle[2].x);
   int yMin = MIN3(triangle[0].y, triangle[1].y, triangle[2].y);

   // Vectors
   double vec1[] = {triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y};
   double vec2[] = {triangle[2].x - triangle[1].x, triangle[2].y - triangle[1].y};
   double vec3[] = {triangle[0].x - triangle[2].x, triangle[0].y - triangle[2].y};

   // Area of whole triangle
   double totalArea = determinant(vec1[0], -vec3[0], vec1[1], -vec3[1]);

   // Check every point in the bounding box
   for (int y = yMin; y < yMax; y++)
   {
        for (int x = xMin; x <= xMax; x++)
        {
            // Calculate the determinant of each of the inner triangles
            double det1 = determinant(vec1[0], x - triangle[0].x, vec1[1], y - triangle[0].y);
            double det2 = determinant(vec2[0], x - triangle[1].x, vec2[1], y - triangle[1].y);
            double det3 = determinant(vec3[0], x - triangle[2].x, vec3[1], y - triangle[2].y);

            // If in the triangle
            if (det1 >= 0 && det2 >= 0 && det3 >= 0)
            { 

                Attributes interAttr;

                // Interpolate each value
                interAttr.argb[0] = interp(totalArea, det1, det2, det3, attrs[0].argb[0], attrs[1].argb[0], attrs[2].argb[0]);
                interAttr.argb[1] = interp(totalArea, det1, det2, det3, attrs[0].argb[1], attrs[1].argb[1], attrs[2].argb[1]);
                interAttr.argb[2] = interp(totalArea, det1, det2, det3, attrs[0].argb[2], attrs[1].argb[2], attrs[2].argb[2]);
                interAttr.argb[3] = interp(totalArea, det1, det2, det3, attrs[0].argb[3], attrs[1].argb[3], attrs[2].argb[3]);

                // Call the fragment shader function previously set
                frag->FragShader(target[y][x], interAttr, *uniforms);
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

        // Test our Fragmentation
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
