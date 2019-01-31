#include "definitions.h"
#include "coursefunctions.h"
#include <algorithm>

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
template <class T>
void DrawPoint(Buffer2D<PIXEL> & target, Vertex* v, Attributes<T>* attrs, Attributes<T> * const uniforms, FragmentShader<T>* const frag)
{
    target[(int)v[0].y][(int)v[0].x] = attrs[0].color;
}

/****************************************
 * DRAW_LINE
 * Renders a line to the screen.
 ***************************************/
template <class T>
void DrawLine(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes<T>* const attrs, Attributes<T>* const uniforms, FragmentShader<T>* const frag)
{
    float x = triangle[0].x;
    float y = triangle[0].y;
    float length = 0;

    float avx = std::abs(triangle[1].x - x);
    float avy = std::abs(triangle[1].y - y);
    (avx < avy) ? length = avy : length = avx;

    int dx = (triangle[1].x - x) / length;
    int dy = (triangle[1].y - y) / length;

    for (int i = x; i < x + dx; i++)
    {
        target[(int)std::round(y)][(int)std::round(x)] = attrs[0].color;
        x += dx;
        y += dy;
    }
}

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
template <class T>
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes<T>* const attrs, Attributes<T>* const uniforms, FragmentShader<T>* const frag)
{
    // Bounding Box
    int yMax = MAX3(triangle[0].y, triangle[1].y, triangle[2].y);
    int yMin = MIN3(triangle[0].y, triangle[1].y, triangle[2].y);
    int xMax = MAX3(triangle[0].x, triangle[1].x, triangle[2].x);
    int xMin = MIN3(triangle[0].x, triangle[1].x, triangle[2].x);

    //Compute first, second, third x-y pairs (used to find area and determinants)
    double firstVec[] = {triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y};
    double secondVec[] = {triangle[2].x - triangle[1].x, triangle[2].y - triangle[1].y};
    double thirdVec[] = {triangle[0].x - triangle[2].x, triangle[0].y - triangle[2].y};

    // Compute area of whole triangle (need to divide determinants by this in interpolation)
    double areaTriangle = determinant(firstVec[0], -thirdVec[0], firstVec[1], -thirdVec[1]);

    // Mega Pixel Loop
    for (int x = xMin; x < xMax; x++)
    {
        for (int y = yMin; y < yMax; y++)
        {
            //Determine if the pixel is in the triangle by the determinant's sign
            double firstDet = determinant(firstVec[0], x - triangle[0].x, firstVec[1], y - triangle[0].y);
            double secondDet = determinant(secondVec[0], x - triangle[1].x, secondVec[1], y - triangle[1].y);
            double thirdDet = determinant(thirdVec[0], x - triangle[2].x, thirdVec[1], y - triangle[2].y);

            // Point is inside triangle
            if(firstDet >= 0 && secondDet >= 0 && thirdDet >= 0)
            {   //Draw (Doesn't look like we need this, but keeping it in case)
                //target[(int)y][(int)x] = attrs[0].color;

                // Interpolate attributes
                Attributes<T> interpolatedAttribs;
                //RGB (or alpha ;)
                interpolatedAttribs.r = interp(areaTriangle, firstDet, secondDet, thirdDet, attrs[0].r, attrs[1].r, attrs[2].r);
                interpolatedAttribs.g = interp(areaTriangle, firstDet, secondDet, thirdDet, attrs[0].g, attrs[1].g, attrs[2].g);
                interpolatedAttribs.b = interp(areaTriangle, firstDet, secondDet, thirdDet, attrs[0].b, attrs[1].b, attrs[2].b);
                //Image units
                interpolatedAttribs.u = interp(areaTriangle, firstDet, secondDet, thirdDet, attrs[0].u, attrs[1].u, attrs[2].u);
                interpolatedAttribs.v = interp(areaTriangle, firstDet, secondDet, thirdDet, attrs[0].v, attrs[1].v, attrs[2].v);

                // Shader call back
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
template <class T>
void VertexShaderExecuteVertices(const VertexShader<T>* vert, Vertex const inputVerts[], Attributes<T> const inputAttrs[], const int& numIn, 
                                 Attributes<T>* const uniforms, Vertex transformedVerts[], Attributes<T> transformedAttrs[])
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
template <class T>
void DrawPrimitive(PRIMITIVES prim, 
                   Buffer2D<PIXEL>& target,
                   const Vertex inputVerts[], 
                   const Attributes<T> inputAttrs[],
                   Attributes<T>* const uniforms,
                   FragmentShader<T>* const frag,                   
                   VertexShader<T>* const vert,
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
    Attributes<T> transformedAttrs[MAX_VERTICES];
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

        /***********************************
         * Trophy Box.
         * ********************************
        //TestDrawPixel(frame);
        //TestDrawTriangle(frame);         */

        TestDrawFragments<double>(frame);

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
