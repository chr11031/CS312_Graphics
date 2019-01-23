#include "definitions.h"
#include "coursefunctions.h"

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
    /// My Code
    target[(int)v[0].y][(int)v[0].x] = attrs[0].color;
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
    // My Code (with help from the reading)
    // This finds the minimum x and y values of the vectors, creating a bouding box
    // around the triangle that we can loop through
    int maxX = MAX3(triangle[0].x, triangle[1].x, triangle[2].x);
    int minX = MIN3(triangle[0].x, triangle[1].x, triangle[2].x);
    int maxY = MAX3(triangle[0].y, triangle[1].y, triangle[2].y);
    int minY = MIN3(triangle[0].y, triangle[1].y, triangle[2].y);

    // set the origin of the colliding vectors to 0, 0. This makes the cross-product work much more simply
    Vertex * vert1 = new Vertex {triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y, 1, 1};
    Vertex * vert2 = new Vertex {triangle[2].x - triangle[0].x, triangle[2].y - triangle[0].y, 1, 1};

    // looping through each pixel in the bounding box
    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            // create a temporary vertex at current pixel in the square
            Vertex * q = new Vertex {x - triangle[0].x, y - triangle[0].y, 1, 1};

            // finds the cross product of the q vertex and the second vertex divided by the cross product of the two main vertices
            float a = (float)((q->x*vert2->y) - (q->y*vert2->x)) / ((vert1->x*vert2->y) - (vert1->y*vert2->x));
            // finds the cross product of the first vertex and the q vertex divided by the cross product of the two main vertices
            float b = (float)((vert1->x*q->y) - (vert1->y*q->x)) / ((vert1->x*vert2->y) - (vert1->y*vert2->x));

            // a >= 0 tests to see if q is in a positive direction from vertex 2
            // b >= 0 tests to see if q is in a negative direction from vertex 1 (if this and a are true, then it is between the vertices)
            // if a + b is more than 1, then q travels farther in length than either of the two vertices, so
            //    logically it must be outside the bounds of the third vertex (and vice versa)
            if (a >= 0 && b >= 0 && (a + b) <= 1)
            {
                // draw the pixel, it is within the bounds of the triangle!
                Vertex *v = new Vertex {x, y, 1, 1};
                DrawPrimitive(POINT, target, v, attrs);
                delete v;
            }

            delete q;
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

        /// My Code
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
