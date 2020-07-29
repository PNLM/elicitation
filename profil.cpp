#include "profil.h"
#include <string>
#include <list>
#include <vector>
#include <fstream>
#include <iostream>
#include <QSettings>
#include <QSignalMapper>
#include <QProgressBar>
#include <QString>
#include <QGridLayout>
#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QPointer>

#include "mainwindow.h"
#include "ui_mainwindow.h"


Profil:: Profil(int ligne, int colonne, int numero, int crit1, int crit2, int crit3, int crit4) :
     m_numero(numero), m_row(ligne), m_col(colonne), m_crit1(crit1), m_crit2(crit2), m_crit3(crit3), m_crit4(crit4), m_clicked(0),m_state(0)

{

    std::vector<QString> criteriaT;
    setAcceptDrops(true);

    QFile file("criteria.pnl");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        criteriaT.push_back(line);

        }

   std::vector<int> percentagesT {crit1,crit2,crit3,crit4};

     this->setLayout(new QVBoxLayout);
    for (int i=0; i<4;i++){



    QProgressBar *chart = new QProgressBar();

    chart->setValue(100*percentagesT[i]/6);
    chart->setFormat(criteriaT[i]);
    chart->setTextVisible(true);
    chart->setAlignment(Qt::AlignCenter);
    chart->setStyleSheet("QProgressBar::chunk {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:1 rgba(97, 97, 97, 117));width: 9px;font-size: 18px;}");
/*

        if (chart->value()<50){
        chart->setStyleSheet("QProgressBar::chunk {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:1 rgba(255, 0, 0, 117));width: 9px;font-size: 18px;}");
        }

        else if (chart->value()>50){
        chart->setStyleSheet("QProgressBar::chunk {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:1 rgba(0, 255, 0, 117));width: 9px;font-size: 18px;}");
       //chart->setStyleSheet("QProgressBar::chunk {background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:1, y2:1, stop:0.20 rgba(255, 0, 0, 117), stop:0.639773 rgba(255, 96, 0, 176), stop:1 rgba(0, 255, 0, 117));font-size: 18px;}");

        }

        else if (chart->value()==50){
        chart->setStyleSheet("QProgressBar::chunk {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:1 rgba(255, 170, 0, 149));width: 9px;font-size: 18px;}");
//chart->setStyleSheet("QProgressBar::chunk{background-color:qlineargradient(spread:pad, x1:0, y1:1, x2:1, y2:1, stop:0.34 rgba(255, 0, 0, 117), stop:1 rgba(255, 96, 0, 176));font-size: 18px;}");

        }
        */



    this->layout()->addWidget(chart);
    }

    this->setMinimumSize(QSize(180, 115));
    this->setMaximumSize(QSize(180, 130));
    this->setText(QString::number(numero));
    QLabel *label = new QLabel();
    label->setText("P"+QString::number(numero));
    label->setStyleSheet("QLabel{font-size: 18px;}");
    this->layout()->addWidget(label);
    label->setMaximumSize(QSize(40, 115));
    label->move(75,90);

//    if (this->m_clicked==1) { this->setStyleSheet("QPushButton:hover{ background-color: yellow };QPushButton{ background-color: yellow }");}
}


// MOUSEMOVEEVENT : Gère les évènements qui ont lieu lorsque l'on fait glisser un profil

void Profil::mouseMoveEvent(QMouseEvent *e)
{



    if (this->m_state==0)
    {
    if(e->buttons() & Qt::LeftButton)
    {


        MainWindow *par = ((MainWindow*)(parent()->parent()->parent()->parent()->parent()));

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        QPixmap *widgetPixmap = new QPixmap(this->size());
        this->render(widgetPixmap);
        drag->setMimeData(mimeData);

        par->ui->label->setText("Faites glisser ce profil sur le profil à droite de l'emplacement désiré.");


    drag->setPixmap(*widgetPixmap);
    Qt::DropAction dropAction = drag->exec();}
}

};


//}

void Profil::dragMoveEvent(QDragMoveEvent *event)
{

    event->accept();
                event->setDropAction(Qt::MoveAction);

        }


// DROPEVENT : Gère les évènements ayant lieu lorsque l'on dépose un profil sur un autre.

void Profil::dropEvent(QDropEvent *event)
{


MainWindow *par = ((MainWindow*)(parent()->parent()->parent()->parent()->parent()));


Profil *a= qobject_cast<Profil*>(event->source());
par->connexionProfil(this,a);

event->acceptProposedAction();
}

void Profil::dragEnterEvent(QDragEnterEvent *event)
{
event->accept();

}
