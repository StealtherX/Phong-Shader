#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
using namespace std;
#include "Vertex.h"
#include "Face.h"

class Object
{
public:
    int numPoints;
    vector<Vertex> points;
    int numFaces;
    vector<Face> faces;

    Object() {}
    void setObject(int num);
    void setFaces(int num);
    void setPoint(Vertex vert);
    void insertFace(int num, int a, int b, int c);
    void findFaceNorms();
    void findVertexNorms();
    void makeFinalVertex();

};

#endif // OBJECT_H
