#include "../include/TPar.h"
#include "../include/TMonitor.h"
#include "../include/TBranch.h"
#include <iostream>

TPar::TPar(int m1, int m2){
  //ctor
  int aux= m1;
  if(m1>m2){
    m1= m2;
    m2= aux;
  }
  m1Id= m1;
  m2Id= m2;
  custo=0;
  alcance=0;

}

TPar::~TPar(){
  //dtor
  branchs.clear();
}

int TPar::getM1(){
  return m1Id;
}

int TPar::getM2(){
  return m2Id;
}

std::list<int> TPar::getLstBranchs(){
  std::list<int> lst(branchs);
  return lst;
}

int TPar::getAlcance(){
  return alcance;
}

int TPar::getCustoPar(){
  return custo;
}

void TPar::setCustoPar(int c){
  custo=c;
}

void TPar::setAlcance(int a){
  alcance=a;
}

void TPar::addBranch(int bId){
  branchs.push_back(bId);
}


void TPar::ordenarLstBranchs(){
  branchs.sort(ordenarBranchs());
  branchs.unique(compararBranchs());
}
