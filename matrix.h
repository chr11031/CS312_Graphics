#ifndef MATRIX_H
#define MATRIX_H

//this just fits better here than in defenitions.h, its going there anyways
double MatrixMultiply(const double r[], const double c[], int numRow)
{
    double product = 0;
    for (int i = 0; i < numRow - 1; i++)
    {
        product += c[i]*r[i];
    }
    return product;
}

class Matrix
{
  public:
    double grid[4][4];
    int numRow;
    int numColumn;
    
    //non-default constructor
    Matrix(double newGrid[][4], int numRow, int numColumn)
    {
        this->numRow = numRow;
        this->numColumn = numColumn;
        for (int i = 0; i < numRow-1; i++)
            for (int j = 0; j < numColumn-1; j++)
            {
                this->grid[i][j] = newGrid[i][j];
            }
    }
    //default constructor
    Matrix(){numColumn = 0; numRow = 0;}

    //EQUALS OPERATOR - BASIC
    void operator=(const Matrix &rhs)
    {
        for (int i = 0; i < rhs.numRow-1; i++)
            for (int j = 0; j < rhs.numColumn-1; j++)
            {
                this->grid[i][j] = rhs.grid[i][j];
            }
        this->numRow = rhs.numRow;
        this->numColumn = rhs.numColumn;
    }

    //*= OPERATOR
    void operator*=(Matrix &rhs)
    {
        double transferGrid[4][4];
        for (int i = 0; i < rhs.numRow-1; i++)
        {
            for (int j = 0; j < numColumn-1; j++)
            {
                double newCol[4];
                for (int k = 0; k < numRow-1; k++)
                {
                    newCol[k] = this->grid[k][j];
                }
                //this will loop from grid 0,0 to 0,1 to 0,2 and so on
                transferGrid[i][j] = MatrixMultiply(rhs.grid[i], newCol, 4);
            }
        }

        
        //copy the transfer grid over
        for (int i = 0; i < numRow-1; i++)
            for (int j = 0; j < numColumn-1; j++)
            {
                this->grid[i][j] = transferGrid[i][j];
            }
    }
};

#endif