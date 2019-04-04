#include "definitions.h"
#include "coursefunctions.h"
#include <algorithm>


using namespace std;

/***********************************************
 * CLEAR_SCREEN
 * Sets the screen to the indicated color value
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

        //Code added by me:
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
			if(cur == SDL_DISABLE)
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
		
		// Translation 
		if((e.key.keysym.sym == 'w' && e.type == SDL_KEYDOWN))
		{
			myCam.z += (cos((myCam.yaw / 180.0) * M_PI)) * 0.5;
			myCam.x += (sin((myCam.yaw / 180.0) * M_PI)) * 0.5;
		}
		if((e.key.keysym.sym == 's' && e.type == SDL_KEYDOWN))
		{
			myCam.z -= (cos((myCam.yaw / 180.0) * M_PI)) * 0.5;
			myCam.x -= (sin((myCam.yaw / 180.0) * M_PI)) * 0.5;
		}
		if((e.key.keysym.sym == 'a' && e.type == SDL_KEYDOWN))
		{
			myCam.x -= (cos((myCam.yaw / 180.0) * M_PI)) * 0.5;
			myCam.z += (sin((myCam.yaw / 180.0) * M_PI)) * 0.5;
		}
		if((e.key.keysym.sym == 'd' && e.type == SDL_KEYDOWN))
		{
			myCam.x += (cos((myCam.yaw / 180.0) * M_PI)) * 0.5;
			myCam.z -= (sin((myCam.yaw / 180.0) * M_PI)) * 0.5;
		}
        //End of code to handle input translation
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
 * crossProduct
 * returns corss product of the vectors
 ************************************************************/
int crossProduct(const int ax, const int ay, const int bx, const int by) 
{
	return ax * by - ay * bx;
}

/****************************************
  * DETERMINANT
  * Find the determinant of a matrix with
  * components A, B, C, D from 2 vectors.
  ***************************************/
 inline double determinant(const double & A, const double & B, const double & C, const double & D)
 {
   return ((A*D) - (B*C));
 }

/****************************************
  * INTERPOLATE FUNCTION
  * Find the percentage of the color
  * depending on their proximity to the
  * vertexes it is close to.
  ***************************************/
 double interp(const double area, const double firstDet,const double secondDet,const double thirdDet,const double r,const double g,const double b)
 {
    //firstDet  /= area;   //How much of the first color
    //secondDet /= area;   //How much of the second color
    //thirdDet  /= area;   //How much of the third color

    // how much from each color into each pixel.
    return ((firstDet/area) * b) + ((secondDet/area) * r) + ((thirdDet/area) * g);
 }

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    // Your code goes here

    /// Create a bounding box
    int minX = MIN3(triangle[0].x, triangle[1].x, triangle[2].x);
    int minY = MIN3(triangle[0].y, triangle[1].y, triangle[2].y);
    int maxX = MAX3(triangle[0].x, triangle[1].x, triangle[2].x);
    int maxY = MAX3(triangle[0].y, triangle[1].y, triangle[2].y);

    // Compute first, second, third X-Y pairs
    double firstVec[] = {triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y};
    double secndVec[] = {triangle[2].x - triangle[1].x, triangle[2].y - triangle[1].y};
    double thirdVec[] = {triangle[0].x - triangle[2].x, triangle[0].y - triangle[2].y};

     // Compute area of the whole triangle
    double areaTriangle = determinant(firstVec[X_KEY], -thirdVec[X_KEY], firstVec[Y_KEY], -thirdVec[Y_KEY]);

    // Loop through every pixel in the grid
    for(int x = minX; x <= maxX; x++)
    {
        for(int y = minY; y <= maxY; y++)
        {
            // Determine if the pixel is in the triangle by the determinant's sign
            double firstDet = crossProduct(firstVec[X_KEY], x - triangle[0].x, firstVec[Y_KEY], y - triangle[0].y);
            double secndDet = crossProduct(secndVec[X_KEY], x - triangle[1].x, secndVec[Y_KEY], y - triangle[1].y);
            double thirdDet = crossProduct(thirdVec[X_KEY], x - triangle[2].x, thirdVec[Y_KEY], y - triangle[2].y);

            double firstWgt = firstDet / areaTriangle;
            double secndWgt = secndDet / areaTriangle;
            double thirdWgt = thirdDet / areaTriangle;

            // All 3 signs > 0 means the center point is inside, to the left of the 3 CCW vectors 
            if(firstDet >= 0 && secndDet >= 0 && thirdDet >= 0)
            {
	        // Find corrected 'Z' value
	        double correctZ = baryInterp(firstWgt, secndWgt, thirdWgt, triangle[0].w, triangle[1].w, triangle[2].w);
		    correctZ = 1.0 / correctZ;
	      
            // Interpolate Attributes for this pixel - In this case the R,G,B values
            Attributes interpolatedAttribs(firstWgt, secndWgt, thirdWgt, attrs[0], attrs[1], attrs[2], correctZ);

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
  if(vert == NULL)
    {
    for(int i = 0; i < numIn; i++)
    {
      transformedVerts[i] = inputVerts[i];
      transformedAttrs[i] = inputAttrs[i];
    }

    return;
    }
  

    // Defaults to pass-through behavior
    for(int i = 0; i < numIn; i++)
    {
        vert->VertShader(transformedVerts[i], transformedAttrs[i], inputVerts[i], inputAttrs[i], *uniforms);
    }
}

void intersectAgainstYLimit(double & along, 
							const double & yLimit, 
							const double & segStartY,
							const double & segEndY)
{
	along = -1;
	
	double segDiffY = (segEndY - segStartY);
	if(segDiffY == 0)
	{
		return;
	}
	
	along = (yLimit - segStartY) / segDiffY;
}

void intersectAtPositiveLine(	double & along, 
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
		return ;
	}
	
	along = (segStartY - segStartX) / (segDiffX - segDiffY);
}

void intersectAtNegativeLine(	double & along, 
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
		return ;
	}
	
	along = (segStartY + segStartX) / (-segDiffX - segDiffY);
}

Vertex VertexBetweenVerts(const Vertex & vertA, const Vertex & vertB, const double & along)
{
	Vertex rv;
	rv.x = (vertA.x) + ((vertB.x-vertA.x) * along);
	rv.y = (vertA.y) + ((vertB.y-vertA.y) * along);
	rv.z = (vertA.z) + ((vertB.z-vertA.z) * along);
	rv.w = (vertA.w) + ((vertB.w-vertA.w) * along);
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
				
    // Clip on each side against wLimit
    double wLimit = 0.001;
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].w > wLimit);    
    }
    for(int i = 0; i < num; i++)
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
            intersectAgainstYLimit(along, wLimit, srcVerts[cur].w, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAgainstYLimit(along, wLimit, srcVerts[cur].w, srcVerts[next].w);

            sinkVerts[numOut] = VertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);

            sinkVerts[numOut] = srcVerts[next];
            sinkAttrs[numOut++] = srcAttrs[next];
        }
        else // (!curVertIn && !nextVertIn)
        {
            ; //Do nothing... 
        }
    }

    // Clip against X

    // Setup pointers
    srcVerts = tmpVertA;
    srcAttrs = tmpAttrA;
    sinkVerts = tmpVertB;
    sinkAttrs = tmpAttrB;
    num = numOut;
    numOut = 0;

    // Clip X=W
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].x < srcVerts[i].w);    
    }
    for(int i = 0; i < num; i++)
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
    }

    // Setup Pointers
    srcVerts = tmpVertB;
    srcAttrs = tmpAttrB;
    sinkVerts = tmpVertA;
    sinkAttrs = tmpAttrA;
    num = numOut;
    numOut = 0;

    // Clip -X=W
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (-srcVerts[i].x < srcVerts[i].w);    
    }
    for(int i = 0; i < num; i++)
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
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].y < srcVerts[i].w);    
    }
    for(int i = 0; i < num; i++)
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
    }

    // Setup Pointers
    srcVerts = tmpVertB;
    srcAttrs = tmpAttrB;
    sinkVerts = tmpVertA;
    sinkAttrs = tmpAttrA;
    num = numOut;
    numOut = 0;

    // Clip -Y=W
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (-srcVerts[i].y < srcVerts[i].w);    
    }
    for(int i = 0; i < num; i++)
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
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (srcVerts[i].z < srcVerts[i].w);    
    }
    for(int i = 0; i < num; i++)
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
    }

    // Setup Pointers
    srcVerts = tmpVertB;
    srcAttrs = tmpAttrB;
    sinkVerts = clippedVertices;
    sinkAttrs = clippedAttrs;
    num = numOut;
    numOut = 0;

    // Clip -Z=W
    for(int i = 0; i < num; i++)
    {
        inBounds[i] = (-srcVerts[i].z < srcVerts[i].w);    
    }
    for(int i = 0; i < num; i++)
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
    }

    // Final number of output vertices
    numClipped = numOut;  
}

void normalizeVertices(	Vertex clippedVertices[], 
						Attributes clippedAttrs[], 
						const int & numClipped)
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
		for(int j = 0; j < clippedAttrs[i].count; j++)
		{
			clippedAttrs[i][j].d /= zValue;
		}		
	}
	
}

void viewportTransform( Buffer2D<PIXEL>& target, 
						Vertex clippedVertices[], 
						const int & numClipped)
{
	// Move from -1 -> 1 space in X,Y to screen coordinates 
	int w = target.width();
	int h = target.height();
	
	for(int i = 0; i < numClipped; i++)
	{
		clippedVertices[i].x = (round( (( (clippedVertices[i].x + 1) / 2.0 * w))));
		clippedVertices[i].y = (round( (( (clippedVertices[i].y + 1) / 2.0 * h))));
		
		/*	Let's say we have clipped, normalized vertex (-0.5, -1)		
		*	Our Box is from -1 to 1 in X,Y 
		*
		*	For screen X,Y positions 
		*	SX = (-0.5 + 1) / 2 * W = 0.25 * Width 
		*	SY = (-1 + 1) / 2 * H = 0.0 * Height 
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

	// Clipping
    Vertex clippedVertices[MAX_VERTICES];
    Attributes clippedAttrs[MAX_VERTICES];
	int numClipped;
	clipVertices(transformedVerts, transformedAttrs, numIn, 
				clippedVertices, clippedAttrs, numClipped);
		
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
			for(int i = 2; i < numClipped; i++)
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

        // Your code goes here
        //TestVertexShader(frame);
        //TestPipeline(frame);
        //CADView(frame);
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
