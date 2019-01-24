#ifndef UTILITIES_FUNCTIONS_H
#define UTILITIES_FUNCTIONS_H

/***************************************************
 * Checks a 3 by 3 grid around one point.
 * Can have a maximum of 8 neighbors.
 **************************************************/
int countNeighbors(int gridY, int gridX, int grid[64][64])
{
    int neightbors = 0;
    int newPointY = 0;
    int newPointX = 0;
    for (int y = -1; y < 2; y++)
    {
        for (int x = -1; x < 2; x++)
        {
            newPointY = (gridY + y); // scaleFactor;
            if (newPointY > 0 && newPointY < 64)
            {
                newPointX = (gridX + x); // scaleFactor;
                if (newPointX > 0 && newPointX < 64)
                {
                    if (grid[newPointY][newPointX] == 1)
                    {
                        if (y == 0 && x == 0)
                        {} else {
                            neightbors++;
                        }
                    }
                }
            }
        }
    }
    return neightbors;
}

#endif