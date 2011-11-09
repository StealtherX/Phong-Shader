#ifndef EDGE_H
#define EDGE_H

class Edge
{
public:
    float yMax;
    float yMin;
    float xLow;
    float oneSlope;
    float xMax;
    float intenMin;
    float intenMax;
    float yOrigMin;

    Edge();
    Edge(float x0, float y0, float x1, float y1, float i0, float i1);

};

#endif // EDGE_H
