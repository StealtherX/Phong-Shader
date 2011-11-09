#include "edge.h"

Edge::Edge(float x0, float y0, float x1, float y1, float i0, float i1)
{
    if (y1 > y0)
    {
        yMax = y1;
        xLow = x0;
        yMin = yOrigMin = y0;
        xMax = x1;
        intenMin = i0;
        intenMax = i1;

        if (x0 == x1)
            oneSlope = 0;
        else
            oneSlope = (x1-x0) / (y1-y0);
    }
    else
    {
        yMax = y0;
        xLow = x1;
        yMin = yOrigMin = y1;
        xMax = x0;
        intenMin = i1;
        intenMax = i0;

        if ((x0 == x1) || (y1 == y0))
            oneSlope = 0;
        else
            oneSlope = (x0-x1) / (y0-y1);
    }


}


