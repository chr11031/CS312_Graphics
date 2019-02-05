#include "definitions.h"
#include "coursefunctions.h"
#include <algorithm>

/***********************************************
 * crossProduct
 * takes in two verteces and returns the cross
 * product as a fload
 **********************************************/
float crossProduct (Vertex v1, Vertex v2)
{
    return v1.x * v2.y - v1.y * v2.x;
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

/****************************************
 * DRAW_POINT
 * Renders a point to the screen with the
 * appropriate coloring.
 ***************************************/
void DrawPoint(Buffer2D<PIXEL> & target, 
               Vertex* v, 
               Attributes* attrs, 
               Attributes * const uniforms, 
               FragmentShader* const frag)
{
    // Set our pixel according to the attribute calue!
    PIXEL fragment = 0;
    frag->FragShader(fragment, *attrs, *uniforms);
    target[(int)v[0].y][(int)v[0].x] = fragment;
}

/****************************************
 * DRAW_LINE
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
void DrawTriangle(Buffer2D<PIXEL> & target, 
                  Vertex* const triangle, 
                  Attributes* const attrs, 
                  Attributes* const uniforms, 
                  FragmentShader* const frag)
{
    // Establish our mins and maxes
    int maxX = std::max(triangle[0].x, std::max(triangle[1].x, triangle[2].x));
    int minX = std::min(triangle[0].x, std::min(triangle[1].x, triangle[2].x));
    int maxY = std::max(triangle[0].y, std::max(triangle[1].y, triangle[2].y));
    int minY = std::min(triangle[0].y, std::min(triangle[1].y, triangle[2].y));

    // Establish our verteces of our bounding box
    Vertex vs1 = (Vertex){triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y, 1, 1};
    Vertex vs2 = (Vertex){triangle[2].x - triangle[0].x, triangle[2].y - triangle[0].y, 1, 1};

    // Establish a constant denominator for our barycentric interpolation
    float denom = ((triangle[1].y - triangle[2].y) * (triangle[0].x - triangle[2].x) + (triangle[2].x - triangle[1].x) * (triangle[0].y - triangle[2].y));

    // iterate through our bounding box in terms of x and y
    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            // Determine if the vertex is in the triangle with cross product.
            Vertex q = (Vertex){x - triangle[0].x, y - triangle[0].y};
            float s = (float)crossProduct(q, vs2) / crossProduct(vs1, vs2);
            float t = (float)crossProduct(vs1, q) / crossProduct(vs1, vs2);

            if ((s >= 0) && (t >= 0) && (s + t <= 1) /*is in triangle*/)
            {
                Vertex vert = {x,y,1,1};
                // Determine the weights barycentric interpolation                
                float w1 = ((triangle[1].y - triangle[2].y) * (vert.x - triangle[2].x) + (triangle[2].x - triangle[1].x) * (vert.y - triangle[2].y)) / denom;                    
                float w2 = ((triangle[2].y - triangle[0].y) * (vert.x - triangle[2].x) + (triangle[0].x - triangle[2].x) * (vert.y - triangle[2].y)) / denom;
                float w3 = 1 - w1 - w2;

                // iterpolate the attributes with our weights predetermined
                Attributes interpolateAttrs; // Our new interpolated attributes
                interpolateAttrs.vars[0] = w1 * attrs[0].vars[0] + w2 * attrs[1].vars[0] + w3 * attrs[2].vars[0];
                interpolateAttrs.vars[1] = w1 * attrs[0].vars[1] + w2 * attrs[1].vars[1] + w3 * attrs[2].vars[1];
                interpolateAttrs.vars[2] = w1 * attrs[0].vars[2] + w2 * attrs[1].vars[2] + w3 * attrs[2].vars[2];
                
                // Send our interpolated the shader to finalize the fragment
                frag->FragShader(target[y][x], interpolateAttrs, *uniforms);
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
