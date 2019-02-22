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
	target[(int)v[0].y][(int)v[0].x] = attrs[0].color;
}


/****************************************
 * determinant (cross product)
 * Takes vertexes and uses cross product math
 * to find the determinant
 ***************************************/
double determinant(const double & vtx1, const double & vtx2, 
                    const double & vty1, const double & vty2)//XXYY
{
    double determ = 0.0;
    determ = (vtx1 * vty2) - (vty1 * vtx2);
    return determ;
}

/****************************************
 * Interpolation helper
 * Area = 1/2 (area * area * area)
 ***************************************/
double interpolate(double areaTri, double determ1, double determ2, 
                    double determ3, double attr1, double attr2, double attr3)
{
    double w1 = (determ2 / areaTri) * attr1;
    double w2 = (determ3 / areaTri) * attr2;
    double w3 = (determ1 / areaTri) * attr3; 

    return w1 + w2 + w3;//((determ1 * attr1) + (determ2 * attr2) + (determ3 * attr3));
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
    // Your code goes here
    //Barycentric fill algorithm, counter clockwise
    //Step one bounding area
    double xmin = MIN3(triangle[0].x, triangle[1].x, triangle[2].x);
    double xmax = MAX3(triangle[0].x, triangle[1].x, triangle[2].x);
    double ymin = MIN3(triangle[0].y, triangle[1].y, triangle[2].y);
    double ymax = MAX3(triangle[0].y, triangle[1].y, triangle[2].y);

    //Step two define pos/neg areas, Start with vertexes
    double a[] = {triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y};
    double b[] = {triangle[2].x - triangle[1].x, triangle[2].y - triangle[1].y};
    double c[] = {triangle[0].x - triangle[2].x, triangle[0].y - triangle[2].y};

    double area = determinant(a[X_Key], -c[X_Key], a[y_Key], -c[y_Key]);
    
    //double areaTri = determinant(a.x,b.x,a.y,b.y);

    //step three, fill in
    //for point in bounding box, if positive drawpixel
    for(int ypix = ymin; ypix <= ymax; ypix++)
    {
        for(int xpix = xmin; xpix <= xmax; xpix++)
        {

            /*if(xpix == 250 && ypix == 140)
            {
                xpix %= 10000;
            }*/

            //iterating point
            Vertex cMid = {(xpix - triangle[0].x) , (ypix - triangle[0].y)};

            //Berycentric calculations (Cross Product deteminants)
            //double c1 = (cMid.x * b.y - cMid.y * b.x) / (a.x * b.y - a.y * b.x);
            //double c2 = (a.x * cMid.y - a.y * cMid.x) / (a.x * b.y - a.y * b.x);

            double firstDet = determinant(a[X_Key], xpix - triangle[0].x, a[y_Key], ypix - triangle[0].y);//1x,2x,1y,2y
            double secondDet = determinant(b[X_Key], xpix - triangle[1].x, b[y_Key], ypix - triangle[1].y);
            double thirdDet = determinant(c[X_Key], xpix - triangle[2].x, c[y_Key], ypix - triangle[2].y);

            //double areaTri = firstDet * 5;
            //double areaTri = areaTriangle;

            //If inside the triangle draw
            if (firstDet >= 0 && secondDet >= 0 && thirdDet >=0)
            {
                target[(int)ypix][(int)xpix] = attrs[0].color;
                //target[ypix][xpix] = attrs[0].color;
                Attributes interpolatedAttribs;
                interpolatedAttribs.r = interpolate(area, firstDet, secondDet, thirdDet, attrs[0].r, attrs[1].r, attrs[2].r);
                interpolatedAttribs.g = interpolate(area, firstDet, secondDet, thirdDet, attrs[0].g, attrs[1].g, attrs[2].g);
                interpolatedAttribs.b = interpolate(area, firstDet, secondDet, thirdDet, attrs[0].b, attrs[1].b, attrs[2].b);

                //for the image
                interpolatedAttribs.u = interpolate(area, firstDet, secondDet, thirdDet, attrs[0].u, attrs[1].u, attrs[2].u);
                interpolatedAttribs.v = interpolate(area, firstDet, secondDet, thirdDet, attrs[0].v, attrs[1].v, attrs[2].v);;

                frag->FragShader(target[ypix][xpix], interpolatedAttribs, *uniforms);
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

        // Your code goes here
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
