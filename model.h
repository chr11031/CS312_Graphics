#include <vector>
#include <fstream>
#include <string>
#include <sstream> 
#include "definitions.h"

#ifndef MODEL_H
#define MODEL_H

class Model
{
    public:
        std::vector<Vertex> vertices;
        std::vector<Normal> normals;
        std::vector<UV> uvs;

        std::vector<int> vertexIndices;
        std::vector<int> normalIndices;
        std::vector<int> uvIndices;

    public:
        Model()
        {
            vertices.push_back(Vertex());
            normals.push_back(Normal());
            uvs.push_back(UV());
        }
};

Model* loadOBJ(std::string filename)
{
    std::ifstream fin(filename);
    if(!fin.good())
    {
        throw "Can't open file: " + filename;
    }

    Model* ret = new Model();
    std::string line;
    std::string token;
    std::stringstream ss;

    while(getline(fin, line))
    {
        ss.str(line);
        ss >> token;

        if(token == "v") // Vertex
        {
            Vertex v;
            ss >> v.x;
            ss >> v.y;
            ss >> v.z;
            ss >> v.w;

            if(ss.fail())
            {
                v.w = 1.0;
                ss.clear();
            }

            ret->vertices.push_back(v);
        }
        else if(token == "vn") // Normal
        {
            Normal v;
            ss >> v.x;
            ss >> v.y;
            ss >> v.z;

            ret->normals.push_back(v);
        }
        else if(token == "vt") // Texture coordinate
        {
            UV u;
            ss >> u.u;
            ss >> u.v;
            if(ss.fail())
            {
                u.v = 0.0;
                u.w = 0.0;
                ss.clear();
            }
            else
            {
                ss >> u.w;
                if(ss.fail())
                {
                    u.w = 0.0;
                    ss.clear();
                }
            }

            ret->uvs.push_back(u);
        }
        else if(token == "f")  // Face
        {
            for(int i = 0; i < 3; i++)
            {
                int index;
                char c;
                ss >> index;
                ret->vertexIndices.push_back(index);

                ss >> c;
                if(c == ' ')
                    continue;

                ss >> index;
                if(ss.fail())
                {
                    index = 0;
                    ss.clear();
                }
                ret->uvIndices.push_back(index);

                ss >> c;
                if(c == ' ')
                    continue;

                ss >> index;
                if(ss.fail())
                {
                    index = 0;
                    ss.clear();
                }
                ret->normalIndices.push_back(index);  
            }
        }
    }

    fin.close();
    return ret;
}

#endif