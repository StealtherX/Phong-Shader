#ifndef __OpenGLCanvas_h__
#define __OpenGLCanvas_h__


// NOTE: GL_GLEXT_PROTOTYPES has to be defined before include QGLWidget on Linux boxes
#ifdef __linux__
#define GL_GLEXT_PROTOTYPES
#endif // __linux__
#include <QGLWidget>
#include "Face.h"
#include "Vertex.h"
#include "Object.h"
#include <QPoint>
#include "edge.h"
#include <vector>
using namespace std;


class QColor;

const double pi = 3.14159;
typedef float Matrix4x4 [4][4];


class OpenGLCanvas : public QGLWidget
{
   Q_OBJECT

   public:
      OpenGLCanvas(QWidget* pQW_Parent=NULL);
      ~OpenGLCanvas();

      QSize minimumSizeHint() const;
      void lineBres(int x0, int y0, int xEnd, int yEnd);
      void lineBresDraw(int x0, int y0, int xEnd, int yEnd);
      void blackout();
      void phongLighting(Object *obj, int fromLoc);
      void normIllum(Object **obj, int numObj);
      void paintAlgXY(Object **obj, int numObj);
      void paintAlgXZ(Object **obj, int numObj);
      void paintAlgYZ(Object**obj, int numObj);

      float intenLight;
      float intenAmbient;
      float ambCoef, diffCoef, specCoef;
      float lightSource[3], lightSourceDraw[3], lightSourceXZ[3], lightSourceYZ[3];
      int phongConst;
      float K;
      bool halfToning;


   public slots:
      void SetPixelPositionX(int iX);
      void SetPixelPositionY(int iY);
      void refreshDraw();





   signals:
      void SizeChanged(QSize QS_NewSize);
      void MousePositionChanged(QPoint QP_NewPosition);


   protected:
      void initializeGL();
      void paintGL();
      void resizeGL(int iWidth, int iHeight);
      void mousePressEvent(QMouseEvent* pQME_Event);
      void mouseMoveEvent(QMouseEvent* pQME_Event);
      float dotProduct(float *a, float *b);
      void gouShadingXY(Face a);
      void gouShadingXZ(Face a);
      void gouShadingYZ(Face a);
      void normalizeFaces(vector<Face> *list);



   private:
      void _MakePixel(const QPoint& QPt_Position, const QColor& QC_Color);
      void _MakePixel(const QPoint& QPt_Position, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
      void _SetPixelPosition(const QPoint& QPt_NewPosition);


      unsigned char* _pucBuffer;
      int bufferSize;
      QPoint _QPt_CurrentPosition;
      Matrix4x4 matComposite;
      int halfWidth;
      int halfHeight;
      int halfMask[3][3];



      float from[3];




};

#endif // __OpenGLCanvas_h__
