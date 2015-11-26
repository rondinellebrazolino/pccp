#ifndef TBUSCALOCAL_H
#define TBUSCALOCAL_H

#include <list>
#include "TInstancia.h"
#include "THMelhorPar.h"
#include "THPareto.h"


class TBuscaLocal
{
  public:
    TBuscaLocal(TInstancia* _pccp);
    virtual ~TBuscaLocal();

    void blHMPR(THMelhorPar* s, float pRand, int corte);
    void buscaLoc_HMPR(THMelhorPar* s, float pRand);

    void blHMPP(THPareto* s, float pRand, int corte);
    void buscaLoc_HMPP(THPareto* s, float pRand);

  protected:
  private:

    TInstancia* pccp;

    ///struct auxiliar de ordenação da lista de monitores
    struct ordMonitores{
      bool operator()(int* m1, int* m2){
        return m1[1]>m2[1];///mais caro na frente
      }
    };
};

#endif // TBUSCALOCAL_H
