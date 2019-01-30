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
 * COMPUTE_DETERMINANT
 * Computes the determinant for a set of
 * 2 Verticies, given in Matrix Form
 * thus:
 * 
 * [a b]
 * [c d]
 * *************************************/
double computeDeterminant(double a, double b, double c, double d) {
    return (a * d) - (b * c);
}

/****************************************
 * INTERPOLATE
 * Interpolates (mixes) components from 3
 * colors.
 * 
 * area - Area of a primitive
 * firstDet - Factor for computing
 *            multiplication of colorAttr1
 * ...
 * color1 - Color for attribute 1
 * *************************************/
double interpolate(double area, double firstDet, double secondDet, double thirdDet, double colorAttr1, double colorAttr2, double colorAttr3)
{
    double component1 = (secondDet / area) * colorAttr1;
    double component2 = (thirdDet / area) * colorAttr2;
    double component3 = (firstDet / area) * colorAttr3;

    return component1 + component2 + component3;
}

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
void DrawTriangle(Buffer2D<PIXEL> &target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    int maxX = MAX3(triangle[0].x, triangle[1].x, triangle[2].x);
    int minX = MIN3(triangle[0].x, triangle[1].x, triangle[2].x);
    int maxY = MAX3(triangle[0].y, triangle[1].y, triangle[2].y);
    int minY = MIN3(triangle[0].y, triangle[1].y, triangle[2].y);

    // Make two sides (Vectors) of the triangle
    Vertex firstVect = {
        triangle[1].x - triangle[0].x,
        triangle[1].y - triangle[0].y,
        1,
        1
    };

    Vertex secondVect = {
        triangle[2].x - triangle[1].x,
        triangle[2].y - triangle[1].y,
        1,
        1
    };

    // Sample Class Code
    Vertex thirdVect = {
        triangle[0].x - triangle[2].x,
        triangle[0].y - triangle[2].y,
        1,
        1
    };

    double area = computeDeterminant(firstVect.x, -thirdVect.x, firstVect.y, -thirdVect.y);

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            // Determinant calculations from class
            double firstDet = computeDeterminant(firstVect.x, x - triangle[0].x, firstVect.y, y - triangle[0].y);
            double secondDet = computeDeterminant(secondVect.x, x - triangle[1].x, secondVect.y, y - triangle[1].y);
            double thirdDet = computeDeterminant(thirdVect.x, x - triangle[2].x, thirdVect.y, y - triangle[2].y);

            // Test if this point is in the triangle
            if ((firstDet >= 0) && (secondDet >= 0) && (thirdDet >= 0)) {
                // From Sample Class code
                Attributes interpolatedAttributes;
                interpolatedAttributes.r = interpolate(area, firstDet, secondDet, thirdDet, attrs[0].r, attrs[1].r, attrs[2].r);
                interpolatedAttributes.g = interpolate(area, firstDet, secondDet, thirdDet, attrs[0].g, attrs[1].g, attrs[2].g);
                interpolatedAttributes.b = interpolate(area, firstDet, secondDet, thirdDet, attrs[0].b, attrs[1].b, attrs[2].b);
                
                // Set the attribute color by sending to fragShader
                frag->FragShader(target[y][x], interpolatedAttributes, *uniforms);
                
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

// Here, I will make a FragmentShader callback function
void GreenFragmentShader(PIXEL & fragment, const Attributes & vertexAttr, const Attributes & uniforms)
{
    // OK, so we will zero out the red and blue and just leave the rest
    //  which will be green
    
    fragment = vertexAttr.color & 0xff00ff00;
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
    BufferImage bmpImage("../battletoads.bmp");

    // Draw loop 
    bool running = true;
    while(running) 
    {           
        // Handle user inputs
        processUserInputs(running);

        // Refresh Screen
        clearScreen(frame);

        TestDrawFragments(frame);
        // TestDrawTriangle(frame);

        // Attributes attrs;
        // FragmentShader greenSdr(GreenFragmentShader);

        // // Here, we will make a double for loop
        // for (int y = 0; y < 256; y++) {
        //     for (int x = 0; x < 256; x++) {
        //         //frame[y][x] = bmpImage[y][x];
        //         //Attributes attrs;
        //         // FragmentShader fragSdr(DefaultFragShader);                
                
        //         attrs.color = bmpImage[y][x];

        //         Vertex v;
        //         v.x = x;
        //         v.y = y;
        //         v.z = 1;
        //         v.w = 1;

        //         DrawPrimitive(POINT, frame, &v, &attrs, NULL, &greenSdr);
        //     }
        // }

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
