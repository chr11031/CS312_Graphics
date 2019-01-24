#include "definitions.h"
#include "coursefunctions.h"
#include "shaders.h"

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
        // Set our pixel according to the attribute value!
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

/****************************************
 * CROSS PRODUCT
 * Calculates the determinant of two vectors
 ***************************************/
int crossProduct(Vertex a, Vertex b) {
    return a.x * b.y - a.y * b.x;
}

/****************************************
 * LERP
 * Linear interpolation between two numbers
 ***************************************/
float lerp(float a, float b, float step) {
    return a + (b - a) * step;
}

/****************************************
 * FIND WEIGHT
 * Performs barycentric interpolation
 ***************************************/
float* findWeights(Vertex* v, Vertex p) {
    // v[3] is the triangle, p is the pixel we are testing
    float weights[3];

    weights[0] = ((v[1].y - v[2].y) * (p.x - v[2].x) +
                  (v[2].x - v[1].x) * (p.y - v[2].y)) /
                 ((v[1].y - v[2].y) * (v[0].x - v[2].x) +
                  (v[2].x - v[1].x) * (v[0].y - v[2].y));

    weights[1] = ((v[2].y - v[0].y) * (p.x - v[2].x) +
                  (v[0].x - v[2].x) * (p.y - v[2].y)) /
                 ((v[1].y - v[2].y) * (v[0].x - v[2].x) +
                  (v[2].x - v[1].x) * (v[0].y - v[2].y));
                
    weights[2] = 1 - weights[0] - weights[1];

    return weights;
}

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    // Barycentric algorithm from: http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html

    // Find the two edge vectors we use to calculate determinants. We don't need all 3, see below
    Vertex vs1 = {triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y, 1, 1};
    Vertex vs2 = {triangle[2].x - triangle[0].x, triangle[2].y - triangle[0].y, 1, 1};

    // Bounding box
    int xmin = MIN3(triangle[0].x, triangle[1].x, triangle[2].x);
    int ymin = MIN3(triangle[0].y, triangle[1].y, triangle[2].y);
    int xmax = MAX3(triangle[0].x, triangle[1].x, triangle[2].x);
    int ymax = MAX3(triangle[0].y, triangle[1].y, triangle[2].y);

    // Iterate over every pixel in the bounding box
    for (int x = xmin; x < xmax; x++) {
        for (int y = ymin; y < ymax; y++) {
            // q is the vector from the pixel we are testing to triangle[0]
            Vertex q = {x - triangle[0].x, y - triangle[0].y};

            // Next we find the cross product (determinant) of vectors formed between q and each of the two edge vectors
            float s = (float)crossProduct(q, vs2) / crossProduct(vs1, vs2);
            float t = (float)crossProduct(vs1, q) / crossProduct(vs1, vs2);

            /*
             We do not need to find the third edge because:
                - If s >= 0, the pixel is above the first edge vector.
                - If t >= 0, the pixel is below the second edge vector.
                - s + t > 1 if and only if the point is far enough away from the first
                  and second vectors as to be outside the triangle (beyond third edge).
             This means we only need two edges, we can calculate without needing the third one.
            */
            if ((s >= 0) && (t >= 0) && (s + t <= 1)) {
                Vertex v = {x, y, 1, 1};
                // Calculate the new Attributes
                float* weights = findWeights(triangle, v);
                Attributes a;
                a.color = (attrs[0].color >> 16) && 0xFF;
                a.color *= weights[0];

                DrawPoint(target, &v, &a, uniforms, frag);
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
