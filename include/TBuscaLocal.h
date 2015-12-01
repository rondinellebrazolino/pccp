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

    void blHMPP(THPareto* s, float pRandB, float pRandP, int corte, bool gravarLog);
    void buscaLoc_HMPP(THPareto* s, float pRandB, float pRandP);

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
