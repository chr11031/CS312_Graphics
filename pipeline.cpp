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
    
    //frag->FragShader(&target, *attrs, *uniforms);
    
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
    //get mins and maxs for barycentric square
    int maxX = MAX3(triangle[0].x, triangle[1].x, triangle[2].x);
    int minX = MIN3(triangle[0].x, triangle[1].x, triangle[2].x);
    int maxY = MAX3(triangle[0].y, triangle[1].y, triangle[2].y);
    int minY = MIN3(triangle[0].y, triangle[1].y, triangle[2].y);

    

    // //find vertex to be used later to find determinate in crossproduct
    // Vertex v1 = {(triangle[1].x - triangle[0].x), (triangle[1].y - triangle[0].y),1, 1};
    // Vertex v2 = {(triangle[2].x - triangle[0].x), (triangle[2].y - triangle[0].y),1, 1};

    double v1[] = {triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y};
    double v2[] = {triangle[2].x - triangle[1].x, triangle[2].y - triangle[1].y};
    double v3[] = {triangle[0].x - triangle[2].x, triangle[0].y - triangle[2].y};

    double area = crossProduct(v1[X_KEY], -v3[X_KEY], v1[Y_KEY], -v3[Y_KEY]);

    

    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x++)
        {
            //find determinants
            double d1 = crossProduct(v1[X_KEY], x - triangle[0].x, v1[Y_KEY], y - triangle[0].y);
            double d2 = crossProduct(v2[X_KEY], x - triangle[1].x, v2[Y_KEY], y - triangle[1].y);
            double d3 = crossProduct(v3[X_KEY], x - triangle[2].x, v3[Y_KEY], y - triangle[2].y);
            
           

            //check to see if you're inside the triangle
            if (d1 >= 0 && d2 >= 0 && d3 >= 0)
            {
                target[(int)y][(int)x] = attrs[0].color;


                //through linear interpolation the color attributes are filled within the triangle
                Attributes interpAttr;


                //get the "correct" z value
                double z = 1/ interp(area, d1, d2, d3, triangle[0].w, triangle[1].w, triangle[2].w);

                //multiply the attributes by the correct z to get proper position
                interpAttr.setR(z * interp(area, d1, d2, d3, attrs[0].getR(), attrs[1].getR(), attrs[2].getR()));
                interpAttr.setG(z * interp(area, d1, d2, d3, attrs[0].getG(), attrs[1].getG(), attrs[2].getG()));
                interpAttr.setB(z * interp(area, d1, d2, d3, attrs[0].getB(), attrs[1].getB(), attrs[2].getB()));

                interpAttr.setU(z * interp(area, d1, d2, d3, attrs[0].getU(), attrs[1].getU(), attrs[2].getU()));
                interpAttr.setV(z * interp(area, d1, d2, d3, attrs[0].getV(), attrs[1].getV(), attrs[2].getV()));

                //calls fragshader callback
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


    // BufferImage myPic("AllMight.bmp");

    // Vertex * point;
    
    // PIXEL colors2[3] = {0xff00ff00, 0xff00ff00, 0xff00ff00};

    
    // Attributes * attr;


    

    // Draw loop 
    bool running = true;
    while(running) 
    
    {           
        // Handle user inputs
        processUserInputs(running);

        // Refresh Screen
        clearScreen(frame);

        TestDrawPerspectiveCorrect(frame);
        
        //TestDrawGreen(frame);

        //TestDrawFragments(frame);

        //TestDrawPixel(frame);

        //TestDrawTriangle(frame);

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
