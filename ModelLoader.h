#include "definitions.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#ifndef MODELLOADER_H
#define MODELLOADER_H

struct UVPair
{
  double u;
  double v;
};


/*******************************************
*
*
*******************************************/
class ModelLoader
{
  private:
    std::vector<std::string> fileLines;
    std::vector<Vertex> verts;
    std::vector<UVPair> uv;
    std::vector<Vertex> triangles;
    std::string filename;

    // reads the file into the fileLines vector
    void readFile()
    {
      // read the file by line.
      std::ifstream fin(filename.c_str());
      if (fin.fail())
      {
        std::cout << "unable to load model\n";
        exit(1);
      }

      std::string temp;
      while (getline(fin, temp))
      {
        fileLines.push_back(temp);
      }
      fin.close();
    }

    // save the vertices into the verts vector
    void saveInfo()
    {
      std::string type;
      if(fileLines.size())
      {
        for (int i = 0; i < fileLines.size(); i++)
        {
          std::stringstream str(fileLines[i]);
          str >> type;
          if (type == "v")
          {
            Vertex tempVert;
            tempVert.w = 10;
            str >> tempVert.x >> tempVert.y >> tempVert.z >> tempVert.w;
            verts.push_back(tempVert);
          }
          else if (type == "vt")
          {
            UVPair tempUV;
            str >> tempUV.u >> tempUV.v;
            uv.push_back(tempUV);
          }
          else if (type == "f")
          {
            Vertex temp;
            std::string tempString;
            temp.w = -1;
            str >> temp.z >> tempString >> temp.y >> tempString >> temp.x >> tempString >> temp.w;
            triangles.push_back(temp);
            if (temp.w != 1)
            {
              Vertex temp2;
              temp2.x = temp.z;
              temp2.y = temp.w;
              temp2.z = temp.x;
              triangles.push_back(temp2);
            }
          }
        }
      }
    }

  public:
    // empty constructor
    ModelLoader() {}

    // constructor to initialize filename.
    ModelLoader(std::string filename)
    {
      this->filename = filename;
      readFile();
      saveInfo();

    }

    // don't need anything in the destructor for now.
    ~ModelLoader() {}

    // get the vertices from the file.
    std::vector<Vertex> getVertices()
    {
      return verts;
    }

    // get the triangles vertices from the file.
    std::vector<Vertex> getTriangles()
    {
      return triangles;
    }

    Vertex operator [] (const int index)
    {
      return triangles[index];
    }

};





#endif