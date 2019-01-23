#include "definitions.h"
#include "coursefunctions.h"
#include <algorithm>

void fillBottomFlatTriangle(Buffer2D<PIXEL> &target,
                            Vertex vertex1,
                            Vertex vertex2,
                            Vertex vertex3,
                            Attributes *const attrs);
void fillTopFlatTriangle(Buffer2D<PIXEL> &target,
                         Vertex vertex1,
                         Vertex vertex2,
                         Vertex vertex3,
                         Attributes *const attrs);

/***********************************************
 * CLEAR_SCREEN
 * Sets the screen to the indicated color value.
 **********************************************/
void clearScreen(Buffer2D<PIXEL> &frame, PIXEL color = 0xff000000)
{
    int h = frame.height();
    int w = frame.width();
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            frame[y][x] = color;
        }
    }
}

/************************************************************
 * UPDATE_SCREEN
 * Blits pixels from RAM to VRAM for rendering.
 ***********************************************************/
void SendFrame(SDL_Texture *GPU_OUTPUT, SDL_Renderer *ren, SDL_Surface *frameBuf)
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
void processUserInputs(bool &running)
{
    SDL_Event e;
    int mouseX;
    int mouseY;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
        {
            running = false;
        }
        if (e.key.keysym.sym == 'q' && e.type == SDL_KEYDOWN)
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
void DrawPoint(Buffer2D<PIXEL> &target, Vertex *v, Attributes *attrs, Attributes *const uniforms, FragmentShader *const frag)
{
    target[(int)v[0].y][(int)v[0].x] = attrs[0].color;
}

/****************************************
 * DRAW_TRIANGLE
 * Renders a line to the screen.
 ***************************************/
void DrawLine(Buffer2D<PIXEL> &target, Vertex *const line, Attributes *const attrs, Attributes *const uniforms, FragmentShader *const frag)
{
    Vertex vertex1 = line[0];
    Vertex vertex2 = line[1];

    for (int x = vertex1.x; x > vertex2.x; x--)
    {
        target[vertex1.y][x] = attrs[0].color;
    }
}

bool ptInTriangle(Vertex p, Vertex p0, Vertex p1, Vertex p2)
{
    float A = (float)1/2 * (-p1.y * p2.x + p0.y * (-p1.x + p2.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y);
    int sign = A < 0 ? -1 : 1;
    int s = (p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * p.x + (p0.x - p2.x) * p.y) * sign;
    int t = (p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y) * sign;

    bool a = s > 0;
    bool b = t > 0;
    bool c = (s+t);
    bool d = (2 * A * sign);
    bool e = c < d;
    return s > 0 && t > 0 && (s + t) < 2 * A * sign;
}

void DrawTriangleBarycentric(Buffer2D<PIXEL> &target, Vertex *const triangle, Attributes *const attrs, Attributes *const uniforms, FragmentShader *const frag)
{
    Vertex v1 = triangle[0];
    Vertex v2 = triangle[1];
    Vertex v3 = triangle[2];

    int maxX = std::max(v1.x, std::max(v2.x, v3.x));
    int minX = std::min(v1.x, std::min(v2.x, v3.x));
    int maxY = std::max(v1.y, std::max(v2.y, v3.y));
    int minY = std::min(v1.y, std::min(v2.y, v3.y));

    // Vertex vs1 = {v2.x - v1.x, v2.y - v1.y, 1, 1};
    // Vertex vs2 = {v3.x - v1.x, v3.y - v1.y, 1, 1};

    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            Vertex q = { x, y, 1, 1};

            bool inTriangle = ptInTriangle(q, v1, v2, v3);

            if (inTriangle)
            { /* inside triangle */
                target[y][(int)x] = attrs[0].color;
            }
        }
    }
}



/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> &target, Vertex *const triangle, Attributes *const attrs, Attributes *const uniforms, FragmentShader *const frag)
{
    float n = sizeof(triangle)/sizeof(triangle[0]); 
    std::sort(triangle, triangle + 3);
    Vertex vertex1;
    Vertex vertex2;
    Vertex vertex3;

    if (triangle[0].y > triangle[1].y && triangle[0].y > triangle[2].y) {
        vertex1 = triangle[0];
        if (triangle[1].y == triangle[2].y) {
            vertex2 = (triangle[1].x > )
        }
    }

    if (vertex2.y == vertex3.y && vertex1.y > vertex2.y)
    {
        fillBottomFlatTriangle(target, vertex1, vertex2, vertex3, attrs);
    }
    else if (vertex2.y == vertex3.y && vertex1.y < vertex2.y)
    {
        fillTopFlatTriangle(target, vertex1, vertex2, vertex3, attrs);
    }
    else
    {
        // Vertex vertex4;
        // if (vertex2.y > vertex1.y && vertex3.y < vertex1.y)
        // {
        //     vertex4 = vertex1;
        // }
        // else if (vertex1.y > vertex2.y && vertex3.y < vertex2.y)
        // {
        //     vertex4 = vertex2;
        // }
        // else
        // {
        //     vertex4 = vertex3;
        // }

        // float slope = ((float)(vertex2.y - vertex3.y) / (vertex2.x - vertex3.x));
        // int distanceFromMiddleToBottom = (vertex4.y - vertex3.y);
        // vertex4.x = (int)(vertex3.x - ((float)distanceFromMiddleToBottom / slope));

        // fillBottomFlatTriangle(target, vertex2, vertex1, vertex4, attrs);
        // fillTopFlatTriangle(target, vertex1, vertex4, vertex3, attrs);
    }
}

void fillBottomFlatTriangle(Buffer2D<PIXEL> &target,
                            Vertex vertex1,
                            Vertex vertex2,
                            Vertex vertex3,
                            Attributes *const attrs)
{
    float invslope1 = abs((vertex2.x - vertex1.x) / (vertex2.y - vertex1.y));
    float invslope2 = (vertex3.x - vertex1.x) / (vertex3.y - vertex1.y);

    float curx1 = vertex1.x;
    float curx2 = vertex1.x;

    int lowerBound = (vertex1.y < vertex2.y) ? vertex1.y : vertex2.y;
    int upperBound = (vertex1.y > vertex2.y) ? vertex1.y : vertex2.y;

    for (int scanlineY = upperBound; scanlineY > lowerBound; scanlineY--)
    {
        for (float x = curx1; x > curx2; x--)
        {
            target[scanlineY][(int)x] = attrs[0].color;
        }
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

void fillTopFlatTriangle(Buffer2D<PIXEL> &target,
                         Vertex vertex1,
                         Vertex vertex2,
                         Vertex vertex3,
                         Attributes *const attrs)
{
    // Vertex v1 = ;
    // Vertex v2 = ;
    // Vertex v3 = ;

    // if (vertex1.y == vertex2.y)
    // {
    //     maxVertex = vertex3;
    //     if (vertex1.x > vertex2.x)
    //     {
    //         botLeft = vertex2;
    //         botRight = vertex1;
    //     }
    //     else {

    //     }
    // }
    // else if (vertex1.y == vertex3.y)
    // {
    //     maxVertex = vertex3;
    //     if (vertex1.x > vertex3.x)
    //     {
    //         botLeft = vertex3;
    //         botRight = vertex1;
    //     }
    // }
    // else
    // {
    //     maxVertex = vertex3;
    //     if (vertex1.x > vertex2.x)
    //     {
    //         botLeft = vertex2;
    //         botRight = vertex1;
    //     }
    // }

    float invslope2 = (vertex2.y - vertex1.y != 0) ? (vertex2.x - vertex1.x) / (vertex2.y - vertex1.y) : 0;
    float invslope1 = (vertex3.x - vertex1.x) / (vertex3.y - vertex1.y);

    float curx1 = vertex1.x;
    float curx2 = vertex3.x;

    int lowerBound = (vertex1.y < vertex3.y) ? vertex1.y : vertex3.y;
    int upperBound = (vertex1.y > vertex3.y) ? vertex1.y : vertex3.y;

    for (int scanlineY = upperBound; scanlineY > lowerBound; scanlineY--)
    {
        for (float x = curx1; x > curx2; x--)
        {
            target[scanlineY][(int)x] = attrs[0].color;
        }
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

/**************************************************************
 * VERTEX_SHADER_EXECUTE_VERTICES
 * Executes the vertex shader on inputs, yielding transformed
 * outputs. 
 *************************************************************/
void VertexShaderExecuteVertices(const VertexShader *vert, Vertex const inputVerts[], Attributes const inputAttrs[], const int &numIn,
                                 Attributes *const uniforms, Vertex transformedVerts[], Attributes transformedAttrs[])
{
    // Defaults to pass-through behavior
    if (vert == NULL)
    {
        for (int i = 0; i < numIn; i++)
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
                   Buffer2D<PIXEL> &target,
                   const Vertex inputVerts[],
                   const Attributes inputAttrs[],
                   Attributes *const uniforms,
                   FragmentShader *const frag,
                   VertexShader *const vert,
                   Buffer2D<double> *zBuf)
{
    // Setup count for vertices & attributes
    int numIn = 0;
    switch (prim)
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
    switch (prim)
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
    SDL_Window *WIN;         // Our Window
    SDL_Renderer *REN;       // Interfaces CPU with GPU
    SDL_Texture *GPU_OUTPUT; // GPU buffer image (GPU Memory)
    SDL_Surface *FRAME_BUF;  // CPU buffer image (Main Memory)

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
    while (running)
    {
        // Handle user inputs
        processUserInputs(running);

        // Refresh Screen
        clearScreen(frame);

        // TestDrawPixel(frame);
        // GameOfLife(frame);
        // TestDrawLine(frame);
        TestDrawTriangle(frame);

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
