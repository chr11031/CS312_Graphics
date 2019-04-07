#include "definitions.h"
#include "coursefunctions.h"
#include <cmath>

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
    // Set our pixel according to the attribute value!
    target[(int)v[0].y][(int)v[0].x] = attrs[0].var.at('color');
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
 * DETERMINANT
 * Finds the determinant of the coordinates given, in our
 * case a 2x2 matrix
 ************************************************************/
inline double determinant(const double &A, const double &B, const double &C, const double &D)
{
    return ((A * D) - (B * C));
}

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    // Creates the bounding box
    int maxX = fmax(triangle[0].x, fmax(triangle[1].x, triangle[2].x));     // Find the max x vertex by searching through the verticies of the triangle
    int minX = fmin(triangle[0].x, fmin(triangle[1].x, triangle[2].x));     // Find the min x vertex by searching through the verticies of the triangle
    int maxY = fmax(triangle[0].y, fmax(triangle[1].y, triangle[2].y));     // Find the max y vertex by searching through the verticies of the triangle
    int minY = fmin(triangle[0].y, fmin(triangle[1].y, triangle[2].y));     // Find the min y vertex by searching through the verticies of the triangle
    
    // get the edges of the triangle as shown in scratchpixel.com
    double fVec[] = {triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y};
    double sVec[] = {triangle[2].x - triangle[1].x, triangle[2].y - triangle[1].y};
    double tVec[] = {triangle[0].x - triangle[2].x, triangle[0].y - triangle[2].y};
    
    // find the area of the entire triangle
    double triangleArea = determinant(fVec[0], -tVec[0], fVec[1], -tVec[1]);
    
    // loop through the space of the triangle
    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            // check to see if the pixel is within the bounds of the triangle
            double fDet = determinant(fVec[0], x - triangle[0].x, fVec[1], y - triangle[0].y);
            double sDet = determinant(sVec[0], x - triangle[1].x, sVec[1], y - triangle[1].y);
            double tDet = determinant(tVec[0], x - triangle[2].x, tVec[1], y - triangle[2].y);
           
           // if all the determinants are 0 or positive, then P is inside the triangle
           if (fDet >= 0 && sDet >= 0 && tDet >= 0)
           {
               target[y][x] = attrs[0].var['color'];

                // Take the reciprocal to get the perspective correct
                double correctZ = 1 / interp(triangleArea, fDet, sDet, tDet, triangle[0].w, triangle[1].w, triangle[2].w);
                
                // interpolated attributes for the colored triangle using the perspective correction
                Attributes interpAttr;
                interpAttr.var['r'] = correctZ * interp(triangleArea, fDet, sDet, tDet, attrs[0].var['r'], attrs[1].var['r'], attrs[2].var['r']);
                interpAttr.var['g'] = correctZ * interp(triangleArea, fDet, sDet, tDet, attrs[0].var['g'], attrs[1].var['g'], attrs[2].var['g']);
                interpAttr.var['b'] = correctZ * interp(triangleArea, fDet, sDet, tDet, attrs[0].var['b'], attrs[1].var['b'], attrs[2].var['b']);

                // interpolated attributes for the image triangle using the perspective correction
                interpAttr.var['u'] = correctZ * interp(triangleArea, fDet, sDet, tDet, attrs[0].var['u'], attrs[1].var['u'], attrs[2].var['u']);
                interpAttr.var['v'] = correctZ * interp(triangleArea, fDet, sDet, tDet, attrs[0].var['v'], attrs[1].var['v'], attrs[2].var['v']);

                // callback for frag shader
                frag->FragShader(target[y][x], interpAttr, *uniforms);
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
    for(int i = 0; i < numIn; i++)
    {
        if(vert == NULL)
        {
            transformedVerts[i] = inputVerts[i];
            transformedAttrs[i] = inputAttrs[i];
        }
        // if vert does not equal NULL
        else
        {
            vert->VertShader(transformedVerts[i], transformedAttrs[i], inputVerts[i], inputAttrs[i], *uniforms);
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

        // Your code goes here
        // GameOfLife(frame);              
        // TestDrawPixel(frame);            
        // TestDrawTriangle(frame);        
        // TestDrawFragments(frame);
        // TestDrawPerspectiveCorrect(frame);
        TestVertexShader(frame);

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
