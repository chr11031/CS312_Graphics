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
    frag->FragShader(target[(int)v[0].y][(int)v[0].x], *attrs, *uniforms);
}

/****************************************
 * DRAW_LINE
 * Renders a line to the screen.
 ***************************************/
void DrawLine(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    // bool changed;
    // int x1 = line[0].x;
    // int y1 = line[0].y;
    // int x2 = line[1].x;
    // int y2 = line[1].y;
    // int x = x1;
    // int y = y1;

    // int dx = abs(x2 - x1);
    // int dy = abs(y2 - y1);

    // int signx = signum(x2 - x1);
    // int signy = signum(y2 - y1);

    // if (dy > dx)
    // {
    //     swap(dx, dy);
    //     changed = true;
    // }

    // Attributes pointAttributes;
    // pointAttributes = attrs;   

    // float e = 2 * dy - dx;
    // for (int i = 1; i <= dx; i++)
    // {
    //     Vertex vert = {x, y, 1, 1};   

    //     DrawPrimitive(POINT, target, &vert, &pointAttributes);
    //     while (e >= 0)
    //     {
    //         if (changed)
    //             x = x + 1;
    //         else
    //             y = y + 1;
    //         e = e - 2 * dx;
    //     }
    //     if (changed)
    //         y += signy;
    //     else
    //         x += signx;
    //     e = e + 2 * dy;
    // } 
}

/****************************************
 * DRAW_TRIANGLE_SCANLINES
 *
 * Renders a triangle/convex polygon
 * to the screen with the appropriate 
 * fill pattern
 ***************************************/
void DrawTriangleScanlines(BufferImage & frame, Vertex* triangle, Attributes* attrs, Attributes* const uniforms, FragmentShader * const frag)
{
    // Sort from least to greatest - simple O(n^2) for small set
    // Y-ordered primarily, X-ordered secondarily 
    int count = 3;
    for(int i = 0; i < count; i++)
    {
        for(int j = 0; j < count - 1; j++)
        {
            if  (triangle[j].y > triangle[j+1].y || 
                ((triangle[j].y == triangle[j+1].y && (triangle[j].x > triangle[j+1].x))))
            {
                SWAP(Vertex, triangle[j], triangle[j+1]);
                SWAP(Attributes, attrs[j], attrs[j+1]);
            }
        }
    }

    // Determinant information
    Vertex firstLeft = triangle[0];
    Vertex lastRight = triangle[count-1]; 
    int diffX = (int)lastRight.x - (int)firstLeft.x;
    int diffY = (int)lastRight.y - (int)firstLeft.y;

    // Build left-right lists
    bool flatTop = triangle[0].y == triangle[1].y;
    bool flatBottom = triangle[count-1].y == triangle[count-2].y;
    int lastIteration = count - 1;
    int numLeft = 0;
    int numRight = 0;
    Vertex left[8];
    Vertex right[8];
    Attributes lAttr[8]; 
    Attributes rAttr[8];
    for(int i = 0; i < count; i++)
    {
        int det = determinant((int)triangle[i].x - (int)firstLeft.x, diffX, 
			      (int)triangle[i].y - (int)firstLeft.y, diffY);
        if(det > 0)
        {
            rAttr[numRight] = attrs[i];
            right[numRight++] = triangle[i];
        }
        else if (det < 0)
        {
            lAttr[numLeft] = attrs[i];
            left[numLeft++] = triangle[i];
        }
        else
        {
            if(!(i == 0 && flatTop))
            {
                rAttr[numRight] = attrs[i];
                right[numRight++] = triangle[i];
            }
            if(!(i == lastIteration && flatBottom))
            {
                lAttr[numLeft] = attrs[i];
                left[numLeft++] = triangle[i];
            }
        }
    }

     // Adjust counts for bounds checks
    --numLeft;
    --numRight;

     // Draw so that we are careful about adjacent polygon fitting
    int iLeftFirst  = -1;
    int iLeftSecond = 0;
    int iRightFirst = -1;
    int iRightSecond= 0;
    int y = left[0].y;
    int lastY = left[numLeft].y;
    double startX = left[0].x;
    double endX = right[0].x;
    double numerator;
    double divisor;
    double stepLeft;
    double stepRight;

    Vertex scanVertices[2];
    while(y <= lastY)
    {
        // Check bounds for interpolation
        if((int)left[iLeftSecond].y == y && iLeftSecond < numLeft)
        {
            ++iLeftFirst;
            ++iLeftSecond;
            stepLeft = 0.0;
            diffY = left[iLeftSecond].y - left[iLeftFirst].y;
            if(diffY != 0) // Why does this diffY even get tested???
            {
                numerator = left[iLeftSecond].x - left[iLeftFirst].x; 
                divisor = diffY;
                stepLeft = numerator / divisor;
            }
        }
        if((int)right[iRightSecond].y == y && iRightSecond < numRight)
        {
            ++iRightSecond;
            ++iRightFirst;
            stepRight = 0.0;
            diffY = right[iRightSecond].y - right[iRightFirst].y;
            if(diffY != 0) // Why does this diffY even get tested???
            {
                numerator = right[iRightSecond].x - right[iRightFirst].x; 
                divisor = diffY;
                stepRight = numerator / divisor;
            }
        }

         // Scanline process
        scanVertices[0].x = startX;
        scanVertices[0].y = y;
        scanVertices[1].x = endX;
        scanVertices[1].y = y;

        int numSteps = (scanVertices[1].x - scanVertices[0].x);
        while(scanVertices[0].x <= scanVertices[1].x)
        {
            frame[scanVertices[0].y][(int)scanVertices[0].x++] = attrs[0].color;
        }

        startX += stepLeft;
        endX += stepRight;
        y++;
    }
}

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    // Compute first, second, third X-Y pairs
    double firstVec[] = {triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y};
    double secndVec[] = {triangle[2].x - triangle[1].x, triangle[2].y - triangle[1].y};
    double thirdVec[] = {triangle[0].x - triangle[2].x, triangle[0].y - triangle[2].y};

    // Compute area of the whole triangle
    double areaTriangle = determinant(firstVec[X_KEY], -thirdVec[X_KEY], firstVec[Y_KEY], -thirdVec[Y_KEY]);

    // bounds of the triangle
    int minX = MIN3(triangle[0].x, triangle[1].x, triangle[2].x);
    int minY = MIN3(triangle[0].y, triangle[1].y, triangle[2].y);
    int maxX = MAX3(triangle[0].x, triangle[1].x, triangle[2].x);
    int maxY = MAX3(triangle[0].y, triangle[1].y, triangle[2].y);

    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x++)
        {
            // Determine if the pixel is in the triangle by the determinant's sign
            double firstDet = determinant(firstVec[X_KEY], x - triangle[0].x, firstVec[Y_KEY], y - triangle[0].y);
            double secndDet = determinant(secndVec[X_KEY], x - triangle[1].x, secndVec[Y_KEY], y - triangle[1].y);
            double thirdDet = determinant(thirdVec[X_KEY], x - triangle[2].x, thirdVec[Y_KEY], y - triangle[2].y);

            // All 3 signs > 0 means the center point is inside, to the left of the 3 CCW vectors 
            if(firstDet >= 0 && secndDet >= 0 && thirdDet >= 0)
            {
                target[(int)y][(int)x] = attrs[0].color;

                double z = 1 / interp(areaTriangle, firstDet, secndDet, thirdDet, triangle[0].w, triangle[1].w, triangle[2].w);

                // Interpolate Attributes for this pixel - In this case the R,G,B values
                Attributes interpolatedAttribs;
                interpolatedAttribs.r = interp(areaTriangle, firstDet, secndDet, thirdDet, attrs[0].r, attrs[1].r, attrs[2].r) * z;
                interpolatedAttribs.g = interp(areaTriangle, firstDet, secndDet, thirdDet, attrs[0].g, attrs[1].g, attrs[2].g) * z;
                interpolatedAttribs.b = interp(areaTriangle, firstDet, secndDet, thirdDet, attrs[0].b, attrs[1].b, attrs[2].b) * z;
                interpolatedAttribs.u = interp(areaTriangle, firstDet, secndDet, thirdDet, attrs[0].u, attrs[1].u, attrs[2].u) * z;
                interpolatedAttribs.v = interp(areaTriangle, firstDet, secndDet, thirdDet, attrs[0].v, attrs[1].v, attrs[2].v) * z;

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

        // Tests perspective correction
        TestDrawPerspectiveCorrect(frame);
        
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
