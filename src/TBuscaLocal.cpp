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


void TBuscaLocal::blHMPP(THPareto* s, float pRandB, float pRandP, int corte, bool gravarLog){

  THPareto* sEstrela= s->criarCopia();///obtém uma cópia da solução corrente
  for(int i=0; i<pccp->getNSize(); i++){///loop nos monitores da instancia
    if(pccp->getMonitor(i)==NULL) continue;
    THPareto* sLinha= s->criarCopia();///solução temporária
    sLinha->mudarUmMonitor(i);///flip no monitor i
    if(sLinha->getMonitores().at(i)) sLinha->limpeza2S();///se o monitor foi colocado na sol., efetua a limpeza
    else sLinha->construtivo(pRandB,pRandP,corte,gravarLog);///caso contrário executa novamente o construtivo
    std::cout<<sLinha->getCustoTot()<<"\\"<<sEstrela->getCustoTot()<<"\n";
    if(sEstrela->getCustoTot() > sLinha->getCustoTot() && sLinha->verificarS()){///se o custo melhorou, atualiza a sol.
      sEstrela->zerarSolucao();
      sEstrela= sLinha->criarCopia();
    }
    //delete sLinha;
  }//fim para i

  s->zerarSolucao();
  s= sEstrela->criarCopia();

  //delete sEstrela;

}

void TBuscaLocal::buscaLoc_HMPP(THPareto* s, float pRandB, float pRandP){
  int custo=0;

  //do{
    //custo = s->getCustoTot();
    blHMPP(s,pRandB,pRandP,custo,false);
  //}while(s->getCustoTot()<custo);

}

