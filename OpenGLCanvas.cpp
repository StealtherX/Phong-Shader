#include "OpenGLCanvas.h"


#include <QColor>
#include <QMouseEvent>
#include <QSize>
#include <stdlib.h>
#include "Face.h"
#include <math.h>
#include <algorithm>
using namespace std;
#include "Vertex.h"
#include "edge.h"




OpenGLCanvas::OpenGLCanvas(QWidget* pQW_Parent)
   : QGLWidget(pQW_Parent), _pucBuffer(NULL)
{
    halfMask[0][0] = 8;
    halfMask[0][1] = 3;
    halfMask[0][2] = 7;
    halfMask[1][0] = 5;
    halfMask[1][1] = 1;
    halfMask[1][2] = 2;
    halfMask[2][0] = 4;
    halfMask[2][1] = 9;
    halfMask[2][2] = 6;


}

OpenGLCanvas::~OpenGLCanvas()
{
   delete[] _pucBuffer;
}

QSize OpenGLCanvas::minimumSizeHint() const
{
   return QSize(100, 100);
}

void OpenGLCanvas::SetPixelPositionX(int iX)
{
   _SetPixelPosition(QPoint(iX, _QPt_CurrentPosition.y()));
}

void OpenGLCanvas::SetPixelPositionY(int iY)
{
   _SetPixelPosition(QPoint(_QPt_CurrentPosition.x(), iY));
}

void OpenGLCanvas::refreshDraw()
{
    update();
}

void OpenGLCanvas::blackout()
{
       if (_pucBuffer != 0x0)
           for(int i=0; i<bufferSize; ++i) _pucBuffer[i]=0;
}


void OpenGLCanvas::initializeGL()
{
   glClearColor(0.f, 0.f, 0.f, 0.f);
   glShadeModel(GL_FLAT);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void OpenGLCanvas::resizeGL(int iWidth, int iHeight)
{
#ifndef GL_VERSION_1_4
   // Projection settings for GL_Version<1.4
   glViewport(0, 0, iWidth, iHeight);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0., iWidth, 0., iHeight, -1., 1.);
   glMatrixMode(GL_MODELVIEW);
#endif // GL_VERSION_1_4

   // Allocate an new buffer
   delete[] _pucBuffer;

   halfWidth = iWidth / 2;
   halfHeight = iHeight / 2;

   const int kiBufferSize=width()*height()*4;
   bufferSize = kiBufferSize;
   _pucBuffer=new unsigned char[kiBufferSize];
   for(int i=0; i<kiBufferSize; ++i) _pucBuffer[i]=0;

   emit SizeChanged(QSize(iWidth, iHeight));
}

void OpenGLCanvas::paintGL()
{
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

#ifndef GL_VERSION_1_4
   glLoadIdentity();
   glRasterPos2i(0, 0);
#else // GL_VERSION_1_4
   glWindowPos2i(0, 0);
#endif // GL_VERSION_1_4


   glDrawPixels(width(), height(), GL_RGBA, GL_UNSIGNED_BYTE, _pucBuffer);
   glFlush(); // Need This??
}

void OpenGLCanvas::mousePressEvent(QMouseEvent* pQME_Event)
{
   emit MousePositionChanged(pQME_Event->pos());
}

void OpenGLCanvas::mouseMoveEvent(QMouseEvent* pQME_Event)
{
   emit MousePositionChanged(pQME_Event->pos());
}

void OpenGLCanvas::_MakePixel(const QPoint& QPt_Position, const QColor& QC_Color)
{
   const int kiX=QPt_Position.x();
   const int kiY=QPt_Position.y();
   const int kiWidth=width();
   const int kiHeight=height();

   if(kiX<0 || kiY<0 || kiX>=kiWidth || kiY>=kiHeight) return;

   _pucBuffer[kiX*4+kiY*kiWidth*4]=static_cast<unsigned char>(QC_Color.value());
   _pucBuffer[kiX*4+1+kiY*kiWidth*4]=static_cast<unsigned char>(QC_Color.value());
   _pucBuffer[kiX*4+2+kiY*kiWidth*4]=static_cast<unsigned char>(QC_Color.value());
   _pucBuffer[kiX*4+3+kiY*kiWidth*4]=static_cast<unsigned char>(QC_Color.value());
}

void OpenGLCanvas::_MakePixel(const QPoint& QPt_Position, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
   const int kiX=QPt_Position.x();
   const int kiY=QPt_Position.y();
   const int kiWidth=width();
   const int kiHeight=height();

   if(kiX<0 || kiY<0 || kiX>=kiWidth || kiY>=kiHeight) return;

   _pucBuffer[kiX*4+kiY*kiWidth*4]=static_cast<unsigned char>(red);
   _pucBuffer[kiX*4+1+kiY*kiWidth*4]=static_cast<unsigned char>(green);
   _pucBuffer[kiX*4+2+kiY*kiWidth*4]=static_cast<unsigned char>(blue);
   _pucBuffer[kiX*4+3+kiY*kiWidth*4]=static_cast<unsigned char>(alpha);
}

void OpenGLCanvas::_SetPixelPosition(const QPoint& QPt_NewPosition)
{
   if(QPt_NewPosition==_QPt_CurrentPosition) return;

   _MakePixel(_QPt_CurrentPosition, Qt::black);
   _QPt_CurrentPosition=QPt_NewPosition;
   _MakePixel(_QPt_CurrentPosition, Qt::white);
   update(); // updateGL()?
}



inline int round(const float a) {return int (a + 0.5);}

//Check for special line cases, then draws line appropriately
void OpenGLCanvas::lineBres(int x0, int y0, int xEnd, int yEnd)
{
    if(x0 == xEnd)
    {
        if (y0 > yEnd)
            swap (y0, yEnd);

        for (int i = y0; i <= yEnd; i++)
            _MakePixel(QPoint(x0, i), Qt::white);
    }
    else if(y0 == yEnd)
    {
        if (x0 > xEnd)
            swap(x0, xEnd);

        for (int i = x0; i <= xEnd; i++)
            _MakePixel(QPoint(i, y0), Qt::white);
    }
    else
        lineBresDraw(x0, y0, xEnd, yEnd);
}


/* Bresenham for any slope*/
void OpenGLCanvas::lineBresDraw(int x0, int y0, int xEnd, int yEnd)
{
    /*Find if m > 1 or < 1. Swap if needed.*/
    bool steep = fabs(yEnd - y0) > fabs(xEnd - x0);
    if (steep)
    {
        swap(x0, y0);
        swap(xEnd, yEnd);
    }
    if (x0 > xEnd)
    {
        swap(x0, xEnd);
        swap(y0, yEnd);
    }

    /*Setup Bresenham values */
    int dx = fabs(xEnd - x0), dy = fabs(yEnd - y0);
    int p = 2 * dy - dx;
    int twoDy = 2 * dy, twoDyMinusDx = 2 * (dy - dx);
    int yDirection;
    int x = x0, y = y0;

    /*Is it positive slope or negative slope?*/
    if(y0 < yEnd)
        yDirection = 1;
    else
        yDirection = -1;


    if (steep)
        _MakePixel(QPoint(y, x), Qt::white);
    else
        _MakePixel(QPoint(x, y), Qt::white);

    /*Draw points on the line till endpoint*/
    while (x < xEnd)
    {
        x++;
        if (p < 0)
            p += twoDy;
        else
        {
            y += yDirection;
            p += twoDyMinusDx;
        }

        if (steep)
            _MakePixel(QPoint(y, x), Qt::white);
        else
            _MakePixel(QPoint(x, y), Qt::white);
    }
}

void OpenGLCanvas::phongLighting(Object *obj, int fromLoc)
{
        float lightVector[3];
        float viewVector[3];
        float reflection[3];
        float x, y, z, mag;

        for (int i = 0; i < obj->numPoints; i++)
        {
            x = lightSource[0] - obj->points[i].x;
            y = lightSource[1] - obj->points[i].y;
            z = lightSource[2] - obj->points[i].z;
            mag = sqrt(x*x + y*y + z*z);

            lightVector[0] = x / mag;
            lightVector[1] = y / mag;
            lightVector[2] = z / mag;

            from[0] = obj->points[i].x;
            from[1] = obj->points[i].y;
            from[2] = obj->points[i].z;
            from[fromLoc] = 0;

            x = from[0] - obj->points[i].x;
            y = from[1] - obj->points[i].y;
            z = from[2] - obj->points[i].z;
            mag = sqrt(x*x + y*y + z*z);

            viewVector[0] = x / mag;
            viewVector[1] = y / mag;
            viewVector[2] = z / mag;

            reflection[0] = (2 * dotProduct(lightVector, obj->points[i].norm) * obj->points[i].norm[0]) - lightVector[0];
            reflection[1] = (2 * dotProduct(lightVector, obj->points[i].norm) * obj->points[i].norm[1]) - lightVector[1];
            reflection[2] = (2 * dotProduct(lightVector, obj->points[i].norm) * obj->points[i].norm[2]) - lightVector[2];

            if (fromLoc == 0)
                obj->points[i].illumYZ = ambCoef * intenAmbient + (intenLight) *
                                   (diffCoef * dotProduct(lightVector, obj->points[i].norm) +
                                    specCoef * pow(dotProduct(reflection, viewVector), phongConst));
            else if (fromLoc == 1)
                obj->points[i].illumXZ = ambCoef * intenAmbient + (intenLight) *
                                   (diffCoef * dotProduct(lightVector, obj->points[i].norm) +
                                    specCoef * pow(dotProduct(reflection, viewVector), phongConst));
            else
                obj->points[i].illumXY = ambCoef * intenAmbient + (intenLight) *
                                   (diffCoef * dotProduct(lightVector, obj->points[i].norm) +
                                    specCoef * pow(dotProduct(reflection, viewVector), phongConst));
        }


}

void OpenGLCanvas::normIllum(Object**obj, int numObj)
{
    float max = 0;
    float min = 9000;

    for (int i = 0; i < numObj; i++)
    {
        for (int j = 0; j < obj[i]->numPoints; j++)
        {
            if (obj[i]->points[j].illumXY > max)
                max = obj[i]->points[j].illumXY;
            if (obj[i]->points[j].illumXY < min)
                min = obj[i]->points[j].illumXY;
            if (obj[i]->points[j].illumXZ > max)
                max = obj[i]->points[j].illumXZ;
            if (obj[i]->points[j].illumXZ < min)
                min = obj[i]->points[j].illumXZ;
            if (obj[i]->points[j].illumYZ > max)
                max = obj[i]->points[j].illumYZ;
            if (obj[i]->points[j].illumYZ < min)
                min = obj[i]->points[j].illumYZ;
        }
    }

    if(min < 0)
    {
        for (int i = 0; i < numObj; i++)
        {
            for (int j = 0; j < obj[i]->numPoints; j++)
            {
              obj[i]->points[j].illumXY += -min;
              obj[i]->points[j].illumXZ += -min;
              obj[i]->points[j].illumYZ += -min;
            }
        }
        max += -min;
     }


    if (max > 1)
    {
    for (int i = 0; i < numObj; i++)
    {
        for (int j = 0; j < obj[i]->numPoints; j++)
        {
            obj[i]->points[j].illumXY /= max;
            obj[i]->points[j].illumXZ /= max;
            obj[i]->points[j].illumYZ /= max;
        }
    }
}
}

float OpenGLCanvas::dotProduct(float *a, float *b)
{
    float ans = 0;

    ans = a[0]*b[0] + a[1]*b[1] + a[2]*b[2];

    return ans;
}

struct faceXY{
    bool operator() (Face a, Face b)
    {
        if (a.minZ > b.minZ)
            return true;
        else if (a.minZ == b.minZ)
        {
            if(a.maxZ > b.maxZ)
                return true;
            else
                return false;
        }
        else
            return false;
    }
} sortingXY;

struct faceXZ{
    bool operator() (Face a, Face b)
    {
        if (a.minY > b.minY)
            return true;
        else if (a.minY == b.minY)
        {
            if(a.maxY > b.maxY)
                return true;
            else
                return false;
        }
        else
            return false;
    }
} sortingXZ;

struct faceYZ{
    bool operator() (Face a, Face b)
    {
        if (a.minX > b.minX)
            return true;
        else if (a.minX == b.minX)
        {
            if(a.maxX > b.maxX)
                return true;
            else
                return false;
        }
        else
            return false;
    }
} sortingYZ;

void OpenGLCanvas::paintAlgXY(Object**obj, int numObj)
{
    vector<Face> temp;

    for (int i = 0; i < numObj; i++)
        for(int j = 0; j < obj[i]->numFaces; j++)
            temp.push_back(obj[i]->faces[j]);

    sort(temp.begin(), temp.end(), sortingXY);
    normalizeFaces(&temp);

    for (int i = 0; i < (int)temp.size(); i++)
    {
        gouShadingXY(temp[i]);
    }

}

void OpenGLCanvas::paintAlgYZ(Object**obj, int numObj)
{
    vector<Face> temp;

    for (int i = 0; i < numObj; i++)
        for(int j = 0; j < obj[i]->numFaces; j++)
            temp.push_back(obj[i]->faces[j]);

    sort(temp.begin(), temp.end(), sortingYZ);
    normalizeFaces(&temp);

    for (int i = 0; i < (int)temp.size(); i++)
    {
        gouShadingYZ(temp[i]);
    }

}

void OpenGLCanvas::paintAlgXZ(Object**obj, int numObj)
{
    vector<Face> temp;

    for (int i = 0; i < numObj; i++)
        for(int j = 0; j < obj[i]->numFaces; j++)
            temp.push_back(obj[i]->faces[j]);

    sort(temp.begin(), temp.end(), sortingXZ);
    normalizeFaces(&temp);

    for (int i = 0; i < (int)temp.size(); i++)
    {
        gouShadingXZ(temp[i]);
    }

}

void OpenGLCanvas::gouShadingXY(Face a)
{
    Edge *newEdge;
    int yMin = 90000, yMax = 0, yScan;
    bool insert = 0;
    int fillLeft, fillRight;
    int edgeNum = 3;
    float illumLeft, illumRight, illum;
    int colorLeft, colorRight, color;
    vector<Edge> edgeTable;
    vector<Edge> activeEdgeTable;

    activeEdgeTable.clear();

    for (int i = 0; i < 3; i++)
    {
        a.finalPoints[i].x *= halfWidth;
        a.finalPoints[i].y *= halfHeight;
    }



    for (int i = 0; i < 2; i++)
    {


      if (round(a.finalPoints[i].y) == round(a.finalPoints[i+1].y))
      {
        edgeNum--;
        continue;
      }
      else
      {
          newEdge = new Edge(round(a.finalPoints[i].x), round(a.finalPoints[i].y), round(a.finalPoints[i+1].x), round(a.finalPoints[i+1].y), a.finalPoints[i].illumXY, a.finalPoints[i+1].illumXY);

          if (newEdge->yMin < yMin)
              yMin = newEdge->yMin;

          if (newEdge->yMax > yMax)
              yMax = newEdge->yMax;

          edgeTable.push_back(*newEdge);

      }
    }
    if (round(a.finalPoints[2].y) != round(a.finalPoints[0].y))
    {
        newEdge = new Edge(round(a.finalPoints[2].x), round(a.finalPoints[2].y), round(a.finalPoints[0].x), round(a.finalPoints[0].y), a.finalPoints[2].illumXY, a.finalPoints[0].illumXY);
        edgeTable.push_back(*newEdge);
    }
    else
    {
        edgeNum--;
    }
    yScan = yMin;


    while (!(edgeTable.empty() && activeEdgeTable.empty()) && yScan != yMax)
    {
      for(int i = 0; i < edgeNum; i++)
      {

          if (edgeTable[i].yMin == yScan)
          {

              if (activeEdgeTable.empty())
              {
                activeEdgeTable.push_back(edgeTable[i]);
              }
              else
              {
                   insert = false;
                  for (unsigned int j = 0; j < activeEdgeTable.size(); j++)
                  {
                      if (activeEdgeTable[j].xLow > edgeTable[i].xLow)
                      {
                          activeEdgeTable.insert((activeEdgeTable.begin() + j), edgeTable[i]);
                          insert = true;
                          break;
                      }
                      if (activeEdgeTable[j].xLow == edgeTable[i].xLow)
                      {
                          if (activeEdgeTable[j].oneSlope < edgeTable[i].oneSlope)
                          {
                              activeEdgeTable.insert((activeEdgeTable.begin() + j + 1), edgeTable[i]);
                              insert =  true;
                              break;
                          }
                          else
                          {
                              activeEdgeTable.insert((activeEdgeTable.begin() + j), edgeTable[i]);
                              insert = true;
                              break;
                          }

                      }
                  }

                  if (!insert)
                  {
                      activeEdgeTable.push_back(edgeTable[i]);
                      insert = true;
                  }
              }

              edgeTable.erase(edgeTable.begin() + i);
              edgeNum--;
              i--;

              if (edgeNum == 1)
                  i = -1;
          }

       }

       for (unsigned int i = 0; i < activeEdgeTable.size(); i++)
      {
           if (activeEdgeTable[i].yMax == yScan)
           {
               activeEdgeTable.erase(activeEdgeTable.begin() + i);
               i--;
           }
       }

       if (!activeEdgeTable.empty())
       {
           if(!halfToning)
           {
               for (unsigned int i = 0; i < activeEdgeTable.size() - 1; i+=2)
               {



                fillLeft = round(activeEdgeTable[i].xLow);
                fillRight = round(activeEdgeTable[i+1].xLow);
                illumLeft = (((float)yScan - activeEdgeTable[i].yOrigMin)/(activeEdgeTable[i].yMax - activeEdgeTable[i].yOrigMin))*activeEdgeTable[i].intenMax +
                        ((activeEdgeTable[i].yMax - (float)yScan)/(activeEdgeTable[i].yMax - activeEdgeTable[i].yOrigMin))*activeEdgeTable[i].intenMin;
                illumRight = (((float)yScan - activeEdgeTable[i+1].yOrigMin)/(activeEdgeTable[i+1].yMax - activeEdgeTable[i+1].yOrigMin))*activeEdgeTable[i+1].intenMax +
                         ((activeEdgeTable[i+1].yMax - (float)yScan)/(activeEdgeTable[i+1].yMax - activeEdgeTable[i+1].yOrigMin))*activeEdgeTable[i+1].intenMin;

                colorLeft = round(illumLeft * 255);
                colorRight = round(illumRight * 255);

                _MakePixel(QPoint(fillLeft, yScan), colorLeft, colorLeft, colorLeft, 255);
                _MakePixel(QPoint(fillRight, yScan), colorRight, colorRight, colorRight, 255);


                for (int j = fillLeft+1; j < fillRight; j++)
                {
                   illum = (fillRight - (float)j)/(fillRight - fillLeft) * illumLeft;
                          illum += (((float)j - fillLeft)/(fillRight - fillLeft)) * illumRight;
                   color = round(illum * 255);

                   _MakePixel(QPoint(j,yScan), color, color, color, 255);
                }
                }

               for (int s = 0; s < 3; s++)
               {
                   color = round(a.finalPoints[s].illumXY * 255);
                    _MakePixel(QPoint(round(a.finalPoints[s].x), round(a.finalPoints[s].y)), color, color, color, 255);
               }
            }
           else
           {
               for (unsigned int i = 0; i < activeEdgeTable.size() - 1; i+=2)
               {
                   fillLeft = round(activeEdgeTable[i].xLow);
                   fillRight = round(activeEdgeTable[i+1].xLow);
                   illumLeft = (((float)yScan - activeEdgeTable[i].yOrigMin)/(activeEdgeTable[i].yMax - activeEdgeTable[i].yOrigMin))*activeEdgeTable[i].intenMax +
                           ((activeEdgeTable[i].yMax - (float)yScan)/(activeEdgeTable[i].yMax - activeEdgeTable[i].yOrigMin))*activeEdgeTable[i].intenMin;
                   illumRight = (((float)yScan - activeEdgeTable[i+1].yOrigMin)/(activeEdgeTable[i+1].yMax - activeEdgeTable[i+1].yOrigMin))*activeEdgeTable[i+1].intenMax +
                            ((activeEdgeTable[i+1].yMax - (float)yScan)/(activeEdgeTable[i+1].yMax - activeEdgeTable[i+1].yOrigMin))*activeEdgeTable[i+1].intenMin;

                   colorLeft = round(illumLeft * 10);
                   colorRight = round(illumRight * 10);

                   for (int y = 0; y < 3; y++)
                   {
                       for (int x = 0; x < 3; x++)
                       {
                            if(halfMask[y][x] <= colorLeft)
                                _MakePixel(QPoint(fillLeft * 3 + x, yScan * 3 + y), Qt::white);
                            else
                                _MakePixel(QPoint(fillLeft * 3 + x, yScan * 3 + y), Qt::black);
                       }
                   }
                   for (int y = 0; y < 3; y++)
                   {
                       for (int x = 0; x < 3; x++)
                       {
                            if(halfMask[y][x] <= colorRight)
                                _MakePixel(QPoint(fillRight * 3 + x, yScan * 3 + y), Qt::white);
                            else
                                _MakePixel(QPoint(fillRight * 3 + x, yScan * 3 + y), Qt::black);
                       }
                   }

                   for (int j = fillLeft; j <= fillRight; j++)
                   {
                      illum = (fillRight - (float)j)/(fillRight - fillLeft) * illumLeft;
                             illum += (((float)j - fillLeft)/(fillRight - fillLeft)) * illumRight;
                      color = round(illum * 10);

                      for (int y = 0; y < 3; y++)
                      {
                          for (int x = 0; x < 3; x++)
                          {
                               if(halfMask[y][x] <= color)
                                   _MakePixel(QPoint(j * 3 + x, yScan * 3 + y), Qt::white);
                               else
                                   _MakePixel(QPoint(j * 3 + x, yScan * 3 + y), Qt::black);
                          }
                      }
                   }
               }

               for (int s = 0; s < 3; s++)
               {
               color = round(a.finalPoints[s].illumXY * 10);

               for (int y = 0; y < 3; y++)
               {
                   for (int x = 0; x < 3; x++)
                   {
                        if(halfMask[y][x] <= color)
                            _MakePixel(QPoint(round(a.finalPoints[s].x) * 3 + x, round(a.finalPoints[s].y) * 3 + y), Qt::white);
                        else
                            _MakePixel(QPoint(round(a.finalPoints[s].x) * 3 + x, round(a.finalPoints[s].y) * 3 + y), Qt::black);
                   }
               }
           }
           }
   }

       yScan++;

       for (unsigned int i = 0; i < activeEdgeTable.size(); i++)
           activeEdgeTable[i].xLow = (activeEdgeTable[i].xLow + activeEdgeTable[i].oneSlope);

    }

    
    if (!halfToning)
    _MakePixel(QPoint(round(lightSourceDraw[0] * halfWidth), round(lightSourceDraw[1] * halfHeight)), 255, 255, 0, 255);
    else
            _MakePixel(QPoint(round(lightSourceDraw[0] * halfWidth) * 3, round(lightSourceDraw[1] * halfHeight) * 3), 255, 255, 0, 255);

}

void OpenGLCanvas::gouShadingXZ(Face a)
{
    Edge *newEdge;
    int yMin = 90000, yMax = 0, yScan;
    bool insert = 0;
    int fillLeft, fillRight;
    int edgeNum = 3;
    float illumLeft, illumRight, illum;
    int colorLeft, colorRight, color;
    vector<Edge> edgeTable;
    vector<Edge> activeEdgeTable;

    activeEdgeTable.clear();

    for (int i = 0; i < 3; i++)
    {
        a.finalPoints[i].x *= halfWidth;
        a.finalPoints[i].z *= halfHeight;
    }

    for (int i = 0; i < 2; i++)
    {

      if (round(a.finalPoints[i].z) == round(a.finalPoints[i+1].z))
      {
        edgeNum--;
        continue;
      }
      else
      {
          newEdge = new Edge(round(a.finalPoints[i].x), round(a.finalPoints[i].z), round(a.finalPoints[i+1].x), round(a.finalPoints[i+1].z), a.finalPoints[i].illumXZ, a.finalPoints[i+1].illumXZ);

          if (newEdge->yMin < yMin)
              yMin = newEdge->yMin;

          if (newEdge->yMax > yMax)
              yMax = newEdge->yMax;

          edgeTable.push_back(*newEdge);

      }
    }
    if (round(a.finalPoints[2].z) != round(a.finalPoints[0].z))
    {
        newEdge = new Edge(round(a.finalPoints[2].x), round(a.finalPoints[2].z), round(a.finalPoints[0].x), round(a.finalPoints[0].z), a.finalPoints[2].illumXZ, a.finalPoints[0].illumXZ);
        edgeTable.push_back(*newEdge);
    }
    else
    {
        edgeNum--;
    }
    yScan = yMin;


    while (!(edgeTable.empty() && activeEdgeTable.empty()) && yScan != yMax)
    {
      for(int i = 0; i < edgeNum; i++)
      {

          if (edgeTable[i].yMin == yScan)
          {

              if (activeEdgeTable.empty())
              {
                activeEdgeTable.push_back(edgeTable[i]);
              }
              else
              {
                   insert = false;
                  for (unsigned int j = 0; j < activeEdgeTable.size(); j++)
                  {
                      if (activeEdgeTable[j].xLow > edgeTable[i].xLow)
                      {
                          activeEdgeTable.insert((activeEdgeTable.begin() + j), edgeTable[i]);
                          insert = true;
                          break;
                      }
                      if (activeEdgeTable[j].xLow == edgeTable[i].xLow)
                      {
                          if (activeEdgeTable[j].oneSlope < edgeTable[i].oneSlope)
                          {
                              activeEdgeTable.insert((activeEdgeTable.begin() + j + 1), edgeTable[i]);
                              insert =  true;
                              break;
                          }
                          else
                          {
                              activeEdgeTable.insert((activeEdgeTable.begin() + j), edgeTable[i]);
                              insert = true;
                              break;
                          }

                      }
                  }

                  if (!insert)
                  {
                      activeEdgeTable.push_back(edgeTable[i]);
                      insert = true;
                  }
              }

              edgeTable.erase(edgeTable.begin() + i);
              edgeNum--;
              i--;

              if (edgeNum == 1)
                  i = -1;
          }

       }

       for (unsigned int i = 0; i < activeEdgeTable.size(); i++)
      {
           if (activeEdgeTable[i].yMax == yScan)
           {
               activeEdgeTable.erase(activeEdgeTable.begin() + i);
               i--;
           }
       }

       if (!activeEdgeTable.empty())
       {
           if (!halfToning)
           {
                for (unsigned int i = 0; i < activeEdgeTable.size() - 1; i+=2)
                {

                 fillLeft = round(activeEdgeTable[i].xLow);
                 fillRight = round(activeEdgeTable[i+1].xLow);
                 illumLeft = (((float)yScan - activeEdgeTable[i].yOrigMin)/(activeEdgeTable[i].yMax - activeEdgeTable[i].yOrigMin))*activeEdgeTable[i].intenMax +
                        ((activeEdgeTable[i].yMax - (float)yScan)/(activeEdgeTable[i].yMax - activeEdgeTable[i].yOrigMin))*activeEdgeTable[i].intenMin;
                 illumRight = (((float)yScan - activeEdgeTable[i+1].yOrigMin)/(activeEdgeTable[i+1].yMax - activeEdgeTable[i+1].yOrigMin))*activeEdgeTable[i+1].intenMax +
                         ((activeEdgeTable[i+1].yMax - (float)yScan)/(activeEdgeTable[i+1].yMax - activeEdgeTable[i+1].yOrigMin))*activeEdgeTable[i+1].intenMin;

                 colorLeft = round(illumLeft * 255);
                 colorRight = round(illumRight * 255);

                 _MakePixel(QPoint(fillLeft, yScan + halfHeight), colorLeft, colorLeft, colorLeft, 255);
                 _MakePixel(QPoint(fillRight, yScan + halfHeight), colorRight, colorRight, colorRight, 255);


                 for (int j = fillLeft+1; j <= fillRight; j++)
                 {
                    illum = (fillRight - (float)j)/(fillRight - fillLeft) * illumLeft;
                       illum += (((float)j - fillLeft)/(fillRight - fillLeft)) * illumRight;
                    color = round(illum * 255);

                    _MakePixel(QPoint(j,yScan + halfHeight), color, color, color, 255);
                 }
                  }
                for (int s = 0; s < 3; s++)
                {
                    color = round(a.finalPoints[s].illumXZ * 255);
                     _MakePixel(QPoint(round(a.finalPoints[s].x), round(a.finalPoints[s].z) + halfHeight), color, color, color, 255);
                }
            }
           else
           {
               for (unsigned int i = 0; i < activeEdgeTable.size() - 1; i+=2)
               {
                   fillLeft = round(activeEdgeTable[i].xLow);
                   fillRight = round(activeEdgeTable[i+1].xLow);
                   illumLeft = (((float)yScan - activeEdgeTable[i].yOrigMin)/(activeEdgeTable[i].yMax - activeEdgeTable[i].yOrigMin))*activeEdgeTable[i].intenMax +
                           ((activeEdgeTable[i].yMax - (float)yScan)/(activeEdgeTable[i].yMax - activeEdgeTable[i].yOrigMin))*activeEdgeTable[i].intenMin;
                   illumRight = (((float)yScan - activeEdgeTable[i+1].yOrigMin)/(activeEdgeTable[i+1].yMax - activeEdgeTable[i+1].yOrigMin))*activeEdgeTable[i+1].intenMax +
                            ((activeEdgeTable[i+1].yMax - (float)yScan)/(activeEdgeTable[i+1].yMax - activeEdgeTable[i+1].yOrigMin))*activeEdgeTable[i+1].intenMin;

                   colorLeft = round(illumLeft * 10);
                   colorRight = round(illumRight * 10);

                   for (int y = 0; y < 3; y++)
                   {
                       for (int x = 0; x < 3; x++)
                       {
                            if(halfMask[y][x] <= colorLeft)
                                _MakePixel(QPoint(fillLeft * 3 + x, yScan * 3 + y + halfHeight), Qt::white);
                            else
                                _MakePixel(QPoint(fillLeft * 3 + x, yScan * 3 + y + halfHeight), Qt::black);
                       }
                   }
                   for (int y = 0; y < 3; y++)
                   {
                       for (int x = 0; x < 3; x++)
                       {
                            if(halfMask[y][x] <= colorRight)
                                _MakePixel(QPoint(fillRight * 3 + x, yScan * 3 + y + halfHeight), Qt::white);
                            else
                                _MakePixel(QPoint(fillRight * 3 + x, yScan * 3 + y + halfHeight), Qt::black);
                       }
                   }

                   for (int j = fillLeft; j <= fillRight; j++)
                   {
                      illum = (fillRight - (float)j)/(fillRight - fillLeft) * illumLeft;
                             illum += (((float)j - fillLeft)/(fillRight - fillLeft)) * illumRight;
                      color = round(illum * 10);

                      for (int y = 0; y < 3; y++)
                      {
                          for (int x = 0; x < 3; x++)
                          {
                               if(halfMask[y][x] <= color)
                                   _MakePixel(QPoint(j * 3 + x, yScan * 3 + y + halfHeight), Qt::white);
                               else
                                   _MakePixel(QPoint(j * 3 + x, yScan * 3 + y + halfHeight), Qt::black);
                          }
                      }
                   }
               }

               for (int s = 0; s < 3; s++)
               {
               color = round(a.finalPoints[s].illumXZ * 10);

               for (int y = 0; y < 3; y++)
               {
                   for (int x = 0; x < 3; x++)
                   {
                        if(halfMask[y][x] <= color)
                            _MakePixel(QPoint(round(a.finalPoints[s].x) * 3 + x, round(a.finalPoints[s].z) * 3 + y + halfHeight), Qt::white);
                        else
                            _MakePixel(QPoint(round(a.finalPoints[s].x) * 3 + x, round(a.finalPoints[s].z) * 3 + y + halfHeight), Qt::black);
                   }
               }
           }
           }
   }

       yScan++;

       for (unsigned int i = 0; i < activeEdgeTable.size(); i++)
           activeEdgeTable[i].xLow = (activeEdgeTable[i].xLow + activeEdgeTable[i].oneSlope);

    }

    if (!halfToning)
    _MakePixel(QPoint(round(lightSourceDraw[0] * halfWidth), round(lightSourceDraw[2] * halfHeight) + halfHeight), 255, 255, 0, 255);
    else
            _MakePixel(QPoint(round(lightSourceDraw[0] * halfWidth) * 3, round(lightSourceDraw[2] * halfHeight) * 3 + halfHeight), 255, 255, 0, 255);

}

void OpenGLCanvas::normalizeFaces(vector<Face> *list)
{
    float max = 0;

    for (int i = 0; i < (int)list->size(); i++)
    {
        if (max < (*list)[i].maxX)
            max = (*list)[i].maxX;
        if (max < (*list)[i].maxY)
            max = (*list)[i].maxY;
        if (max < (*list)[i].maxZ)
            max = (*list)[i].maxZ;
    }

    max += 1;

    if (halfToning)
    {
        max *= 3;
    }
    for (int i = 0; i < (int)list->size(); i++)
    {
        for (int j = 0; j < 3; j++)
        {
            (*list)[i].finalPoints[j].x = ((*list)[i].finalPoints[j].x / max);
            (*list)[i].finalPoints[j].y = ((*list)[i].finalPoints[j].y / max);
            (*list)[i].finalPoints[j].z = ((*list)[i].finalPoints[j].z / max);
        }

    }

    lightSourceDraw[0] = lightSource[0] / max;
    lightSourceDraw[1] = lightSource[1] / max;
    lightSourceDraw[2] = lightSource[2] / max;


}

void OpenGLCanvas::gouShadingYZ(Face a)
{
    Edge *newEdge;
    int yMin = 90000, yMax = 0, yScan;
    bool insert = 0;
    int fillLeft, fillRight;
    int edgeNum = 3;
    float illumLeft, illumRight, illum;
    int colorLeft, colorRight, color;
    vector<Edge> edgeTable;
    vector<Edge> activeEdgeTable;

    activeEdgeTable.clear();

    for (int i = 0; i < 3; i++)
    {
        a.finalPoints[i].y *= halfWidth;
        a.finalPoints[i].z *= halfHeight;
    }

    for (int i = 0; i < 2; i++)
    {


      if (round(a.finalPoints[i].z) == round(a.finalPoints[i+1].z))
      {
        edgeNum--;
        continue;
      }
      else
      {
          newEdge = new Edge(round(a.finalPoints[i].y), round(a.finalPoints[i].z), round(a.finalPoints[i+1].y), round(a.finalPoints[i+1].z), a.finalPoints[i].illumYZ, a.finalPoints[i+1].illumYZ);

          if (newEdge->yMin < yMin)
              yMin = newEdge->yMin;

          if (newEdge->yMax > yMax)
              yMax = newEdge->yMax;

          edgeTable.push_back(*newEdge);

      }
    }
    if (round(a.finalPoints[2].z) != round(a.finalPoints[0].z))
    {
        newEdge = new Edge(round(a.finalPoints[2].y), round(a.finalPoints[2].z), round(a.finalPoints[0].y), round(a.finalPoints[0].z), a.finalPoints[2].illumYZ, a.finalPoints[0].illumYZ);
        edgeTable.push_back(*newEdge);
    }
    else
    {
        edgeNum--;
    }
    yScan = yMin;


    while (!(edgeTable.empty() && activeEdgeTable.empty()) && yScan != yMax)
    {
      for(int i = 0; i < edgeNum; i++)
      {

          if (edgeTable[i].yMin == yScan)
          {

              if (activeEdgeTable.empty())
              {
                activeEdgeTable.push_back(edgeTable[i]);
              }
              else
              {
                   insert = false;
                  for (unsigned int j = 0; j < activeEdgeTable.size(); j++)
                  {
                      if (activeEdgeTable[j].xLow > edgeTable[i].xLow)
                      {
                          activeEdgeTable.insert((activeEdgeTable.begin() + j), edgeTable[i]);
                          insert = true;
                          break;
                      }
                      if (activeEdgeTable[j].xLow == edgeTable[i].xLow)
                      {
                          if (activeEdgeTable[j].oneSlope < edgeTable[i].oneSlope)
                          {
                              activeEdgeTable.insert((activeEdgeTable.begin() + j + 1), edgeTable[i]);
                              insert =  true;
                              break;
                          }
                          else
                          {
                              activeEdgeTable.insert((activeEdgeTable.begin() + j), edgeTable[i]);
                              insert = true;
                              break;
                          }

                      }
                  }

                  if (!insert)
                  {
                      activeEdgeTable.push_back(edgeTable[i]);
                      insert = true;
                  }
              }

              edgeTable.erase(edgeTable.begin() + i);
              edgeNum--;
              i--;

              if (edgeNum == 1)
                  i = -1;
          }

       }

       for (unsigned int i = 0; i < activeEdgeTable.size(); i++)
      {
           if (activeEdgeTable[i].yMax == yScan)
           {
               activeEdgeTable.erase(activeEdgeTable.begin() + i);
               i--;
           }
       }

       if (!activeEdgeTable.empty())
       {
          if (!halfToning)
           {
       for (unsigned int i = 0; i < activeEdgeTable.size() - 1; i+=2)
       {

            fillLeft = round(activeEdgeTable[i].xLow);
            fillRight = round(activeEdgeTable[i+1].xLow);
            illumLeft = (((float)yScan - activeEdgeTable[i].yOrigMin)/(activeEdgeTable[i].yMax - activeEdgeTable[i].yOrigMin))*activeEdgeTable[i].intenMax +
                        ((activeEdgeTable[i].yMax - (float)yScan)/(activeEdgeTable[i].yMax - activeEdgeTable[i].yOrigMin))*activeEdgeTable[i].intenMin;
            illumRight = (((float)yScan - activeEdgeTable[i+1].yOrigMin)/(activeEdgeTable[i+1].yMax - activeEdgeTable[i+1].yOrigMin))*activeEdgeTable[i+1].intenMax +
                         ((activeEdgeTable[i+1].yMax - (float)yScan)/(activeEdgeTable[i+1].yMax - activeEdgeTable[i+1].yOrigMin))*activeEdgeTable[i+1].intenMin;

            colorLeft = round(illumLeft * 255);
            colorRight = round(illumRight * 255);

            _MakePixel(QPoint(fillLeft + halfWidth, yScan + halfHeight), colorLeft, colorLeft, colorLeft, 255);
            _MakePixel(QPoint(fillRight + halfWidth, yScan + halfHeight), colorRight, colorRight, colorRight, 255);


            for (int j = fillLeft+1; j <= fillRight; j++)
            {
               illum = (fillRight - (float)j)/(fillRight - fillLeft) * illumLeft;
                       illum += (((float)j - fillLeft)/(fillRight - fillLeft)) * illumRight;
               color = round(illum * 255);

               _MakePixel(QPoint(j + halfWidth,yScan + halfHeight), color, color, color, 255);
            }
      }
       for (int s = 0; s < 3; s++)
       {
           color = round(a.finalPoints[s].illumYZ * 255);
            _MakePixel(QPoint(round(a.finalPoints[s].y) + halfWidth, round(a.finalPoints[s].z) + halfHeight), color, color, color, 255);
       }
            }
          else
          {
              for (unsigned int i = 0; i < activeEdgeTable.size() - 1; i+=2)
              {
                  fillLeft = round(activeEdgeTable[i].xLow);
                  fillRight = round(activeEdgeTable[i+1].xLow);
                  illumLeft = (((float)yScan - activeEdgeTable[i].yOrigMin)/(activeEdgeTable[i].yMax - activeEdgeTable[i].yOrigMin))*activeEdgeTable[i].intenMax +
                          ((activeEdgeTable[i].yMax - (float)yScan)/(activeEdgeTable[i].yMax - activeEdgeTable[i].yOrigMin))*activeEdgeTable[i].intenMin;
                  illumRight = (((float)yScan - activeEdgeTable[i+1].yOrigMin)/(activeEdgeTable[i+1].yMax - activeEdgeTable[i+1].yOrigMin))*activeEdgeTable[i+1].intenMax +
                           ((activeEdgeTable[i+1].yMax - (float)yScan)/(activeEdgeTable[i+1].yMax - activeEdgeTable[i+1].yOrigMin))*activeEdgeTable[i+1].intenMin;

                  colorLeft = round(illumLeft * 10);
                  colorRight = round(illumRight * 10);

                  for (int y = 0; y < 3; y++)
                  {
                      for (int x = 0; x < 3; x++)
                      {
                           if(halfMask[y][x] <= colorLeft)
                               _MakePixel(QPoint(fillLeft * 3 + x + halfWidth, yScan * 3 + y + halfHeight), Qt::white);
                           else
                               _MakePixel(QPoint(fillLeft * 3 + x + halfWidth, yScan * 3 + y + halfHeight), Qt::black);
                      }
                  }
                  for (int y = 0; y < 3; y++)
                  {
                      for (int x = 0; x < 3; x++)
                      {
                           if(halfMask[y][x] <= colorRight)
                               _MakePixel(QPoint(fillRight * 3 + x + halfWidth, yScan * 3 + y + halfHeight), Qt::white);
                           else
                               _MakePixel(QPoint(fillRight * 3 + x + halfWidth, yScan * 3 + y + halfHeight), Qt::black);
                      }
                  }

                  for (int j = fillLeft; j <= fillRight; j++)
                  {
                     illum = (fillRight - (float)j)/(fillRight - fillLeft) * illumLeft;
                            illum += (((float)j - fillLeft)/(fillRight - fillLeft)) * illumRight;
                     color = round(illum * 10);

                     for (int y = 0; y < 3; y++)
                     {
                         for (int x = 0; x < 3; x++)
                         {
                              if(halfMask[y][x] <= color)
                                  _MakePixel(QPoint(j * 3 + x + halfWidth, yScan * 3 + y + halfHeight), Qt::white);
                              else
                                  _MakePixel(QPoint(j * 3 + x + halfWidth, yScan * 3 + y + halfHeight), Qt::black);
                         }
                     }
                  }
              }

              for (int s = 0; s < 3; s++)
              {
              color = round(a.finalPoints[s].illumYZ * 10);

              for (int y = 0; y < 3; y++)
              {
                  for (int x = 0; x < 3; x++)
                  {
                       if(halfMask[y][x] <= color)
                           _MakePixel(QPoint(round(a.finalPoints[s].y) * 3 + x + halfWidth, round(a.finalPoints[s].z) * 3 + y + halfHeight), Qt::white);
                       else
                           _MakePixel(QPoint(round(a.finalPoints[s].y) * 3 + x + halfWidth, round(a.finalPoints[s].z) * 3 + y + halfHeight), Qt::black);
                  }
              }
          }
          }
   }

       yScan++;

       for (unsigned int i = 0; i < activeEdgeTable.size(); i++)
           activeEdgeTable[i].xLow = (activeEdgeTable[i].xLow + activeEdgeTable[i].oneSlope);

    }

    if (!halfToning)
    _MakePixel(QPoint(round(lightSourceDraw[1] * halfWidth) + halfWidth, round(lightSourceDraw[2] * halfHeight) + halfHeight), 255, 255, 0, 255);
    else
    _MakePixel(QPoint(round(lightSourceDraw[1] * halfWidth) * 3 + halfWidth, round(lightSourceDraw[2] * halfHeight) * 3 + halfHeight), 255, 255, 0, 255);

}
