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
int LIGNE_MAX;
std::list<QLabel *> labelList;
std::vector<QLabel *> labelVec;

// FENÊTRE PRINCIPALE------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setAcceptDrops(false);
    ui->setupUi(this);
    this->setWindowTitle("Elicitation de préférences");
    //addAction(ui->actionQuitter);


    //Désactive la possibilité d'éditer le tableWidget qui accueillera les résultats à la fin
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);


    //Lecture des degrés de comparaison présents dans le fichier "degrees.ini"

    QFile file("degrees.ini");
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

//Création du profil utilisé dans l'écran d'instructions
    ui->exProfil->hide();
    ui->exProfil= new Profil(0,0,0,{4,6,2,3});
    ui->exProfil->setGeometry(10,40,180,100);


//Divers éléments créés dans le form qu'il faut cacher au lancement de l'appli
    ui->undo->hide();
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


    //Connexions entre boutons et fonctions
    QObject::connect(ui->valider, SIGNAL(clicked()),this , SLOT(confirm()));
    QObject::connect(ui->cont2, SIGNAL(clicked()),this , SLOT(confirm2()));
    QObject::connect(ui->begin, SIGNAL(clicked()),this , SLOT(hideMenu()));
    QObject::connect(ui->cont, SIGNAL(clicked()),this , SLOT(endSplash()));
    QObject::connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this, SLOT(checkDiff(int)));
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
    QFile file("data.ini");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         return;



    //Lecture des lignes de data.ini
     QTextStream in(&file);
     while (!in.atEnd()) {
         QString line = in.readLine();
         std::vector<int> contenuLigne;
         std::vector<int> criterium;
             contenuLigne.push_back((int)line[0].digitValue());  //ligne
             contenuLigne.push_back((int)line[1].digitValue());  //colonne
             contenuLigne.push_back((QString(line[2])+QString(line[3])).toInt()); //numéro (deux caractères aloués)
             for (int i=4;i<line.size();i++){
             criterium.push_back((int)line[i].digitValue());} //les caractères restants sont aussi nombreux qu'il y a de critères. Ils sont regroupés dans la liste criterium.


             //Création d'un profil à partir des informations parsées.

            prof = new Profil(contenuLigne[0],contenuLigne[1],contenuLigne[2],criterium);
            prof->setParent(ui->scrollArea);
            prof->setObjectName(QString::number(contenuLigne[2]));

            // LES SIX PREMIERS PROFILS ONT LE DROIT A UNE COULEUR DIFFERENTE
            if (prof->getNumber()<7)
            { prof->setStyleSheet("QPushButton{background-color:rgb(255, 249, 164);color:rgb(255, 249, 164);}");}


            allProfils.push_back(prof);


            ui->gridLayout->addWidget(prof,contenuLigne[0],contenuLigne[1]);



            QLabel *info = new QLabel();
            info->setMinimumSize(QSize(40, 115));
            info->setMaximumSize(QSize(40, 115));
            info->setText("   >");



                    if (contenuLigne[0]==LIGNE_MAX)
                    {
                        if ((contenuLigne[1]<8))

                       { ui->gridLayout->addWidget(info,contenuLigne[0],contenuLigne[1]+1);}}


                    else {ui->gridLayout->addWidget(info,contenuLigne[0],contenuLigne[1]+1);}

                    if (indicesOrdre.size()<allProfils.size()){
                   indicesOrdre.push_back(prof->getNumber());
                   profilsOrdre.push_back(prof);}

                   ui->begin->setEnabled(true);
     }

     LIGNE_MAX=prof->getRow();
     std::cout << LIGNE_MAX <<std::endl;




        ui->tableWidget->setRowCount(allProfils.size()+1);
        ui->tableWidget->setColumnCount(levels.size()+1);
        ui->tableWidget->setItem(0, 0,new QTableWidgetItem(QString::number(allProfils.size())));

        ui->tableWidget->setItem(0, 1, new QTableWidgetItem(QString::number(levels.size()-1)));
        ui->tableWidget->setItem(0, 2, new QTableWidgetItem(""));
        ui->tableWidget->setItem(0, 3, new QTableWidgetItem(""));
        ui->tableWidget->setItem(0, 4, new QTableWidgetItem(""));

}


// BEGINRANK : Gère l'affichage des profils lors de la phase de comparaison 2 à 2.
// S'occupe en même temps de l'écriture du csv.

void MainWindow::beginRank(){

        for (unsigned long long i=0; i<allProfils.size()-1; i++)
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



                // labelDeg : le label indiquant le degré de comparaison dans l'affichage

                QLabel *labelDeg = new QLabel();
                if (QString::number(compaActuelle) == "0")
                {labelDeg->setText("<=>");}

                else if  (QString::number(compaActuelle) != "-1")
                {labelDeg->setText("(" + levels[compaActuelle+1].left(5) +") \n    > ");}

                labelDeg->setMaximumSize(45,115);
                labelList.push_back(labelDeg);
                labelVec.push_back(labelDeg);

                if (i>=1&&i<(unsigned long long)profPerRow+1){
                ui->gridTest->addWidget(profilsOrdre[i-1],1,2*(i-1));
                ui->gridTest->addWidget(labelDeg,1,2*(i-1)+1);
                profilsOrdre[i-1]->setEnabled(false);

                }
                else if (i==0){}

                else

                    for (int r=2; r<LIGNE_MAX+1;r++)

                    {if (i<(unsigned long long)(r*profPerRow)+1){

                            ui->gridTest->addWidget(profilsOrdre[i-1],r,2*(i-1-(r-1)*profPerRow));
                            ui->gridTest->addWidget(labelDeg,r,2*(i-1-(r-1)*profPerRow)+1);
                            profilsOrdre[i-1]->setEnabled(false);
                            break;
                        }

                    }

                ui->label_eval->show();
                }


                //CAS DU DERNIER PROFIL A AFFICHER

                if ((int)checkClicks==(int)(allProfils.size()-1)){

                    QLabel *labelDeg = new QLabel();
                    if (QString::number(compaActuelle) == "0")
                    {labelDeg->setText("<=>");}
                    else if  (QString::number(compaActuelle) != "-1")

                        // On ne garde que les 6 premiers caractères du nom du degré pour avoir uen longueur à peu près homogène
                    {labelDeg->setText("(" + levels[compaActuelle+1].left(5) +") \n    > ");}

                    labelDeg->setMaximumSize(45,115);

                    int a = allProfils.size()-1;
                    ui->tableWidget->setItem(allProfils.size(),0,new QTableWidgetItem(QString::number(((float)profilsOrdre[a]->getPhysique()-1)/5)));
                     ui->tableWidget->setItem(allProfils.size(),1,new QTableWidgetItem(QString::number(((float)profilsOrdre[a]->getPsycho()-1)/5)));
                      ui->tableWidget->setItem(allProfils.size(),2,new QTableWidgetItem(QString::number(((float)profilsOrdre[a]->getNutri()-1)/5)));
                       ui->tableWidget->setItem(allProfils.size(),3,new QTableWidgetItem(QString::number(((float)profilsOrdre[a]->getCog()-1)/5)));
                        ui->tableWidget->setItem(allProfils.size(),4,new QTableWidgetItem(QString::number(compaActuelle)));

                       ui->gridTest->addWidget(profilsOrdre[a-1]);

                       ui->gridTest->addWidget(labelDeg);
                        profilsOrdre[a-1]->setEnabled(false);

                        ui->gridTest->addWidget(profilsOrdre[a]);
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
}

// SAVEFILE : Gère la sauvegarde du fichier de résultats

void MainWindow::saveFile(){

    QString filename = QFileDialog::getSaveFileName(this, "Sauvegarder les résultats", "results.csv", "CSV files (.csv)", 0, 0);
    QFile data(filename);
    if(data.open(QFile::WriteOnly |QFile::Truncate))
    {
        QTextStream output(&data);

        QStringList strList;

        for( int r = 0; r < allProfils.size()+1; ++r )
        {
            strList.clear();
            for( int c = 0; c < levels.size()+1; ++c )
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

    loadFile();
    profilsOrdre.clear();
    indicesOrdre.clear();
    compaActuelle = -1;
    checkClicks=0;

}


// checkDiff : GERE L'ECRITURE DANS LE CSV & LE PASSAGE AUX COMPARAISONS SUIVANTES
// Le nom du degré de différence est récupéré dans la liste `levels`

void MainWindow::checkDiff(int k)
{
    if (ui->comboBox->currentIndex()!=0) {

    // COMPARAISONS
    //AUCUNE (à part puisque le message est différent)
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


}

// SHOWPROFILS : Affiche tous les profils

void MainWindow::ShowProfils(){
    for (unsigned long long i =0; i<allProfils.size();i++)
    {allProfils[i]->show();}


}

// HIDEMENU : Affiche les instructions après avoir cliqué sur "Commencer"


void MainWindow::hideMenu(){
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget->addWidget(ui->exProfil);
    ui->exProfil->show();

}


// CONFIRM ! Fonction associée au bouton "Valider l'ordre"
// Teste s'il reste des incohérences dans le classement des profils (un profil inférieur en tout point à un autre ne peut pas être mieux classé)
// S'il n'y a aucune incohérence, la fonction fait basculer au prochain écran d'instructions
// A noter que la comparaison n'est effectuée que sur les 4 premiers critères !


void MainWindow::confirm(){

    //flag indiquant s'il y a des incohérences, conditionne le passage à l'écran suivant
    int inco = 0;

    //Détection des incohérences : les profils mal placés sont coloriés en rouge !

    for (unsigned long long i =1; i<profilsOrdre.size()-1;i++)
    {Profil *A = profilsOrdre[i];

        if (A->getNumber()<7)
        { A->setStyleSheet("QPushButton{background-color:rgb(255, 249, 164);color:rgb(255, 249, 164);}");}


        for (unsigned long long j =i+1; j<profilsOrdre.size()-1;j++){
        Profil *B = profilsOrdre[j];

        if ( ((A->getPhysique()<B->getPhysique()) && (A->getPsycho()<B->getPsycho()) && (A->getNutri()<B->getNutri()) && (A->getCog()< B->getCog()) ))
{            ui->label->setText("Il reste des incohérences !");
           A->setStyleSheet("QPushButton{background-color:rgb(255, 0, 0);color:rgb(255, 249, 164);}");
        inco +=1;}}
}


    //"Si pas d'incohérences..."
    if (inco == 0){

    ui->stackedWidget->setCurrentIndex(2);


    //Pour chaque profil, `m_state` passe à 1 ce qui empêche notamment de pouvoir les déplacer/cliquer pendant le phase de comparaison.
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
// Gère également le "déplacement" des profils dans la grille (en réalité une nouvelle grille est créée à la place de l'ancienne)


void MainWindow::connexionProfil(Profil* A, Profil* B){

    std::vector<Profil *>::iterator itA = std::find(profilsOrdre.begin(), profilsOrdre.end(), A);
    std::vector<Profil *>::iterator itB = std::find(profilsOrdre.begin(), profilsOrdre.end(), B);

if ( ((A->getPhysique()>=B->getPhysique()) && (A->getPsycho()>=B->getPsycho()) && (A->getNutri()>=B->getNutri()) && (A->getCog()>= B->getCog()) && itA<itB)
     ||
     ((A->getPhysique()<=B->getPhysique()) && (A->getPsycho()<=B->getPsycho()) && (A->getNutri()<=B->getNutri()) && (A->getCog()<= B->getCog()) && itA>itB)

        )
{ui->label->setText("Comparaison impossible !");
ui->label->setStyleSheet("QLabel{color:rgb(255, 0, 0)}");}


// SI LA COMPARAISON EST VALIDE :
// La grille contenant les profils est recréée dans l'ordre qui découle du déplacement de profil.
// Cela signifie qu'il faut réorganiser les listes indicesOrdre et profilsOrdre


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
                                if (row==LIGNE_MAX)
                                {if ((col<8))

                                   { ui->gridLayout->addWidget(info,row,col+1);}}


                                else {ui->gridLayout->addWidget(info,row,col+1);}
                                if (col<8){col +=2;}
                                else {col=0; row +=1;}
                    }

                     return;
           }
}}}}}
