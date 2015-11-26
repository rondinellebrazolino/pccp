#ifndef TBRANCH_H
#define TBRANCH_H

#include <vector>
#include <list>

#include "../include/TPar.h"



///-------------------- Classe: TBRANCH --------------------//
///
///  Representa um branch com suas propriedades
///  e métodos de acesso
///
///--------------------------------------------------------//

class TBranch
{
  public:
    TBranch(int bId);
    virtual ~TBranch();
    int getId();
    void addPar(TPar* p);
    int getCobertura();

    void ordenarLstPares();
    std::list<TPar*> getLstPares();

  protected:
  private:
    int id;///id do branch
    std::list<TPar*> pares;///lista de pares que cobrem o branch


    ///struct auxiliar para ordenação da lista de pares do branch
    struct ordenarPares{
      bool operator()(TPar* p1, TPar* p2){
        bool ret;
        ///critério: menor id do 1º monitor e depois menor id do 2º monitor do par
        if(p1->getM1()==p2->getM1()) ret= (p1->getM2()<p2->getM2());
        else ret= (p1->getM1()<p2->getM1());
        return ret;
      }
    };

    ///struct de comparação dos pares que cobrem o branch
    struct compararPares{
      bool operator()(TPar* p1, TPar* p2){
        return p1->getM1()==p2->getM1() && p1->getM2()==p2->getM2();
      }
    };
};

#endif // TBRANCH_H
