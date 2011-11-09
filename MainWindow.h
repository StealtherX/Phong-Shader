#ifndef __MainWindow_h__
#define __MainWindow_h__


#include <QMainWindow>
#include "Object.h"


class QAction;


#define ERR_OPENGL_NOT_SUPPORTED 1

namespace Ui{
    class mainWindowGui;
}

class MainWindow : public QMainWindow
{
   Q_OBJECT

   public:
      MainWindow(QWidget* pQW_Parent=NULL);
      //void writeFile();

   signals:
    //void updateGraphics(Polygon* poly);
    void refresh();


   protected:
      void closeEvent(QCloseEvent* pQCE);
      void redraw();
      void updateList();



   private slots:
      void _FileOpen();
      void _Exit();
      void CanvasMousePositionChanged(QPoint QP_NewPosition);
      void CanvasSizeChanged(QSize QS_NewSize);
      void updatePhong();
      void updateLight();
      void updateHalfTone(bool t);
      void selectObj(int x);


   private:
      void _CheckOpenGL();

      Ui::mainWindowGui *myGui;
      int numObj;
      Object **obj;
      string name;
      int currObj;
      bool fileIsOpen;


      // Actions
      QAction* _pQA_File_Open;
      QAction* _pQA_File_Exit;
};

#endif // __MainWindow_h__
