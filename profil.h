#ifndef PROFIL_H
#define PROFIL_H
#include <QPushButton>
#include <QSignalMapper>
#include <QMainWindow>

class Profil : public QPushButton
{
    Q_OBJECT


public:

    Profil(int ligne, int colonne, int numero,std::vector<int> crits);

    int getRow() const { return m_row; } ;
    int getCol() const { return m_col; } ;
    int getNumber() const {return m_numero;} ;
    int getPhysique () const {return m_crits[0];} ;
    int getPsycho () const {return m_crits[1];} ;
    int getNutri () const {return m_crits[2];} ;
    int getCog () const {return m_crits[3];} ;
    int gerCrit5() const {return m_crits[4];} ;
    int gerCrit6() const {return m_crits[5];} ;

    void mouseMoveEvent(QMouseEvent *e) override;
   void dropEvent(QDropEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    // Attributs (doivent toujours être privés)

    private :
        int m_numero;
        int m_row;
        int m_col;

        std::vector<int> m_crits;

      public :
        int m_clicked;
        int m_state;

};

#endif // PROFIL_H
