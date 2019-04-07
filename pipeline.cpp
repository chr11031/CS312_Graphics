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
    // Your code goes here
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
 * CROSS_PRODUCT
 * Calculate the cross product given two vertices
 * Note: only calculates the determinant for now
 ************************************************************/
float crossProduct(Vertex* const vertex1, Vertex* const vertex2)
{
    return ((*vertex1).x * (*vertex2).y) - ((*vertex2).x * (*vertex1).y);
}

/*************************************************************
 * INTERPOLATE
 * Assign the appropriate weight to the pixel at it's x and
 * y coordinate. The weight is determined by its distance
 * from all three vertices
 ************************************************************/
double interoplate(double y, double x, Vertex* const triangle, double attr1, double attr2, double attr3)
{
    double triangleArea = ((triangle[1].y - triangle[2].y) * (triangle[0].x - triangle[2].x) + (triangle[2].x - triangle[1].x) * (triangle[0].y - triangle[2].y));
    // % of weight that attribute 1 will be diluted
    double w1 = ((triangle[1].y - triangle[2].y) * (x - triangle[2].x) + (triangle[2].x - triangle[1].x) * (y - triangle[2].y)) / triangleArea;
          
    // % of weight that attribute 2 will be diluted
    double w2 = ((triangle[2].y - triangle[0].y) * (x - triangle[2].x) + (triangle[0].x - triangle[2].x) * (y - triangle[2].y)) / triangleArea;
    // % of weight that attribute 3 will be diluted
    // since the weights are percentages, the third weight will be the 100% - the other 
    // two calculated percents
    // Note that the two denominators are the same to calculate w1 and w2. The distance from the vertex with the attribute over the whole
    // creates a percentage
    double w3 = 1 - w1 - w2;
    // return the true interpolated attribute (ex. color) value for this pixel after diluting the
    // three attributes
    return ((w1 * attr1) + (w2 * attr2) + (w3 * attr3));
}

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    // Create a bounding box for the triangle
    // If the pixel is out of bounds then it can be ignored
    int maxX = MAX3(triangle[0].x,triangle[1].x,triangle[2].x);
    int minX = MIN3(triangle[0].x,triangle[1].x,triangle[2].x);
    int maxY = MAX3(triangle[0].y,triangle[1].y,triangle[2].y);
    int minY = MIN3(triangle[0].y,triangle[1].y,triangle[2].y);

    // Length of left side of triangle in x and y directions
    Vertex vs1 = Vertex{triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y,1,1};
    // Length of right side of triangle in x and y directions
    Vertex vs2 = Vertex{triangle[2].x - triangle[0].x, triangle[2].y - triangle[0].y,1,1};

    // Loop through all the pixels in the bounding box, if inside triangle draw the pixel
    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            // Difference between one of the vertices and the values x and y in iteration
            Vertex v1 = Vertex{x - triangle[0].x, y - triangle[0].y, 1, 1};

            float s = crossProduct(&v1, &vs2) / crossProduct(&vs1, &vs2);
            float t = crossProduct(&vs1, &v1) / crossProduct(&vs1, &vs2);

            if (( s >= 0) && (t >= 0) && (s + t <= 1))
            {
                Attributes interpolatedAttribs;

                // for perspective correction it is necessary to 
                // 1) interpolate the w value, and invert it
                // this will compute the correct value for z
                double z = 1 / interoplate(y, x, triangle, triangle[0].w, triangle[1].w, triangle[2].w );
                // interpolate values, taking into account the proper depth z
                interpolatedAttribs.attr[0] = z * interoplate(y, x, triangle, attrs[0].attr[0],
                                                          attrs[1].attr[0],attrs[2].attr[0]);
                interpolatedAttribs.attr[1] = z * interoplate(y, x, triangle, attrs[0].attr[1],
                                                          attrs[1].attr[1],attrs[2].attr[1]);
                interpolatedAttribs.attr[2] = z * interoplate(y, x, triangle, attrs[0].attr[2],
                                                          attrs[1].attr[2],attrs[2].attr[2]);
                interpolatedAttribs.attr[3] = z * interoplate(y, x, triangle, attrs[0].attr[1],
                                                          attrs[1].attr[1],attrs[2].attr[1]);

                // call the fragShader
                frag->FragShader(target[y][x],interpolatedAttribs, *uniforms);
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
    else if((*vert).VertShader == &TransformationVertShader)
    {
        for(int i = 0; i < numIn; i++)
        {
            (*vert).VertShader(transformedVerts[i], transformedAttrs[i], inputVerts[i], inputAttrs[i], (*uniforms));         
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

        // Your code goes here
        //TestDrawPerspectiveCorrect(frame);
        TestVertexShader(frame);
        //TestDrawFragments(frame);

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
