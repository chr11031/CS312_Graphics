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

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    // Max and min x and y values for the triangle, and making sure we are in the scope of the window 
    int maxX = MIN(MAX3(triangle[0].x, triangle[1].x, triangle[2].x), S_WIDTH  - 1);
    int maxY = MIN(MAX3(triangle[0].y, triangle[1].y, triangle[2].y), S_HEIGHT - 1);
    int minX = MAX(MIN3(triangle[0].x, triangle[1].x, triangle[2].x), 0);
    int minY = MAX(MIN3(triangle[0].y, triangle[1].y, triangle[2].y), 0);

    // Get the three vertexs for the triangle
    double firstVec[]  = {triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y};
    double secondVec[] = {triangle[2].x - triangle[1].x, triangle[2].y - triangle[1].y};
    double thirdVec[]  = {triangle[0].x - triangle[2].x, triangle[0].y - triangle[2].y};

    // Compute area of the whole triangle by solving the determinat
    double areaTriangle = determinant(firstVec[X_KEY], -thirdVec[X_KEY], firstVec[Y_KEY], -thirdVec[Y_KEY]);

    // Loop through every pixel in the grid
    for(int y = minY; y < maxY; y++)
    {
        for(int x = minX; x < maxX; x++)
        {
            // Calculate the pixel's sign by finding the determinat's sign
            double firstDet  = determinant(firstVec[X_KEY], x - triangle[0].x, firstVec[Y_KEY], y - triangle[0].y);
            double secondDet = determinant(secondVec[X_KEY], x - triangle[1].x, secondVec[Y_KEY], y - triangle[1].y);
            double thirdDet  = determinant(thirdVec[X_KEY], x - triangle[2].x, thirdVec[Y_KEY], y - triangle[2].y);
            double det[3]    = {firstDet, secondDet, thirdDet};

            // If one of the three dets are greater than 0 then we are in the triangle 
            if(det[0] >= 0 && det[1] >= 0 && det[2] >= 0)
            {
                // Gets a pixel which is in the triangle
                target[(int)y][(int)x] = attrs[0].color;

                // Interpolate Attributes for this pixel - In this case the R,G,B values
                Attributes interpolatedAttribs;
                interpolatedAttribs.setRed(interp(areaTriangle, det, attrs[0].getRed(), attrs[1].getRed(), attrs[2].getRed()));
                interpolatedAttribs.setGreen(interp(areaTriangle, det, attrs[0].getGreen(), attrs[1].getGreen(), attrs[2].getGreen()));
                interpolatedAttribs.setBlue(interp(areaTriangle, det, attrs[0].getBlue(), attrs[1].getBlue(), attrs[2].getBlue()));

                // Interpolate Attributes for this pixel - In this case the U,V Cooridnates
                interpolatedAttribs.setBU(interp(areaTriangle, det, attrs[0].getBU(), attrs[1].getBU(), attrs[2].getBU()));
                interpolatedAttribs.setBV(interp(areaTriangle, det, attrs[0].getBV(), attrs[1].getBV(), attrs[2].getBV()));

                // Call shader callback
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

        // Draws the triangle
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
