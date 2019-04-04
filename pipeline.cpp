#include "definitions.h"
#include "coursefunctions.h"
#include "trans.h"
#include <algorithm>
#define SDL_MAIN_HANDLED
#define SCREEN_W 640.0f
#define SCREEN_H 480.0f
#include "SDL2/SDL.h"
#include "glm/glm.hpp"
//#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
using glm::mat4;
using glm::vec3;
using glm::vec2;

#include "gl/glew.h"
#include "SDL2/SDL_opengl.h"
// Friendly, Other libraries
#include "stdio.h"
#include "stdlib.h"
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

/***********************************************
 * Sets the screen to the indicated color value.
 **********************************************/
void clearScreen(Buffer2D<PIXEL> & frame, PIXEL color = 0xff000000)
{
    int h = frame.height();
    int w = frame.width();
    int y = 0;
    while (y < h)
    {
        int x = 0;
        while (x < w)
        {
            frame[y][x] = color;
            x++;
        }
        y++;
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


        if (e.type == SDL_MOUSEMOTION)
        {
            int cur = SDL_ShowCursor(SDL_QUERY);
            if (cur == SDL_DISABLE)
            {
                double mouseX = e.motion.xrel;
                double mouseY = e.motion.yrel;

                myCam.yaw += mouseX * 0.02;
                myCam.pitch += mouseY * 0.02;
            }
        }
        if (e.type == SDL_MOUSEBUTTONDOWN)
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
    }

    if ((e.key.keysym.sym == 'w' && e.type == SDL_KEYDOWN))
    {
        myCam.z += (cos((myCam.yaw / 180.0) * M_PI)) * 0.05;
        myCam.x += (sin((myCam.yaw / 180.0) * M_PI)) * 0.05;
    }
    if ((e.key.keysym.sym == 's' && e.type == SDL_KEYDOWN))
    {
        myCam.z -= (cos((myCam.yaw / 180.0) * M_PI)) * 0.05;
        myCam.x -= (sin((myCam.yaw / 180.0) * M_PI)) * 0.05;
    }
    if ((e.key.keysym.sym == 'a' && e.type == SDL_KEYDOWN))
    {
        myCam.z -= (cos((myCam.yaw / 180.0) * M_PI)) * 0.05;
        myCam.x += (sin((myCam.yaw / 180.0) * M_PI)) * 0.05;
    }
    if ((e.key.keysym.sym == 'd' && e.type == SDL_KEYDOWN))
    {
        myCam.z += (cos((myCam.yaw / 180.0) * M_PI)) * 0.05;
        myCam.x -= (sin((myCam.yaw / 180.0) * M_PI)) * 0.05;
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
    //target[(int)v[0].y][(int)v[0].x] = attrs[0].color;
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

/*******************************************
 * INTERP
 * Serves as an interpolation algorithm.
 *****************************************/
double interp(double triangleArea, double firstDet, double secndDet, double thirdDet, double attrs1, double attrs2, double attrs3)
{
    return ((firstDet / triangleArea * attrs3) + (secndDet / triangleArea * attrs1) + (thirdDet / triangleArea * attrs2));
}

/************************************************************
 * DETERMINANT
 * Returns the determinant of the parameters.
 ***********************************************************/
double determinant(double a, double b, double c, double d)
{
    return ((a * d) - (b * c));
}

/*********************************************************
 * Renders a triangle/convex polygon to the screen with
 * the appropriate fill pattern.
 * *******************************************************/
void DrawTriangleScanlines(BufferImage & frame, Vertex* triangle, Attributes* attrs, Attributes* const uniforms, FragmentShader * const frag)
{
    int count = 3;
    int i = 0;
    while (i < count)
    {
        int j = 0;
        while (j < count - 1)
        {
            if  (triangle[j].y > triangle[j+1].y || 
                ((triangle[j].y == triangle[j+1].y && (triangle[j].x > triangle[j+1].x))))
            {
                SWAP(Vertex, triangle[j], triangle[j+1]);
                SWAP(Attributes, attrs[j], attrs[j+1]);
            }
            j++;
        }
        i++;
    }

    Vertex firstLeft = triangle[0];
    Vertex lastRight = triangle[count-1];
    int diffX = (int)lastRight.x - (int)firstLeft.x;
    int diffY = (int)lastRight.y - (int)firstLeft.y;

    bool flatTop = triangle[0].y == triangle[1].y;
    bool flatBottom = triangle[count-1].y == triangle[count-2].y;
    int lastIteration = count - 1;
    int numLeft = 0;
    int numRight = 0;
    Vertex left[8];
    Vertex right[8];
    Attributes lAttr[8]; 
    Attributes rAttr[8];
    i = 0;
    while (i < count)
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
        i++;
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
            frame[scanVertices[0].y][(int)scanVertices[0].x++] = 0xffff0000; // Pre-mixed but can be attributes tweaked later
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
    //This is used to get the triangle bounding box whose dimensions are
    // based on the max and min x and y values of the vertices
    int maxX = max(triangle[0].x, max(triangle[1].x, triangle[2].x));
    int minX = min(triangle[0].x, min(triangle[1].x, triangle[2].x));
    int maxY = max(triangle[0].y, max(triangle[1].y, triangle[2].y));
    int minY = min(triangle[0].y, min(triangle[1].y, triangle[2].y));

    //Used for determinant  and area calculations
    Vertex v1 = {triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y};
    Vertex v2 = {triangle[2].x - triangle[1].x, triangle[2].y - triangle[1].y};
    Vertex v3 = {triangle[0].x - triangle[2].x, triangle[0].y - triangle[2].y};

    double areaTriangle = determinant(v1.x,-v3.x,v1.y,-v3.y);

    int x = minX;
    while (x <= maxX)
    {
        int y = minY;
        while (y <= maxY)
        {
            double firstDet = determinant(v1.x, x - triangle[0].x, v1.y, y - triangle[0].y);
            double secndDet = determinant(v2.x, x - triangle[1].x, v2.y, y - triangle[1].y);
            double thirdDet = determinant(v3.x, x - triangle[2].x, v3.y, y - triangle[2].y);

            if ((firstDet >= 0) && (secndDet >= 0) && (thirdDet >= 0))
            {
                Attributes interpolatedAttrs;
                //Calculates the correct z value using the reciprocal of the interpolated w value
                double correctZ = (1/(interp(areaTriangle, firstDet, secndDet, thirdDet, triangle[0].w, triangle[1].w, triangle[2].w)));
                // value[0] and value[1] are multiplied by correctZ to give us the correct perspective
                interpolatedAttrs.value[0].d = (correctZ * interp(areaTriangle, firstDet, secndDet, thirdDet,
                   attrs[0].value[0].d,attrs[1].value[0].d,attrs[2].value[0].d));
                interpolatedAttrs.value[1].d = (correctZ * interp(areaTriangle, firstDet, secndDet, thirdDet,
                   attrs[0].value[1].d,attrs[1].value[1].d,attrs[2].value[1].d));
                interpolatedAttrs.value[2].d = interp(areaTriangle, firstDet, secndDet, thirdDet,attrs[0].value[2].d,attrs[1].value[2].d,attrs[2].value[2].d);
                frag->FragShader(target[y][x], interpolatedAttrs, *uniforms);
            }
            y++;
        }
        x++;
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
    int i = 0;
    while (i < numIn)
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
        i++;
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
    {
        return;
    }

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

    if(segDiffX == segDiffY)
    {
        return;
    }
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
    {
        return;
    }
    along = (segStartY + segStartX) / (-segDiffX - segDiffY);
}

Vertex VertexBetweenVerts(const Vertex & vertA, const Vertex & vertB, const double & along)
{
    Vertex rv;
    rv.x = (vertA.x) + ((vertB.x - vertA.x) * along);
    rv.y = (vertA.y) + ((vertB.y - vertA.y) * along);
    rv.z = (vertA.z) + ((vertB.z - vertA.z) * along);
    rv.w = (vertA.w) + ((vertB.w - vertA.w) * along);
    return rv;
}

void clipVertices(Vertex const transformedVerts[], Attributes const transformedAttrs[],
                    const int & numIn,
                    Vertex clippedVertices[], Attributes clippedAttrs[], int & numClipped)
{
    // TMP Clip buffers
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

    // Setup Pointers for the first round of clipping
    srcVerts = transformedVerts;
    srcAttrs = transformedAttrs;
    sinkVerts = tmpVertA;
    sinkAttrs = tmpAttrA;
    num = numIn;
    numOut = 0;
    double wLimit = 0.001;
    int i = 0;
    while (i < num)
    {
        inBounds[i] = (srcVerts[i].w > wLimit);
        i++;
    }
    i = 0;
    while (i < num)
    {
        int cur = i;
        int next = (i + 1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if (curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if (curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAgainstYLimit(along, wLimit, srcVerts[cur].w, srcVerts[next].w);
            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if (!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAgainstYLimit(along, wLimit, srcVerts[cur].w, srcVerts[next].w);
            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        i++;
    }

    srcVerts = tmpVertA;
    srcAttrs = tmpAttrA;
    sinkVerts = tmpVertB;
    sinkAttrs = tmpAttrB;
    num = numOut;
    numOut = 0;

    // Clip X=W
    i = 0;
    while (i < num)
    {
        inBounds[i] = (srcVerts[i].x < srcVerts[i].w);
        i++;    
    }
    i = 0;
    while (i < num)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];

        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);          

            sinkVerts[numOut] = srcVerts[next];

            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else // (!curVertIn && !nextVertIn)
        {
            ; //Do nothing... 
        }
        i++;
    }

    // Setup Pointers
    srcVerts = tmpVertB;
    srcAttrs = tmpAttrB;
    sinkVerts = tmpVertA;
    sinkAttrs = tmpAttrA;
    num = numOut;
    numOut = 0;

    // Clip -X=W
    i = 0;
    while (i < num)
    {
        inBounds[i] = (-srcVerts[i].x < srcVerts[i].w);
        i++;    
    }
    i = 0;
    while (i < num)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];

        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else // (!curVertIn && !nextVertIn)
        {
            ; //Do nothing... 
        }
        i++;
    }

    // Clip against Y

    // Setup pointers
    srcVerts = tmpVertA;
    srcAttrs = tmpAttrA;
    sinkVerts = tmpVertB;
    sinkAttrs = tmpAttrB;
    num = numOut;
    numOut = 0;

    // Clip Y=W
    i = 0;
    while (i < num)
    {
        inBounds[i] = (srcVerts[i].y < srcVerts[i].w);
        i++;
    }
    i = 0;
    while (i < num)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];

        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else // (!curVertIn && !nextVertIn)
        {
            ; //Do nothing... 
        }
        i++;
    }

    // Setup Pointers
    srcVerts = tmpVertB;
    srcAttrs = tmpAttrB;
    sinkVerts = tmpVertA;
    sinkAttrs = tmpAttrA;
    num = numOut;
    numOut = 0;

    // Clip -Y=W
    i = 0;
    while (i < num)
    {
        inBounds[i] = (-srcVerts[i].y < srcVerts[i].w);
        i++;    
    }
    i = 0;
    while (i < num)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];

        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else // (!curVertIn && !nextVertIn)
        {
            ; //Do nothing... 
        }
        i++;
    }

    // Clip against Z

    // Setup pointers
    srcVerts = tmpVertA;
    srcAttrs = tmpAttrA;
    sinkVerts = tmpVertB;
    sinkAttrs = tmpAttrB;
    num = numOut;
    numOut = 0;

    // Clip Z=W
    i = 0;
    while (i < num)
    {
        inBounds[i] = (srcVerts[i].z < srcVerts[i].w);
        i++;    
    }
    i = 0;
    while (i < num)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];

        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else // (!curVertIn && !nextVertIn)
        {
            ; //Do nothing... 
        }
        i++;
    }

    // Setup Pointers
    srcVerts = tmpVertB;
    srcAttrs = tmpAttrB;
    sinkVerts = clippedVertices;
    sinkAttrs = clippedAttrs;
    num = numOut;
    numOut = 0;

    // Clip -Z=W
    i = 0;
    while (i < num)
    {
        inBounds[i] = (-srcVerts[i].z < srcVerts[i].w);
        i++;
    }
    i = 0;
    while (i < num)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertIn = inBounds[cur];
        bool nextVertIn = inBounds[next];

        if(curVertIn && nextVertIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertIn && !nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);
            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else // (!curVertIn && !nextVertIn)
        {
            ; //Do nothing... 
        }
        i++;
    }

    // Final number of output vertices
    numClipped = numOut;  
}

void normalizeVertices(Vertex clippedVertices[], Attributes
                        clippedAttrs[],
                        const int & numClipped)
{
    int i = 0;
    while (i < numClipped)
    {
        // Normalize X, Y, Z components of homogenous coordinates
        clippedVertices[i].x /= clippedVertices[i].w;
        clippedVertices[i].y /= clippedVertices[i].w;
        clippedVertices[i].z /= clippedVertices[i].w;

        double zValue = clippedVertices[i].w;
        clippedVertices[i].w = 1.0 / zValue;

        // Setup Attirbutes
        int j = 0;
        while (j < clippedAttrs[i].numValues)
        {
            clippedAttrs[i].value[j].d /= zValue;
            j++;
        }
        i++;
    }
}

void viewportTransform(Buffer2D<PIXEL>& target, Vertex clippedVertices[], const int & numClipped)
{
    // Move from -1 -> 1 space in X, Y to screen coordinates
    int w = target.width() - 1;
    int h = target.height() - 1;
    int i = 0;
    while (i < numClipped)
    {
        clippedVertices[i].x = (round((((clippedVertices[i].x + 1) / 2.0 * w))));
        clippedVertices[i].y = (round((((clippedVertices[i].y + 1) / 2.0 * h))));
        i++;

        /* Let's say we have clipped, normalized vertex (-0.5, -1)
        *  Our Box is from -1 to 1 in X,Y
        * 
        * For screen X,Y positions
        * SX = (-0.5 + 1) / 2 * W = 0.25 * Width
        * SY = (-1 + 1) / 2 * H = 0.0 * Height
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
    Vertex transformedVerts[MAX_VERTICES];
    Attributes transformedAttrs[MAX_VERTICES];
    VertexShaderExecuteVertices(vert, inputVerts, inputAttrs, numIn, uniforms, transformedVerts, transformedAttrs);

    //Clipping
    Vertex clippedVertices[MAX_VERTICES];
    Attributes clippedAttrs[MAX_VERTICES];
    int numClipped;
    clipVertices(transformedVerts, transformedAttrs, numIn, clippedVertices, clippedAttrs, numClipped);

    // Normalize 
	normalizeVertices(clippedVertices, clippedAttrs, numClipped);
    
    // Adapt to viewport 
	viewportTransform(target, clippedVertices, numClipped);

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
            Vertex tri[3];
            Attributes vAttr[3];
            for (int i = 2; i < numClipped; i++)
            {
                tri[0] = clippedVertices[0];
                tri[1] = clippedVertices[i-1];
                tri[2] = clippedVertices[i];
                vAttr[0] = clippedAttrs[0];
                vAttr[1] = clippedAttrs[i-1];
                vAttr[2] = clippedAttrs[i];
                DrawTriangle(target, tri, vAttr, uniforms, frag);
            }
    }
}

#include <glm/glm.hpp>
#include <gl/gl.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <GL/glew.h>

/*#include "mesh.h"

class Mesh
{
private:
    GLuint vbo_vertices, vbo_normals, ibo_elements;
public:
    vector <glm::vec4> vertices;
    vector <glm::vec3> normals;
    vector <GLushort> elements;
    glm::mat4 object2world;
    
    Mesh() : vertices(1), normals(1), elements(3), object2world(glm::mat4(1)) {}
    ~Mesh(void){} ;
    void Mesh::draw(void) ;
};
*/

vector<glm::vec4> obj_vertices;
vector<glm::vec3> obj_normals;
vector<GLushort> obj_elements;

//load_obj("______.obj", obj_vertices, obj_normals, obj_elements);

/*************************************************************
 * Loads OBJ model files
 *************************************************************/
/*void load_obj(const char* filename, vector<glm::vec4> &vertices, vector<glm::vec3> &normals, vector<GLushort> &elements)
//void load_obj(const char* filename, Mesh* mesh)
{
    ifstream in(filename, ios::in);
    if (!in)
    {
        cerr << "Cannot open " << filename << endl; exit(1);
    }

    string line;
    while (getline(in, line))
    {
        if (line.substr(0,2) == "v ")
        {
            istringstream s(line.substr(2));
            glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
            vertices.push_back(v);
        }
        else if (line.substr(0,2) == "f ")
        {
            istringstream s(line.substr(2));
            GLushort a,b,c;
            s >> a; s >> b; s >> c;
            a--; b--; c--;
           elements.push_back(a); elements.push_back(b); elements.push_back(c);
        }
        else if (line[0] == '#')
        {
            ;// ignoring this line
        }
        else
        {
            ;// ignoring this line
        }
    }

    normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
    for (int i = 0; i < elements.size(); i+=3)
    {
        GLushort ia = elements[i];
        GLushort ib = elements[i+1];
        GLushort ic = elements[i+2];
        glm::vec3 normal = glm::normalize(glm::cross(
        glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
        glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
        normals[ia] = normals[ib] = normals[ic] = normal;
    }
    /*
    ifstream in(filename, ios::in);
    if (!in)
    {
        cerr << "Can't open " << filename << endl; exit(1);
    }
    vector<int> nb_seen;
    vector<glm::vec4> dummy;
    string line;
    while (getline(in, line))
    {
        if (line.substr(0,2) == "v ")
        {
            istringstream s(line.substr(2));
            glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0;
            dummy.push_back(v);
            mesh->vertices.push_back(v);
        }
    }
    */
//}
//*/

void load_mdl()
{
    ;
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

        //TestDrawFragments(frame);//
        TestPipeline(frame);
        //GameOfLife(frame);
        //load_obj("batman.obj", obj_vertices, obj_normals, obj_elements);//

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
