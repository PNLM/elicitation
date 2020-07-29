#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "profil.h"
#include <string>
#include <vector>
#include <iostream>
#include <QSettings>
#include <QSignalMapper>
#include <QProgressBar>
#include <QString>
#include <QPointer>
#include <QFileDialog>
#include <QTextStream>
#include <QTableWidgetItem>
#include <sstream>
#include <QComboBox>
#include <QDragMoveEvent>
#include <QMimeData>

int numprofil1 = 0; int numprofil2 = 0;
int checkClicks = 0;
int checkRank =0;
std::vector<QPushButton *> newProfils;
typedef std::pair<int,int> intPair;
std::vector<std::pair<int,int>> liste;
std::vector<Profil *> allProfils;
std::vector<Profil *> allProfilsDoz;
std::vector<int> indicesOrdre;
std::vector<Profil *> profilsOrdre;
std::vector<std::vector<QPushButton*>> listeGroupes;
std::vector<Profil *> temp;
int groupeActuel = 0;
int compaActuelle = -1;
int nbGroupes =0;
int profPerRow = 6;
std::vector<QString> levels{"Sélectionnez..."};
QPointer<Profil> prof;

std::list<QLabel *> labelList;
std::vector<QLabel *> labelVec;
// FENÊTRE PRINCIPALE------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setAcceptDrops(false);
    ui->setupUi(this);

    QPixmap bkgnd(":/img/stripes-light.png");
        bkgnd = bkgnd.scaled(this->size(), Qt::KeepAspectRatio);

        QPalette palette;
        palette.setBrush(QPalette::Background, bkgnd);

        this->setPalette(palette);


    this->setWindowTitle("Elicitation de préférences");
    //addAction(ui->actionQuitter);

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QFile file("degrees.pnl");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        levels.push_back(line);
        }

    for (int i =0; i<(int)levels.size();i++){
        ui->comboBox->addItem(levels[i]);
    }
    ui->tableWidget->hide();
    loadFile();


    ui->spinBox->hide();
    ui->exProfil->hide();
    ui->exProfil= new Profil(0,0,0,4,6,2,3);
    ui->exProfil->setGeometry(10,40,180,100);
    ui->undo->hide();
    ui->labelNbGroupes->hide();
    ui->pushButton->hide();

    ui->save->hide();
    ui->suite->hide();
    ui->quit->hide();

    ui->groupeSuivant->hide();
    ui->scrollArea->hide();
    ui->label->hide();

    ui->valider->hide();
    ui->stackedWidget->raise();
    ui->stackedWidget->setCurrentIndex(1);

    QWidget *w = new QWidget;
    w->setLayout(ui->gridLayout);
    ui->scrollArea->setWidget(w);
    ui->label_eval->hide();
    ui->historique->hide();
    ui->labelValeur->hide();
    ui->comboBox->hide();
    ui->label_eval->hide();

    QObject::connect(ui->valider, SIGNAL(clicked()),this , SLOT(confirm()));
    QObject::connect(ui->cont2, SIGNAL(clicked()),this , SLOT(confirm2()));
    QObject::connect(ui->begin, SIGNAL(clicked()),this , SLOT(hideMenu()));
    QObject::connect(ui->cont, SIGNAL(clicked()),this , SLOT(endSplash()));
    QObject::connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this, SLOT(CheckDiff(int)));
    QObject::connect(ui->pushButton, SIGNAL(clicked()),this , SLOT(loadFile()));
    //QObject::connect(ui->groupeSuivant, SIGNAL(clicked()),this , SLOT(groupeSuivant()));
    QObject::connect(ui->save, SIGNAL(clicked()),this , SLOT(saveFile()));
    QObject::connect(ui->actionQuitter, SIGNAL(triggered()),this , SLOT(close()));
    //QObject::connect(ui->actionReset, SIGNAL(triggered()),this , SLOT(reset()));
    QObject::connect(ui->quit, SIGNAL(clicked()),this , SLOT(close()));
    QObject::connect(ui->suite, SIGNAL(clicked()),this , SLOT(showResults()));
    QObject::connect(ui->actionInstructions, SIGNAL(triggered()),this , SLOT(helpMe()));
    QObject::connect(ui->undo, SIGNAL(clicked()),this , SLOT(previousCompa()));

}


MainWindow::~MainWindow()
{
    delete ui;
}


// LOADFILE : charge les données contenues dans le fichier data, à savoir les valeurs de chaque critère pour chacun des profils


void MainWindow::loadFile(){
    //QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"), "", tr("Text files (*.txt)"));
    //QFile file(fileName);
    QFile file("data.pnl");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         return;

     QTextStream in(&file);
     while (!in.atEnd()) {
         QString line = in.readLine();
         std::vector<int> contenuLigne;
         if (line.size()==7){
         for (int i=0;i<line.size();i++){
         contenuLigne.push_back((int)line[i].digitValue());
         }
         prof = new Profil(contenuLigne[0],contenuLigne[1],contenuLigne[2],contenuLigne[3],contenuLigne[4],contenuLigne[5],contenuLigne[6]);
         prof->setParent(ui->scrollArea);
             prof->setObjectName(QString::number(contenuLigne[2]));
             allProfils.push_back(prof);
         }
         else if (line.size()==8){
             contenuLigne.push_back((int)line[0].digitValue());
             contenuLigne.push_back((int)line[1].digitValue());
             contenuLigne.push_back((QString(line[2])+QString(line[3])).toInt());
             for (int i=4;i<line.size();i++){
             contenuLigne.push_back((int)line[i].digitValue());}

             prof = new Profil(contenuLigne[0],contenuLigne[1],contenuLigne[2],contenuLigne[3],contenuLigne[4],contenuLigne[5],contenuLigne[6]);
             prof->setParent(ui->scrollArea);
                 prof->setObjectName(QString::number(contenuLigne[2]));
                 allProfilsDoz.push_back(prof);
         }

                    ui->gridLayout->addWidget(prof,contenuLigne[0],contenuLigne[1]);
                    QLabel *info = new QLabel();
                    info->setMinimumSize(QSize(40, 115));
                    info->setMaximumSize(QSize(40, 115));
                    info->setText("   >");



                    if (contenuLigne[0]==3)
                    {
                        if ((contenuLigne[1]<8))

                       { ui->gridLayout->addWidget(info,contenuLigne[0],contenuLigne[1]+1);}}


                    else {ui->gridLayout->addWidget(info,contenuLigne[0],contenuLigne[1]+1);}

                    if (indicesOrdre.size()<20){
                   indicesOrdre.push_back(prof->getNumber());
                   profilsOrdre.push_back(prof);}

                   ui->begin->setEnabled(true);
     }




       ui->tableWidget->setRowCount(21);
       ui->tableWidget->setColumnCount(5);
        ui->tableWidget->setItem(0, 0,new QTableWidgetItem(QString::number(allProfils.size()+allProfilsDoz.size())));
        ui->tableWidget->setItem(0, 1, new QTableWidgetItem("4"));
        ui->tableWidget->setItem(0, 2, new QTableWidgetItem(""));
        ui->tableWidget->setItem(0, 3, new QTableWidgetItem(""));
        ui->tableWidget->setItem(0, 4, new QTableWidgetItem(""));

}


// BEGINRANK : Gère l'affichage des profils lors de la phase de comparaison 2 à 2.

void MainWindow::beginRank(){

        for (unsigned long long i=0; i<allProfils.size()+allProfilsDoz.size()-1; i++)
        {
                checkRank = 1;
                if (checkClicks ==(int)i){

                 ui->tableWidget->setItem(i+1,0,new QTableWidgetItem(QString::number(((float)profilsOrdre[i]->getPhysique()-1)/5)));
                 ui->tableWidget->setItem(i+1,1,new QTableWidgetItem(QString::number(((float)profilsOrdre[i]->getPsycho()-1)/5)));
                  ui->tableWidget->setItem(i+1,2,new QTableWidgetItem(QString::number(((float)profilsOrdre[i]->getNutri()-1)/5)));
                   ui->tableWidget->setItem(i+1,3,new QTableWidgetItem(QString::number(((float)profilsOrdre[i]->getCog()-1)/5)));
                    ui->tableWidget->setItem(i+1,4,new QTableWidgetItem(QString::number(compaActuelle)));
                numprofil1=indicesOrdre[i];
                ui->labelValeur->setText(QString::number(numprofil1));
                numprofil2=indicesOrdre[i+1];
                ui->gridCompa->addWidget(profilsOrdre[i],0,0);
                ui->gridCompa->addWidget(profilsOrdre[i+1],0,1);
                profilsOrdre[i]->show();
                profilsOrdre[i]->setStyleSheet("QToolButton{background-color:white;color: white;}");
                profilsOrdre[i+1]->show();
                profilsOrdre[i]->setStyleSheet("QToolButton{background-color:white;color: white;}");

                QLabel *TEST = new QLabel();
                if (QString::number(compaActuelle) == "0")
                {TEST->setText("<=>");}
                else if(QString::number(compaActuelle) == "1")
                {TEST->setText(" (faible) \n    > ");}
                else if(QString::number(compaActuelle) == "2")
                {TEST->setText(" (interm.) \n     >");}
                else if(QString::number(compaActuelle) == "3")
                {TEST->setText(" (forte) \n    > ");}

                TEST->setMaximumSize(45,115);
                labelList.push_back(TEST);
                labelVec.push_back(TEST);

                if (i>=1&&i<(unsigned long long)profPerRow+1){
                ui->gridTest->addWidget(profilsOrdre[i-1],1,2*(i-1));
                ui->gridTest->addWidget(TEST,1,2*(i-1)+1);
                profilsOrdre[i-1]->setEnabled(false);

                }
                else if (i==0){}
                else if (i<(unsigned long long)(2*profPerRow)+1){

                ui->gridTest->addWidget(profilsOrdre[i-1],2,2*(i-7));
                ui->gridTest->addWidget(TEST,2,2*(i-7)+1);
                profilsOrdre[i-1]->setEnabled(false);}

                else if (i<(unsigned long long)(3*profPerRow)+1){

                ui->gridTest->addWidget(profilsOrdre[i-1],3,2*(i-13));
                ui->gridTest->addWidget(TEST,3,2*(i-13)+1);
                profilsOrdre[i-1]->setEnabled(false);}

                ui->label_eval->show();
                }

                if ((int)checkClicks==(int)(allProfils.size()+allProfilsDoz.size()-1)){

                    QLabel *TEST = new QLabel();
                    if (QString::number(compaActuelle) == "0")
                    {TEST->setText("<=>");}
                    else if(QString::number(compaActuelle) == "1")
                    {TEST->setText(" (faible) \n    > ");}
                    else if(QString::number(compaActuelle) == "2")
                    {TEST->setText(" (interm.) \n     >");}
                    else if(QString::number(compaActuelle) == "3")
                    {TEST->setText(" (forte) \n    > ");}

                    TEST->setMaximumSize(45,115);

                    int a = allProfils.size()+allProfilsDoz.size()-1;
                    ui->tableWidget->setItem(allProfils.size()+allProfilsDoz.size(),0,new QTableWidgetItem(QString::number(((float)profilsOrdre[a]->getPhysique()-1)/5)));
                     ui->tableWidget->setItem(allProfils.size()+allProfilsDoz.size(),1,new QTableWidgetItem(QString::number(((float)profilsOrdre[a]->getPsycho()-1)/5)));
                      ui->tableWidget->setItem(allProfils.size()+allProfilsDoz.size(),2,new QTableWidgetItem(QString::number(((float)profilsOrdre[a]->getNutri()-1)/5)));
                       ui->tableWidget->setItem(allProfils.size()+allProfilsDoz.size(),3,new QTableWidgetItem(QString::number(((float)profilsOrdre[a]->getCog()-1)/5)));
                        ui->tableWidget->setItem(allProfils.size()+allProfilsDoz.size(),4,new QTableWidgetItem(QString::number(compaActuelle)));

                       ui->gridTest->addWidget(profilsOrdre[a-1],4,0);
                       ui->gridTest->addWidget(TEST,4,1);
                        profilsOrdre[a-1]->setEnabled(false);
                        ui->gridTest->addWidget(profilsOrdre[a],4,2);
                        profilsOrdre[a]->setEnabled(false);

                    ui->label_eval->setText("Comparaisons terminées");

                ui->suite->show();
                ui->comboBox->hide();
                ui->save->show();
                ui->quit->show();
                ui->historique->hide();
                ui->undo->hide();
                return;
                };
            }
    }

// PREVIOUSCOMPA : Permet de revenir en arrière lors de la sélection du degré de différence

void MainWindow::previousCompa(){

    if (checkClicks>0){

checkClicks -=1;
numprofil1=indicesOrdre[checkClicks];
ui->labelValeur->setText(QString::number(numprofil1));
numprofil2=indicesOrdre[checkClicks+1];
ui->gridCompa->addWidget(profilsOrdre[checkClicks],0,0);
profilsOrdre[checkClicks]->setEnabled(true);
ui->gridCompa->addWidget(profilsOrdre[checkClicks+1],0,1);
profilsOrdre[checkClicks+1]->raise();

QLabel *lastWidget = labelList.back();
std::cout<< lastWidget->text().toStdString() << std::endl;
ui->gridTest->removeWidget(lastWidget);
lastWidget->hide();
labelList.pop_back();

    }
}

// SHOWRESULTS : Affiche le csv de résultats au sein de la fenêtre.


void MainWindow::showResults(){

    ui->tableWidget->show();
    ui->suite->hide();


    for (unsigned long long i =0; i<labelVec.size();i++)
    {QLabel *lastWidget = labelVec[i];
        ui->gridTest->removeWidget(lastWidget);
        lastWidget->hide();

   }

}

// SAVEFILE : Gère la sauvegarde du fichier de résultats

void MainWindow::saveFile(){

    QString filename = QFileDialog::getSaveFileName(this, "Sauvegarder les résultats", "results.csv", "CSV files (.csv)", 0, 0);
    QFile data(filename);
    if(data.open(QFile::WriteOnly |QFile::Truncate))
    {
        QTextStream output(&data);

        QStringList strList;

        for( int r = 0; r < 21; ++r )
        {
            strList.clear();
            for( int c = 0; c < 5; ++c )
            {
                strList << "\" "+ui->tableWidget->item( r, c )->text()+"\" ";
            }
           output<< strList.join( ";" )+"\n";

        }

        data.close();

    }
}


//----------------------------------------------

//RESET : gère la remise à zéro (non implémentée)


void MainWindow::reset(){

    ui->stackedWidget->show();
    ui->suite->hide();
    ui->stackedWidget->raise();
    ui->stackedWidget->setCurrentIndex(1);
    ui->label->hide();
    ui->scrollArea->hide();
    ui->historique->hide();
    ui->quit->hide();
    ui->save->hide();
    ui->tableWidget->hide();
    ui->undo->hide();
    ui->label_eval->hide();
    ui->comboBox->hide();

    allProfils.clear();
    allProfilsDoz.clear();
    loadFile();
    profilsOrdre.clear();
    indicesOrdre.clear();
    compaActuelle = -1;
    checkClicks=0;

}


// CHECKDIFF : GERE L'ECRITURE DANS LE CSV & LE PASSAGE AUX COMPARAISONS SUIVANTES


void MainWindow::CheckDiff(int k)
{


    if (ui->comboBox->currentIndex()!=0) {
        //if (combo->currentIndex()!=0) {

    // COMPARAISONS
    //AUCUNE
    if (ui->comboBox->currentText()==levels[1])

    {
    if(numprofil1== ui->labelValeur->text().toInt())
        {compaActuelle=0;}

    if (numprofil1!=0 && numprofil2 != 0)
    {liste.push_back(intPair(numprofil1,numprofil2));
    liste.push_back(intPair(numprofil2,numprofil1));
    ui->historique->setText("Aucune différence d'attractivité entre le profil "+ QString::number(numprofil1)+ " et le profil "  + QString::number(numprofil2));

    ui->historique->show();
}
}

    if (ui->comboBox->currentText()==levels[k] && k>1)

       {
        if(numprofil1== ui->labelValeur->text().toInt())
            {compaActuelle=k-1;
        }

        if (numprofil1!=0 && numprofil2 != 0)
        {liste.push_back(intPair(numprofil1,numprofil2));
        liste.push_back(intPair(numprofil2,numprofil1));
        ui->historique->setText( "Différence d'attractivité " +QString(levels[k]) + " entre le profil "+ QString::number(numprofil1)+ " et le profil "  + QString::number(numprofil2));
        ui->historique->show();

    }
    }

    checkRank=0;
    checkClicks+=1;
    beginRank();
    ui->comboBox->setCurrentIndex(0);
}
}



// UTILITAIRES----------------------------
// HELPME : Affiche à nouveau les instructions

void MainWindow::helpMe(){
    ui->stackedWidget->show();
    ui->scrollArea->hide();
    ui->label->hide();
    ui->valider->hide();
    ui->exProfil->show();
}


// DISABLEPROFILS : Désactive tous les profils

void MainWindow::DisableProfils(){

    for (unsigned long long i =0; i<allProfils.size();i++)
    {allProfils[i]->m_clicked=3;
        allProfils[i]->setStyleSheet("");
    }

    for (unsigned long long i =0; i<allProfilsDoz.size();i++)
    {allProfilsDoz[i]->m_clicked=3;
    allProfilsDoz[i]->setStyleSheet("");}
}

// SHOWPROFILS : Affiche tous les profils

void MainWindow::ShowProfils(){
    for (unsigned long long i =0; i<allProfils.size();i++)
    {allProfils[i]->show();}

    for (unsigned long long i =0; i<allProfilsDoz.size();i++)
    {allProfilsDoz[i]->show();}
}

// HIDEMENU : Affiche les instructions après avoir cliqué sur "Commencer"
// Prend en compte le nombre de groupes spécifié

void MainWindow::hideMenu(){
    ui->stackedWidget->setCurrentIndex(0);
    nbGroupes = ui->spinBox->value();
    ui->stackedWidget->addWidget(ui->exProfil);
    ui->exProfil->show();

    for (int i=0; i<nbGroupes ; i++)
    {
        std::vector<QPushButton* > groupe;
        listeGroupes.push_back(groupe);

    }
}


// CONFIRM ! Fonction associée au bouton "Valider l'ordre"
// Teste s'il reste des incohérences dans le classement des profils (un profil inférieur en tout point à un autre ne peut pas être mieux classé)
// S'il n'y a aucune incohérence, la fonction fait basculer au prochain écran d'instructions


void MainWindow::confirm(){
    int inco = 0;
    for (unsigned long long i =1; i<profilsOrdre.size()-1;i++)
    {Profil *A = profilsOrdre[i];
        if (A->getNumber()<7)
        { A->setStyleSheet("QToolButton{background-color:rgb(255, 249, 164);color:rgb(255, 249, 164);}");
        }
        else{A->setStyleSheet("QToolButton{background-color:white;color:white;}");}

        for (unsigned long long j =i+1; j<profilsOrdre.size()-1;j++){
        Profil *B = profilsOrdre[j];

        if ( ((A->getPhysique()<B->getPhysique()) && (A->getPsycho()<B->getPsycho()) && (A->getNutri()<B->getNutri()) && (A->getCog()< B->getCog()) ))
{            ui->label->setText("Il reste des incohérences !");
           A->setStyleSheet("QPushButton{background-color:rgb(255, 0, 0);color:rgb(255, 249, 164);}");
            // A->setStyleSheet("QToolButton{background-color:rgb(255, 0, 0);}");
        inco +=1;}}
}
    if (inco == 0){

    ui->stackedWidget->setCurrentIndex(2);

    for (unsigned long long i =0; i<profilsOrdre.size();i++)
    {profilsOrdre[i]->m_state=1;}
    ui->stackedWidget->show();
     ui->stackedWidget->raise();
     ui->scrollArea->hide();
     ui->valider->hide();
     ui->label->hide();

}

     }

// CONFIRM2 : Fonction associée au bouton "Continuer" sur l'écran qui suit la validation du classement.
// Affiche l'écran de comparaison des profils 2 à 2

void MainWindow::confirm2(){
     ui->undo->show();
      ui->actionInstructions->setEnabled(false);
     ui->comboBox->show();
    ui->stackedWidget->hide();
    ShowProfils();
    beginRank();
    ui->scrollArea->hide();
    DisableProfils();

}


// ENDSPLASH : cache l'écran d'instructions

void MainWindow::endSplash(){
ui->valider->show();
ui->exProfil->hide();
ShowProfils();
ui->stackedWidget->hide();
ui->label->show();
ui->scrollArea->show();
ui->scrollArea->raise();

}


// CONNEXIONPROFIL : établit le lien entre un objet Profil et la fenêtre dans laquelle il se trouve.
// Est utilisée pour comparer deux profils lorsque l'on en fait glisser un sur un autre.


void MainWindow::connexionProfil(Profil* A, Profil* B){

    std::vector<Profil *>::iterator itA = std::find(profilsOrdre.begin(), profilsOrdre.end(), A);
    std::vector<Profil *>::iterator itB = std::find(profilsOrdre.begin(), profilsOrdre.end(), B);

if ( ((A->getPhysique()>=B->getPhysique()) && (A->getPsycho()>=B->getPsycho()) && (A->getNutri()>=B->getNutri()) && (A->getCog()>= B->getCog()) && itA<itB)
     ||
     ((A->getPhysique()<=B->getPhysique()) && (A->getPsycho()<=B->getPsycho()) && (A->getNutri()<=B->getNutri()) && (A->getCog()<= B->getCog()) && itA>itB)

        )
{ui->label->setText("Comparaison impossible !");
ui->label->setStyleSheet("QLabel{color:rgb(255, 0, 0)}");}

else {
    ui->label->setText("Classez les profils du plus fragile au moins fragile");
    ui->label->setStyleSheet("QLabel{color:black}");

    for (int i = 0; i < (int)indicesOrdre.size(); ++i){
        if (indicesOrdre[i]==B->getNumber()){
            std::cout <<"B :" << B->getNumber() <<std::endl;
            for (int k = 0; k < (int)indicesOrdre.size(); ++k){
                if (indicesOrdre[k]==A->getNumber()){
                    std::cout <<"A :" << A->getNumber() <<std::endl;

                    indicesOrdre.erase(indicesOrdre.begin()+i);
                    profilsOrdre.erase(profilsOrdre.begin()+i);
                    indicesOrdre.insert(indicesOrdre.begin()+k,B->getNumber());
                    profilsOrdre.insert(profilsOrdre.begin()+k,B);

                   for (int m = 0; m < (int)indicesOrdre.size(); ++m){
                               ui->gridLayout->removeWidget(profilsOrdre[m]);}

                   int row =0;
                   int col =0;
                    for (int m = 0; m < (int)indicesOrdre.size(); ++m){


                                ui->gridLayout->addWidget(profilsOrdre[m],row,col);
                                QLabel *info = new QLabel();
                                info->setMinimumSize(QSize(40, 115));
                                info->setMaximumSize(QSize(40, 115));
                                info->setText("   >");
                                if (row==3)
                                {if ((col<8))

                                   { ui->gridLayout->addWidget(info,row,col+1);}}


                                else {ui->gridLayout->addWidget(info,row,col+1);}
                                if (col<8){col +=2;}
                                else {col=0; row +=1;}
                    }

                     return;
           }
}}}}}
