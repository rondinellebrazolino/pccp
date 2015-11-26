#include "../include/TBranch.h"

#include <iostream>
#include <iterator>

TBranch::TBranch(int bId){
  //ctor
  id = bId;
}

TBranch::~TBranch(){
  //dtor
  ///limpa a lista de pares
  pares.clear();

}

///Retorna o id do branch
int TBranch::getId(){
  return id;
}

///adiciona um par � lista de pares do branch
void TBranch::addPar(TPar* p){
  pares.push_back(p);
}

///retorna quantos pares est�o cobrindo o branch
int TBranch::getCobertura(){
  return pares.size();
}

///retorna uma c�pia da lista de pares que cobrem o branch
std::list<TPar*> TBranch::getLstPares(){
  std::list<TPar*> lst(pares);
  return lst;
}

///ordena e remove duplica��es na lista de pares do branch
void TBranch::ordenarLstPares(){
  pares.sort(ordenarPares());
  pares.unique(compararPares());
}
