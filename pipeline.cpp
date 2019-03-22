#include "definitions.h"
#include "coursefunctions.h"
#include "graphicMatrix.h"
#include "shaders.h"
#include "objLoader.h"

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

                myCam.yaw -= mouseX * .02;
                myCam.pitch +=mouseY * .02;
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
        //Translation
        if((e.key.keysym.sym == 'w' && e.type == SDL_KEYDOWN))
        {
            myCam.z += (cos((myCam.yaw / 180) * M_PI)) * 0.05;
            myCam.x -= (sin((myCam.yaw / 180) * M_PI)) * 0.05;
        }
        if((e.key.keysym.sym == 's' && e.type == SDL_KEYDOWN))
        {
            myCam.z -= (cos((myCam.yaw / 180) * M_PI)) * 0.05;
            myCam.x += (sin((myCam.yaw / 180) * M_PI)) * 0.05;
        }
        if((e.key.keysym.sym == 'a' && e.type == SDL_KEYDOWN))
        {
            myCam.z -= (cos((myCam.yaw / 180) * M_PI)) * 0.05;
            myCam.x -= (sin((myCam.yaw / 180) * M_PI)) * 0.05;
        }
        if((e.key.keysym.sym == 'd' && e.type == SDL_KEYDOWN))
        {
            myCam.z += (cos((myCam.yaw / 180) * M_PI)) * 0.05;
            myCam.x += (sin((myCam.yaw / 180) * M_PI)) * 0.05;
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
	target[(int)v[0].y][(int)v[0].x] = attrs[0].color;
}

/****************************************
 * DRAW_Line
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
void DrawTriangle(Buffer2D<PIXEL> & target, Vertex* const triangle, 
                Attributes* const attrs, Attributes* const uniforms, 
                FragmentShader* const frag)
{
    // Your code goes here
    //Barycentric fill algorithm, counter clockwise
    //Step one bounding area
    double xmin = MIN3(triangle[0].x, triangle[1].x, triangle[2].x);
    double xmax = MAX3(triangle[0].x, triangle[1].x, triangle[2].x);
    double ymin = MIN3(triangle[0].y, triangle[1].y, triangle[2].y);
    double ymax = MAX3(triangle[0].y, triangle[1].y, triangle[2].y);

    //in bounds fix
    if (xmin < 0) {xmin = 0;}
    if (ymin < 0) {ymin = 0;}
    if (xmax >= target.width()) {xmax = target.width()-1;}
    if (ymax >= target.height()) {ymax = target.height()-1;}

    //define vertexes
    double a[] = {triangle[1].x - triangle[0].x, triangle[1].y - triangle[0].y};
    double b[] = {triangle[2].x - triangle[1].x, triangle[2].y - triangle[1].y};
    double c[] = {triangle[0].x - triangle[2].x, triangle[0].y - triangle[2].y};

    double area = determinant(a[X_Key], -c[X_Key], a[y_Key], -c[y_Key]);

    //for point in bounding box, if positive drawpixel
    for(int ypix = ymin; ypix <= ymax; ypix++)
    {
        for(int xpix = xmin; xpix <= xmax; xpix++)
        {

            //determinants of triangle
            double firstDet = determinant(a[X_Key], xpix - triangle[0].x, a[y_Key], ypix - triangle[0].y);//1x,2x,1y,2y
            double secondDet = determinant(b[X_Key], xpix - triangle[1].x, b[y_Key], ypix - triangle[1].y);
            double thirdDet = determinant(c[X_Key], xpix - triangle[2].x, c[y_Key], ypix - triangle[2].y);

            //If inside the triangle draw
            if (firstDet >= 0 && secondDet >= 0 && thirdDet >=0)
            {
                if(attrs[0].color != 0){
                    target[(int)ypix][(int)xpix] = attrs[0].color;
                }//SEG FAULTS IN PIPELINE
                Attributes interpolatedAttribs;

                double zCorrect = 1 / interpolate(area, firstDet, secondDet, thirdDet, triangle[0].w, triangle[1].w, triangle[2].w); 

                //for the color RGB and UV
                
                interpolatedAttribs.insertDbl(interpolate(area, firstDet, secondDet, thirdDet, attrs[0].att[0].d, attrs[1].att[0].d, attrs[2].att[0].d, zCorrect));
                interpolatedAttribs.insertDbl(interpolate(area, firstDet, secondDet, thirdDet, attrs[0].att[1].d, attrs[1].att[1].d, attrs[2].att[1].d, zCorrect));
                interpolatedAttribs.insertDbl(interpolate(area, firstDet, secondDet, thirdDet, attrs[0].att[2].d, attrs[1].att[2].d, attrs[2].att[2].d, zCorrect));
                

                frag->FragShader(target[ypix][xpix], interpolatedAttribs, *uniforms);
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
    if (vert == NULL)
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

//y limit intersect
void intersectAgainstYLimit(double & along, const double & yLimit, const double & segStartY, const double & segEndY)
{
    along = -1;
    double segDiffY = (segEndY - segStartY);

    if(segDiffY == 0){ return ;}

    along = (yLimit - segStartY) / segDiffY;
    
}

// intersect at positive line
void intersectAtPositiveLine(double & along, const double & segStartX, const double & segStartY, const double & segEndX, const double & segEndY)
{
    along = -1;
    double segDiffX = (segEndX - segStartX);
    double segDiffY = (segEndY - segEndY);

    if(segDiffX == segDiffY){ return ;}

    along = (segStartY - segStartX) / (segDiffX - segDiffY);
}

void intersectAtNegativeLine(double & along, const double & segStartX, const double & segStartY, const double & segEndX, const double & segEndY)
{
    along = -1;
	double segDiffX = (segEndX - segStartX);
	double segDiffY = (segEndY - segStartY);
	
	if(segDiffX == segDiffY) { return ; }//neg or pos segDiffX
	
    along = (segStartY + segStartX) / (-segDiffX - segDiffY);
}

Vertex vertexBetweenVerts(const Vertex & vertA, const Vertex &vertB, const double &along)
{
    Vertex rv;
    rv.x = (vertA.x + (vertB.x - vertA.x) * along);
    rv.y = (vertA.y + (vertB.y - vertA.y) * along);
    rv.z = (vertA.z + (vertB.z - vertA.z) * along);
    rv.w = (vertA.w + (vertB.w - vertA.w) * along);
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

            sinkVerts[numOut] = vertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAgainstYLimit(along, wLimit, srcVerts[cur].w, srcVerts[next].w);

            sinkVerts[numOut] = vertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
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

            sinkVerts[numOut] = vertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);

            sinkVerts[numOut] = vertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
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

            sinkVerts[numOut] = vertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].x, srcVerts[cur].w, srcVerts[next].x, srcVerts[next].w);

            sinkVerts[numOut] = vertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
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

            sinkVerts[numOut] = vertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);

            sinkVerts[numOut] = vertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
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

            sinkVerts[numOut] = vertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].y, srcVerts[cur].w, srcVerts[next].y, srcVerts[next].w);

            sinkVerts[numOut] = vertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
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

            sinkVerts[numOut] = vertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtPositiveLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);

            sinkVerts[numOut] = vertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
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

            sinkVerts[numOut] = vertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
            sinkAttrs[numOut++] = Attributes(srcAttrs[cur], srcAttrs[next], along);
        }
        else if(!curVertIn && nextVertIn)
        {
            double along;
            intersectAtNegativeLine(along, srcVerts[cur].z, srcVerts[cur].w, srcVerts[next].z, srcVerts[next].w);

            sinkVerts[numOut] = vertexBetweenVerts(srcVerts[cur], srcVerts[next], along);
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

//normalizeVerticies
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
			clippedAttrs[i].att[j].d /= zValue;
		}		
	}
}

//viewpoint transform
void viewportTransform(Buffer2D<PIXEL> & target, Vertex clippedVertices[], const int & numClipped){
    //move from -1 to 1 spance in X,Y screen
    int w = target.width()-1;//-1?
    int h = target.height()-1;//-1?

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
    VertexShaderExecuteVertices(vert, inputVerts, inputAttrs, 
        numIn, uniforms, transformedVerts, transformedAttrs);

    // Clipping
    Vertex clippedVertices[MAX_VERTICES];
    Attributes clippedAttrs[MAX_VERTICES];
    int numClipped;
    clipVertices(transformedVerts, transformedAttrs, numIn, 
                clippedVertices, clippedAttrs, numClipped);

    // Normalization
    normalizeVertices(clippedVertices, clippedAttrs, numClipped);

    // Adapt to viewport
    viewportTransform(target, clippedVertices, numClipped);

    // Vertex Interpolation & Fragment Drawing
    switch(prim)
    {
        case POINT:
            DrawPoint(target, clippedVertices, clippedAttrs, uniforms, frag);
            break;
        case LINE:
            DrawLine(target, clippedVertices, clippedAttrs, uniforms, frag);
            break;
        case TRIANGLE:
            //DrawTriangle(target, transformedVerts, transformedAttrs, uniforms, frag);
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

//Model cad view
//template <class T>
void CADViewModel(Buffer2D<PIXEL> & target)
{
        // Each CAD Quadrant
        static int halfWid = target.width()/2;
        static int halfHgt = target.height()/2;
        static Buffer2D<PIXEL> topLeft(halfWid, halfHgt);  //top view
        static Buffer2D<PIXEL> topRight(halfWid, halfHgt); //3d full pipeline
        static Buffer2D<PIXEL> botLeft(halfWid, halfHgt);  //right veiw
        static Buffer2D<PIXEL> botRight(halfWid, halfHgt); //left view

        static Buffer2D<double> zBuf(target.width(), target.height());

        //loop through the model
        //for(std::vector<T>::iterator it = v.begin(); it != v.end(); ++it) {
    /* std::cout << *it; ... /
        }*/
        

        /*Vertex quad[] = { {-20,-20, 50, 1},
                          {20, -20, 50, 1},
                          {20, 20, 50, 1},
                          {-20,20, 50, 1}};

        Vertex verticesImgA[3];
        Attributes imageAttributesA[3];
        verticesImgA[0] = quad[0];
        verticesImgA[1] = quad[1];
        verticesImgA[2] = quad[2];

        Vertex verticesImgB[3];        
        Attributes imageAttributesB[3];
        verticesImgB[0] = quad[2];
        verticesImgB[1] = quad[3];
        verticesImgB[2] = quad[0];

        double coordinates[4][2] = { {0,0}, {1,0}, {1,1}, {0,1} };*/
        // Your texture coordinate code goes here for 'imageAttributesA, imageAttributesB'
        
        /*imageAttributesA[0].insertDbl(coordinates[0][0]);
        imageAttributesA[0].insertDbl(coordinates[0][1]);
        imageAttributesA[1].insertDbl(coordinates[1][0]);
        imageAttributesA[1].insertDbl(coordinates[1][1]);
        imageAttributesA[2].insertDbl(coordinates[2][0]);
        imageAttributesA[2].insertDbl(coordinates[2][1]);

        imageAttributesB[0].insertDbl(coordinates[2][0]);
        imageAttributesB[0].insertDbl(coordinates[2][1]);
        imageAttributesB[1].insertDbl(coordinates[3][0]);
        imageAttributesB[1].insertDbl(coordinates[3][1]);
        imageAttributesB[2].insertDbl(coordinates[0][0]);
        imageAttributesB[2].insertDbl(coordinates[0][1]);*/
        Model mesh = Model("testCube.txt");

        static BufferImage myImage("checker.bmp");
        Attributes imageUniformsTopR;
        Attributes imageUniformsTopL;
        Attributes imageUniformsBottomR;
        Attributes imageUniformsBottomL;

        //uniforms
        // 0 -> image reference
        // 1 -> model transform
        // 2 -> view transform
        // 3 -> projection/orthographic

        Matrix model = Matrix(4,4);

        Matrix topLcamView = Matrix(4,4);
        topLcamView.cameraTransform(0,0,0,90,0,0);//top view
        Matrix topRcamView = Matrix(4,4);
        topRcamView.cameraTransform(myCam.x,myCam.y,myCam.z,myCam.pitch,myCam.yaw,myCam.roll);//Moveable view
        Matrix bottomRcamView = Matrix(4,4);
        bottomRcamView.cameraTransform(0,0,0,0,0,0);//front view
        Matrix bottomLcamView = Matrix(4,4);
        bottomLcamView.cameraTransform(0,0,0,0,90,0);//right view
        
        Matrix projection = Matrix(4,4);
        projection.transformPerspective(60.0, 1.0, 1, 200); //FOV, AspectRatio, Near, Far
        /*
        imageUniformsTopL.insertPtr((void*)&myImage);
        imageUniformsTopL.insertPtr((void*)&model);
        imageUniformsTopL.insertPtr((void*)&topLcamView);
        imageUniformsTopL.insertPtr((void*)&projection);

        imageUniformsTopR.insertPtr((void*)&myImage);
        imageUniformsTopR.insertPtr((void*)&model);
        imageUniformsTopR.insertPtr((void*)&topRcamView);
        imageUniformsTopR.insertPtr((void*)&projection);

        imageUniformsBottomL.insertPtr((void*)&myImage);
        imageUniformsBottomL.insertPtr((void*)&model);
        imageUniformsBottomL.insertPtr((void*)&bottomLcamView);
        imageUniformsBottomL.insertPtr((void*)&projection);

        imageUniformsBottomR.insertPtr((void*)&myImage);
        imageUniformsBottomR.insertPtr((void*)&model);
        imageUniformsBottomR.insertPtr((void*)&bottomRcamView);
        imageUniformsBottomR.insertPtr((void*)&projection);*/

        /*FragmentShader fragImg;
        fragImg.FragShader = ImageFragShader;

        VertexShader vertImg;
        vertImg.VertShader = SimpleVertexShader2;*/

        mesh.draw(topLeft, topLcamView, projection, &zBuf, "checker.bmp");
        mesh.draw(topRight, topRcamView, projection, &zBuf, "checker.bmp");//camera
        mesh.draw(botLeft, bottomLcamView, projection, &zBuf, "checker.bmp");
        mesh.draw(botRight, bottomRcamView, projection, &zBuf, "checker.bmp");
/*
        DrawPrimitive(TRIANGLE, topLeft, verticesImgA, imageAttributesA, &imageUniformsTopL, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, topLeft, verticesImgB, imageAttributesB, &imageUniformsTopL, &fragImg, &vertImg, &zBuf);

        DrawPrimitive(TRIANGLE, topRight, verticesImgA, imageAttributesA, &imageUniformsTopR, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, topRight, verticesImgB, imageAttributesB, &imageUniformsTopR, &fragImg, &vertImg, &zBuf);

        DrawPrimitive(TRIANGLE, botLeft, verticesImgA, imageAttributesA, &imageUniformsBottomL, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, botLeft, verticesImgB, imageAttributesB, &imageUniformsBottomL, &fragImg, &vertImg, &zBuf);

        DrawPrimitive(TRIANGLE, botRight, verticesImgA, imageAttributesA, &imageUniformsBottomR, &fragImg, &vertImg, &zBuf);
        DrawPrimitive(TRIANGLE, botRight, verticesImgB, imageAttributesB, &imageUniformsBottomR, &fragImg, &vertImg, &zBuf);
*/


        // Blit four panels to target
        int yStartSrc = 0;
        int xStartSrc = 0;
        int yLimitSrc = topLeft.height();
        int xLimitSrc = topLeft.width();
        for(int ySrc = yStartSrc; ySrc < yLimitSrc; ySrc++)
        {
                for(int xSrc = xStartSrc; xSrc < xLimitSrc; xSrc++)
                {
                        target[ySrc][xSrc]                 = botLeft[ySrc][xSrc];
                        target[ySrc][xSrc+halfWid]         = botRight[ySrc][xSrc];
                        target[ySrc+halfHgt][xSrc]         = topLeft[ySrc][xSrc];
                        target[ySrc+halfHgt][xSrc+halfWid] = topRight[ySrc][xSrc];
                }
        }
}

void Project1Pipeline(Buffer2D<PIXEL> & target)
{
        // This is similar to TestDrawPerspectiveCorrect 
        // except that:
        //      1) perspective projection is expected from
        //         the programmer in the vertex shader.
        //      2) Clipping/normalization must be turned on.
        //      3) The ViewPort Transform must be applied.
        //      4) The Z-Buffer is incorporated into drawing.
        //      5) You may want to involve camera variables:
        //              i)   camYaw
        //              ii)  camPitch
        //              iii) camRoll, 
        //              iv)  camX
        //              v)   camY
        //              vi)  camZ
        //      To incorporate a view transform (add movement)
        
        static Buffer2D<double> zBuf(target.width(), target.height());
        // Will need to be cleared every frame, like the screen

        /**************************************************
        * 1. Image quad (2 TRIs) Code (texture interpolated)
        **************************************************/
        Model mesh = Model("testCube.txt");
        
        //uniforms
        // 0 -> image reference
        // 1 -> model transform
        // 2 -> view transform (camera)
        // 3 -> perspective transform

        Matrix camView = Matrix(4,4);
        camView.cameraTransform(myCam.x,myCam.y,myCam.z,myCam.pitch,myCam.yaw,myCam.roll);
        Matrix projection = Matrix(4,4);
        projection.transformPerspective(60.0, 1.0, 1, 200); //FOV, AspectRatio, Near, Far
        
        mesh.draw(target, camView, projection, &zBuf, "");//draw from the mesh object.draw
        
        // NOTE: To test the Z-Buffer additinonal draw calls/geometry need to be called into this scene
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
        //TestDrawFragments(frame); //good
        //TestDrawPerspectiveCorrect(frame); //good
        //TestVertexShader(frame); //good
        //TestPipeline(frame); //goodish //seg top, seg sides
        //CADView(frame);//CAD View
        //CADViewModel(frame);
        Project1Pipeline(frame);

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
