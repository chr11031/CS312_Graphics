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

        // Mouse Camera Controls
        if(e.type == SDL_MOUSEMOTION)
        {
            int cur = SDL_ShowCursor(SDL_QUERY);
            if(cur == SDL_DISABLE)
            {
                double mouseX = e.motion.xrel;
                double mouseY = e.motion.yrel;

                myCam.yaw += mouseX * 0.02;
                myCam.pitch += mouseY * 0.02;
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

        // WASD Movement
        if(e.key.keysym.sym == 'w' && e.type == SDL_KEYDOWN)
        {
            myCam.z += (cos((myCam.yaw / 180) * M_PI)) * 0.5;
            myCam.x += (sin((myCam.yaw / 180) * M_PI)) * 0.5;
        }
        if(e.key.keysym.sym == 's' && e.type == SDL_KEYDOWN)
        {
            myCam.z -= (cos((myCam.yaw / 180) * M_PI)) * 0.5;
            myCam.x -= (sin((myCam.yaw / 180) * M_PI)) * 0.5;
        }
        if(e.key.keysym.sym == 'a' && e.type == SDL_KEYDOWN)
        {
            myCam.x -= (cos((myCam.yaw / 180) * M_PI)) * 0.5;
            myCam.z += (sin((myCam.yaw / 180) * M_PI)) * 0.5;
        }
        if(e.key.keysym.sym == 'd' && e.type == SDL_KEYDOWN)
        {
            myCam.x += (cos((myCam.yaw / 180) * M_PI)) * 0.5;
            myCam.z -= (sin((myCam.yaw / 180) * M_PI)) * 0.5;
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
    // Set the dot to our given attribute    
    //target[(int)v[0].y][(int)v[0].x] = attrs[0].color; 
}

/****************************************
 * DRAW_TRIANGLE
 * Renders a line to the screen.
 ***************************************/
void DrawLine(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    // Your code goes here
}

/**************************************************************
 * CROSS_PRODUCT
 * Calculates the cross product of two vertecies.
 *************************************************************/
float crossProduct(Vertex a, Vertex b)
{
    return (a.x * b.y) - (a.y * b.x);
}

/**************************************************************
 * DETERMINANT
 * Calculates the determinant with the four parameters.
 *************************************************************/
double determinant(double ax, double bx, double ay, double by) 
{
    // Find the area of the triangle
    return ((ax * by) - (bx * ay));
}

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
   // Bounding box
   int xMax = MAX3(triangle[0].x, triangle[1].x, triangle[2].x);
   int yMax = MAX3(triangle[0].y, triangle[1].y, triangle[2].y);
   int xMin = MIN3(triangle[0].x, triangle[1].x, triangle[2].x);
   int yMin = MIN3(triangle[0].y, triangle[1].y, triangle[2].y);

   // Vectors
   double vec1[] = {triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y};
   double vec2[] = {triangle[2].x - triangle[1].x, triangle[2].y - triangle[1].y};
   double vec3[] = {triangle[0].x - triangle[2].x, triangle[0].y - triangle[2].y};

   // Area of whole triangle
   double totalArea = determinant(vec1[0], -vec3[0], vec1[1], -vec3[1]);

   // Check every point in the bounding box
   for (int y = yMin; y < yMax; y++)
   {
        for (int x = xMin; x < xMax; x++)
        {
            // Calculate the determinant of each of the inner triangles
            double det1 = determinant(vec1[0], x - triangle[0].x, vec1[1], y - triangle[0].y);
            double det2 = determinant(vec2[0], x - triangle[1].x, vec2[1], y - triangle[1].y);
            double det3 = determinant(vec3[0], x - triangle[2].x, vec3[1], y - triangle[2].y);

            // If in the triangle
            if (det1 >= 0 && det2 >= 0 && det3 >= 0)
            { 
                double correctedZ;
                // Find the corrected Depth
                correctedZ = 1.0 / interp(totalArea, det1, det2, det3, triangle[0].w, triangle[1].w, triangle[2].w);

                // Interpolate each value
                // interAttr[0].d = interp(totalArea, det1, det2, det3, attrs[0][0].d, attrs[1][0].d, attrs[2][0].d);
                // interAttr[1].d = interp(totalArea, det1, det2, det3, attrs[0][1].d, attrs[1][1].d, attrs[2][1].d);
                // interAttr[2].d = interp(totalArea, det1, det2, det3, attrs[0][2].d, attrs[1][2].d, attrs[2][2].d);
                // interAttr[3].d = interp(totalArea, det1, det2, det3, attrs[0][3].d, attrs[1][3].d, attrs[2][3].d);

                // interAttr[0].d *= correctedZ;
                // interAttr[1].d *= correctedZ; 

                Attributes interAttr = Attributes(det1, det2, det3, attrs[0], attrs[1], attrs[2], correctedZ, totalArea);

                // Call the fragment shader function previously set
                frag->FragShader(target[y][x], interAttr, *uniforms);
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
    if(vert != NULL) 
    {
        for(int i = 0; i < numIn; i++)
        {
            vert->VertShader(transformedVerts[i], transformedAttrs[i], inputVerts[i], inputAttrs[i], *uniforms);
        }
    }
}

// Helper for intersections
void intersectAgainstYLimit(double & along, const double& yLimit,
                          const double& segStartY, const double& segEndY)
{
    along = -1;

    double segDiffY = segEndY - segStartY;
    if(segDiffY == 0)
    {
        return;
    }

    along = (yLimit - segStartY) / segDiffY;
}

// Helper for intersections
void intersectAtPositiveLine(double & along, const double& segStartX,
                          const double& segStartY, const double& segEndX, const double& segEndY)
{
    along = -1;
    double segDiffX = segEndX - segStartX;
    double segDiffY = segEndY - segStartY;

    if(segDiffX == segDiffY)
    {
        return;
    }

    along = (segStartY - segStartX) / (segDiffX - segDiffY);
}

// Helper for intersections
void intersectAtNegativeLine(double & along, const double& segStartX,
                          const double& segStartY, const double& segEndX, const double& segEndY)
{
    along = -1;
    double segDiffX = segEndX - segStartX;
    double segDiffY = segEndY - segStartY;

    if(segDiffX == segDiffY)
    {
        return;
    }

    along = (segStartY + segStartX) / (-segDiffX - segDiffY);
}

// Returns a vertex between two other vertecies
Vertex VertexBetweenVerts(const Vertex& vertA, const Vertex& vertB, const double& along)
{
    Vertex rv;
    rv.x = (vertA.x) + ((vertB.x - vertA.x) * along);
    rv.y = (vertA.y) + ((vertB.y - vertA.y) * along);
    rv.z = (vertA.z) + ((vertB.z - vertA.z) * along);
    rv.w = (vertA.w) + ((vertB.w - vertA.w) * along);
    return rv;
}

// Clip Vertices
void clipVertices(Vertex const transformedVerts[], Attributes const transformedAttrs[], const int &numIn, 
                    Vertex clippedVertices[], Attributes clippedAttrs[], int &numClipped)
{
    // TMP clip buffers
    int num;
    int numOut;
    bool inBounds[MAX_VERTICES];
    Vertex tmpVertsA[MAX_VERTICES];
    Vertex tmpVertsB[MAX_VERTICES];
    Attributes tmpAttrsA[MAX_VERTICES];
    Attributes tmpAttrsB[MAX_VERTICES];

    Vertex const* srcVerts;
    Vertex* sinkVerts;

    Attributes const* srcAttrs;
    Attributes* sinkAttrs;

    // Setup pointers for the firt round of clipping

    srcVerts = transformedVerts;
    srcAttrs = transformedAttrs;
    sinkVerts = tmpVertsA;
    sinkAttrs = tmpAttrsA;
    num = numIn;
    numOut = 0;

    double wLimit = 0.001;
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].w > wLimit);
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if(curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAgainstYLimit(along, wLimit, srcVerts[cur].w, srcVerts[next].w);
            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAgainstYLimit(along, wLimit, srcVerts[cur].w, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            // Do Nothing
        }
    }

    // Clip against X
    srcVerts = tmpVertsA;
    srcAttrs = tmpAttrsA;
    sinkVerts = tmpVertsB;
    sinkAttrs = tmpAttrsB;
    num = numOut;
    numOut = 0;
    
    // Clip against X = w
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].x < srcVerts[i].w);
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if(curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);
            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            // Do Nothing
        }
    }

    // Setup pointers
    srcVerts = tmpVertsB;
    srcAttrs = tmpAttrsB;
    sinkVerts = tmpVertsA;
    sinkAttrs = tmpAttrsA;
    num = numOut;
    numOut = 0;


    // Clip against -X = w
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (-srcVerts[i].x < srcVerts[i].w);
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if(curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);
            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            // Do Nothing
        }
    }

    // Clip against Y
    srcVerts = tmpVertsA;
    srcAttrs = tmpAttrsA;
    sinkVerts = tmpVertsB;
    sinkAttrs = tmpAttrsB;
    num = numOut;
    numOut = 0;
    
    // Clip against Y = w
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].y < srcVerts[i].w);
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if(curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);
            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            // Do Nothing
        }
    }

    // Setup pointers
    srcVerts = tmpVertsB;
    srcAttrs = tmpAttrsB;
    sinkVerts = tmpVertsA;
    sinkAttrs = tmpAttrsA;
    num = numOut;
    numOut = 0;


    // Clip against -Y = w
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (-srcVerts[i].y < srcVerts[i].w);
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if(curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);
            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            // Do Nothing
        }
    }

    // Clip against Z
    srcVerts = tmpVertsA;
    srcAttrs = tmpAttrsA;
    sinkVerts = tmpVertsB;
    sinkAttrs = tmpAttrsB;
    num = numOut;
    numOut = 0;
    
    // Clip against Z = w
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].z < srcVerts[i].w);
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if(curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);
            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            // Do Nothing
        }
    }

    // Setup pointers
    srcVerts = tmpVertsB;
    srcAttrs = tmpAttrsB;
    sinkVerts = clippedVertices;
    sinkAttrs = clippedAttrs;
    num = numOut;
    numOut = 0;


    // Clip against -Z = w
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (-srcVerts[i].z < srcVerts[i].w);
    }
    for(int i = 0; i < num; i++)
    {
        int cur = i;
        int next = (i+1) % num;
        bool curVertexIn = inBounds[cur];
        bool nextVertexIn = inBounds[next];

        if(curVertexIn && nextVertexIn)
        {
            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else if(curVertexIn && !nextVertexIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);
            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertexIn && nextVertexIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else
        {
            // Do Nothing
        }
    }

    numClipped = numOut;
}

// Normalize Vertices
void NormalizeVertices(Vertex clippedVertices[], Attributes clippedAttrs[], const int& numClipped) 
{
	for(int i = 0; i < numClipped; i++)
	{
		// Normalize X,Y,Z components of homogeneous coordinates
		clippedVertices[i].x /= clippedVertices[i].w;
		clippedVertices[i].y /= clippedVertices[i].w;
		clippedVertices[i].z /= clippedVertices[i].w;
		
		// Setup W value for depth interpolation 
		double zValue = clippedVertices[i].w;
		clippedVertices[i].w = 1.0 / zValue;
			
		// Setup Attributes 
		for(int j = 0; j < clippedAttrs[i].numMembers; j++)
		{
			clippedAttrs[i][j].d /= zValue;
		}		
	}
}

// Transformation for viewport
void viewportTransform(Buffer2D<PIXEL>& target, Vertex clippedVertices[], const int& numClipped)
{
    int w = target.width();
    int h = target.height();

    for(int i = 0; i < numClipped; i++)
    {
        clippedVertices[i].x = (round( (( (clippedVertices[i].x + 1) / 2.0 * w))));
        clippedVertices[i].y = (round( (( (clippedVertices[i].y + 1) / 2.0 * h))));
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


    // Clipping
    Vertex clippedVertices[MAX_VERTICES];
    Attributes clippedAttrs[MAX_VERTICES];
    int numClipped;
    clipVertices(transformedVerts, transformedAttrs, numIn, 
                    clippedVertices, clippedAttrs, numClipped);

    // Normalize
    NormalizeVertices(clippedVertices, clippedAttrs, numClipped);

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
            for(int i =2; i <numClipped; i++)
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

        // Test our Fragmentation
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
