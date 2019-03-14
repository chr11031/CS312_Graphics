#include "definitions.h"
#include "coursefunctions.h"
#include <cmath>

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
        if(e.type == SDL_MOUSEMOTION)
        {
            int cur - SDL_ShowCursor(SDL_QUERY);
            if(cur == SDL_DISABLE)
            {
                double mouseX = e.motion.xrel;
                double mouseY = e.motion.yrel;

                myCam.yaw -= mouseX * 0.02;
                myCam.pitch += mouseY * 0.02;
            }
        }
        
        if (e.type == SDL_MOUSEBUTTONDOWN)
        {
            int cur = SDL_ShowCursor(SDL_QUERY);
            if(cur == SDL_DISABLE)
            {
                SDL_ShowCursor(SDL_ENABLE);
                SDL_SetRelativeMouseMode(SDL_FALSE);
            }
            else{
                SDL_ShowCursor(SDL_DISABLE);
                SDL_SetRelativeMouseMode(SDL_TRUE);
            }
        }

        // Translation
        if ((e.key.keysym.sym == 'w' && e.type == SDL_KEYDOWN))
        {
            myCam.z += cos(myCam.yaw * DEG_TO_RAD) * 0.05;
            myCam.x -= sin(myCam.yaw * DEG_TO_RAD) * 0.05;
        }
        if ((e.key.keysym.sym == 's' && e.type == SDL_KEYDOWN))
        {
            myCam.z += cos(myCam.yaw * DEG_TO_RAD) * 0.05;
            myCam.x -= sin(myCam.yaw * DEG_TO_RAD) * 0.05;
        }
        if ((e.key.keysym.sym == 'a' && e.type == SDL_KEYDOWN))
        {
            myCam.x -= cos(myCam.yaw * DEG_TO_RAD) * 0.05;
            myCam.z -= sin(myCam.yaw * DEG_TO_RAD) * 0.05;
        }
        if ((e.key.keysym.sym == 'd' && e.type == SDL_KEYDOWN))
        {
            myCam.x += cos(myCam.yaw * DEG_TO_RAD) * 0.05;
            myCam.z -= sin(myCam.yaw * DEG_TO_RAD) * 0.05;
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
    // Set our pixel according to the attribute value
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
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms = NULL, FragmentShader* const frag = NULL)
{
    // My Code (with help from the reading and Bro Christiansen)
    // This finds the minimum x and y values of the vectors, creating a bouding box
    // around the triangle that we can loop through
    int maxX = MAX3(triangle[0].x, triangle[1].x, triangle[2].x);
    int minX = MIN3(triangle[0].x, triangle[1].x, triangle[2].x);
    int maxY = MAX3(triangle[0].y, triangle[1].y, triangle[2].y);
    int minY = MIN3(triangle[0].y, triangle[1].y, triangle[2].y);

    // set the origin of each vector to 0. This makes the cross-product/determinant much easier
    Vertex * vert1 = new Vertex {triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y, 1, 1};
    Vertex * vert2 = new Vertex {triangle[2].x - triangle[1].x, triangle[2].y - triangle[1].y, 1, 1};
    Vertex * vert3 = new Vertex {triangle[0].x - triangle[2].x, triangle[0].y - triangle[2].y, 1, 1};

    // find the total area of the triangle, we need this to find relative areas later
    double totArea = Determinant(vert1->x, vert1->y, vert2->x, vert2->y);

    // looping through each pixel in the bounding box
    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {

            // find the determinant of each vertex, later we compare this to the entire triangle
            double firstD = Determinant(vert1->x, x - triangle[0].x, vert1->y, y - triangle[0].y);
            double secD = Determinant(vert2->x, x - triangle[1].x, vert2->y, y - triangle[1].y);
            double thirD = Determinant(vert3->x, x - triangle[2].x, vert3->y, y - triangle[2].y);

            // if each of the determinants are positive relative to each vertex we made, then it is within the triangle
            if (firstD >= 0 && secD >= 0 && thirD >= 0)
            {
                // interpolate w first, before we pass it into the interpolate function
                // the corect z is 1/w

                Attributes intAttrs;
                // find the interpolated value based on the percentage each area is of the whole triangle
                intAttrs = Interpolate(totArea, firstD, secD, thirD, attrs, triangle);
                // also interpolate with the w value, then take the reciprocal

                // call the fragment shader with the interpolated attributes
                frag->FragShader(target[y][x], intAttrs, *uniforms);
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

// interpolating between two vertices with along as the two vertices
Vertex vertBetweenVerts(const Vertex &vertA, const Vertex &vertB, const double &along)
{
    Vertex rv;
    rv.x = (VertA.x) + (vertB.x - vertA.x) * along);
    rv.y = (VertA.y) + (vertB.y - vertA.y) * along);
    rv.z = (VertA.z) + (vertB.z - vertA.z) * along);
    rv.w = (VertA.w) + (vertB.w - vertA.w) * along);

    return rv;
}

// TODO: copy brother christiansen's entier clip vertices class
// clips vertices
void clipVertices(Vertex const transformedVerts, Attributes const transformedAttrs, const int &numIn, Vertex clippedVertices[], Attributes clippedAttrs, int & numClipped)
{
    // pass-through behavior: no longer needed
    // numclipped = numIn;
    // for(int i = 0; i < numClipped; i++)
    // {
    //     clippedVerts[i] = transformedVerts[i];
    //     clippedAttrs[i] = transformedAttrs[i];
    // }

    // TMP clip buffers
    int num = 0;
    int numOut = 0;
    bool inBounds[MAX_VERTICES];
    Vertex tmpVertA[MAX_VERTICES];
    Vertex tmpVertB[MAX_VERTICES];
    Attributes tmpAttrA[MAX_VERTICES];
    Attributes tmpAttrB[MAX_VERTICES];
    
    Vertex const * srcVerts;
    Vertex * sink verts;
    Attributes const * srcAttrs;
    Attributes * sinkAttrs;

    // setup pointers for first round
    srcVerts = transformedVerts;
    srcAttrs = transfomrmedAttrs;
    sinkVerts = tmpVertA;
    sinkAttrs = tmpAttrA;
    num = numIn;
    numOut = 0;

    double wLimit = 0.001;
    for (int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].w > wLimit);
    }
    for (int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i + 1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if (curVertexIn && nexVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if (curVertexIn && !nextVertexIn)
        {
            double along = 0;
            intersectAgainstYLimit(along, wLimit, srcVerts[cur].w, srcVerts[next].w);
            sinkVerts[numOuut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOuut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertexIn && nextVertexIn)
        {
            double along = 0;
            intersectAgainstYLimit(along, wLimit, srcVerts[cur].w, srcVerts[next].w)

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOUt] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            // do nothing
        }
    }

    /*******************************
     * first pass
     ******************************/

    // clip against x
    srcVerts = tmpVertA;
    srcAttrs = tmpAttrA;
    sinkVerts = tmpVertB;
    sinkAttrs = tmpAttrB;
    num = numOut;
    numOut = 0;

    // clip against X==W
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].x < srcVerts[i].w);
    }
    for (int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i + 1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if (curVertexIn && nexVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if (curVertexIn && !nextVertexIn)
        {
            double along = 0;
            intersectAtPositiveLine(along, wLimit, srcVerts[cur].x, srcVerts[cur].w,
                                    srcVerts[next].w, srcVerts[next].w);
            sinkVerts[numOuut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOuut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertexIn && nextVertexIn)
        {
            double along = 0;
            intersectAtPositiveLine(along, srcVerts[cur].x, srcVerts[cur].w,
                                    srcVerts[next].x, srcVerts[next].w)
            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOUt] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            // do nothing
        }
    }

    // setup pointers
    srcVerts = tmpVertB;
    srcAttrs = tmpAttrB;
    sinkVerts = tmpVertA;
    sinkAttrs = tmpAttrA;

    // clip against -X==W
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].x < srcVerts[i].w);
    }
    for (int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i + 1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if (curVertexIn && nexVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if (curVertexIn && !nextVertexIn)
        {
            double along = 0;
            intersectAtNegativeLine(along, wLimit, srcVerts[cur].x, srcVerts[cur].w,
                                    srcVerts[next].w, srcVerts[next].w);
            sinkVerts[numOuut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOuut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertexIn && nextVertexIn)
        {
            double along = 0;
            intersectAtNegativeLine(along, srcVerts[cur].x, srcVerts[cur].w,
                                    srcVerts[next].x, srcVerts[next].w)
            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOUt] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            // do nothing
        }
    }

    /*******************************
     * second pass
     ******************************/

    // clip against x second pass
    srcVerts = tmpVertA;
    srcAttrs = tmpAttrA;
    sinkVerts = tmpVertB;
    sinkAttrs = tmpAttrB;
    num = numOut;
    numOut = 0;

    // clip against X==W
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].x < srcVerts[i].w);
    }
    for (int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i + 1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if (curVertexIn && nexVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if (curVertexIn && !nextVertexIn)
        {
            double along = 0;
            intersectAtPositiveLine(along, wLimit, srcVerts[cur].x, srcVerts[cur].w,
                                    srcVerts[next].w, srcVerts[next].w);
            sinkVerts[numOuut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOuut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertexIn && nextVertexIn)
        {
            double along = 0;
            intersectAtPositiveLine(along, srcVerts[cur].x, srcVerts[cur].w,
                                    srcVerts[next].x, srcVerts[next].w)
            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOUt] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            // do nothing
        }
    }

    // setup pointers
    srcVerts = tmpVertB;
    srcAttrs = tmpAttrB;
    sinkVerts = tmpVertA;
    sinkAttrs = tmpAttrA;

    // clip against -X==W second pass
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].x < srcVerts[i].w);
    }
    for (int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i + 1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if (curVertexIn && nexVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if (curVertexIn && !nextVertexIn)
        {
            double along = 0;
            intersectAtNegativeLine(along, wLimit, srcVerts[cur].x, srcVerts[cur].w,
                                    srcVerts[next].w, srcVerts[next].w);
            sinkVerts[numOuut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOuut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertexIn && nextVertexIn)
        {
            double along = 0;
            intersectAtNegativeLine(along, srcVerts[cur].x, srcVerts[cur].w,
                                    srcVerts[next].x, srcVerts[next].w)
            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOUt] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            // do nothing
        }
    }

    /*******************************
     * third pass
     ******************************/

    // clip against x third pass
    srcVerts = tmpVertA;
    srcAttrs = tmpAttrA;
    sinkVerts = tmpVertB;
    sinkAttrs = tmpAttrB;
    num = numOut;
    numOut = 0;

    // clip against X==W
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (-srcVerts[i].x < srcVerts[i].w);
    }
    for (int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i + 1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if (curVertexIn && nexVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if (curVertexIn && !nextVertexIn)
        {
            double along = 0;
            intersectAtPositiveLine(along, wLimit, srcVerts[cur].x, srcVerts[cur].w,
                                    srcVerts[next].w, srcVerts[next].w);
            sinkVerts[numOuut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOuut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertexIn && nextVertexIn)
        {
            double along = 0;
            intersectAtPositiveLine(along, srcVerts[cur].x, srcVerts[cur].w,
                                    srcVerts[next].x, srcVerts[next].w)
            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOUt] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            // do nothing
        }
    }

    // setup pointers
    srcVerts = tmpVertB;
    srcAttrs = tmpAttrB;
    sinkVerts = tmpVertA;
    sinkAttrs = tmpAttrA;

    // clip against -X==W third pass
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].x < srcVerts[i].w);
    }
    for (int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i + 1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if (curVertexIn && nexVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if (curVertexIn && !nextVertexIn)
        {
            double along = 0;
            intersectAtNegativeLine(along, wLimit, srcVerts[cur].x, srcVerts[cur].w,
                                    srcVerts[next].w, srcVerts[next].w);
            sinkVerts[numOuut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOuut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertexIn && nextVertexIn)
        {
            double along = 0;
            intersectAtNegativeLine(along, srcVerts[cur].x, srcVerts[cur].w,
                                    srcVerts[next].x, srcVerts[next].w)
            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOUt] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            // do nothing
        }
    }
    /*******************************
     * fourth pass
     ******************************/

    // clip against y fourth pass
    srcVerts = tmpVertA;
    srcAttrs = tmpAttrA;
    sinkVerts = tmpVertB;
    sinkAttrs = tmpAttrB;
    num = numOut;
    numOut = 0;

    // clip against X==W
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].y < srcVerts[i].w);
    }
    for (int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i + 1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if (curVertexIn && nexVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if (curVertexIn && !nextVertexIn)
        {
            double along = 0;
            intersectAtPositiveLine(along, wLimit, srcVerts[cur]yx, srcVerts[cur].w,
                                    srcVerts[next].w, srcVerts[next].w);
            sinkVerts[numOuut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOuut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertexIn && nextVertexIn)
        {
            double along = 0;
            intersectAtPositiveLine(along, srcVerts[cur].y, srcVerts[cur].w,
                                    srcVerts[next].y, srcVerts[next].w)
            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOUt] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            // do nothing
        }
    }

    // setup pointers
    srcVerts = tmpVertB;
    srcAttrs = tmpAttrB;
    sinkVerts = tmpVertA;
    sinkAttrs = tmpAttrA;

    // clip against -X==W third pass
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].y < srcVerts[i].w);
    }
    for (int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i + 1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if (curVertexIn && nexVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if (curVertexIn && !nextVertexIn)
        {
            double along = 0;
            intersectAtNegativeLine(along, wLimit, srcVerts[cur].y, srcVerts[cur].w,
                                    srcVerts[next].w, srcVerts[next].w);
            sinkVerts[numOuut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOuut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertexIn && nextVertexIn)
        {
            double along = 0;
            intersectAtNegativeLine(along, srcVerts[cur].y, srcVerts[cur].w,
                                    srcVerts[next].y, srcVerts[next].w)
            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOUt] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            // do nothing
        }
    }
}

// normalizes vertices
normalizeVertices(Vertex &clippedVertices, Attributes const &clippedAttrs, const int &numClipped)
{
    for (int i = 0; i < numClipped; i++)
    {
        // normalize x, y, and z components of homogeneous coordinates
        for clippedVertices[i].x /= clippedVertices[i].w;
        for clippedVertices[i].y /= clippedVertices[i].w;
        for clippedVertices[i].z /= clippedVertices[i].w;

        // setub w value for depth interpolation
        double zValue = clippedVertices[i].w;
        for clippedVertices[i].w = 1.0 / clippedVertices[i].w;

        // setup attributes
        for(int j = 0; j < clippedAttrs[i].count; j++)
        {
            clippedAttrs[i][j].dbl /= zValue;
        }
    }
}

// viewport transforming to 2D screen
viewportTransform(Vertex clippedVertices[], Buffer2D<PIXEL> &target, const int &numClipped)
{
    int w = target.width();
    int h = target.height();

    for (int i = 0; i < numClipped; i++)
    {
        clippedVertices[i].x = round( (( (clippedVertices[i].x + 1) / 2.0 * w))); // gives a percentage then multiplies by width
        clippedVertices[i].y = round( (( (clippedVertices[i].y + 1) / 2.0 * h))); // same then by height
    }
}

void intersectAgainstYLimit(double &along, const double &yLimit, const double &segStartY,
                            const double &segEndY)
{
    along = -1;
    double segDiffY = (segEndY - segStartY);
    if (segDiff == 0)
    {
        return;
    }

    along = (yLimit - segStartY) / segDiffY;
}

void intersectAtPositiveLine(double &along, const double &segStartX, const double &segStartY,
                            const double &segEndX, const double &segEndY)
{
    along = -1;
    double segDiffX = (segEndX - segStartX);
    double segDiffY = (segEndY - segStartY);

    if (segDiffX == segDiffY)
    {
        return;
    }

    along = (segStartY - segStartX) / (segDiffX - segDiffY);
}

void intersectAtNegativeLine(double &along, const double &segStartX, const double &segStartY,
                            const double &segEndX, const double &segEndY)
{
    along = -1;
    double segDiffX = (segEndX - segStartX);
    double segDiffY = (segEndY - segStartY);

    if (segDiffX == segDiffY)
    {
        return;
    }

    along = (segStartY + segStartX) / (-segDiffX - segDiffY);
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

    // Clipping
    Vertex clippedVertices[MAX_VERTICES];
    Attributes clippedAttrs[MAX_VERTICES];
    int numClipped;
    clipVertices(transformedVerts, transformedAttrs, numIn, clippedVertices, clippedAttrs, numClipped);

    // Normalize
    normalizeVertices(clippedVertices,clippedAttrs, numClipped);

    // viewport transforming to 2D screen
    viewportTransform(clippedVertices, target, numClipped);

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
            Vertex tri[3];
            Attributes vAttr[3];
            for(int i = 2; i < numClipped; i++)
            {
                tri[0] = clippedVertices[0];
                tri[1] = clippedVertices[i - 1];
                tri[2] = clippedVertices[i];

                vAttr[0] = clippedVertices[0];
                vAttr[1] = clippedVertices[i - 1];
                vAttr[2] = clippedVertices[i];

                DrawTriangle(target, tri, vAttr, uniforms, frag);
            }
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
        // TestDrawFragments(frame);
        // TestDrawTriangle(frame);
        // TestDrawPerspectiveCorrect(frame);
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
