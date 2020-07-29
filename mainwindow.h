#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QPushButton>
#include <QSignalMapper>
#include <QMainWindow>
#include <QGridLayout>
#include "profil.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void DisableProfils();
    void helpMe();
    void previousCompa();
    void reset();
    void loadFile();
    void saveFile();
    void hideMenu();
    void beginRank();
    void showResults();
    void connexionProfil(Profil* A, Profil* B);
    void ShowProfils();
    void CheckDiff(int k);
    void endSplash();
    void confirm();
    void confirm2();

public:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
