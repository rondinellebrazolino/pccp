#include "../include/THMelhorPar.h"
#include "../include/TMonitor.h"

#include <climits>
#include <iostream>






THMelhorPar::THMelhorPar(TInstancia* _pccp){
  //ctor
  ///inicialização das variáveis
  pccp= _pccp;
  monitores.resize(pccp->getNSize(),false);
  branchs.resize(pccp->getNSize(),false);
  cobertura.resize(pccp->getNSize(),0);
  custoTotal=0;///custo zero
  bSize=0;///zero branchs cobertos na solução


  ///loop nos pares da instancia
  for(int i=0; i<pccp->getNSize();i++)
    for(int j=i; j<pccp->getNSize();j++)
      ///atualiza o "custo" do par
      if(pccp->existePar(i,j)) pccp->getPar(i,j)->setCustoPar(pccp->getMonitor(i)->getCusto()+pccp->getMonitor(j)->getCusto());

}

THMelhorPar::THMelhorPar(TInstancia* _pccp, std::vector<bool> vetM, std::vector<bool> vetB){
  //ctor
  ///inicialização das variáveis
  pccp= _pccp;

  std::vector<bool>v1(vetM);
  std::vector<bool>v2(vetB);
  monitores= v1;
  branchs= v2;
  custoTotal=0;
  bSize=0;


  ///loop nos pares da instancia
  for(int i=0; i<pccp->getNSize();i++){
    custoTotal+= monitores[i]?pccp->getMonitor(i)->getCusto():0;
    bSize+= branchs[i]?1:0;
    for(int j=i; j<pccp->getNSize();j++)
      ///atualiza o "custo" do par
      if(pccp->existePar(i,j)) pccp->getPar(i,j)->setCustoPar(pccp->getMonitor(i)->getCusto()+pccp->getMonitor(j)->getCusto());
  }
}

THMelhorPar::~THMelhorPar(){
  //dtor
  monitores.clear();
  branchs.clear();
  cobertura.clear();

}
/*
///Cria a lista de branchs candidatos
std::list<int*> THMelhorPar::criarLcBranchs(){
  std::list<int*> lc;///lista inicialmente vaiza

  for(int i=0; i<pccp->getNSize(); i++){
    if(pccp->getBranch(i)==NULL || branchs[i]) continue;///ignora branchs que não fazem parte do problema ou que já estão cobertos
    int* v= new int[2];
    v[0] = i;///posição 1: id do branch
    v[1] = pccp->getBranch(i)->getCobertura();///posição 2: nº de pares que cobrem o branch
    lc.push_back(v);///insere branch na lista
  }

  lc.sort(ordBranchs());///ordena a lista

  return lc;
}

///cria a lista de pares candidatos
std::list<int*> THMelhorPar::criarLcPares(int bId){
  std::list<int*> lc;///lista inicialmente vazia

  if(pccp->getBranch(bId)==NULL) return lc;

  ///loop nos pares que cobrem o branch
  std::list<TPar*> pares=pccp->getBranch(bId)->getLstPares();
  for(std::list<TPar*>::iterator it=pares.begin();it!=pares.end();it++){
    TPar *p= *it;
    if((monitores[p->getM1()] && monitores[p->getM2()])|| p->getCustoPar()<1) continue;///ignora pares que já estão na solução
    int* v= new int[4];
    v[0]= p->getM1();///posição 1: id do monitor 1 do par
    v[1]= p->getM2();///posição 2: id do monitor 2 do par
    v[2]= p->getAlcance();///posição 3: nº de branchs cobertos pelo par
    v[3]= p->getCustoPar();///posição 4: "custo" do par
    lc.push_back(v);
  }

  lc.sort(ordPares());///ordena os pares

  return lc;
}

///seleciona um branch pra entrar na solução
int THMelhorPar::selecionarBranch(float pRand){
  std::list<int*>lc = criarLcBranchs();///gera a lista de candidatos
  if(lc.size()<1) return 0;

  int limite = lc.size()*pRand;
  if(limite<1) return lc.front()[0];

  int ix= rand()%limite;

  int* v= new int[2];
  for(std::list<int*>::iterator it=lc.begin();it!=lc.end() && ix>0;it++){
    v= (int*) *it;
    ix--;
    it++;
  }
  return v[0];///posição 1 da lista de canditatos: id do branch

}

///seleciona um par pra entrar na solução
int* THMelhorPar::selecionarPar(int bId, float pRand){
  std::list<int*>lc = criarLcPares(bId);///gera a lista de candidatos
  int* v= new int[4];
  v[0]=v[1]=v[2]=v[3]=0;

  if(lc.size()<1) return v;

  v= lc.front();

  int limite = lc.size()*pRand;
  if(limite<1) return v;

  int ix= rand()%limite;

  for(std::list<int*>::iterator it=lc.begin();it!=lc.end() && ix>0;it++){
    v= (int*) *it;
    ix--;
    it++;
  }
  return v;///retorna o vetor de 4 posições com informações do par escolhido

}


///marca na solução como cobertos os branchs de um determinado par
void THMelhorPar::cobrirBranchsPar(int m1Id, int m2Id){
  TPar* p= pccp->getPar(m1Id,m2Id);
  if(p!=NULL){
    std::list<int> b= p->getLstBranchs();
    ///loop na lista de branchs que o par cobre
    for(std::list<int>::iterator it=b.begin();it!=b.end();it++){
      int ix= *it;
      if(branchs[ix]) continue;///ignora os branchs que já estão na solução
      cobertura.at(ix)++;
      branchs[ix]= true;///marca o branch como coberto
      bSize++;///incrementa o contador de branchs cobertos
    }
  }
}

void THMelhorPar::atualizarPar(int m1Id, int m2Id){
  TPar* p= pccp->getPar(m1Id,m2Id);
  if(p==NULL) return;
  int c=0;
  std::list<int> b= p->getLstBranchs();
  ///loop na lista de branchs que o par cobre
  for(std::list<int>::iterator it=b.begin();it!=b.end();it++){
    int b= *it;
    if(branchs[b]) c++;///branch que o par cobre já está coberto na solução
  }

  p->setAlcance(c);///atualiza o alcance do par
  int custo= monitores[m1Id]?0:pccp->getMonitor(m1Id)->getCusto();
  custo+= monitores[m2Id]?0:pccp->getMonitor(m2Id)->getCusto();
  p->setCustoPar(custo);
}

///atualiza a solução à partir de um novo par selecionado para ser colocado nela
void THMelhorPar::atualizarS(int m1Id, int m2Id){
  TPar* p= pccp->getPar(m1Id,m2Id);

  if(p!=NULL){
    cobrirBranchsPar(m1Id,m2Id);///cobre os branchs do par
    ///atualiza o custo total da solução
    if(!(monitores[m1Id])) custoTotal+= pccp->getMonitor(m1Id)->getCusto();
    if(!(monitores[m2Id])) custoTotal+= pccp->getMonitor(m2Id)->getCusto();
    ///marca os monitores do par como ativos na solução
    monitores[m1Id]=true;
    monitores[m2Id]=true;

    ///verifica a formação de novos pares à partir do par atual com os monitores que já estão na solução
    ///e faz a atualização de cobertura
    for(int i=0; i<pccp->getNSize(); i++){
      TPar *p= pccp->getPar(m1Id,i);
      if(p!=NULL) p->setCustoPar(p->getCustoPar()-pccp->getMonitor(m1Id)->getCusto());
      p= pccp->getPar(m2Id,i);
      if(p!=NULL) p->setCustoPar(p->getCustoPar()-pccp->getMonitor(m2Id)->getCusto());
      if(!(monitores[i])) continue;
      cobrirBranchsPar(m1Id,i);
      cobrirBranchsPar(m2Id,i);
    }//fim para I

    ///loop nos pares
    for(int i=0; i<pccp->getNSize(); i++)
      for(int j=i; j<pccp->getNSize(); j++)
        if(pccp->getPar(i,j)!=NULL && pccp->getPar(i,j)->getAlcance()>0 && pccp->getPar(i,j)->getCustoPar()>0)
          atualizarPar(i,j);

  }//fim se p!=NULL
}

///verificação rápida da solução (comparação de contadores)
bool THMelhorPar::verificarS(){
  return bSize>=pccp->getBSize();
}


///construtivo
void THMelhorPar::construtivo(float pRandB, float pRandP, int corte){
  int limite= pccp->getNSize();///limite de tentativas
  if(corte<1) corte= INT_MAX;///valor de corte para interrupção
  while(!verificarS() && limite>0 && custoTotal<corte){
    int b= selecionarBranch(pRandB);
    int* p= selecionarPar(b,pRandP);
    if(p[0]!=0 && p[1]!=0)atualizarS(p[0],p[1]);
    limite--;
  }

  //std::cout<<"Executando limpeza\n";
  //limpezaS();
}

///retorna o nro de branchs cobertos na solução corrente
int THMelhorPar::getBSize(){
  return bSize;
}

///retorna o nro de monitores ativos na solução corrente
int THMelhorPar::getNroMonitores(){
  int c=0;
  for(int i=0; i<pccp->getNSize(); i++)
    if(monitores[i]) c++;
  return c;
}

///retorna o custo total da solução corrente
int THMelhorPar::getCustoTot(){
  return custoTotal;
}

///retorna uma cópia do vetor de monitores da solução
std::vector<bool> THMelhorPar::getMonitores(){
  std::vector<bool> v(monitores);
  return v;
}


///marca como inativo um monitor na solução corrente
void THMelhorPar::removerMonitor(int mId){

  if(!monitores[mId] || pccp->getMonitor(mId)==NULL) return;

  ///loop nos branchs da instancia
  for(int i=0; i<pccp->getNSize(); i++){
    TBranch* b= pccp->getBranch(i);
    if(b==NULL) continue;
    std::list<TPar*> lstP= b->getLstPares();
    bool ok=false;
    ///loop nos pares que cobrem o branch
    for(std::list<TPar*>::iterator it= lstP.begin(); it!=lstP.end();it++){
      TPar* p= *it;
      ok= (p->getM1()!=mId && p->getM2()!=mId)&&(monitores[p->getM1()]&& monitores[p->getM2()]);
      if(ok) break;///se o par não possui o monitor a ser desligado como um dos componentes, marca como coberto o branch
    }
    branchs[b->getId()]= ok;
  }//fim para I

  ///atualiza o nro de branchs cobertos na solução corrente
  bSize=0;
  for(int i=0; i<pccp->getNSize(); i++){
    if(branchs[i]) bSize++;
  }//fim para I

  monitores[mId]= false;///desativa o monitor na solução corrente
  custoTotal-= pccp->getMonitor(mId)->getCusto();///atualiza o custo total da solução corrente

}

///atualiza o vetor de monitores da solução
void THMelhorPar::setMonitores(std::vector<bool> v){

  for(int i=0; i<pccp->getNSize(); i++)
    monitores[i]= v[i];

  ///reset dos valores da instancia
  bSize=0;
  custoTotal=0;
  for(int i=0; i<pccp->getNSize();i++){
    branchs[i]= false;
    cobertura[i]=0;
    if(monitores[i] && pccp->getMonitor(i)!=NULL) custoTotal+= pccp->getMonitor(i)->getCusto();
  }

  ///loop nos branchs da instancia
  for(int i=0; i<pccp->getNSize();i++){
    TBranch* b= pccp->getBranch(i);
    if(b==NULL) continue;
    std::list<TPar*> lstP= b->getLstPares();
    ///loop nos pares que cobrem o branch
    for(std::list<TPar*>::iterator it=lstP.begin(); it!=lstP.end();it++){
      TPar* p= *it;
      ///se o par possui os monitores ativos no novo vetor, marca o branch como coberto
      if(monitores[p->getM1()] && monitores[p->getM2()]) branchs[i]= true;
    }//fim iterator IT
    if(branchs[i]) bSize++;///se o branch foi coberto, incrementa o contador da solução corrente
  }//fim para I
}

///executa uma limpeza gulosa na solução
void THMelhorPar::limpezaS(){

  std::list<int*> lc;
  for(int i=0; i<pccp->getNSize();i++){
    if(!(monitores[i]) || pccp->getMonitor(i)==NULL) continue;
    int* v= new int[2];
    v[0]= i;///id do monitor
    v[1]= pccp->getMonitor(i)->getCusto();///custo do monitor
    lc.push_back(v);
  }

  lc.sort(ordMonitores());

  for(std::list<int*>::iterator it= lc.begin(); it!=lc.end();it++){
    int* m= *it;
    THMelhorPar *s = new THMelhorPar(pccp);
    s->setMonitores(monitores);
    s->removerMonitor(m[0]);
    if(s->verificarS() && s->getCustoTot()<custoTotal){
      monitores[m[0]]= false;
    }
    //std::cout<<"Custo: "<<s->getCustoTot()<<" "<<s->getBSize()<<"/"<<bSize<<"\n";
    delete s;
  }

  setMonitores(monitores);

}


///verificador completo da solução
bool THMelhorPar::verificadorFull(){
  int qtdeB= 0;///conta quantos branchs precisam ser cobertos
  for(int i=0; i<pccp->getNSize();i++){
    qtdeB+= pccp->getBranch(i)==NULL?0:1;
    branchs[i]=false;
  }

  ///pra cada monitor
  for(int i=0; i<pccp->getNSize();i++){
    if(!(monitores[i])) continue;///se monitor não está na solução, ignora
    for(int j=i; j<pccp->getNSize();j++){
      if(!(monitores[j])) continue;
      TPar* p= pccp->getPar(i,j);
      if(p==NULL) continue;
      std::list<int> b= p->getLstBranchs();
      ///loop nos branchs que o par cobre
      for(std::list<int>::iterator it=b.begin();it!=b.end();it++){
        int indice= *it;
        branchs[indice]= true;///marca o branch como coberto na solução corrente
      }
    }//fim para j
  }//fim para i

  ///quanto quantos branchs foram cobertos e quantos não foram
  int qtdeS= 0;
  int naoCobertos= 0;
  for(int i=0; i<pccp->getNSize();i++){
    qtdeS+= (branchs[i])?1:0;
    if(pccp->getBranch(i)!=NULL) naoCobertos+= (branchs[i])?0:1;
  }

  std::cout<<qtdeS<<"/"<<qtdeB<<" "<<naoCobertos<<"\n";

  ///retorna true caso a cobertura da instancia seja completa, dados os monitores de entrada
  ///retora false caso contrário
  return (naoCobertos<1)&&(qtdeB==qtdeS);
}


void THMelhorPar::exportar(){

  std::cout << "\tExportando arquivo de log da saida" <<std::endl;

  std::ofstream fout("saida_log.pccp", std::ios::trunc);///arquivo de destino

  if(fout.is_open() && fout.good()){
    ///vetor de monitores
    for(int i=0;i<pccp->getNSize();i++){
      if(!(monitores[i])) continue;
      fout <<i<<std::endl;
    }

    ///fecha os arquivos
    fout.close();
    std::cout << "\t>>Exportacao concluida - saida_log.pccp" <<std::endl;
  }else{///fim se abriu os arquivos
    std::cout << "\t>>Falha ao exportar o arquivo" <<std::endl;
  }

}
*/



///Cria a lista de branchs candidatos
std::list<int*> THMelhorPar::criarLcBranchs(){
  std::list<int*> lc;///lista inicialmente vaiza

  for(int i=0; i<pccp->getNSize(); i++){
    if(pccp->getBranch(i)==NULL || branchs[i]) continue;///ignora branchs que não fazem parte do problema ou que já estão cobertos
    int* v= new int[2];
    v[0] = i;///posição 1: id do branch
    v[1] = pccp->getBranch(i)->getCobertura();///posição 2: nº de pares que cobrem o branch
    lc.push_back(v);///insere branch na lista
  }

  lc.sort(ordBranchs());///ordena a lista

  return lc;
}

std::list<TPar*> THMelhorPar::criarLcPares(int bId){
  std::list<TPar*> lc;///lista inicialmente vazia
  if(pccp->getBranch(bId)==NULL) return lc;

  ///loop nos pares do branch
  std::list<TPar*> pares= pccp->getBranch(bId)->getLstPares();
  for(std::list<TPar*>::iterator it=pares.begin(); it!=pares.end(); it++){
    TPar* p= *it;
    int custo= (monitores[p->getM1()])?0:pccp->getMonitor(p->getM1())->getCusto();
    custo+= (monitores[p->getM2()])?0:pccp->getMonitor(p->getM2())->getCusto();
    p->setCustoPar(custo);
    lc.push_back(p);
  }


  lc.sort(ordPares());

  return lc;///retorna a lista de pares de pareto do branch
}

///seleciona um branch pra entrar na solução
int THMelhorPar::selecionarBranch(float pRand, bool gravarLog){
  std::list<int*>lc = criarLcBranchs();///gera a lista de candidatos
  if(lc.size()<1) return 0;

  if(gravarLog){
    logFile<<"listaB["<<lc.size()<<"]: ";
    for(std::list<int*>::iterator it=lc.begin();it!=lc.end();it++){
      int* v= new int[2];
      v = (int*) *it;
      logFile<<v[0]<<" ";
    }
    logFile<<"\n";
  }

  int limite = lc.size()*pRand;
  if(limite<1) return lc.front()[0];

  int ix= rand()%limite;

  int* v= new int[2];
  for(std::list<int*>::iterator it=lc.begin();it!=lc.end() && ix>0;it++){
    v= (int*) *it;
    ix--;
    it++;
  }
  return v[0];///posição 1 da lista de canditatos: id do branch

}

///seleciona um par pra entrar na solução
TPar* THMelhorPar::selecionarPar(int bId, float pRand, bool gravarLog){

  std::list<TPar*>lc = criarLcPares(bId);///gera a lista de candidatos

  if(lc.size()<1) return NULL;

  if(gravarLog){
    logFile<<"listaP["<<lc.size()<<"]: ";
    for(std::list<TPar*>::iterator it=lc.begin();it!=lc.end();it++){
      TPar* p= *it;
      logFile<<p->getM1()<<","<<p->getM2()<<" ";
    }
    logFile<<"\n";
  }

  TPar* p= lc.front();

  int limite = lc.size()*pRand;
  if(limite<1) return p;

  int ix= rand()%limite;

  for(std::list<TPar*>::iterator it=lc.begin();it!=lc.end() && ix>0;it++){
    p= *it;
    ix--;
    it++;
  }
  return p;///retorna o par selecionado

}


///marca na solução como cobertos os branchs de um determinado par
void THMelhorPar::cobrirBranchsPar(int m1Id, int m2Id){
  TPar* p= pccp->getPar(m1Id,m2Id);
  if(p!=NULL){
    std::list<int> b= p->getLstBranchs();
    ///loop na lista de branchs que o par cobre
    for(std::list<int>::iterator it=b.begin();it!=b.end();it++){
      int ix= *it;
      if(branchs[ix]) continue;///ignora os branchs que já estão na solução
      cobertura.at(ix)++;
      branchs[ix]= true;///marca o branch como coberto
      bSize++;///incrementa o contador de branchs cobertos
    }
    p->setCustoPar(0);
    p->setAlcance(0);
  }
}


///atualiza a solução à partir de um novo par selecionado para ser colocado nela
void THMelhorPar::atualizarS(int m1Id, int m2Id){
  TPar* p= pccp->getPar(m1Id,m2Id);

  if(p!=NULL){
    cobrirBranchsPar(m1Id,m2Id);///cobre os branchs do par
    ///atualiza o custo total da solução
    if(!(monitores[m1Id])) custoTotal+= pccp->getMonitor(m1Id)->getCusto();
    if(!(monitores[m2Id])) custoTotal+= pccp->getMonitor(m2Id)->getCusto();
    ///marca os monitores do par como ativos na solução
    monitores[m1Id]=true;
    monitores[m2Id]=true;

    ///verifica a formação de novos pares à partir do par atual com os monitores que já estão na solução
    ///e faz a atualização de cobertura
    for(int i=0; i<pccp->getNSize(); i++){
      TPar *p= pccp->getPar(m1Id,i);
      if(p!=NULL) p->setCustoPar(p->getCustoPar()-pccp->getMonitor(m1Id)->getCusto());
      p= pccp->getPar(m2Id,i);
      if(p!=NULL) p->setCustoPar(p->getCustoPar()-pccp->getMonitor(m2Id)->getCusto());
      if(!(monitores[i])) continue;
      cobrirBranchsPar(m1Id,i);
      cobrirBranchsPar(m2Id,i);
    }//fim para I

  }//fim se p!=NULL
}

///verificação rápida da solução (comparação de contadores)
bool THMelhorPar::verificarS(){
  return bSize>=pccp->getBSize();
}


///construtivo
void THMelhorPar::construtivo(float pRandB, float pRandP, int corte, bool gravarLog){
  int limite= pccp->getNSize();///limite de tentativas
  if(corte<1) corte= INT_MAX;///valor de corte para interrupção
  if(gravarLog){
    std::string nomeF= pccp->getNome()+"_log_exec_mpar.pccp";
    logFile.open(nomeF.c_str(), std::ios::trunc);///arquivo log detalhado
  }
  while(!verificarS() && limite>0 && custoTotal<corte){
    int b= selecionarBranch(pRandB, gravarLog);
    if(gravarLog) logFile<<"Escolheu B: "<<b<<"\n";
    TPar* p= selecionarPar(b,pRandP,gravarLog);
    if(gravarLog)
      if(p!=NULL) logFile<<"Escolheu P: "<<p->getM1()<<","<<p->getM2()<<"\n";
      else logFile<<"NULL\n";
    if(p!=NULL)atualizarS(p->getM1(),p->getM2());
    limite--;
  }

  //std::cout<<"Executando limpeza\n";
  //limpezaS();
}


///retorna o nro de branchs cobertos na solução corrente
int THMelhorPar::getBSize(){
  return bSize;
}

///retorna o nro de monitores ativos na solução corrente
int THMelhorPar::getNroMonitores(){
  int c=0;
  for(int i=0; i<pccp->getNSize(); i++)
    if(monitores[i]) c++;
  return c;
}

///retorna o custo total da solução corrente
int THMelhorPar::getCustoTot(){
  return custoTotal;
}

///retorna uma cópia do vetor de monitores da solução
std::vector<bool> THMelhorPar::getMonitores(){
  std::vector<bool> v(monitores);
  return v;
}

///marca como inativo um monitor na solução corrente
bool THMelhorPar::removerMonitor(int mId, bool comInterrupt){

  if(!monitores[mId] || pccp->getMonitor(mId)==NULL) return true;

  ///loop nos branchs da instancia
  for(int i=0; i<pccp->getNSize(); i++){
    TBranch* b= pccp->getBranch(i);
    if(b==NULL) continue;
    std::list<TPar*> lstP= b->getLstPares();
    bool ok=false;
    ///loop nos pares que cobrem o branch
    for(std::list<TPar*>::iterator it= lstP.begin(); it!=lstP.end();it++){
      TPar* p= *it;
      ok= (p->getM1()!=mId && p->getM2()!=mId)&&(monitores[p->getM1()]&& monitores[p->getM2()]);
      if(ok) break;///se o par não possui o monitor a ser desligado como um dos componentes, marca como coberto o branch
    }
    if((!ok)&&(comInterrupt)) return false;
    branchs[b->getId()]= ok;
  }//fim para I

  ///atualiza o nro de branchs cobertos na solução corrente
  bSize=0;
  for(int i=0; i<pccp->getNSize(); i++){
    if(branchs[i]) bSize++;
  }//fim para I

  monitores[mId]= false;///desativa o monitor na solução corrente
  custoTotal-= pccp->getMonitor(mId)->getCusto();///atualiza o custo total da solução corrente

  return true;
}


///atualiza o vetor de monitores da solução
void THMelhorPar::setMonitores(std::vector<bool> v){
  for(int i=0; i<pccp->getNSize(); i++)
    monitores[i]= v[i];

  ///reset dos valores da instancia
  bSize=0;
  custoTotal=0;
  for(int i=0; i<pccp->getNSize();i++){
    branchs[i]= false;
    cobertura[i]=0;
    if(monitores[i] && pccp->getMonitor(i)!=NULL) custoTotal+= pccp->getMonitor(i)->getCusto();
  }

  ///loop nos branchs da instancia
  for(int i=0; i<pccp->getNSize();i++){
    TBranch* b= pccp->getBranch(i);
    if(b==NULL) continue;
    std::list<TPar*> lstP= b->getLstPares();
    ///loop nos pares que cobrem o branch
    for(std::list<TPar*>::iterator it=lstP.begin(); it!=lstP.end();it++){
      TPar* p= *it;
      ///se o par possui os monitores ativos no novo vetor, marca o branch como coberto
      if(monitores[p->getM1()] && monitores[p->getM2()]) branchs[i]= true;
    }//fim iterator IT
    if(branchs[i]) bSize++;///se o branch foi coberto, incrementa o contador da solução corrente
  }//fim para I
}

///executa uma limpeza gulosa na solução
void THMelhorPar::limpezaS(bool comInterrupt){
  std::list<int*> lc;
  for(int i=0; i<pccp->getNSize();i++){
    if(!(monitores[i]) || pccp->getMonitor(i)==NULL) continue;
    int* v= new int[2];
    v[0]= i;///id do monitor
    v[1]= pccp->getMonitor(i)->getCusto();///custo do monitor
    lc.push_back(v);
  }

  lc.sort(ordMonitores());

  for(std::list<int*>::iterator it= lc.begin(); it!=lc.end();it++){
    int* m= *it;
    THMelhorPar *s = new THMelhorPar(pccp, monitores, branchs);
    if(s->removerMonitor(m[0],comInterrupt)){
      monitores[m[0]]= false;
    }
    //std::cout<<"Custo: "<<s->getCustoTot()<<" "<<s->getBSize()<<"/"<<bSize<<"\n";
    delete s;
  }

  custoTotal=0;
  bSize=0;
  for(int i=0; i<pccp->getNSize(); i++){
    branchs[i]= pccp->getBranch(i)!=NULL?true:false;
    custoTotal+= monitores[i]?pccp->getMonitor(i)->getCusto():0;
    bSize+= branchs[i]?1:0;
  }
  //setMonitores(monitores);

}



///verificador completo da solução
bool THMelhorPar::verificadorFull(){
  int qtdeB= 0;///conta quantos branchs precisam ser cobertos
  for(int i=0; i<pccp->getNSize();i++){
    qtdeB+= pccp->getBranch(i)==NULL?0:1;
    branchs[i]=false;
  }

  ///pra cada monitor
  for(int i=0; i<pccp->getNSize();i++){
    if(!(monitores[i])) continue;///se monitor não está na solução, ignora
    for(int j=i; j<pccp->getNSize();j++){
      if(!(monitores[j])) continue;
      TPar* p= pccp->getPar(i,j);
      if(p==NULL) continue;
      std::list<int> b= p->getLstBranchs();
      ///loop nos branchs que o par cobre
      for(std::list<int>::iterator it=b.begin();it!=b.end();it++){
        int indice= *it;
        branchs[indice]= true;///marca o branch como coberto na solução corrente
      }
    }//fim para j
  }//fim para i

  ///quanto quantos branchs foram cobertos e quantos não foram
  int qtdeS= 0;
  int naoCobertos= 0;
  for(int i=0; i<pccp->getNSize();i++){
    qtdeS+= (branchs[i])?1:0;
    if(pccp->getBranch(i)!=NULL) naoCobertos+= (branchs[i])?0:1;
  }

  std::cout<<qtdeS<<"/"<<qtdeB<<" "<<naoCobertos<<"\n";

  ///retorna true caso a cobertura da instancia seja completa, dados os monitores de entrada
  ///retora false caso contrário
  return (naoCobertos<1)&&(qtdeB==qtdeS);
}

void THMelhorPar::exportar(){

  std::cout << "Exportando arquivo de log da saida" <<std::endl;

  std::string nomeF= pccp->getNome()+"_sol_mpar.pccp";
  std::ofstream fout(nomeF.c_str(), std::ios::trunc);///arquivo de destino

  if(fout.is_open() && fout.good()){
    ///vetor de monitores
    for(int i=0;i<pccp->getNSize();i++){
      //if(!(monitores[i])) continue;
      fout <<monitores[i];
    }
    fout<<" "<<custoTotal<<std::endl;
    ///fecha os arquivos
    fout.close();
    std::cout << ">>Exportacao concluida - result_log.pccp" <<std::endl;
  }else{///fim se abriu os arquivos
    std::cout << ">>Falha ao exportar o arquivo" <<std::endl;
  }

}

void THMelhorPar::importar(char* path){

  std::ifstream fin;
  fin.open(path, std::ios::in);

  std::cout <<"Importando saida "<< path <<std::endl;
  if(fin.is_open() && fin.good()){

    std::vector<bool>v(pccp->getNSize(),false);
    while(!fin.fail()){
      std::string linha;
      getline(fin,linha);
      int ix= atoi(linha.c_str());
      if(ix<pccp->getNSize()) v[ix]= true;
    }
    setMonitores(v);
    std::cout<<"Leitura concluida\n";
  }else{//fim-se abriu o arquivo
    std::cout << ">>Falha ao tentar abrir o arquivo "<< path <<std::endl;
  }

}
