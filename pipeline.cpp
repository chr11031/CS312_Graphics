#include "definitions.h"
#include "coursefunctions.h"
#include <iostream>
#define MAX_VERTICES 8 
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
            exit(0);
            running = false;
        }
		
		
		if(e.type == SDL_MOUSEMOTION)
		{
			int cur = SDL_ShowCursor(SDL_QUERY);
			if(cur == SDL_DISABLE)
			{
				double mouseX = e.motion.xrel;
				double mouseY = e.motion.yrel;
				
				myCam.yaw += mouseX * 0.22;
				myCam.pitch += mouseY * 0.22;
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
    // target[(int)v[0].y][(int)v[0].x] = attrs[0].color;
}

/****************************************
 * DRAW_TRIANGLE
 * Renders a line to the screen.
 ***************************************/
void DrawLine(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    // Your code goes here
}
/********************************************
 *Determinant
 *Find the area of the triangle between
 * two vectors. 
 ******************************************/
double Determinant(double a, double b, double c, double d) 
{
    return (a * d) - (b * c); 
}

double Interp(double area, double d1, double d2, double d3, double a1, double a2, double a3) 
{
   double percentd1 = (d1) * a3; 
   double percentd2 = (d2) * a1; 
   double percentd3 = (d3) * a2; 
   
   return percentd1 + percentd2 + percentd3;
}

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
//get the box around the triangle
float maxX = MAX3(triangle[0].x,triangle[1].x,triangle[2].x);
float maxY = MAX3(triangle[0].y,triangle[1].y,triangle[2].y);
float minX = MIN3(triangle[0].x,triangle[1].x,triangle[2].x);
float minY = MIN3(triangle[0].y,triangle[1].y,triangle[2].y);
//first determinant is proportional to 
// edge corresponds with opposite vertex

Vertex vt1;
Vertex vt2;
Vertex vt3;

vt1.x = (triangle[1].x - triangle[0].x);
vt1.y = (triangle[1].y - triangle[0].y);
vt1.z = (triangle[1].z - triangle[0].z);
vt1.w = (triangle[1].w - triangle[0].w);

vt2.x = (triangle[2].x - triangle[1].x);
vt2.y = (triangle[2].y - triangle[1].y);
vt2.z = (triangle[2].z - triangle[1].z);
vt2.w = (triangle[2].w - triangle[1].w);

vt3.x = (triangle[0].x - triangle[2].x);
vt3.y = (triangle[0].y - triangle[2].y);
vt3.z = (triangle[0].z - triangle[2].z);
vt3.w = (triangle[0].w - triangle[2].w);

//are (first vecx, -third vex, first vec y, -third vecy)
double area = Determinant(vt1.x, -vt3.x, vt1.y, -vt3.y);
//loop through every pixel in box
    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {   

            //determine where the pixel is at
            double detSign1 = Determinant(vt1.x, (x - triangle[0].x), vt1.y, (y - triangle[0].y));
            double detSign2 = Determinant(vt2.x, (x - triangle[1].x), vt2.y, (y - triangle[1].y));
           // double detSign3 = Determinant(vt3.x, (x - triangle[2].x), vt3.y, (y - triangle[2].y));
            double detSign3 = area - detSign1 - detSign2;
            
            //we are inside
            if((detSign1 >= 0) && (detSign2 >= 0) && (detSign3 >= 0))
            {
                Attributes interpAtt;
                int size = (sizeof(interpAtt.arr)/sizeof(*interpAtt.arr));
                double lerpz = (1/Interp(area, detSign1, detSign2, detSign3, triangle[0].w, triangle[1].w, triangle[2].w));

                for (int i = 0; i < size; i++)
                {
                    interpAtt.arr[i].d = Interp(area, detSign1, detSign2, detSign3, attrs[0].arr[i].d, attrs[1].arr[i].d, attrs[2].arr[i].d) * lerpz;
                }
                frag->FragShader(target[y][x],interpAtt, *uniforms);
                
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
		for(int j = 0; j < clippedAttrs[i].numMembers; j++)
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
	int w = target.width() -1;
	int h = target.height() -1;
	
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
          
        PersonalProject(frame);
    
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
