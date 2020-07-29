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

// Pour paramétrer la taille des objets "Profil". La taille est optimisée pour 4 critères.
QSize TAILLE_PROFIL_MIN = QSize(180, 115);
QSize TAILLE_PROFIL_MAX = QSize(180, 130);

// Profil :
// `ligne`, `colonne` indiquent la position du Profil dans la grille
// La liste `crits` contient les valeurs associées aux critères définis dans "criteria.pnl"
// m_clicked indique s'il a été cliqué (1) ou non (0)
// m_state

Profil:: Profil(int ligne, int colonne, int numero, std::vector<int> crits) :
     m_numero(numero), m_row(ligne), m_col(colonne),m_crits(crits), m_clicked(0),m_state(0)

{
    std::vector<QString> criteriaT;

// L'instruction ci-dessous rend possible de glisser des objets sur un Profil (en l'occurence, il s'agira de glisser un profil sur un autre)

    setAcceptDrops(true);

// Lecture des critères dans le fichier "criteria.pnl"

    QFile file("criteria.pnl");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        criteriaT.push_back(line);

        }


    std::vector<int> percentagesT = crits;

    this->setLayout(new QVBoxLayout);


    // Création des objets "chart", qui représentent chacun la barre de progression associée à un critère.

    for (unsigned long long i=0; i<crits.size();i++){

    QProgressBar *chart = new QProgressBar();

    chart->setValue(100*percentagesT[i]/6);
    chart->setFormat(criteriaT[i]);
    chart->setTextVisible(true);
    chart->setAlignment(Qt::AlignCenter);
    chart->setStyleSheet("QProgressBar::chunk {background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:1 rgba(97, 97, 97, 117));width: 9px;font-size: 18px;}");
    this->layout()->addWidget(chart);
    }



    // Réglage de la taille du profil

    this->setMinimumSize(TAILLE_PROFIL_MIN);
    this->setMaximumSize(TAILLE_PROFIL_MAX);
    this->setText(QString::number(numero));

    // Ajout d'un label avec le numéro du profil sous la forme "P" + n° de profil

    QLabel *label = new QLabel();
    label->setText("P"+QString::number(numero));
    label->setStyleSheet("QLabel{font-size: 18px;}");
    this->layout()->addWidget(label);
    label->setMaximumSize(QSize(40, 115));
    label->move(75,90);

}


// MOUSEMOVEEVENT : Gère les évènements qui ont lieu lorsque l'on fait glisser un profil

void Profil::mouseMoveEvent(QMouseEvent *e)
{

    if (this->m_state==0)
    {
    if(e->buttons() & Qt::LeftButton)
    {


        // par : c'est la fenêtre dans laquelle est situé l'objet profil cliqué

        MainWindow *par = ((MainWindow*)(parent()->parent()->parent()->parent()->parent()));

        // Les instructions suivantes créent un objet de type QDrag qui gère l'affichage du profil déplacé sous le curseur
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
