#include "definitions.h"
#include "coursefunctions.h"
#include <iostream>

/*********************************************************
 * FIND_DETERMINANT
 * INPUTS: Vertex, Vertex
 * OUTPUTS: double
 * Finds and returns the determinant of the two given vertices.
 * *******************************************************/
double determinant(const double &A, const double &B, const double &C, const double &D)
{
    return ((A * D) - (B * C));
}

double interpolateZ(const double & area, const double & det1, const double & det2, const double & det3,
                  const Vertex vertices[3])
{
    double w1 = det1 / area;
    double w2 = det2 / area;
    double w3 = 1 - w1 - w2;

    double fractionalZ = vertices[0].w * w2 +
                         vertices[1].w * w3 +
                         vertices[2].w * w1;
    return 1 / fractionalZ;
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

        if(e.type == SDL_MOUSEMOTION)
        {
            int cur = SDL_ShowCursor(SDL_QUERY);
            if (cur == SDL_DISABLE)
            {
                double mouseX = e.motion.xrel;
                double mouseY = e.motion.yrel;

                myCam.yaw += mouseX * MOUSE_SENSITIVITY;
                myCam.pitch += mouseY * MOUSE_SENSITIVITY;
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

        if (e.key.keysym.sym == 'w' && e.type == SDL_KEYDOWN)
        {
            myCam.z += (cos((myCam.yaw / 180) * M_PI)) * MOVE_SENSITIVITY;
            myCam.x += (sin((myCam.yaw / 180) * M_PI)) * MOVE_SENSITIVITY;
        }

        if (e.key.keysym.sym == 's' && e.type == SDL_KEYDOWN)
        {
            myCam.z -= (cos((myCam.yaw / 180) * M_PI)) * MOVE_SENSITIVITY;
            myCam.x -= (sin((myCam.yaw / 180) * M_PI)) * MOVE_SENSITIVITY;
        }

        if (e.key.keysym.sym == 'a' && e.type == SDL_KEYDOWN)
        {
            myCam.x -= (cos((myCam.yaw / 180) * M_PI)) * MOVE_SENSITIVITY;
            myCam.z += (sin((myCam.yaw / 180) * M_PI)) * MOVE_SENSITIVITY;
        }

        if (e.key.keysym.sym == 'd' && e.type == SDL_KEYDOWN)
        {
            myCam.x += (cos((myCam.yaw / 180) * M_PI)) * MOVE_SENSITIVITY;
            myCam.z -= (sin((myCam.yaw / 180) * M_PI)) * MOVE_SENSITIVITY;
        }

        if (e.key.keysym.sym == 'f' && e.type == SDL_KEYDOWN)
        {
            myCam.y += MOVE_SENSITIVITY;
        }

        if (e.key.keysym.sym == 'g' && e.type == SDL_KEYDOWN)
        {
            myCam.y -= MOVE_SENSITIVITY;
        }

        if(e.key.keysym.sym == 'r' && e.type == SDL_KEYDOWN)
        {
            ORTH_VIEW_BOX += 0.05;
            std::cout << ORTH_VIEW_BOX << std::endl;
        }

        if (e.key.keysym.sym == 't' && e.type == SDL_KEYDOWN)
        {
            ORTH_VIEW_BOX -= 0.05;
            std::cout << ORTH_VIEW_BOX << std::endl;
        }
    }
}

//double interpolation(const double &w1, const double &w2)

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
    Vertex boxMax;
    Vertex boxMin;

    boxMax = {
        MAX3(triangle[0].x, triangle[1].x, triangle[2].x),
        MAX3(triangle[0].y, triangle[1].y, triangle[2].y)
    };

    boxMin = {
        MIN3(triangle[0].x, triangle[1].x, triangle[2].x),
        MIN3(triangle[0].y, triangle[1].y, triangle[2].y)
    };


    // vectors necessary to get interpolation weights 
    Vertex vector01 = {triangle[1].x - triangle[0].x,
                      triangle[1].y - triangle[0].y};
    Vertex vector12 = {triangle[2].x - triangle[1].x,
                      triangle[2].y - triangle[1].y};
    Vertex vector20 = {triangle[0].x - triangle[2].x,
                      triangle[0].y - triangle[2].y};

    double area = determinant(vector01.x, -vector20.x, vector01.y, -vector20.y);

    // Obtained from the point we are looking at in the loop
    Vertex pointVector;

    // Variables to hold the weights
    double det1;
    double det2;
    double det3;
    for (int y = boxMin.y; y <= boxMax.y; y++)
    {
        for (int x = boxMin.x; x <= boxMax.x; x++)
        {
            // Obtain the first determinant
            pointVector = {x - triangle[0].x, y - triangle[0].y};
            det1 = determinant(vector01.x, pointVector.x, vector01.y, pointVector.y);

            // Obtain the second determinant
            pointVector = {x - triangle[1].x, y - triangle[1].y};
            det2 = determinant(vector12.x, pointVector.x, vector12.y, pointVector.y);

            // Obatin the third determinant
            pointVector = {x - triangle[2].x, y - triangle[2].y};
            det3 = determinant(vector20.x, pointVector.x, vector20.y, pointVector.y);

         
            if ((det1 >= 0.0) && (det2 >= 0.0) && (det3 >= 0.0))
            {
                double correctedZ = 0.0;
                Attributes interpolatedAttrs;
                interpolatedAttrs.numMembers = attrs[0].numMembers;

                correctedZ = interpolateZ(area, det1, det2, det3, triangle);
                interpolatedAttrs.interpolateValues(area, det1, det2, det3, attrs);
                interpolatedAttrs.correctPerspective(correctedZ);

                frag->FragShader(target[y][x], interpolatedAttrs, *uniforms);
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
        return;
    }
    else
    {
        for (int i = 0; i < numIn; i++)
        {
            vert->VertShader(transformedVerts[i], transformedAttrs[i], inputVerts[i], inputAttrs[i], *uniforms);
        }
    }
}

/***************************************************************************
 *  VIEWPORT TRANSFORM
 * ************************************************************************/
viewportTransform(const Buffer2D<PIXEL>& target,
                  Vertex clippedVerts[],
                  const int & numClipped)
{
    // Move from -1 > 1 space in X,Y to screen coordinates
    int w = target.width()-1;
    int h = target.height()-1;

    for (int i = 0; i < numClipped; i++)
    {
        clippedVerts[i].x = round((clippedVerts[i].x + 1) / 2.0 * w);
        clippedVerts[i].y = round((clippedVerts[i].y + 1) / 2.0 * h);
    }
}

/***************************************************************************
 *  NORMALIZE VERTICIES
 * ************************************************************************/
normalizeVerticies(Vertex clippedVerts[], Attributes clippedAttrs[], const int & numClipped)
{
    for (int i = 0; i < numClipped; i++)
    {
        // Normalize X,Y,Z components of homogeneous coordinates
        clippedVerts[i].x /= clippedVerts[i].w;
        clippedVerts[i].y /= clippedVerts[i].w;
        clippedVerts[i].z /= clippedVerts[i].w;

        // Setup W value for depth interpolation
        double zValue = clippedVerts[i].w;
        clippedVerts[i].w = 1.0 / zValue;

        // Setup Attributes
        for (int j = 0; j < clippedAttrs[i].numMembers; j++)
        {
            clippedAttrs[i][j].d /= zValue;
        }
    }
}

void intersectAgainstYLimit(double & along, 
                            const double & yLimit, 
                            const double & segStartY,
                            const double & segEndY)
{
    along = -1;
    double segDiffY = (segEndY - segStartY);

    if (segDiffY == 0)
        return;
    
    along = (yLimit - segStartY) / segDiffY;
}

void intersectAtPositiveLine(double & along,
                             const double & segStartX,
                             const double & segStartY,
                             const double & segEndX,
                             const double & segEndY)
{
    along = -1;

    double segDiffX = (segEndX - segStartX);
    double segDiffY = (segEndY - segStartY);

    if (segDiffX == segDiffY)
        return;

    along = (segStartY - segStartX) / (segDiffX - segDiffY);
}


void intersectAtNegativeLine(double & along,
                             const double & segStartX,
                             const double & segStartY,
                             const double & segEndX,
                             const double & segEndY)
{
    along = -1;

    double segDiffX = (segEndX - segStartX);
    double segDiffY = (segEndY - segStartY);

    if (segDiffX == segDiffY)
        return;

    along = (segStartY + segStartX) / (-segDiffX - segDiffY);
}

Vertex vertBetweenVerts(const Vertex & vertA, const Vertex & vertB, const double & along)
{
    Vertex rv;
    rv.x = vertA.x + ((vertB.x - vertA.x) * along);
    rv.y = vertA.y + ((vertB.y - vertA.y) * along);
    rv.z = vertA.z + ((vertB.z - vertA.z) * along);
    rv.w = vertA.w + ((vertB.w - vertA.w) * along);
    return rv;
}


/***************************************************************************
 *  CLIP VERTICIES
 * ************************************************************************/
void clipVertices(Vertex const transformedVerts[], Attributes const transformedAttrs[], const int & numIn, 
                   Vertex clippedVerts[], Attributes clippedAttrs[], int & numClipped)
{
    // TMP CLip buffers
    int num;
    int numOut;
    bool inBounds[MAX_VERTICES];
    Vertex tmpVertA[MAX_VERTICES];
    Vertex tmpVertB[MAX_VERTICES];
    Attributes tmpAttrA[MAX_VERTICES];
    Attributes tmpAttrB[MAX_VERTICES];

    Vertex const * srcVerts;
    Vertex* sinkVerts;

    Attributes const * srcAttrs;
    Attributes* sinkAttrs;

    // Setup Pointers for the first round of clipping (1st Pass)
    srcVerts = transformedVerts;
    srcAttrs = transformedAttrs;
    sinkVerts = tmpVertA;
    sinkAttrs = tmpAttrA;
    num = numIn;
    numOut = 0;

    double wLimit = 0.001;
    for (int i = 0; i < num; i++)
        inBounds[i] = srcVerts[i].w > wLimit;

    for (int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i + 1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if(curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if (curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAgainstYLimit(along, wLimit, srcVerts[cur].w, srcVerts[next].w);
            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if (!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAgainstYLimit(along, wLimit, srcVerts[cur].w, srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            ;// Do nothing
        }
    }

    // Clip against X (2nd Pass)
    srcVerts = tmpVertA;
    srcAttrs = tmpAttrA;
    sinkVerts = tmpVertB;
    sinkAttrs = tmpAttrB;
    num = numOut;
    numOut = 0;

    // CLip against X=W
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].x < srcVerts[i].w);
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i + 1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if(curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if (curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x , srcVerts[next].w);
            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if (!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x , srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            ;// Do nothing
        }
    }

    //Setup Pointers (3rd Pass)
    srcVerts = tmpVertB;
    srcAttrs = tmpAttrB;
    sinkVerts = tmpVertA;
    sinkAttrs = tmpAttrA;
    num = numOut;
    numOut = 0;

    // CLip against -X=W
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (-srcVerts[i].x < srcVerts[i].w);
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i + 1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if(curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if (curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x , srcVerts[next].w);
            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if (!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x , srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            ;// Do nothing
        }
    }

    // Clip against Y
    srcVerts = tmpVertA;
    srcAttrs = tmpAttrA;
    sinkVerts = tmpVertB;
    sinkAttrs = tmpAttrB;
    num = numOut;
    numOut = 0;

    // CLip against Y=W (4th Pass)
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].y < srcVerts[i].w);
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i + 1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if(curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if (curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y , srcVerts[next].w);
            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if (!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y , srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            ;// Do nothing
        }
    }

    //Setup Pointers
    srcVerts = tmpVertB;
    srcAttrs = tmpAttrB;
    sinkVerts = tmpVertA;
    sinkAttrs = tmpAttrA;
    num = numOut;
    numOut = 0;

    // CLip against -Y=W (5th Pass)
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (-srcVerts[i].y < srcVerts[i].w);
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i + 1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if(curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if (curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y , srcVerts[next].w);
            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if (!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y , srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            ;// Do nothing
        }
    }

    // Clip against Z
    srcVerts = tmpVertA;
    srcAttrs = tmpAttrA;
    sinkVerts = tmpVertB;
    sinkAttrs = tmpAttrB;
    num = numOut;
    numOut = 0;

    // CLip against Z=W (6th Pass)
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].z < srcVerts[i].w);
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i + 1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if(curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if (curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z , srcVerts[next].w);
            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if (!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z , srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            ;// Do nothing
        }
    }

    //Setup Pointers
    srcVerts = tmpVertB;
    srcAttrs = tmpAttrB;
    sinkVerts = clippedVerts;
    sinkAttrs = clippedAttrs;
    num = numOut;
    numOut = 0;

    // CLip against -Z=W (7th Pass)
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (-srcVerts[i].z < srcVerts[i].w);
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i + 1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if(curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if (curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z , srcVerts[next].w);
            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if (!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z , srcVerts[next].w);

            sinkVerts[numOut] = vertBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            ;// Do nothing
        }
    }

    numClipped = numOut;

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
    Vertex clippedVerts[MAX_VERTICES];
    Attributes clippedAttrs[MAX_VERTICES];
    int numClipped;
    clipVertices(transformedVerts, transformedAttrs, numIn, clippedVerts, clippedAttrs, numClipped);

    // Normalization
    normalizeVerticies(clippedVerts, clippedAttrs, numClipped);

    // Adapt to viewport
    viewportTransform(target, clippedVerts, numClipped);


    // Vertex Interpolation & Fragment Drawing
    switch(prim)
    {
        case POINT:
            DrawPoint(target, clippedVerts, clippedAttrs, uniforms, frag);
            break;
        case LINE:
            DrawLine(target, clippedVerts, clippedAttrs, uniforms, frag);
            break;
        case TRIANGLE:
            Vertex tri[3];
            Attributes vAttr[3];
            for(int i = 2; i < numClipped; i++)
            {
                tri[0] = clippedVerts[0];
                tri[1] = clippedVerts[i - 1];
                tri[2] = clippedVerts[i];

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

    BufferImage bmpImage("c.bmp");

    // Draw loop 
    bool running = true;
    while(running) 
    {           
        //GameOfLife(frame);
        
        // Handle user inputs
        processUserInputs(running);

        // Refresh Screen
        clearScreen(frame);

        // Your code goes here
        CADView(frame);

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
