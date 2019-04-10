#include "graphicMatrix.h"
#include "definitions.h"
#include "shaders.h"
#include "coursefunctions.h"
#include <istream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

#ifndef OBJLOADER_H
#define OBJLOADER_H

struct UV{
    double u = 0;
    double v = 0;
    double w = 0;
};

void readFile(char* filename);

class Model
{
    public:
        vector <Vertex> verticesCoordsXYZ;
        vector <Vertex> verticiesVN;
        vector <UV> textureUV;

        //indexs
        vector <int> vertexIndex;
        vector <int> normalIndex;
        vector <int> UVIndex;
        
        Matrix objTransform = Matrix(4,4);//deftault transform to identity matrix

     Model(){
         verticesCoordsXYZ.push_back(Vertex());
         verticiesVN.push_back(Vertex());
         textureUV.push_back(UV());
     };
     Model(char* fileName){
         verticesCoordsXYZ.push_back(Vertex());
         verticiesVN.push_back(Vertex());
         textureUV.push_back(UV());



        //read in obj data
        ifstream fin(fileName);
        string line;
        string token;
        stringstream ss;
        char buffer[256];

        //fin.open(fileName);
        if(!fin.is_open()){
            ;//throw "Error reading " + fileName;//error!
        }
        else
        {

            while (!fin.eof())
            {
                fin.getline(buffer, 256);
                ss.str(buffer);
                ss >> token;
                Vertex tempV;

                if (token == "v")//vertexes
                {   
                    ss >> tempV.x >> tempV.y >> tempV.z;
                    tempV.w = 1.0;
                    if(ss.fail())
                    {
                        tempV.w = 1.0;
                        ss.clear();
                    }
                    verticesCoordsXYZ.push_back(tempV);   
                }

                else if (token == "vn")//vertex Normals
                {
                    ss >> tempV.x >> tempV.y >> tempV.z;
                    tempV.w = 1.0;
                    if(ss.fail())
                    {
                        tempV.w = 1.0;
                        ss.clear();
                    }
                    verticiesVN.push_back(tempV);   
                }

                else if (token == "vt")//texture
                {
                    UV tempUV;
                    ss >> tempUV.u;
                    ss >> tempUV.v;
                    if(ss.fail())
                    {
                        //tempUV.v = 0.0;
                        tempUV.w = 0.0;
                        ss.clear();
                    }
                    else
                    {
                        ss >> tempUV.w;
                        if(ss.fail())
                        {
                            tempUV.w = 0.0;
                            ss.clear();
                        }
                    }

                    textureUV.push_back(tempUV);
                }

                else if (token == "f")//faces
                {
                    for(int i = 0; i < 3; i++)
                    {
                        int index;
                        char c;
                        ss >> index;
                        vertexIndex.push_back(index);

                        ss >> c;
                        if(c == ' ')
                            continue;

                        ss >> index;
                        if(ss.fail())
                        {
                            index = 0;
                            ss.clear();
                        }
                        UVIndex.push_back(index);

                        ss >> c;
                        if(c == ' ')
                            continue;

                        ss >> index;
                        if(ss.fail())
                        {
                            index = 0;
                            ss.clear();
                        }
                        normalIndex.push_back(index);

                    }   
                }
                else
                {
                    ss.clear();//non object line. no need to read.
                }
            }  
            fin.close();

        }   

    }


    void draw(Buffer2D<PIXEL> &target,
                  Matrix &view, Matrix &projection,
                  Buffer2D<double>* zBuf, string textureS)
        {
            Vertex tri[3];//triangle
			Attributes vAttr[3];//need to pull three points to make a triangle
            Attributes uniforms;//for image
            
            //uniforms
            // 0 -> image reference
            // 1 -> model transform
            // 2 -> view transform (camera)
            // 3 -> perspective transform
            static BufferImage myImage("checker.bmp");//replace with textureS later
            uniforms.insertPtr(&myImage);
            uniforms.insertPtr((void*)&(this->objTransform));
            uniforms.insertPtr(&view);
            uniforms.insertPtr(&projection);

            if(textureS == "")
            {
                //rgb
                uniforms.att[0].d = 1;
                uniforms.att[1].d = 0;
                uniforms.att[2].d = 0;
            }
            
            FragmentShader frag;
            VertexShader vert;
            if(textureS == ""){
                frag.FragShader = ColorFragShader;
            }
            else if(textureS != ""){
                frag.FragShader = ImageFragShader; 
            }
            vert.VertShader = SimpleVertexShader2;

            int indexCount = this->vertexIndex.size();
            for(int i = 0; i < indexCount; )
            {
                if(textureS != "")
                {
                    tri[2] = this->verticesCoordsXYZ[this->vertexIndex[i]];
                    vAttr[2].insertDbl(this->textureUV[this->UVIndex[i]].u);
                    vAttr[2].insertDbl(this->textureUV[this->UVIndex[i++]].v);

                    tri[1] = this->verticesCoordsXYZ[this->vertexIndex[i]];
                    vAttr[1].insertDbl(this->textureUV[this->UVIndex[i]].u);
                    vAttr[1].insertDbl(this->textureUV[this->UVIndex[i++]].v);

                    tri[0] = this->verticesCoordsXYZ[this->vertexIndex[i]];
                    vAttr[0].insertDbl(this->textureUV[this->UVIndex[i]].u);
                    vAttr[0].insertDbl(this->textureUV[this->UVIndex[i++]].v);
                }
                else
                {
                    tri[2] = this->verticesCoordsXYZ[this->vertexIndex[i++]];
                    tri[1] = this->verticesCoordsXYZ[this->vertexIndex[i++]];
                    tri[0] = this->verticesCoordsXYZ[this->vertexIndex[i++]];

                    /*double coordinates[3][2] = { {0,0}, {1,0}, {1,1} };//need?How?
                    vAttr[0].insertDbl(coordinates[0][0]);
                    vAttr[0].insertDbl(coordinates[0][1]);
                    vAttr[1].insertDbl(coordinates[1][0]);
                    vAttr[1].insertDbl(coordinates[1][1]);
                    vAttr[2].insertDbl(coordinates[2][0]);
                    vAttr[2].insertDbl(coordinates[2][1]);*/
                    vAttr[0].insertDbl(1);
                    vAttr[0].insertDbl(1);
                    vAttr[0].insertDbl(1);

                }
                
                DrawPrimitive(TRIANGLE, target, tri, vAttr, &uniforms, &frag, &vert, zBuf);
                
            }
        }

};





#endif