#include "Object.h"
#include "Vertex.h"
#include <math.h>


void Object::setObject(int num)
{
    numPoints = num;

}

void Object::setFaces(int num)
{
    numFaces = num;
}

void Object::setPoint(Vertex vert)
{
    points.push_back(vert);
}

void Object::insertFace(int num, int a, int b, int c)
{
    points[a - 1].faces.push_back(num);
    points[a - 1].numFaces++;
    points[b - 1].faces.push_back(num);
    points[b - 1].numFaces++;
    points[c - 1].faces.push_back(num);
    points[c - 1].numFaces++;

    faces.push_back(Face(a, b, c));
}

void Object::findFaceNorms()
{
    float tempX, tempY, tempZ, mag;
    float v1[3], v2[3];


    for (int i = 0; i < numFaces;i++)
    {
        v1[0] = points[faces[i].vertices[1] - 1].x - points[faces[i].vertices[0] - 1].x;
        v1[1] = points[faces[i].vertices[1] - 1].y - points[faces[i].vertices[0] - 1].y;
        v1[2] = points[faces[i].vertices[1] - 1].z - points[faces[i].vertices[0] - 1].z;
        v2[0] = points[faces[i].vertices[2] - 1].x - points[faces[i].vertices[0] - 1].x;
        v2[1] = points[faces[i].vertices[2] - 1].y - points[faces[i].vertices[0] - 1].y;
        v2[2] = points[faces[i].vertices[2] - 1].z - points[faces[i].vertices[0] - 1].z;

        tempX = v1[1] * v2[2] - v1[2] * v2[1];
        tempY = v1[2] * v2[0] - v1[0] * v2[2];
        tempZ = v1[0] * v2[1] - v1[1] * v2[0];

        mag = sqrt(tempX*tempX + tempY*tempY + tempZ*tempZ);

        faces[i].norm[0] = tempX / mag;
        faces[i].norm[1] = tempY / mag;
        faces[i].norm[2] = tempZ / mag;
    }
}

void Object::findVertexNorms()
{
    float x = 0, y = 0, z = 0;
    float mag;

    for (int i = 0; i < numPoints; i++)
    {
        for (int j = 0; j < points[i].numFaces; j++)
        {
            x += faces[points[i].faces[j]].norm[0];
            y += faces[points[i].faces[j]].norm[1];
            z += faces[points[i].faces[j]].norm[2];
        }
        points[i].norm[0] = x / points[i].numFaces;
        points[i].norm[1] = y / points[i].numFaces;
        points[i].norm[2] = z / points[i].numFaces;

        mag = sqrt(x*x + y*y + z*z);

        points[i].norm[0] = x / mag;
        points[i].norm[1] = y / mag;
        points[i].norm[2] = z / mag;


        x = y = z = 0;
    }
}

void Object::makeFinalVertex()
{
    for (int i = 0; i < numFaces; i++)
    {
        faces[i].finalPoints.clear();
        for (int j = 0; j < 3; j++)
        {
            faces[i].finalPoints.push_back(points[faces[i].vertices[j] - 1 ]);
        }
        faces[i].findMin();
    }
}
