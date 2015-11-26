#ifndef TINSTANCIA_H
#define TINSTANCIA_H

#include <iostream>
#include <vector>
#include <list>

#include "TBranch.h"

class TBranch;
class TMonitor;
class TPar;



///-------------------- Classe: TINSTANCIA --------------------//
///
///  Representa uma instancia com suas propriedades
///  e métodos de acesso
///
///-----------------------------------------------------------//


class TInstancia
{
  public:
    TInstancia();
    //TInstancia(int bSize, int mSize, int pSize, int bMax, int mMax);
    virtual ~TInstancia();

    void show();

    TPar* getPar(int m1, int m2);
    TMonitor* getMonitor(int mId);
    TBranch* getBranch(int bId);

    std::vector<TBranch*> getVetBranchs();
    std::vector<TMonitor*> getVetMonitores();
    std::vector<TPar*> getVetPares();

    int getNSize();
    int getBSize();

    TBranch* addBranch(int bId);
    TMonitor* addMonitor(int mId);
    TPar* addPar(int m1, int m2);

    bool monitorCobreBranch(int mId, int bId);
    bool existePar(int m1Id, int m2Id);

    void loadFile(std::string path);
    void exportar();

    std::string getNome();



  protected:
  private:
    int bSize;///nº de branchs que precisam ser cobertos na instancia
    int nSize;///valor máximo dos id's de branchs e monitores existentes na instancia
    std::vector<TBranch*> vetB;///vetor de branchs
    std::vector<TMonitor*> vetM;///vetor de monitores
    std::vector<TPar*> vetP;///vetor de pares
    bool** vetMxB;///matriz de tamanho M x B que indica se o monitor mi cobre o branch bj

    std::string nome;


};

#endif // TINSTANCIA_H
