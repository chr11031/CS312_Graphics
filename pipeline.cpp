#include "definitions.h"
#include "coursefunctions.h"
//#include <cmath>

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
    PIXEL color = 0xff000000;
     color += (unsigned int)(attrs[0].rgb[0] * 0xff) << 16;
     color += (unsigned int)(attrs[0].rgb[1] * 0xff) << 8;
     color += (unsigned int)(attrs[0].rgb[2] * 0xff) << 0;
    // Set our pixel according to the attribute value!
    target[(int)v[0].y][(int)v[0].x] = color;
    // std::cout << std::hex << attrs->color[0] << std::endl;
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
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    // we only want to look at the box that the triangle is in. If we go outside
    // that box we are just wasting processing power and speed.
    int maxX = MAX3(triangle[0].x, triangle[1].x, triangle[2].x);
    int minX = MIN3(triangle[0].x, triangle[1].x, triangle[2].x);
    int maxY = MAX3(triangle[0].y, triangle[1].y, triangle[2].y);
    int minY = MIN3(triangle[0].y, triangle[1].y, triangle[2].y);
    
    // vector from vertex 1 to vertex 2
    double vs1[2] = {(triangle[1].x - triangle[0].x), (triangle[1].y - triangle[0].y)};

    // vector from vertex 2 to vertex 3
    double vs2[2] = {(triangle[2].x - triangle[1].x), (triangle[2].y - triangle[1].y)};
    
    // vector from vertex 3 to vertex 1
    double vs3[2] = {(triangle[0].x - triangle[2].x), (triangle[0].y - triangle[2].y)};

    float totalArea = triangleArea(vs1, vs2);

    int yTop = MAX3(triangle[0].y, triangle[1].y, triangle[2].y);
    int yBottom = MIN3(triangle[0].y, triangle[1].y, triangle[2].y);
    int yRange = yTop - yBottom;

    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {   
            // create the vectors to be compared with the three side vectors.
            double q1[2] = {x - triangle[0].x, y - triangle[0].y};
            double q2[2] = {x - triangle[1].x, y - triangle[1].y};
            double q3[2] = {x - triangle[2].x, y - triangle[2].y};

            // find the cross products of the three outside vectors with
            // the vector pointing to the point to be drawn
            float areaT1 = triangleArea(vs2, q2);
            float areaT3 = triangleArea(vs1, q1);
            float areaT2 = triangleArea(vs3, q3);

            // If any of the cross products are less than zero then the pixel is outside of the triangle.
            if ( (areaT1 >= 0) && (areaT3 >= 0) && ((areaT2 >= 0)))
            {
                //The point is inside the triangle.
                target[y][x] = attrs[0].color;

                double zt = 1/lerp(totalArea, areaT1, areaT2, areaT3, triangle[0].w, triangle[1].w, triangle[2].w);

                // interpolate colors here into new attributes object
                Attributes lerpedAttrs;
                lerpedAttrs.rgb[0] = zt * lerp(totalArea, areaT1, areaT2, areaT3, attrs[0].rgb[0], attrs[1].rgb[0], attrs[2].rgb[0]);
                lerpedAttrs.rgb[1] = zt * lerp(totalArea, areaT1, areaT2, areaT3, attrs[0].rgb[1], attrs[1].rgb[1], attrs[2].rgb[1]);
                lerpedAttrs.rgb[2] = zt * lerp(totalArea, areaT1, areaT2, areaT3, attrs[0].rgb[2], attrs[1].rgb[2], attrs[2].rgb[2]);

                lerpedAttrs.uv[0] = zt * lerp(totalArea, areaT1, areaT2, areaT3, attrs[0].uv[0], attrs[1].uv[0], attrs[2].uv[0]);
                lerpedAttrs.uv[1] = zt * lerp(totalArea, areaT1, areaT2, areaT3, attrs[0].uv[1], attrs[1].uv[1], attrs[2].uv[1]);

                frag->FragShader(target[y][x], lerpedAttrs, *uniforms);
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
    else
    {
        for(int i = 0; i < numIn; i++)
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

        // TestDrawPixel(frame);
        //GameOfLife(frame);
        // TestDrawTriangle(frame);
        // TestDrawFragments(frame);
        // TestDrawPerspectiveCorrect(frame);
        TestVertexShader(frame);

        //TestDrawPixel(frame);
        // GameOfLife(frame);
        
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
