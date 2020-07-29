#ifndef PROFIL_H
#define PROFIL_H
#include <QPushButton>
#include <QSignalMapper>
#include <QMainWindow>

class Profil : public QPushButton
{
    Q_OBJECT


public:

    Profil(int ligne, int colonne, int numero, int crit1, int crit2, int crit3, int crit4);

    int getRow() const { return m_row; } ;
    int getCol() const { return m_col; } ;
    int getNumber() const {return m_numero;} ;
    int getPhysique () const {return m_crit1;} ;
    int getPsycho () const {return m_crit2;} ;
    int getNutri () const {return m_crit3;} ;
    int getCog () const {return m_crit4;} ;
    int gerCrit5() const {return m_crit5;} ;
    int gerCrit6() const {return m_crit6;} ;

    void mouseMoveEvent(QMouseEvent *e) override;
     //void mousePressEvent(QMouseEvent *e) override;
   void dropEvent(QDropEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    // Attributs (doivent toujours être privés)

    private :
        int m_numero;
        int m_row;
        int m_col;


        int m_crit1;
        int m_crit2;
        int m_crit3; 
        int m_crit4;
        int m_crit5;
        int m_crit6;
      public :
        int m_clicked;
        int m_state;

};

#endif // PROFIL_H
