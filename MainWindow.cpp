#include "MainWindow.h"


#include <QApplication>
#include <QCloseEvent>
#include <QFileDialog>
#include <QGLFormat>
#include <QMenuBar>  // <QMenu>, <QAction>
#include <QMessageBox>
#include <QMessageBox>
#include <QString>
#include <fstream>
#include <stdlib.h>
#include "Vertex.h"
#include <cmath>

#include "ui_mainwindow.h"




MainWindow::MainWindow(QWidget* pQW_Parent)
   : QMainWindow(pQW_Parent)
{
   // Check OpenGL support
   _CheckOpenGL();

   // Create GUIs
   myGui = new Ui::mainWindowGui();
   myGui->setupUi(this);
   fileIsOpen = false;
   updatePhong();
   updateLight();

}

void MainWindow::_CheckOpenGL()
{
   // Check OpenGL support
   if(!QGLFormat::hasOpenGL())
   {
      qCritical("Error: This program requires OpenGL!");

      exit(ERR_OPENGL_NOT_SUPPORTED);
   }
}


void MainWindow::closeEvent(QCloseEvent* pQCE)
{
   QMessageBox::StandardButton QMB_SB_Choice=QMessageBox::warning(this, "ECS-175 Project 1", "Do you really want to quit this application?", QMessageBox::Yes|QMessageBox::No);

   if(QMessageBox::Yes==QMB_SB_Choice)
   {
      pQCE->accept();

      _Exit();
   }
   // else if(QMessageBox::No==QMB_SB_Choice)
   {
      pQCE->ignore();

      return;
   }
}

void MainWindow::_Exit()
{
   qApp->quit();
}

void MainWindow::_FileOpen()
{
   // TODO: More than one file?
   const QString& kQStr_FileName=QFileDialog::getOpenFileName(this, "Select a data file", ".", "Datasets (*.3d)");
   int numPts = 0, numFaces = 0;
   float tempFloat1 = 0.0, tempFloat2 = 0.0, tempFloat3 = 0.0;
   ifstream myfile;
   int tempFace1 = 0, tempFace2 = 0, tempFace3;
   char tempCh = 'A';

   if(!kQStr_FileName.isEmpty())
   {
      //emit openTheFile(kQStr_FileName);

   }
   else // i.e. cQStr_FileName.isEmpty()
   {
      qWarning("Warning: No file selected!");
   } // if(!kQStr_FileName.isEmpty())




   name = kQStr_FileName.toStdString();

   myfile.open(name.c_str());

   if (myfile.fail())
   {
       QMessageBox::warning(this, tr("Warning!"), tr("Cannot read file."));
       return;
   }

   myfile >> numObj;
   obj = new Object*[numObj];

   myGui->comboBox->clear();

   for (int i = 0; i < numObj; i++)
   {
       myfile >> numPts;
       obj[i] = new Object();
       obj[i]->setObject(numPts);
       for (int j = 0; j < numPts; j++)
       {
           myfile >> tempFloat1 >> tempFloat2 >> tempFloat3;

           obj[i]->setPoint(Vertex(tempFloat1, tempFloat2, tempFloat3));

       }

       myfile >> numFaces;

       obj[i]->setFaces(numFaces);
       for (int j = 0; j < numFaces; j++)
       {
           myfile >> tempFace1 >> tempFace2 >> tempFace3;
           obj[i]->insertFace(j, tempFace1, tempFace2, tempFace3);
       }

       obj[i]->findFaceNorms();
       obj[i]->findVertexNorms();

       myGui->comboBox->addItem(QString(tempCh), i);
        tempCh++;
   }


   fileIsOpen = true;


   myfile.close();
   redraw();
}
/*
void MainWindow::writeFile()
{
    ofstream myFile;

    myFile.open(name.c_str());

    if (myFile.fail())
    {
        QMessageBox::warning(this, tr("Warning!"), tr("Cannot write file."));
        return;
    }

    myFile << numObj << endl;

    for (int i = 0; i < numObj; i++)
    {
        myFile << endl;
        myFile << obj[i]->numPoints << endl;
        for (int j = 0; j < obj[i]->numPoints; j++)
        {
            myFile << obj[i]->points[j].x << " " << obj[i]->points[j].y << " " << obj[i]->points[j].z << endl;
        }
        myFile << obj[i]->numEdges << endl;
        for (int j = 0; j < obj[i]->numEdges; j++)
            myFile << obj[i]->edges[j].first << " " << obj[i]->edges[j].second << endl;
    }

    myFile.close();
}
*/
void MainWindow::CanvasMousePositionChanged(QPoint QP_NewPosition)
{
    myGui->horizontalSlider->setValue(QP_NewPosition.x());
    myGui->verticalSlider->setValue(myGui->graphics->height() - QP_NewPosition.y()-1);

}

void MainWindow::CanvasSizeChanged(QSize QS_NewSize)
{
   const int kiNewWidth=QS_NewSize.width();
   const int kiNewHeight=QS_NewSize.height();

    myGui->horizontalSlider->setRange(0, kiNewWidth-1);
    myGui->verticalSlider->setRange(0, kiNewHeight-1);
    myGui->horizontalSlider->setValue(kiNewWidth>>1);
    myGui->verticalSlider->setValue(kiNewHeight>>1);

    redraw();
}

void MainWindow::selectObj(int x)
{
     currObj = x;
    updateList();

}

void MainWindow::updateList()
{
    QString temp;
    myGui->listWidget->clear();

    for (int i = 0; i < obj[currObj]->numPoints; i++)
        myGui->listWidget->addItem(QString("%1: X: %2 Y: %3 Z: %4").arg(i+1).arg(obj[currObj]->points[i].x)
                                   .arg(obj[currObj]->points[i].y)
                                   .arg(obj[currObj]->points[i].z));
}


void MainWindow::redraw()
{

    myGui->graphics->blackout();

    if (fileIsOpen)
    {
        for (int i = 0; i < numObj; i++)
        {
            myGui->graphics->phongLighting(obj[i], 2);
            myGui->graphics->phongLighting(obj[i], 1);
            myGui->graphics->phongLighting(obj[i], 0);
        }
            myGui->graphics->normIllum(obj, numObj);
        for(int i = 0; i < numObj; i++)
        {
            obj[i]->makeFinalVertex();
        }
            myGui->graphics->paintAlgXY(obj, numObj);
            myGui->graphics->paintAlgXZ(obj, numObj);
            myGui->graphics->paintAlgYZ(obj, numObj);

    }
    emit refresh();
}


void MainWindow::updatePhong()
{
    myGui->graphics->ambCoef = myGui->doubleSpinBox->value();
    myGui->graphics->diffCoef = myGui->doubleSpinBox_2->value();
    myGui->graphics->specCoef = myGui->doubleSpinBox_3->value();
    myGui->graphics->phongConst = myGui->spinBox->value();

    redraw();
}

void MainWindow::updateHalfTone(bool t)
{
    myGui->graphics->halfToning = t;

    redraw();
}

void MainWindow::updateLight()
{
    float x, y, z;

    myGui->graphics->lightSource[0] = x = myGui->doubleSpinBox_4->value();
    myGui->graphics->lightSource[1] = y = myGui->doubleSpinBox_5->value();
    myGui->graphics->lightSource[2] = z = myGui->doubleSpinBox_6->value();
    myGui->graphics->intenAmbient = myGui->doubleSpinBox_7->value();
    myGui->graphics->intenLight = myGui->doubleSpinBox_8->value();

    x -= 10;
    y -= 10;
    z -= 10;

    myGui->graphics->K = sqrt(x*x + y*y + z*z);

    redraw();
}
