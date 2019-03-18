#include "shaders.h"
#include "definitions.h"
#include "coursefunctions.h"

#define MOVE_SPEED 1
#define MOUSE_SPEED 0.05

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
            int cur = SDL_ShowCursor(SDL_QUERY);
            if(cur == SDL_DISABLE)
            {
                double mouseX = e.motion.xrel;
                double mouseY = e.motion.yrel;

                myCam.yaw -= mouseX * MOUSE_SPEED;
                myCam.pitch += mouseY * MOUSE_SPEED;
            }
        }

        if(e.type == SDL_MOUSEBUTTONDOWN)
        {
            int cur = SDL_ShowCursor(SDL_QUERY);
            if (cur == SDL_DISABLE)
            {
                SDL_ShowCursor(SDL_ENABLE);
                SDL_SetRelativeMouseMode(SDL_FALSE);
            }
            else
            {
                SDL_ShowCursor(SDL_DISABLE);
                SDL_SetRelativeMouseMode(SDL_TRUE);
            }
        }

        // translation
        if (e.key.keysym.sym == 'w' && e.type == SDL_KEYDOWN)
        {
            myCam.z += (cos((myCam.yaw / 180) * M_PI)) * MOVE_SPEED;
            myCam.x += (sin((myCam.yaw / 180) * M_PI)) * MOVE_SPEED;
        }
        if (e.key.keysym.sym == 's' && e.type == SDL_KEYDOWN)
        {
            myCam.z -= (cos((myCam.yaw / 180) * M_PI)) * MOVE_SPEED;
            myCam.x -= (sin((myCam.yaw / 180) * M_PI)) * MOVE_SPEED;
        }
        if (e.key.keysym.sym == 'a' && e.type == SDL_KEYDOWN)
        {
            myCam.x -= (cos((myCam.yaw / 180) * M_PI)) * MOVE_SPEED;
            myCam.z += (sin((myCam.yaw / 180) * M_PI)) * MOVE_SPEED;
        }
        if (e.key.keysym.sym == 'd' && e.type == SDL_KEYDOWN)
        {
            myCam.x += (cos((myCam.yaw / 180) * M_PI)) * MOVE_SPEED;
            myCam.z -= (sin((myCam.yaw / 180) * M_PI)) * MOVE_SPEED;
        }
    }
}

/*************************************************************
 * CROSS_PRODUCT
 * Does the determinant function AD-BC. Essential 
 * building block for most of drawing.
 ************************************************************/
float crossProduct(const double & a, const double & b, const double & c, const double & d)
{
    return ((a * d) - (b * c));
}

/****************************************
 * DRAW_POINT
 * Renders a point to the screen with the
 * appropriate coloring.
 ***************************************/
void DrawPoint(Buffer2D<PIXEL> & target, Vertex* v, Attributes* attrs, Attributes * const uniforms, FragmentShader* const frag)
{
    // Set our pixel according to the attribute value!       
    frag->FragShader(target[(int)v[0].y][(int)v[0].x], *attrs, *uniforms);
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
    // gives us the four corners of the bounding box for the triangle
    int maxX = MAX3(triangle[0].x, triangle[1].x, triangle[2].x);
    int maxY = MAX3(triangle[0].y, triangle[1].y, triangle[2].y);
    int minX = MIN3(triangle[0].x, triangle[1].x, triangle[2].x);
    int minY = MIN3(triangle[0].y, triangle[1].y, triangle[2].y);

    // these are the vectors of two of the sides
    Vertex v1 = (Vertex){triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y};
    Vertex v2 = (Vertex){triangle[2].x - triangle[1].x, triangle[2].y - triangle[1].y};
    Vertex v3 = (Vertex){triangle[0].x - triangle[2].x, triangle[0].y - triangle[2].y}; 

    double area = crossProduct(v1.x, -v3.x, v1.y, -v3.y);

    // a loop to iterate through every pixel within our bounding box
    for (int x = minX; x <= maxX; x++) // starting at the leftmost side
    {
        for (int y = minY; y <= maxY; y++) // starting at the bottomost side
        {
            // now we do the crossproduct of the moving vert (v3, which is being incremented) and constant verts (v1 & v2) and put them in respect to the original crossproduct by dividing by the crossproduct of v1 and v2
            double det1 = crossProduct(v1.x, x - triangle[0].x, v1.y, y - triangle[0].y);
            double det2 = crossProduct(v2.x, x - triangle[1].x, v2.y, y - triangle[1].y);
            // double det3 = crossProduct(v3.x, x - triangle[2].x, v3.y, y - triangle[2].y);

            // now we are checking to see if the area ratio (a or b) are positive, then we see if combined they equal or are less than the actual triangle area
            if ((det1 >= 0) && (det2 >= 0) && (det1 + det2 < area))
            {
                Attributes interpolatedAttribs;
                interpolatedAttribs.numValues = attrs[0].numValues;
                interpolatedAttribs.interpolateValues(det1, det2, area, attrs, triangle);
                
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
    else
    {
        for (int i = 0; i < numIn; i++)
        {
            vert->VertShader(transformedVerts[i], transformedAttrs[i], inputVerts[i], inputAttrs[i], *uniforms);
        }
    }
    
}

/**************************************************************
 * INTERSECT AGAINST Y LIMIT
 * Helper function
 *************************************************************/
void intersectAgainstYLimit(double &along, const double &yLimit, const double &segStartY, const double &segEndY)
{
    along = -1;

    double segDiffY = (segEndY - segStartY);
    if (segDiffY == 0)
        return;

    along = (yLimit - segStartY) / segDiffY;
}

/**************************************************************
 * INTERSECT AT POSITIVE LINE
 * Helper function
 *************************************************************/
void intersectAtPositiveLine(double & along, const double &segStartX, const double &segStartY, const double &segEndX, const double &segEndY)
{
    along = -1;
    double segDiffX = (segEndX - segStartX);
    double segDiffY = (segEndY - segStartY);

    if (segDiffX == segDiffY)
        return;

    along = (segStartY - segStartX) / (segDiffX - segDiffY);
}

/**************************************************************
 * INTERSECT AT NEGATIVE LINE
 * Helper function
 *************************************************************/
void intersectAtNegativeLine(double & along, const double &segStartX, const double &segStartY, const double &segEndX, const double &segEndY)
{
    along = -1;
    double segDiffX = (segEndX - segStartX);
    double segDiffY = (segEndY - segStartY);

    if (segDiffX == segDiffY)
        return;

    along = (segStartY + segStartX) / (-segDiffX - segDiffY);
}

Vertex vertBetweenVerts(const Vertex &vertA, const Vertex &vertB, const double &along)
{
    Vertex rv;
    rv.x = (vertA.x) + ((vertB.x - vertA.x) * along);
    rv.y = (vertA.y) + ((vertB.y - vertA.y) * along);
    rv.z = (vertA.z) + ((vertB.z - vertA.z) * along);
    rv.w = (vertA.w) + ((vertB.w - vertA.w) * along);

    return rv;
}

/**************************************************************
 * CLIP VERTICES
 * 
 *************************************************************/
void clipVertices(Vertex const transformedVerts[], Attributes const transformedAttrs[], const int &numIn, Vertex clippedVertices[], Attributes clippedAttrs[], int &numClipped)
{
    // TMP Clip buffers
    int  num;
    int  numOut;
    bool inBounds[MAX_VERTICES];
    Vertex tempVertA[MAX_VERTICES];
    Vertex tempVertB[MAX_VERTICES];
    Attributes tempAttrA[MAX_VERTICES];
    Attributes tempAttrB[MAX_VERTICES];

    Vertex const     * srcVerts;
    Vertex*          sinkVerts;
    Attributes const * srcAttrs;
    Attributes*      sinkAttrs;

    // set up pointers for the first round of clipping
    srcVerts  = transformedVerts;
    srcAttrs  = transformedAttrs;
    sinkVerts = tempVertA;
    sinkAttrs = tempAttrA;
    num       = numIn;
    numOut    = 0;

    // clip against w (1st pass)
    double wLimit = 0.001;
    for (int i = 0; i < num; i++)
        inBounds[i] = (srcVerts[i].w > wLimit);

    for(int i = 0; i < num; i++)
    {
        int  cur  = i;
        int  next = (i + 1) % num;
        bool curVertexIn  = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if (curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut]   = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAgainstYLimit(along, wLimit, srcVerts[cur].w, srcVerts[next].w);
            sinkVerts[numOut]   = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes      (srcAttrs[cur], srcAttrs[next], along);
        }
        else if (!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAgainstYLimit(along, wLimit, srcVerts[cur].w, srcVerts[next].w);

            sinkVerts[numOut]   = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes      (srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut]   = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            ; // purposefully do nothing.
        }
        
    }

    // clip against X 
    srcVerts  = tempVertA;
    srcAttrs  = tempAttrA; 
    sinkVerts = tempVertB;
    sinkAttrs = tempAttrB;
    num       = numOut;
    numOut    = 0;

    // clip against x = w (pass 2)
    for (int i = 0; i < num; i++)
        inBounds[i] = (srcVerts[i].x < srcVerts[i].w);

    for (int i = 0; i < num; i++)
    {
        int  cur  = i;
        int  next = (i + 1) % num;
        bool curVertexIn  = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if (curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut]   = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);
            sinkVerts[numOut]   = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes      (srcAttrs[cur], srcAttrs[next], along);
        }
        else if (!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);

            sinkVerts[numOut]   = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes      (srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut]   = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            ; // purposefully do nothing.
        }
    }

    // set up pointers
    srcVerts  = tempVertB;
    srcAttrs  = tempAttrB;
    sinkVerts = tempVertA;
    sinkAttrs = tempAttrA;
    num       = numOut;
    numOut    = 0;

    // clip against -x = w (pass 3)
    for (int i = 0; i < num; i++)
        inBounds[i] = (-srcVerts[i].x < srcVerts[i].w);

    for (int i = 0; i < num; i++)
    {
        int  cur  = i;
        int  next = (i + 1) % num;
        bool curVertexIn  = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if (curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut]   = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);
            sinkVerts[numOut]   = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes      (srcAttrs[cur], srcAttrs[next], along);
        }
        else if (!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);

            sinkVerts[numOut]   = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut]   = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            ; // purposefully do nothing.
        }
    }

    // clip against Y    sdjflkdjsfd;lghdifjlkfsijgshgd;kaofijsghklg;fa'flgjshkskgdfsli
    
    // set up pointers
    srcVerts  = tempVertA;
    srcAttrs  = tempAttrA;
    sinkVerts = tempVertB;
    sinkAttrs = tempAttrB;
    num       = numOut;
    numOut    = 0;

    // clip against y = w (pass 4)
    for (int i = 0; i < num; i++)
        inBounds[i] = (srcVerts[i].y < srcVerts[i].w);

    for (int i = 0; i < num; i++)
    {
        int  cur  = i;
        int  next = (i + 1) % num;
        bool curVertexIn  = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if (curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut]   = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);
            sinkVerts[numOut]   = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes      (srcAttrs[cur], srcAttrs[next], along);
        }
        else if (!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);

            sinkVerts[numOut]   = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes      (srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut]   = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            ; // purposefully do nothing.
        }
    }

    // set up pointers
    srcVerts  = tempVertB;
    srcAttrs  = tempAttrB;
    sinkVerts = tempVertA;
    sinkAttrs = tempAttrA;
    num       = numOut;
    numOut    = 0;

    // clip against -y = w (pass 5)
    for (int i = 0; i < num; i++)
        inBounds[i] = (-srcVerts[i].y < srcVerts[i].w);

    for (int i = 0; i < num; i++)
    {
        int  cur  = i;
        int  next = (i + 1) % num;
        bool curVertexIn  = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if (curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut]   = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);
            sinkVerts[numOut]   = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes      (srcAttrs[cur], srcAttrs[next], along);
        }
        else if (!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);

            sinkVerts[numOut]   = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes      (srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut]   = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            ; // purposefully do nothing.
        }
    }

    // clip against Z sdfl;joiegjirughejkgelkflads;kjfeirhugjoajdshb
    srcVerts  = tempVertA;
    srcAttrs  = tempAttrA;
    sinkVerts = tempVertB;
    sinkAttrs = tempAttrB;
    num       = numOut;
    numOut    = 0;

    // clip against z = w (pass 6)
    for (int i = 0; i < num; i++)
        inBounds[i] = (srcVerts[i].z < srcVerts[i].w);

    for (int i = 0; i < num; i++)
    {
        int  cur  = i;
        int  next = (i + 1) % num;
        bool curVertexIn  = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if (curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut]   = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);
            sinkVerts[numOut]   = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes      (srcAttrs[cur], srcAttrs[next], along);
        }
        else if (!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);

            sinkVerts[numOut]   = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes      (srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut]   = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            ; // purposefully do nothing.
        }
    }

    // set up pointers
    srcVerts  = tempVertB;
    srcAttrs  = tempAttrB;
    sinkVerts = clippedVertices;
    sinkAttrs = clippedAttrs;
    num       = numOut;
    numOut    = 0;


    // clip against -z = w (pass 7)
    for (int i = 0; i < num; i++)
        inBounds[i] = (-srcVerts[i].z < srcVerts[i].w);

    for (int i = 0; i < num; i++)
    {
        int  cur  = i;
        int  next = (i + 1) % num;
        bool curVertexIn  = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if (curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut]   = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);
            sinkVerts[numOut]   = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes      (srcAttrs[cur], srcAttrs[next], along);
        }
        else if (!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);

            sinkVerts[numOut]   = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes      (srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut]   = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            ; // purposefully do nothing.
        }
    }

    // Final number of output vertices - previous nothing here or next line
    numClipped = numOut;
}

/**************************************************************
 * NORMALIZE VERTICES
 * 
 *************************************************************/
void normalizeVertices(Vertex clippedVertices[], Attributes clippedAttrs[], const int &numClipped)
{
    for (int i = 0; i < numClipped; i++)
    {
        clippedVertices[i].x /= clippedVertices[i].w;
        clippedVertices[i].y /= clippedVertices[i].w;
        clippedVertices[i].z /= clippedVertices[i].w;

        // set up w value for depth interpolation
        double zValue = clippedVertices[i].w;
        clippedVertices[i].w = 1.0 / zValue;

        // set up attributes
        for (int j = 0; j < clippedAttrs[i].numValues; j++)
            clippedAttrs[i][j].d /= zValue;
    }
}

/**************************************************************
 * VIEWPORT TRANSFORM
 * Helper function
 *************************************************************/
void  viewportTransform(Buffer2D<PIXEL> &target, Vertex clippedVertices[], const int &numClipped) // previously vertx, int ,buffer
{
    int w = target.width() - 1;
    int h = target.height() - 1;

    // move from -1 to 1 space in X,Y to screen coordinates
    for (int i  = 0; i < numClipped; i++)
    {
        clippedVertices[i].x = (round( ( (clippedVertices[i].x + 1) / 2.0 * w)));
        clippedVertices[i].y = (round( ( (clippedVertices[i].y + 1) / 2.0 * h))); // previously clippedVerts[i].x

        /* Let's say we have clipped, normalized vertex (-0.5, -1)
        *  Our box is from -1 to 1 in X,Y
        * 
        *  For screen X,Y positions
        *  SX = (-0.5 +1 ) / 2 * W = 0.25 * Width
        *  SY = (-1 + 1) / 2 * H = 0.0 * Height
        */
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
    Vertex     transformedVerts[MAX_VERTICES];
    Attributes transformedAttrs[MAX_VERTICES];
    VertexShaderExecuteVertices(vert, inputVerts, inputAttrs, numIn, uniforms, transformedVerts, transformedAttrs);

    // Clipping
    int numClipped;

    Vertex     clippedVertices[MAX_VERTICES];
    Attributes clippedAttrs   [MAX_VERTICES];
    clipVertices(transformedVerts, transformedAttrs, numIn, clippedVertices, clippedAttrs, numClipped);

    // normalize
    normalizeVertices(clippedVertices, clippedAttrs, numClipped);

    // adapt to viewport
    viewportTransform(target, clippedVertices, numClipped); // previously vertices, clipped, target

    // Vertex Interpolation & Fragment Drawing
    switch(prim)
    {
        case POINT:
            DrawPoint(target, transformedVerts, transformedAttrs, uniforms, frag); // previously clipedV clippedA
            break;
        case LINE:
            DrawLine(target, transformedVerts, transformedAttrs, uniforms, frag); // previous clippedV clippedA
            break;
        case TRIANGLE:
            Vertex     tri  [3];
            Attributes vAttr[3];

            for (int i = 2; i < numClipped; i++)
            {
                tri[0] = clippedVertices[0];
                tri[1] = clippedVertices[i - 1];
                tri[2] = clippedVertices[i];

                vAttr[0] = clippedAttrs[0];
                vAttr[1] = clippedAttrs[i - 1];
                vAttr[2] = clippedAttrs[i];

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

        // Test Draw
        // GameOfLife(frame); // to run this, comment out other draw functions, clearscreen, and processuserinputs
        // TestVertexShader(frame);
        TestPipeline(frame);

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
