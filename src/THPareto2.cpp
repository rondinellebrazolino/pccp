#include "../include/THPareto2.h"
#include "../include/TParesPareto.h"

#include <climits>
#include <iostream>




THPareto2::THPareto2(TInstancia* _pccp){
  //ctor
  ///inicializa��o das vari�veis
  pccp= _pccp;
  monitores.resize(pccp->getNSize(),false);
  branchs.resize(pccp->getNSize(),false);
  cobertura.resize(pccp->getNSize(),0);
  custoTotal=0;
  bSize=0;


  ///loop nos pares da instancia
  for(int i=0; i<pccp->getNSize();i++)
    for(int j=i; j<pccp->getNSize();j++)
      ///atualiza o "custo" do par
      if(pccp->existePar(i,j)) pccp->getPar(i,j)->setCustoPar(pccp->getMonitor(i)->getCusto()+pccp->getMonitor(j)->getCusto());

}

THPareto2::THPareto2(TInstancia* _pccp, std::vector<bool> vetM, std::vector<bool> vetB){
  //ctor
  ///inicializa��o das vari�veis
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

THPareto2::~THPareto2(){
  //dtor
  monitores.clear();
  branchs.clear();
  cobertura.clear();

}

void THPareto2::criarLcPares(){

  candidatos.clear();
  ///loop nos pares da instancia
  for(int i=0; i<pccp->getNSize();i++)
    for(int j=i; j<pccp->getNSize();j++){
      if(!pccp->existePar(i,j)) continue;
      TPar* p= pccp->getPar(i,j);
      if((monitores[p->getM1()])&&(monitores[p->getM2()])) continue;
      int custo= (monitores[p->getM1()])?0:pccp->getMonitor(p->getM1())->getCusto();
      custo+= (monitores[p->getM2()])?0:pccp->getMonitor(p->getM2())->getCusto();
      p->setCustoPar(custo);
      candidatos.push_back(p);
    }

  TParesPareto* pp= new TParesPareto();///gera objeto de contru��o de pareto
  pp->criarLC(candidatos, monitores);///cria a lista de pareto nos pares
  candidatos= pp->pares;///retorna a lista de pares de pareto

}

///seleciona um par pra entrar na solu��o
TPar* THPareto2::selecionarPar(float pRand, bool gravarLog){

  if(candidatos.size()<1) return NULL;

  if(gravarLog){
    logFile<<"listaP["<<candidatos.size()<<"]: ";
    for(std::list<TPar*>::iterator it=candidatos.begin();it!=candidatos.end();it++){
      TPar* p= *it;
      logFile<<p->getM1()<<","<<p->getM2()<<" ";
    }
    logFile<<"\n";
  }

  TPar* p= candidatos.front();

  int limite = candidatos.size()*pRand;
  if(limite<1) return p;

  int ix= rand()%limite;

  for(std::list<TPar*>::iterator it=candidatos.begin();it!=candidatos.end() && ix>0;it++){
    p= *it;
    ix--;
    it++;
  }
  return p;///retorna o par selecionado

}


///marca na solu��o como cobertos os branchs de um determinado par
void THPareto2::cobrirBranchsPar(int m1Id, int m2Id){
  TPar* p= pccp->getPar(m1Id,m2Id);
  if(p!=NULL){
    std::list<int> b= p->getLstBranchs();
    ///loop na lista de branchs que o par cobre
    for(std::list<int>::iterator it=b.begin();it!=b.end();it++){
      int ix= *it;
      if(branchs[ix]) continue;///ignora os branchs que j� est�o na solu��o
      cobertura.at(ix)++;
      branchs[ix]= true;///marca o branch como coberto
      bSize++;///incrementa o contador de branchs cobertos
    }
    p->setCustoPar(0);
    p->setAlcance(0);
  }
}


///atualiza a solu��o � partir de um novo par selecionado para ser colocado nela
void THPareto2::atualizarS(int m1Id, int m2Id){
  TPar* p= pccp->getPar(m1Id,m2Id);

  if(p!=NULL){
    cobrirBranchsPar(m1Id,m2Id);///cobre os branchs do par
    ///atualiza o custo total da solu��o
    if(!(monitores[m1Id])) custoTotal+= pccp->getMonitor(m1Id)->getCusto();
    if(!(monitores[m2Id])) custoTotal+= pccp->getMonitor(m2Id)->getCusto();
    ///marca os monitores do par como ativos na solu��o
    monitores[m1Id]=true;
    monitores[m2Id]=true;

    ///verifica a forma��o de novos pares � partir do par atual com os monitores que j� est�o na solu��o
    ///e faz a atualiza��o de cobertura
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

///verifica��o r�pida da solu��o (compara��o de contadores)
bool THPareto2::verificarS(){
  return bSize>=pccp->getBSize();
}


///construtivo
void THPareto2::construtivo(float pRandB, float pRandP, int corte, bool gravarLog){
  int limite= pccp->getNSize();///limite de tentativas
  if(corte<1) corte= INT_MAX;///valor de corte para interrup��o
  if(gravarLog){
    std::string nomeF= pccp->getNome()+"_log_exec_pareto2.pccp";
    logFile.open(nomeF.c_str(), std::ios::trunc);///arquivo log detalhado
  }
  while(!verificarS() && limite>0 && custoTotal<corte){
    criarLcPares();///cria a lista de pares
    TPar* p= selecionarPar(pRandP,gravarLog);///seleciona o par
    if(gravarLog)
      if(p!=NULL) logFile<<"Escolheu P: "<<p->getM1()<<","<<p->getM2()<<"\n";
      else logFile<<"NULL\n";
    if(p!=NULL)atualizarS(p->getM1(),p->getM2());///atualiza a solucao
    limite--;
  }

  //std::cout<<"Executando limpeza\n";
  //limpezaS();
}


///retorna o nro de branchs cobertos na solu��o corrente
int THPareto2::getBSize(){
  return bSize;
}

///retorna o nro de monitores ativos na solu��o corrente
int THPareto2::getNroMonitores(){
  int c=0;
  for(int i=0; i<pccp->getNSize(); i++)
    if(monitores[i]) c++;
  return c;
}

///retorna o custo total da solu��o corrente
int THPareto2::getCustoTot(){
  return custoTotal;
}

///retorna uma c�pia do vetor de monitores da solu��o
std::vector<bool> THPareto2::getMonitores(){
  std::vector<bool> v(monitores);
  return v;
}

///marca como inativo um monitor na solu��o corrente
bool THPareto2::removerMonitor(int mId, bool comInterrupt){

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
      if(ok) break;///se o par n�o possui o monitor a ser desligado como um dos componentes, marca como coberto o branch
    }
    if((!ok)&&(comInterrupt)) return false;
    branchs[b->getId()]= ok;
  }//fim para I

  ///atualiza o nro de branchs cobertos na solu��o corrente
  bSize=0;
  for(int i=0; i<pccp->getNSize(); i++){
    if(branchs[i]) bSize++;
  }//fim para I

  monitores[mId]= false;///desativa o monitor na solu��o corrente
  custoTotal-= pccp->getMonitor(mId)->getCusto();///atualiza o custo total da solu��o corrente

  return true;
}


///atualiza o vetor de monitores da solu��o
void THPareto2::setMonitores(std::vector<bool> v){
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
    if(branchs[i]) bSize++;///se o branch foi coberto, incrementa o contador da solu��o corrente
  }//fim para I
}

///executa uma limpeza gulosa na solu��o
void THPareto2::limpezaS(bool comInterrupt){
  ///lista de monitores candidatos (todos da instancia)
  std::list<int*> lc;
  ///loop nos monitores da instancia
  for(int i=0; i<pccp->getNSize();i++){
    if(!(monitores[i]) || pccp->getMonitor(i)==NULL) continue;
    int* v= new int[2];///armazena as info do monitor num vetor de 2 posi��es
    v[0]= i;///id do monitor
    v[1]= pccp->getMonitor(i)->getCusto();///custo do monitor
    lc.push_back(v);
  }

  ///ordena os monitores (por custo: maior para o menor)
  lc.sort(ordMonitores());

  ///loop na LC de monitores
  for(std::list<int*>::iterator it= lc.begin(); it!=lc.end();it++){
    int* m= *it;
    THPareto2 *s = new THPareto2(pccp, monitores, branchs);///cria uma sol temporaria � partir da atual
    if(s->removerMonitor(m[0], comInterrupt)){///se � poss�vel remover o monitor
      monitores[m[0]]= false;///desmarca o mesmo na solu��o corrente
    }
    //std::cout<<"Custo: "<<s->getCustoTot()<<" "<<s->getBSize()<<"/"<<bSize<<"\n";
    delete s;
  }

  ///depois das remo��es, atualiza os par�metros da solu��o
  custoTotal=0;
  bSize=0;
  for(int i=0; i<pccp->getNSize(); i++){
    branchs[i]= pccp->getBranch(i)!=NULL?true:false;
    custoTotal+= monitores[i]?pccp->getMonitor(i)->getCusto():0;
    bSize+= branchs[i]?1:0;
  }
  //setMonitores(monitores);

}



///verificador completo da solu��o
bool THPareto2::verificadorFull(){
  int qtdeB= 0;///conta quantos branchs precisam ser cobertos
  for(int i=0; i<pccp->getNSize();i++){
    qtdeB+= pccp->getBranch(i)==NULL?0:1;
    branchs[i]=false;
  }

  ///pra cada monitor
  for(int i=0; i<pccp->getNSize();i++){
    if(!(monitores[i])) continue;///se monitor n�o est� na solu��o, ignora
    for(int j=i; j<pccp->getNSize();j++){
      if(!(monitores[j])) continue;
      TPar* p= pccp->getPar(i,j);
      if(p==NULL) continue;
      std::list<int> b= p->getLstBranchs();
      ///loop nos branchs que o par cobre
      for(std::list<int>::iterator it=b.begin();it!=b.end();it++){
        int indice= *it;
        branchs[indice]= true;///marca o branch como coberto na solu��o corrente
      }
    }//fim para j
  }//fim para i

  ///quanto quantos branchs foram cobertos e quantos n�o foram
  int qtdeS= 0;
  int naoCobertos= 0;
  for(int i=0; i<pccp->getNSize();i++){
    qtdeS+= (branchs[i])?1:0;
    if(pccp->getBranch(i)!=NULL) naoCobertos+= (branchs[i])?0:1;
  }

  std::cout<<qtdeS<<"/"<<qtdeB<<" "<<naoCobertos<<"\n";

  ///retorna true caso a cobertura da instancia seja completa, dados os monitores de entrada
  ///retora false caso contr�rio
  return (naoCobertos<1)&&(qtdeB==qtdeS);
}

void THPareto2::exportar(){

  std::cout << "Exportando arquivo de log da saida" <<std::endl;

  std::string nomeF= pccp->getNome()+"_sol_pareto2.pccp";
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

void THPareto2::importar(char* path){

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
