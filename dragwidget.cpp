

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include "profil.h"

#include <iostream>

    : QPushButton(parent)
{
    //setMinimumSize(10,115);
    //setMaximumSize(10,130);
    this->setMinimumSize(QSize(180, 115));
    this->setMaximumSize(QSize(180, 130));
//this->setStyleSheet("QPushButton { background-color: rgba(10, 0, 0, 0); }");
    setAcceptDrops(true);

}


void DragWidget::dragEnterEvent(QDragEnterEvent *event)
{
event->accept();

}

void DragWidget::dragMoveEvent(QDragMoveEvent *event)
{

event->accept();
            event->setDropAction(Qt::MoveAction);
            event->accept();
             event->source();
        }




void DragWidget::dropEvent(QDropEvent *event)
{
    std::cout <<"putain" <<std::endl;

    QPushButton *a= qobject_cast<QPushButton*>(event->source());
    //a->hide();
    //QLayout *zob = a->layout();
    //QGridLayout *grid = static_cast<QGridLayout*>(a->layout());

QRect geo  = this->geometry();
QRect gea = a -> geometry();

this->setGeometry(gea);
a->setGeometry(geo);




event->acceptProposedAction();
}


