#ifndef VERTEX_H
#define VERTEX_H

#include <vector>
using namespace std;

class Vertex
{
public:
    float illumXY, illumXZ, illumYZ;
    float norm[3];
    float x, y, z;
    int numFaces;
    vector<int> faces;

    Vertex(float x, float y, float z)
    {
        this->x = x; this->y = y; this->z = z;
        numFaces = 0;
        illumXY = illumXZ = illumYZ = 0;
    }
};

#endif // VERTEX_H
