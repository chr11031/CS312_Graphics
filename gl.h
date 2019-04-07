#ifndef OBJ_H
#define OBJ_H

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "gl/glew.h"
#include "SDL2/SDL_opengl.h" 
#include "stdio.h"
#include "stdlib.h"
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

// More references 
#define SCREEN_W 640.0f
#define SCREEN_H 480.0f

// Abbreviate namespace references 
using glm::mat4;
using glm::vec3;
using glm::vec2;
using std::string;
using std::cout;
using std::endl;
using std::fstream;


// Reference all GLM types for 'using'
using glm::mat4;
using glm::vec3;
using glm::vec2;
using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::fstream;



/**********************************************************
 * GLOBAL VARIABLES
 *********************************************************/
// Our 'Camera' 
struct userCamera
{
	float camX;
	float camY;
	float camZ;

	float pitch;
	float yaw;
	float roll;
} myCam;

float threshold = 1.0;
/**********************************************************
 * < END OF GLOBALS >
 *********************************************************/
#define CAM_INCREMENT 0.05
#define STEP_INCREMENT 0.05

// Update state based on keyboard
bool processUserInputs(bool & running)
{
	SDL_Event e;
	while(SDL_PollEvent(&e)) 
	{
		if(e.type == SDL_QUIT) 
		{
			running = false;
		}
		if(e.type == SDL_KEYDOWN && e.key.keysym.sym == 'q') 
		{
			running = false;
			break;
		}
		if(e.type == SDL_KEYDOWN && e.key.keysym.sym == 't') 
		{
			threshold -= 0.01;
			if(threshold < 0)
			{
				threshold = 0;
			}
			break;
		}
		if(e.type == SDL_KEYDOWN && e.key.keysym.sym == 'y') 
		{
			threshold += 0.01;
			if(threshold > 1.0)
			{
				threshold = 1.0;
			}
			break;
		}

        if(e.type == SDL_MOUSEMOTION)
        {
            int cur = SDL_ShowCursor(SDL_QUERY);
            if(cur == SDL_DISABLE)
            {
                double mouseX = e.motion.xrel;
                double mouseY = e.motion.yrel;
                myCam.yaw   -= (mouseX * CAM_INCREMENT);
                myCam.pitch -= (mouseY * CAM_INCREMENT);
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

            myCam.camZ -= (cos((myCam.yaw / 180.0) * M_PI)) * STEP_INCREMENT;
            myCam.camX -= (sin((myCam.yaw / 180.0) * M_PI)) * STEP_INCREMENT;
        }
        if(e.key.keysym.sym == 's' && e.type == SDL_KEYDOWN)
        {
            myCam.camZ += (cos((myCam.yaw / 180.0) * M_PI)) * STEP_INCREMENT;
            myCam.camX += (sin((myCam.yaw / 180.0) * M_PI)) * STEP_INCREMENT;
        }
        if(e.key.keysym.sym == 'a' && e.type == SDL_KEYDOWN)
        {
            myCam.camX -= (cos((myCam.yaw / 180.0) * M_PI)) * STEP_INCREMENT;
            myCam.camZ += (sin((myCam.yaw / 180.0) * M_PI)) * STEP_INCREMENT;
        }
        if(e.key.keysym.sym == 'd' && e.type == SDL_KEYDOWN)
        {
			myCam.camX += (cos((myCam.yaw / 180.0) * M_PI)) * STEP_INCREMENT;
            myCam.camZ -= (sin((myCam.yaw / 180.0) * M_PI)) * STEP_INCREMENT;
        }
	}
}


struct bmpRGB
{
  unsigned char b;
  unsigned char g; 
  unsigned char r;
};

// The portion of the Bitmap header we want to read
struct bmpLayout
{
  int offset;
  int headerSize;
  int width;
  int height;
  unsigned short colorPlanes;
  unsigned short bpp;
};

// How to read our file
bool readBMP(const char* fileName, bmpRGB* & data, int & w, int & h)
{
	// Read in Header - check signature
	FILE * fp = fopen(fileName, "rb");	    
	if(fp == NULL)
	{
	    printf("Could not open file: %s\n", fileName);
	    return false;
	}
	char signature[2];
	fread(signature, 1, 2, fp);
	if(!(signature[0] == 'B' && signature[1] == 'M'))
	{
		printf("Invalid header for file: \"%c%c\"", signature[0], signature[1]);
		return 1;
	}

	// Read in BMP formatting - verify type constraints
	bmpLayout layout;
	fseek(fp, 8, SEEK_CUR);
	fread(&layout, sizeof(layout), 1, fp);
	if(layout.width % 2 != 0 || layout.width <= 4)
	{
		printf("Size Width MUST be a power of 2 larger than 4; not %d\n", w);
		return false;		
	}
	if(layout.bpp != 24)
	{
		printf("Bits per pixel of image must be 24; not %d\n", layout.bpp);
		return false;
	}

	// Copy W+H information
	w = layout.width;
	h = layout.height;

	// Advance to beginning of pixel data, read values in
	data = (bmpRGB*)malloc(sizeof(bmpRGB)*w*h);
	fseek(fp, layout.offset, SEEK_SET);  	
	fread(data, sizeof(bmpRGB), w*h, fp);
	fclose(fp); 
	return true;
}


bool parseFile(char* ref, string & out)
{
	// Check for read success
	fstream fin(ref);
	if(fin.fail())
	{
		cout << "Failed to read file " << ref << endl;
		return false;
	}
	string tmp;
	while(getline(fin, tmp))
	{
		out += tmp;
		out += "\n";
	}
	fin.close();
	return true;
}

bool compileShader(std::string shader, int type, int & handle)
{
	int shaderID = glCreateShader(type);
	if(shaderID == 0)
	{
		cout << "Could not create shader of type " << type << endl;
		return false;
	}
	const char* src = shader.c_str();
	glShaderSource(shaderID, 1, &src, NULL);
	glCompileShader(shaderID);

	int status;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);  
	if(status == 0)
	{
		int buffLen;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &buffLen);
		char logString[buffLen+1];
		glGetShaderInfoLog(shaderID, buffLen, 0, logString);
		cout << "Error Compiling shader of type " << type << endl
			 << logString << endl;	
		return false;
	}

	handle = shaderID;
	return true;
}

bool compileProgram(int vertexID, int fragID, int & handle)
{
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexID); 
	glAttachShader(shaderProgram, fragID);
	glLinkProgram(shaderProgram);

	int status;
	SDL_Delay(10);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if(status == 0)
	{
		cout << "Error Linking shaders" << endl;
		return false;
	}
		handle = shaderProgram;
		return true;
}

void validate(bool success, char * message)
{
	success |= glGetError();
	if(!success)
	{
		cout << "FAILURE: " << message << endl;
		exit(1);
	}
}

bool loadTexture(char * fileName, int & handle)
{
	// Read our data to a 2D array
	bmpRGB* data;
	int w;
	int h;

	bool success = readBMP(fileName, data, w, h);
	if(!success)
	{
		cout << "FAILURE TO LOAD FILE THROUGH SDL2 " << fileName << endl;
		return false;
	}

	// Convert to RGBA data 
	unsigned int* rgbaRef = NULL;
	rgbaRef = (unsigned int*)malloc(w*h*sizeof(unsigned int));


	if(rgbaRef == NULL)
	{
	    printf("COULD NOT ALLOCATE TEXTURE BUFFER\n");
	    return false;
	}
	int len = w*h;
	for(int i = 0; i < len; i++)
	{
	    unsigned int r = data[i].r;
	    unsigned int g = data[i].g;
	    unsigned int b = data[i].b;
	    rgbaRef[i] = (0xff << 24) + (b << 16) + (g << 8) + r;
	}
	free(data);

	// OpenGL bindings
	int textureID;
	glGenTextures(1, (GLuint*)&textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)rgbaRef);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind from current call
	int err = glGetError();
	if(err != GL_NO_ERROR)
	{
		cout << "FAILURE TO INITIALIZE IMAGE IN OPENGL. Returned: " << err << endl;
		return false;
	}

	free(rgbaRef);
	handle = textureID;
	return true;
}

void setupMVP(mat4 & mvp)
{
	mat4 proj = glm::perspective(glm::radians(60.0f), SCREEN_W / SCREEN_H, 0.1f, 100.0f);  // Perspective matrix
	mat4 view = glm::mat4(1.0);
	view = 		glm::rotate(view, 			glm::radians(-myCam.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	view = 		glm::rotate(view, 			glm::radians(-myCam.yaw), glm::vec3(0.0, 1.0f, 0.0));
	view = 		glm::translate(view, 		glm::vec3(-myCam.camX, -myCam.camY, -myCam.camZ));
	mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, -10.0));
	mvp = proj * view * model; 
}

struct vertexData
{
	float vert[3];
	float uv[2];
	float normal[3];
	int materialID;
};

struct material
{
	char name[256];
	int Ns;
	float Ka[3];
	float Kd[3];
	float Ks[3];
	int d;
	int illum;
	char map_Kd[256];	
};

bool getData(const char* fileName, 
		vector<material> & materials,
		vector<vertexData> & vertexBuffer, bool & hasUV, bool & hasNormals)
{
	// MTL Links 
	vector<string> mtlFiles;
	vector<string> vertexMaterials;
	string currentMaterial = "NA";
	currentMaterial.reserve(256);

	// Open file
	FILE* fp;
	fp = fopen(fileName, "r");
	if(fp == NULL)
	{
		cout << "OBJ: " << fileName << " could not be opened\n";
		return false;
	}

	// Read in a line at a time 
	vector<vec3> vertexIndex;
	vector<vec2> uvIndex;
	vector<vec3> normalIndex;
	char lineHeader[256];
	while(fscanf(fp, "%s", lineHeader) != EOF)
	{
		if(strcmp(lineHeader, "mtllib") == 0)
		{
			string mtlFile;
			mtlFile.reserve(256);
			fscanf(fp, "%s\n", &mtlFile[0], mtlFile.size());
			mtlFiles.push_back(mtlFile);

			// Assignment operator is bad so manual copy is needed 
			mtlFiles[mtlFiles.size()-1].reserve(256);
			strcpy(&((mtlFiles[mtlFiles.size()-1])[0]), &mtlFile[0]);
		}
		else if(strcmp(lineHeader, "usemtl") == 0)
		{
			fscanf(fp, "%s\n", &currentMaterial[0], currentMaterial.size());
		}
		else if(strcmp(lineHeader, "v") == 0) // Check for "V" Vertex data
		{
			vec3 vertex;
			fscanf(fp, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertexIndex.push_back(vertex);
		}
		else if(strcmp(lineHeader, "vt") == 0) // Check for "VT" UV Data
		{
			vec2 uv;
			fscanf(fp, "%f %f\n", &uv.x, &uv.y); 
			uvIndex.push_back(uv);
		}
		else if(strcmp(lineHeader, "vn") == 0) // Check for "VN" Normal data
		{
			vec3 normal;
			fscanf(fp, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			normalIndex.push_back(normal);
		}
		else if(strcmp(lineHeader, "vn") == 0) // Check for "VN" Normal data
		{
			vec3 normal;
			fscanf(fp, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			normalIndex.push_back(normal);
		}
		else if(strcmp(lineHeader, "f") == 0) // Generate faces "F" to VBO buff
		{
			unsigned int vertexI[3], uvI[3], normalI[3];
			
			int matches = fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
			&vertexI[0], &uvI[0], &normalI[0],
			&vertexI[1], &uvI[1], &normalI[1],
			&vertexI[2], &uvI[2], &normalI[2]);
			if(matches != 9)
			{
				std::cout << "File can't be read by our simple parser.... :(\n";
				return false;	      
			}

			for(int i = 0; i < 3; i++)
			{
				vertexData tmp;
				tmp.vert[0] = vertexIndex[vertexI[i]-1].x;
				tmp.vert[1] = vertexIndex[vertexI[i]-1].y;
				tmp.vert[2] = vertexIndex[vertexI[i]-1].z;
			
				if(uvIndex.size() > 0)
				{
					tmp.uv[0] = uvIndex[uvI[i]-1].x;
					tmp.uv[1] = uvIndex[uvI[i]-1].y;
				}
				if(normalIndex.size() > 0)
				{
					tmp.normal[0] = normalIndex[normalI[i]-1].x;
					tmp.normal[1] = normalIndex[normalI[i]-1].y;
					tmp.normal[2] = normalIndex[normalI[i]-1].z;
				}

				// No associated material so far
				tmp.materialID = -1;
			
				// Add data to vector
				vertexBuffer.push_back(tmp);
				
				// Associate a material with each vertex 
				vertexMaterials.push_back(currentMaterial);				

				// Assignment operator is bad so manual copy is needed 
				vertexMaterials[vertexMaterials.size()-1].reserve(256);
				strcpy(&((vertexMaterials[vertexMaterials.size()-1])[0]), &currentMaterial[0]);
			}
		}
	}
	hasUV = (uvIndex.size() > 0);
	hasNormals = (normalIndex.size() > 0);
	fclose(fp);

	
	// Go through each MTL file... parse each material	
	for(int i = 0; i < mtlFiles.size(); i++)
	{
		// Read in a file
		FILE* fp;
		fp = fopen(mtlFiles[i].c_str(), "r");
		if(fp == NULL)
		{
			cout << "MTL FILE : " << mtlFiles[i].c_str() << " could not be opened\n";
			return false;
		}
		
		char lineHeader[256];
		while(fscanf(fp, "%s", lineHeader) != EOF)
		{
			if(strcmp(lineHeader, "newmtl") == 0)
			{
				material mat;
				fscanf(fp, "%s\n", mat.name, 256);
				materials.push_back(mat);
			}
			else if(strcmp(lineHeader, "Ns") == 0)
			{
				fscanf(fp, "%d\n", &(materials.back().Ns));
			}
			else if(strcmp(lineHeader, "Ka") == 0)
			{
				fscanf(fp, "%f %f %f\n", &(materials.back().Ka[0]), &(materials.back().Ka[1]), &(materials.back().Ka[2]));
			}
			else if(strcmp(lineHeader, "Kd") == 0)
			{
				fscanf(fp, "%f %f %f\n", &(materials.back().Kd[0]), &(materials.back().Kd[1]), &(materials.back().Kd[2]));
			}
			else if(strcmp(lineHeader, "Ks") == 0)
			{
				fscanf(fp, "%f %f %f\n", &(materials.back().Ks[0]), &(materials.back().Ks[1]), &(materials.back().Ks[2]));
			}
			else if(strcmp(lineHeader, "d") == 0)
			{
				fscanf(fp, "%d\n", &(materials.back().d));
			}
			else if(strcmp(lineHeader, "illum") == 0)
			{
				fscanf(fp, "%d\n", &(materials.back().illum));
			}
			else if(strcmp(lineHeader, "map_Kd") == 0)
			{
				fscanf(fp, "%s\n", (materials.back().map_Kd));
			}
		}		
		fclose(fp);
	}
	
	
	// Resolve indexes for material file 
	for(int i = 0; i < vertexBuffer.size(); i++)
	{
		// Find Material associated with its name 
		int match = -1;
		for(int j = 0; j < materials.size(); j++)
		{
			const char * a = vertexMaterials[i].c_str();
			const char * b = materials[j].name;
			if(strcmp(vertexMaterials[i].c_str(), materials[j].name) == 0)
			{
				match = j;
			}
		}	
		
		// Associate Material with Vertex 
		if(match != -1)
		{
			vertexBuffer[i].materialID = match;
		}	
	}

	
	return true;
}

#endif 
