
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

/**********************************************************************
 * CrossProduct
 * computes the cross product (also the determinant) of a 2X2 matrix
 **********************************************************************/
double CrossProduct(double a, double b, double c, double d)
{
    return ((a * d) - (b * c));
}

/***************************************************
 * interpolate
 * interpolate the color of each pixel to make the correct shading in the triangles.
 ***************************************************************/
double interpolate(double area, double det1, double det2, double det3, double const color1, double const color2, double const color3)
{
    double c1 = (det2 / area) * color1;
    double c2 = (det3 / area) * color2;
    double c3 = (det1 / area) * color3;

    return c1 + c2 + c3;
}

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    //create the bounding box with the max and min x, y values
    double maxX = MAX3(triangle[0].x, triangle[1].x, triangle[2].x);
    double minX = MIN3(triangle[0].x, triangle[1].x, triangle[2].x);
    double maxY = MAX3(triangle[0].y, triangle[1].y, triangle[2].y);
    double minY = MIN3(triangle[0].y, triangle[1].y, triangle[2].y);

    //establish 3 vertices and make sure it's the correct winding order
    Vertex vect01 = {
        triangle[1].x - triangle[0].x,
        triangle[1].y - triangle[0].y,
        triangle[1].z - triangle[0].z,
        triangle[1].w - triangle[0].w
    };
    
    Vertex vect02 = {
        triangle[2].x - triangle[1].x,
        triangle[2].y - triangle[1].y,
        triangle[2].z - triangle[1].z,
        triangle[2].w - triangle[1].w
    };

    Vertex vect03 = {
        triangle[0].x - triangle[2].x,
        triangle[0].y - triangle[2].y,
        triangle[0].z - triangle[2].z,
        triangle[0].w - triangle[2].w
    };

    //calculate the area using two vectors
    double area = CrossProduct(vect01.x, -vect03.x, vect01.y, -vect03.y);

    //loop through and compute determinants
    for(int x = minX; x <= maxX; x++)
    {
        for(int y = minY; y <=maxY; y++)
        {
            double det1 = CrossProduct(vect01.x, x - triangle[0].x, vect01.y, y - triangle[0].y);
            double det2 = CrossProduct(vect02.x, x - triangle[1].x, vect02.y, y - triangle[1].y);
            double det3 = CrossProduct(vect03.x, x - triangle[2].x, vect03.y, y - triangle[2].y);
            //draw the pixel if inside the triangle
            if ((det1 >= 0) && (det2 >= 0) && (det3 >= 0))
            {
                Attributes interpolatedAttribs;
                //calculate percentages for colors or coordinates with associated depths using the reciprocal of the interpolated w
                double lerpedZ = (1/interpolate(area, det1, det2, det3, triangle[0].w, triangle[1].w, triangle[2].w));
                //loop through the attibs and interpolate.
                for(int i = 0; i <= attrs->size; i++)
                {
                    interpolatedAttribs.cc[i] = interpolate(area, det1, det2, det3, attrs[0].cc[i], attrs[1].cc[i], attrs[2].cc[i]) * lerpedZ;
                }
                //call shader
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
    if(vert != NULL)
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
    BufferImage img ("starcraftskerri.bmp");

    // Draw loop 
    bool running = true;
    while(running) 
    {           
        // Handle user inputs
        processUserInputs(running);

        // Refresh Screen
        clearScreen(frame);

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
