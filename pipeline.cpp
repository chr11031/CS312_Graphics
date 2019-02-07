#include "definitions.h"
#include "coursefunctions.h"
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
    // Your code goes here
    // Set our pixel according to the attribute value!     
    // target[(int)v[0].y][(int)v[0].x] = attrs[0].color;
}

/****************************************
 * DRAW_TRIANGLE
 * Renders a line to the screen.
 ***************************************/
void DrawLine(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    // Your code goes here
}
/********************************************
 *Determinant
 *Find the area of the triangle between
 * two vectors. 
 ******************************************/
double Determinant(double a, double b, double c, double d) 
{
    return (a * d) - (b * c); 
}

double Interp(double area, double d1, double d2, double d3, double a1, double a2, double a3) 
{
   double percentd1 = (d1/area) * a3; 
   double percentd2 = (d2/area) * a1; 
   double percentd3 = (d3/area) * a2; 
   
   return percentd1 + percentd2 + percentd3;
}

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
//get the box around the triangle
float maxX = MAX3(triangle[0].x,triangle[1].x,triangle[2].x);
float maxY = MAX3(triangle[0].y,triangle[1].y,triangle[2].y);
float minX = MIN3(triangle[0].x,triangle[1].x,triangle[2].x);
float minY = MIN3(triangle[0].y,triangle[1].y,triangle[2].y);
//first determinant is proportional to 
// edge corresponds with opposite vertex

Vertex vt1;
Vertex vt2;
Vertex vt3;

vt1.x = (triangle[1].x - triangle[0].x);
vt1.y = (triangle[1].y - triangle[0].y);
vt1.z = (triangle[1].z - triangle[0].z);
vt1.w = (triangle[1].w - triangle[0].w);

vt2.x = (triangle[2].x - triangle[1].x);
vt2.y = (triangle[2].y - triangle[1].y);
vt2.z = (triangle[2].z - triangle[1].z);
vt2.w = (triangle[2].w - triangle[1].w);

vt3.x = (triangle[0].x - triangle[2].x);
vt3.y = (triangle[0].y - triangle[2].y);
vt3.z = (triangle[0].z - triangle[2].z);
vt3.w = (triangle[0].w - triangle[2].w);

//are (first vecx, -third vex, first vec y, -third vecy)
double area = Determinant(vt1.x, -vt3.x, vt1.y, -vt3.y);
//loop through every pixel in box
    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {   

            //determine where the pixel is at
            double detSign1 = Determinant(vt1.x, (x - triangle[0].x), vt1.y, (y - triangle[0].y));
            double detSign2 = Determinant(vt2.x, (x - triangle[1].x), vt2.y, (y - triangle[1].y));
            double detSign3 = Determinant(vt3.x, (x - triangle[2].x), vt3.y, (y - triangle[2].y));
            
            //we are inside
            if((detSign1 >= 0) && (detSign2 >= 0) && (detSign3 >= 0))
            {
                Attributes interpAtt;
                int size = (sizeof(interpAtt.collector)/sizeof(*interpAtt.collector));
                double lerpz = (1/Interp(area, detSign1, detSign2, detSign3, triangle[0].w, triangle[1].w, triangle[2].w));

                for (int i = 0; i < size; i++)
                {
                    interpAtt.collector[i] = Interp(area, detSign1, detSign2, detSign3, attrs[0].collector[i], attrs[1].collector[i], attrs[2].collector[i]) * lerpz;
                }
                frag->FragShader(target[y][x],interpAtt, *uniforms);
                
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
    BufferImage img("red.bmp");
    // Draw loop 
    bool running = true;
    while(running) 
    {           
        // Handle user inputs
        processUserInputs(running);

        // Refresh Screen
        clearScreen(frame);

        // Your code goes here
        // TestDrawPixel(frame);
       // TestDrawTriangle(frame);
     //  for (int x = 0; x < 512; x++) {
       //    for (int y = 0; y < 512; y++) {
         //      DrawPrimitive();
           //    frame[y][x] = img[y][x];
          // }
       //} 
        TestDrawPerspectiveCorrect(frame);
       //  TestDrawFragments(frame);
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
