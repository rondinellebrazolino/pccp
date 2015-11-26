#ifndef TPARESPARETO_H
#define TPARESPARETO_H


#include <vector>
#include <list>
#include <cstdlib>


#include "TInstancia.h"
#include "TPar.h"
#include "TBranch.h"
#include "TMonitor.h"


///-------------------- Classe: TPARESPARETO --------------------//
///
///  Representa a construção de uma lista de pareto
///  (cobertura/custo) dos pares de um branch
///
///--------------------------------------------------------------//

class TParesPareto
{
  public:
    TParesPareto();
    virtual ~TParesPareto();


    TPar* novoCandidato(TPar *candidato);
    void otimizarLC(TPar *escolhido);
    TPar* selecionarRand(float pRand);
    void criarLC(std::list<TPar*> lstPares, std::vector<bool>vetM);



    std::list<TPar*> pares;

  protected:
  private:

    struct ordenarPares{
      bool operator()(TPar* p1, TPar* p2){
        float c1= p1->getCustoPar()==0? 9999999999: p1->getCustoPar();
        float c2= p2->getCustoPar()==0? 9999999999: p2->getCustoPar();
        return (c1/p1->getAlcance())<(c2/p2->getAlcance());
      }
    };

};

#endif // TPARESPARETO_H
