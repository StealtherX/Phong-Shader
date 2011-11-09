#ifndef FACE_H
#define FACE_H

#include <Vertex.h>
#include <vector>
using namespace std;

class Face
{
public:
    int vertices[3];
    float norm[3];
    vector<Vertex> finalPoints;
    float minX, minY, minZ;
    float maxX, maxY, maxZ;

    Face(int first, int second, int third)
    {
        vertices[0] = first; vertices[1] = second; vertices[2] = third;
    }
    void findMin()
    {
        minX = minY = minZ = 9000;
        maxX = maxY = maxZ = 0;

        for (int i = 0; i < 3; i++)
        {
            if (finalPoints[i].x < minX)
                minX = finalPoints[i].x;
            if (finalPoints[i].y < minY)
                minY = finalPoints[i].y;
            if (finalPoints[i].z < minZ)
                minZ = finalPoints[i].z;
            if (finalPoints[i].x > maxX)
                maxX = finalPoints[i].x;
            if (finalPoints[i].y > maxY)
                maxY = finalPoints[i].y;
            if (finalPoints[i].z > maxZ)
                maxZ = finalPoints[i].z;
        }
    }

    inline bool operator<(const Face &b) const
    {
        if (minZ > b.minZ)
            return true;
        else if (minZ == b.minZ)
        {
            if(maxZ > b.maxZ)
                return true;
            else
                return false;
        }
        else
            return false;
    }
};

#endif // FACE_H
