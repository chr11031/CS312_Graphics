#include "definitions.h"
#include "coursefunctions.h"

#include <algorithm> 
#include <iostream>
#include <string>


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
                double mouseY= e.motion.yrel;

                myCam.yaw -= mouseX * 0.02;
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
            myCam.z += (cos((myCam.yaw/180.0) * M_PI)) * 0.05;
            myCam.x -= (sin((myCam.yaw/180.0) * M_PI)) * 0.05;
        }
        if((e.key.keysym.sym == 's' && e.type == SDL_KEYDOWN))
        {
            myCam.z -= (cos((myCam.yaw/180.0) * M_PI)) * 0.05;
            myCam.x += (sin((myCam.yaw/180.0) * M_PI)) * 0.05;
        }
        if((e.key.keysym.sym == 'a' && e.type == SDL_KEYDOWN))
        {
            myCam.x -= (cos((myCam.yaw/180.0) * M_PI)) * 0.05;
            myCam.z -= (sin((myCam.yaw/180.0) * M_PI)) * 0.05;
        }
        if((e.key.keysym.sym == 'd' && e.type == SDL_KEYDOWN))
        {
            myCam.x += (cos((myCam.yaw/180.0) * M_PI)) * 0.05;
            myCam.z += (sin((myCam.yaw/180.0) * M_PI)) * 0.05;
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
    target[(int)v[0].y][(int)v[0].x] = attrs[0][0].d;
}

/****************************************
 * DRAW_TRIANGLE
 * Renders a line to the screen.
 ***************************************/
void DrawLine(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{
    // Your code goes here
}

/****************************************
 * Calculate the cross product
 **************************************/
int crossProduct(int v1x, int v1y, int v2x, int v2y) 
{ 
    return v1x * v2y - v1y * v2x;
}

double interp(double x, double y, Vertex* const triangle, double attr1, double attr2, double attr3){
    
    double wv1;
    double wv2;
    double wv3;

    double denominator = ((triangle[1].y - triangle[2].y) * (triangle[0].x - triangle[2].x) + (triangle[2].x - triangle[1].x) * (triangle[0].y - triangle[2].y));
    
    wv1 = ((triangle[1].y - triangle[2].y) * (x - triangle[2].x) + (triangle[2].x - triangle[1].x) * (y - triangle[2].y))/
    denominator; 

    wv2 = ((triangle[2].y - triangle[0].y) * (x - triangle[2].x) + (triangle[0].x - triangle[2].x) * (y - triangle[2].y))/
    denominator; 

    wv3 = 1 - (wv1 + wv2);

    return attr1 * wv1 + attr2 * wv2 + attr3 * wv3;

}

/*************************************************************
 * DRAW_TRIANGLE
 * Renders a triangle to the target buffer. Essential 
 * building block for most of drawing.
 ************************************************************/
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, Attributes* const attrs, Attributes* const uniforms, FragmentShader* const frag)
{

    // target[(int)triangle[0].y][(int)triangle[0].x] = attrs[0][0].d;
    // target[(int)triangle[1].y][(int)triangle[1].x] = attrs[1][0].d;
    // target[(int)triangle[2].y][(int)triangle[2].x] = attrs[2][0].d;

    // Testing
    double r = attrs[2][0].d;
    double g = attrs[2][1].d;
    double b = attrs[2][2].d;

    /* get the bounding box of the triangle */
    int maxX = std::max(triangle[0].x, std::max(triangle[1].x, triangle[2].x));
    int minX = std::min(triangle[0].x, std::min(triangle[1].x, triangle[2].x));
    int maxY = std::max(triangle[0].y, std::max(triangle[1].y, triangle[2].y));
    int minY = std::min(triangle[0].y, std::min(triangle[1].y, triangle[2].y));

    /* spanning vectors of edge (v1,v2) and (v1,v3) */
    Vertex vs1 = {triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y};
    Vertex vs2 = {triangle[2].x - triangle[0].x, triangle[2].y - triangle[0].y};

    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
  
            Vertex q = {x - triangle[0].x, y - triangle[0].y};

            float s = (float)crossProduct(q.x,q.y, vs2.x,vs2.y) / crossProduct(vs1.x,vs1.y, vs2.x,vs2.y);
            float t = (float)crossProduct(vs1.x,vs1.y, q.x, q.y) / crossProduct(vs1.x,vs1.y, vs2.x,vs2.y);

            if ( (s >= 0) && (t >= 0) && (s + t <= 1))
            { 
                /* inside triangle */

                Attributes interpolatedAttrs;

                // INTERPOLATE W for finding the corrected z
                // z = 1/w
                // Lerp 3 things 1/z u/z v/z, the last two are already done for us

                // 1) Divide attr. by depth
                // 2) Lerp as normal
                // 3) Divide by the corrected z

                double interpolatedW = interp(x,y,triangle,triangle[0].w,triangle[1].w,triangle[2].w);

                for(int i = 0; i < attrs[0].numMembers; i++){
                    interpolatedAttrs.insertDbl(interp(x,y,triangle,attrs[0][i].d,attrs[1][i].d,attrs[2][i].d)/interpolatedW);
                }

                interpolatedAttrs;

                frag->FragShader(target[y][x],interpolatedAttrs,*uniforms);


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

    int i = 0;
    // Defaults to pass-through behavior
    if(vert == NULL)
    {
        for(; i < numIn; i++)
        {
            transformedVerts[i] = inputVerts[i];
            transformedAttrs[i] = inputAttrs[i];
        }
    }
    else{
        for(; i < numIn; i++)
        {
            (*vert).VertShader(transformedVerts[i], transformedAttrs[i], inputVerts[i], inputAttrs[i], (*uniforms));   
            transformedAttrs[i] = inputAttrs[i];  
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
		}
		

		
	
	
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
	int w = target.width();
	int h = target.height();
	
	for(int i = 0; i < numClipped; i++)
	{
		clippedVertices[i].x = (round (clippedVertices[i].x + 1) / 2.0 * w);
		clippedVertices[i].y = (round (clippedVertices[i].y + 1) / 2.0 * h);
		
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
        // TestDrawFragments(frame);
        TestVertexShader(frame);
        // TestPipeline(frame);

        //GameOfLife(frame);

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
