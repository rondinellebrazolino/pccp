#include "../include/TMonitor.h"
#include "../include/TPar.h"
#include "../include/TBranch.h"

#include <iostream>

TMonitor::TMonitor(int mId){
  //ctor
  id = mId;
}

TMonitor::~TMonitor(){
  //dtor

}

///Retorna o id do monitor
int TMonitor::getId(){
  return id;
}

///retorna o custo do monitor
int TMonitor::getCusto(){
  return custo;
}

///Atualiza o custo do monitor
void TMonitor::setCusto(int c){
  custo = c;
}
