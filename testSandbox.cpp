#include "graphicMatrix.h"
#include <iostream>

//using namespace std;

int main(){
    Matrix test = Matrix(4,4);

    std::cout << "The matrix is\n";

    for (int i = 0; i < 16; i++)
    {
        std::cout << test[i];
        if(i % 4 == 0)
        {
            cout << "\n";
        }
    }


    return 0;
}