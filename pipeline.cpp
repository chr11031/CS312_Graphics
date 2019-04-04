#include "definitions.h"
#include "coursefunctions.h"
#include <algorithm>

/***********************************************
 * Cross_PRODUCT
 **********************************************/
double crossProduct(Vertex v1, Vertex v2)
{
    double result;

    result = (v1.x * v2.y) - (v1.y * v2.x);
    return result;
}

/************************************************
 * INTERPOLATION
 * 
 * interpolates the colors using the weighing
 * method discussed in the reading.
 * **********************************************/

double interp(double area, double determinate1, double determinate2, double determinate3, double & triColor1, double & triColor2, double & triColor3)
{
    // Barycentric Coordinates Holders
    double triArea1 = determinate1 / area;
    double triArea2 = determinate2 / area;
    double triArea3 = determinate3 / area;

    //Now is the time to relate the colors to the position in the triangle
    double color = triArea2 * triColor1 + triArea3 * triColor2 + triArea1 * triColor3;
    return color;
}

/************************************************
 * CORRECTED PERSPECTIVE INTERPOLATION
 * 
 * interpolates the colors using the weighing
 * method discussed in the reading.
 * **********************************************/
Attributes correctInterp(double area, double determinate1, double determinate2, double determinate3, Attributes* const attrs, double* inverted)
{
    Attributes correct;
    double triArea1 = determinate1 / area;
    double triArea2 = determinate2 / area;
    double triArea3 = determinate3 / area;

    //computing correct locations for colors based on depth
    correct.r = triArea2 * attrs[0].r + triArea3 * attrs[1].r + triArea1 * attrs[2].r;
    correct.g = triArea2 * attrs[0].g + triArea3 * attrs[1].g + triArea1 * attrs[2].g;
    correct.b = triArea2 * attrs[0].b + triArea3 * attrs[1].b + triArea1 * attrs[2].b;

    //computing correct places for texture based on depth
    correct.u = triArea2 * attrs[0].u + triArea3 * attrs[1].u + triArea1 * attrs[2].u;
    correct.v = triArea2 * attrs[0].v + triArea3 * attrs[1].v + triArea1 * attrs[2].v;

    double z = 1 / (triArea2 * inverted[0] + triArea3 * inverted[1] + triArea1 * inverted[2]);

    //color
    correct.r *= z;
    correct.g *= z;
    correct.b *= z;

    //texture
    correct.u *= z;
    correct.v *= z;

    return correct;
}

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
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag) //changed Attributes* attrs
{
    //Creating Bounding Box
    int maxX = std::max(triangle[0].x, std::max(triangle[1].x, triangle[2].x));
    int minX = std::min(triangle[0].x, std::min(triangle[1].x, triangle[2].x));
    int maxY = std::max(triangle[0].y, std::max(triangle[1].y, triangle[2].y));
    int minY = std::min(triangle[0].y, std::min(triangle[1].y, triangle[2].y));

    //I'm modifying the code to be doubles from vertexes althought secretly it is the same thing
    double firstVector[]  = {(triangle[1].x - triangle[0].x), (triangle[1].y - triangle[0].y)};
    double secondVector[] = {(triangle[2].x - triangle[1].x), (triangle[2].y - triangle[1].y)};
    double thirdVector[]  = {(triangle[0].x - triangle[2].x), (triangle[0].y - triangle[2].y)};

    //Time to get the area of the whole triangle
    double areaTriangle = determinant(firstVector[X_KEY], -thirdVector[X_KEY], firstVector[Y_KEY], -thirdVector[Y_KEY]);


    //Everything up to the for loop is for computing perspective correct attributes
    Attributes correct [3];
    //divide attribute by the z coordinate this is for color
    correct[0].r = attrs[0].r / triangle[0].z;
    correct[0].g = attrs[0].g / triangle[0].z;
    correct[0].b = attrs[0].b / triangle[0].z;

    //Second Vertex
    correct[1].r = attrs[1].r / triangle[1].z;
    correct[1].g = attrs[1].g / triangle[1].z;
    correct[1].b = attrs[1].b / triangle[1].z;

    //Third Vertex
    correct[2].r = attrs[2].r / triangle[2].z;
    correct[2].g = attrs[2].g / triangle[2].z;
    correct[2].b = attrs[2].b / triangle[2].z;

    //Divide texture map by the Z axis u for X-axis and v for Y-axis
    correct[0].u = attrs[0].u / triangle[0].z;
    correct[1].u = attrs[1].u / triangle[1].z;
    correct[2].u = attrs[2].u / triangle[2].z;

    //Y-axis now
    correct[0].v = attrs[0].v / triangle[0].z;
    correct[1].v = attrs[1].v / triangle[1].z;
    correct[2].v = attrs[2].v / triangle[2].z;

    //inverses of all Z values
    double inverted_Zs [3];

    inverted_Zs [0] = 1 / triangle[0].z;
    inverted_Zs [1] = 1 / triangle[1].z;
    inverted_Zs [2] = 1 / triangle[2].z;


    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
                            // Determine if the pixel is in the triangle by the determinant's sign
                double firstDet = determinant(firstVector[X_KEY], x - triangle[0].x, firstVector[Y_KEY], y - triangle[0].y);
                double secndDet = determinant(secondVector[X_KEY], x - triangle[1].x, secondVector[Y_KEY], y - triangle[1].y);
                double thirdDet = determinant(thirdVector[X_KEY], x - triangle[2].x, thirdVector[Y_KEY], y - triangle[2].y);

                // All 3 signs > 0 means the center point is inside, to the left of the 3 CCW vectors 
                if(firstDet >= 0 && secndDet >= 0 && thirdDet >= 0)
                {

                //colors everything red in case I mess something up.
                target[(int)y][(int)x] = attrs[0].color;

                //Place to store perspective attributes
                Attributes interpolatedAttribs;

                // Interpolate Attributes for this pixel - In this case the R,G,B values this is for affline
                /*
                interpolatedAttribs.r = interp(areaTriangle, firstDet, secndDet, thirdDet, attrs[0].r, attrs[1].r, attrs[2].r);
                interpolatedAttribs.g = interp(areaTriangle, firstDet, secndDet, thirdDet, attrs[0].g, attrs[1].g, attrs[2].g);
                interpolatedAttribs.b = interp(areaTriangle, firstDet, secndDet, thirdDet, attrs[0].b, attrs[1].b, attrs[2].b);


                //Now its time for image interpolation
                //firstDet * u1 + secondDet * u2 + thirdDet * u3;
                interpolatedAttribs.u = interp(areaTriangle, firstDet, secndDet, thirdDet, attrs[0].u, attrs[1].u, attrs[2].u);
                interpolatedAttribs.v = interp(areaTriangle, firstDet, secndDet, thirdDet, attrs[0].v, attrs[1].v, attrs[2].v);
                */
                //This is for corrected perspective
                interpolatedAttribs = correctInterp(areaTriangle, firstDet, secndDet, thirdDet, attrs, inverted_Zs);

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
void VertexShaderExecuteVertices(const VertexShader* vert, Vertex const inputVerts[], Attributes const inputAttrs[], const int& numIn, Attributes* const uniforms, Vertex transformedVerts[], Attributes transformedAttrs[])
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
        //processUserInputs(running);

        // Refresh Screen
        //clearScreen(frame);

        // Your code goes here
        //TestDrawFragments(frame);
        //TestDrawPerspectiveCorrect(frame);
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
