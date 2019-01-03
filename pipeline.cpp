#include "definitions.h"
#include "coursefunctions.h"
#include "coursesolutions.h"

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
void processUserInputs(bool & playing)
{
    SDL_Event e;
    int mouseX;
    int mouseY;
    while(SDL_PollEvent(&e)) 
    {
        if(e.type == SDL_QUIT) 
        {
            playing = false;
        }
        if(e.key.keysym.sym == 'q' && e.type == SDL_KEYDOWN) 
        {
            playing = false;
        }

        // Camera Rotation
        if(e.type == SDL_MOUSEMOTION)
        {
            int cur = SDL_ShowCursor(SDL_QUERY);
            if(cur == SDL_DISABLE)
            {
                mouseX = e.motion.xrel;
                mouseY = e.motion.yrel;
                camYaw   -= (mouseX * CAM_INCREMENT);
                camPitch -= (mouseY * CAM_INCREMENT);
            }
        }

        // Hide-Show mouse
        if(e.type == SDL_MOUSEBUTTONDOWN)
        {
            int cur = SDL_ShowCursor(SDL_QUERY);
            if(cur == SDL_DISABLE)
            {
                SDL_ShowCursor(SDL_ENABLE);
                SDL_SetRelativeMouseMode(SDL_FALSE);
            }
            else
            {
                SDL_ShowCursor(SDL_DISABLE);
                SDL_SetRelativeMouseMode(SDL_TRUE);
            }
        }

        // Camera Translation
        if(e.key.keysym.sym == 'w' && e.type == SDL_KEYDOWN)
        {
            camZ += (cos((camYaw / 180.0) * M_PI)) * STEP_INCREMENT;
            camX -= (sin((camYaw / 180.0) * M_PI)) * STEP_INCREMENT;
        }
        if(e.key.keysym.sym == 's' && e.type == SDL_KEYDOWN)
        {
            camZ -= (cos((camYaw / 180.0) * M_PI)) * STEP_INCREMENT;
            camX += (sin((camYaw / 180.0) * M_PI)) * STEP_INCREMENT;
        }
        if(e.key.keysym.sym == 'a' && e.type == SDL_KEYDOWN)
        {
            camX -= (cos((camYaw / 180.0) * M_PI)) * STEP_INCREMENT;
            camZ -= (sin((camYaw / 180.0) * M_PI)) * STEP_INCREMENT;
        }
        if(e.key.keysym.sym == 'd' && e.type == SDL_KEYDOWN)
        {
            camX += (cos((camYaw / 180.0) * M_PI)) * STEP_INCREMENT;
            camZ += (sin((camYaw / 180.0) * M_PI)) * STEP_INCREMENT;
        }
    }
}


/****************************************
 * DRAW_POINT
 * Renders a point to the screen with the
 * appropriate coloring
 ***************************************/
void DrawPoint(Buffer2D<PIXEL> & frame, Vertex* v, VBO* attrs, VBO * const uniforms, FragmentShader* const frag)
{
    // Your code goes here
}

/****************************************
 * Renders a line to the screen with the
 * appropriate coloring
 ***************************************/
void DrawLine(Buffer2D<PIXEL> & frame, Vertex* line, VBO* attrs, VBO* const uniforms, FragmentShader* const frag)
{
    // Your code goes here
}

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* triangle, VBO* attrs, VBO* const uniforms, FragmentShader* const frag)
{
    // Your code goes here
}

/**************************************************************
 * VERTEX_SHADER_EXECUTE_VERTICES
 * Executes the vertex shader on inputs, yielding transformed
 * outputs. 
 *************************************************************/
void VertexShaderExecuteVertices(const VertexShader* vert, Vertex const inputVerts[], VBO const inputAttrs[], const int& numIn, 
                                 VBO* const uniforms, Vertex transformedVerts[], VBO transformedAttrs[])
{
    // Defaults to pass-through behavior
    if(vert == NULL)
    {
        for(int i = 0; i < numIn; i++)
        {
            transformedVerts[i] = inputVerts[i];
        }
    }
    else
    {
        // Your Vertex Shader Callback goes here
    }
}

/***************************************************************************
 * DRAW_PRIMITIVE
 * Processes the indiecated PRIMITIVES type through pipeline stages of:
 *  1) Vertex Transformation
 *  2) Clipping
 *  3) Normalization
 *  4) ViewPort transform
 *  5) Rasterization & Fragment Shading
 **************************************************************************/
void DrawPrimitive(PRIMITIVES prim, 
                   Buffer2D<PIXEL>& target,
                   const Vertex inputVerts[], 
                   const VBO inputAttrs[],
                   VBO* const uniforms,
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
    VBO transformedAttrs[MAX_VERTICES];
    VertexShaderExecuteVertices(vert, inputVerts, inputAttrs, numIn, uniforms, transformedVerts, transformedAttrs);

    // After implementing the projection transform, uncomment the functions:
    //      1) 'ClipVertices(...)'
    //      2) 'NormalizeVertices(...)'
    //      3) 'ViewTransformVertices(...)'
    // And remove the loop code tagged as 'Bypass loop'

    // Clip to our frustrum
    Vertex clippedVerts[MAX_VERTICES];
    VBO clippedAttrs[MAX_VERTICES];
    int numClipped; 
    //ClipVertices(transformedVerts, transformedAttrs, numIn, clippedVerts, clippedAttrs, numClipped);

    // Normalization
    //NormalizeVertices(clippedVerts, numClipped);

    // View Transform
    Vertex viewVerts[MAX_VERTICES];
    VBO viewAttrs[MAX_VERTICES];
    int numView;
    //ViewTransformVertices(target, clippedVerts, clippedAttrs, numClipped, viewVerts, viewAttrs, numView);

    // 'Bypass loop' (used while we don't have clipping, view transform enabled)
    for(numView = 0; numView < numIn; numView++)
    {
        viewVerts[numView] = transformedVerts[numView];
        viewAttrs[numView] = transformedAttrs[numView];
    }

    // Vertex Interpolation & Fragment Drawing
    switch(prim)
    {
        case POINT:
            DrawPoint(target, viewVerts, viewAttrs, uniforms, frag);
            break;
        case LINE:
            DrawLine(target, viewVerts, viewAttrs, uniforms, frag);
            break;
        case TRIANGLE:
            DrawTriangle(target, viewVerts, viewAttrs, uniforms, frag);
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

        SOL_TestDrawPixel(frame);

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
