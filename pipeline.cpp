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
 * Finds the determinant.
 ************************************************************/
double Determinant(int ax, int ay, int bx, int by)
{
    return (ax * by) - (ay * bx);
}

double interpolate(double area, double det1, double det2, double det3, double c1, double c2, double c3)
{
    //Finding where the point is in the traingle and how much color is in each part
    det1 /= area;
    det2 /= area;
    det3 /= area;

    return (det1 * c3) + (det2 * c1) + (det3 * c2);
}

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    //gets the boundaries
    int minx = MIN3(triangle[0].x, triangle[1].x, triangle[2].x);
    int miny = MIN3(triangle[0].y, triangle[1].y, triangle[2].y);
    int maxx = MAX3(triangle[0].x, triangle[1].x, triangle[2].x);
    int maxy = MAX3(triangle[0].y, triangle[1].y, triangle[2].y);

    //gets the values of the two other vectors relative to eachother
    double vx = triangle[1].x - triangle[0].x;
    double vy = triangle[1].y - triangle[0].y;
    double wx = triangle[2].x - triangle[1].x;
    double wy = triangle[2].y - triangle[1].y;
    double ux = triangle[0].x - triangle[2].x;
    double uy = triangle[0].y - triangle[2].y;

    //finds the area of the triangle using the determinant.
    double area = Determinant(vx, vy, -ux, -uy);
    //loops for each possible x and y value
    for (int x = minx; x <= maxx; x++)
    {
        for(int y = miny; y <= maxy; y++)
        {
            //As much as I liked the praise for only doing 3 determinant calculations
            //per pixel, I need all 3 to interpolate properly. This also fixed the jagged
            //edges on my triangles.
            double deta = Determinant(vx, vy, x - triangle[0].x, y - triangle[0].y);
            double detb = Determinant(wx, wy, x - triangle[1].x, y - triangle[1].y);
            double detc = Determinant(ux, uy, x - triangle[2].x, y - triangle[2].y);
            //the determinants must be positive for it to be in the triangle
            if (deta >= 0 && detb >= 0 && detc >= 0)
            {
                //for if just sent empty pixels.
                target[y][x] = attrs[0].color;

                double z = 1 / interpolate(area, deta, detb, detc, triangle[0].w, triangle[1].w, triangle[2].w);

                Attributes newattribs;
                newattribs.numValues= attrs[0].numValues;
                
                for (int i = 0; i < newattribs.numValues; i++)
                    newattribs.values[i] = z * interpolate(area, deta, detb, detc, attrs[0].values[i], attrs[1].values[i], attrs[2].values[i]);

                //sends the new attributes to the fragment shader
                frag->FragShader(target[y][x], newattribs, *uniforms);
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
