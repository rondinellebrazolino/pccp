#include "../include/TBuscaLocal.h"
#include "../include/TMonitor.h"
#include <climits>

TBuscaLocal::TBuscaLocal(TInstancia* _pccp){
  //ctor
  pccp = _pccp;
}

TBuscaLocal::~TBuscaLocal()
{
  //dtor
}


void TBuscaLocal::blHMPR(THMelhorPar* s, float pRand, int corte){

  std::vector<bool> m1(s->getMonitores());

  for(int i=0; i<m1.size();i++){
    if(!(m1[i])) continue;
    m1[i]= false;
    s->setMonitores(m1);
    s->construtivo(pRand,0,corte, false);
    if(s->verificarS() && s->getCustoTot()<corte){
      corte= s->getCustoTot();
      i=0;
    }else m1[i]= true;
  }

  s->setMonitores(m1);
}


void TBuscaLocal::blHMPP(THPareto* s, float pRand, int corte){

  std::vector<bool> m1(s->getMonitores());

  for(int i=0; i<m1.size();i++){
    if(!(m1[i])) continue;
    m1[i]= false;
    s->setMonitores(m1);
    s->construtivo(pRand,0,corte, false);
    if(s->verificarS() && s->getCustoTot()<corte){
      corte= s->getCustoTot();
      i=0;
    }else m1[i]= true;
  }

  s->setMonitores(m1);
}

void TBuscaLocal::buscaLoc_HMPR(THMelhorPar* s, float pRand){
  int custo;

  do{
    custo = s->getCustoTot();
    blHMPR(s,pRand,custo);
  }while(s->getCustoTot()<custo);

}


void TBuscaLocal::buscaLoc_HMPP(THPareto* s, float pRand){
  int custo;

  do{
    custo = s->getCustoTot();
    blHMPP(s,pRand,custo);
  }while(s->getCustoTot()<custo);

}

