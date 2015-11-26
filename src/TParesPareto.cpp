#include "../include/TParesPareto.h"

TParesPareto::TParesPareto(){
  //ctor
}

TParesPareto::~TParesPareto(){
  //dtor
  pares.clear();
}


///Tendo um par como entrada, verifica se ele pode fazer parte da lista de candidatos
///segundo uma análise de Pareto
TPar* TParesPareto::novoCandidato(TPar *candidato){
  bool venceu=true;
  for(std::list<TPar*>::const_iterator it=pares.begin(); it!=pares.end() && venceu;it++){
    TPar *p = (TPar*) *it;

    venceu= (candidato->getCustoPar()<=p->getCustoPar())||
      (candidato->getAlcance()>=p->getAlcance());
  }

  if(venceu) return candidato;

  return NULL;
}


///à partir de um par selecionado, atualiza a lista de candidatos removendo os pares
///dominados pelo par escolhido
void TParesPareto::otimizarLC(TPar *escolhido){

  for(std::list<TPar*>::iterator it=pares.begin(); it!=pares.end();it++){
    TPar *p = (TPar*) *it;

    if ((escolhido->getCustoPar()<p->getCustoPar())
         &&(escolhido->getAlcance()>p->getAlcance())){
      pares.erase(it);
      it--;
    }
  }

  pares.push_back(escolhido);

}

///Seleção do par através de um parametro de randomização
TPar* TParesPareto::selecionarRand(float pRand){

  int limite = pares.size()*pRand;
  if(limite<1) return pares.front();

  int ix = rand()%limite;

  TPar *p = pares.front();
  for(std::list<TPar*>::iterator it=pares.begin();it!=pares.end() && ix>0;it++){
    ix--;
    p = (TPar*) *it;
    it++;
  }
  return p;

}

///Criação da lista de candidatos:
///Recebe a lista que contém os elementos a serem classificados segundo a distribuição de Pareto
///Recebe um vector de Monitores na solução para análise dos parâmetros
void TParesPareto::criarLC(std::list<TPar*> lstPares, std::vector<bool>vetM){
  pares.clear();
  ///loop nos pares da instancia p/ criar a lista de Pareto dos pares
  //for(int i=0; i<lstPares.size();i++){
  for(std::list<TPar*>::iterator it=lstPares.begin();it!=lstPares.end();it++){
    TPar *p = *it;
    if(p==NULL || (vetM.at(p->getM1()) && vetM.at(p->getM2()))) continue;
    TPar *vencedor = novoCandidato(p);
    if(vencedor!=NULL) otimizarLC(vencedor);

  }

  pares.sort(ordenarPares());

}

