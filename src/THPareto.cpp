#include "../include/THPareto.h"
#include "../include/TParesPareto.h"

#include <climits>
#include <iostream>




THPareto::THPareto(TInstancia* _pccp){
  //ctor
  ///inicializa��o das vari�veis
  pccp= _pccp;
  monitores.resize(pccp->getNSize(),false);
  branchs.resize(pccp->getNSize(),false);
  custoTotal=0;
  bSize=0;

  //matriz: monitor x branch
  vetMxB = new int*[pccp->getNSize()];
  for (int i = 0; i < pccp->getNSize(); ++i){
    vetMxB[i] = new int[pccp->getNSize()];
    for(int j=0;j<pccp->getNSize();j++)
      vetMxB[i][j]=0;
  }

  ///loop nos pares da instancia
  for(int i=0; i<pccp->getNSize();i++){
    for(int j=i; j<pccp->getNSize();j++){
      ///atualiza o "custo" do par
      if(pccp->existePar(i,j)){
        pccp->getPar(i,j)->setCustoPar(pccp->getMonitor(i)->getCusto()+pccp->getMonitor(j)->getCusto());
      }
    }//fim para J
  }//fim para I

}

THPareto::THPareto(std::vector<bool> _monitores,std::vector<bool> _branchs,TInstancia* _pccp,
int _custoTotal,int _bSize,int** _vetMxB){

  ///inicializa��o das vari�veis
  pccp= _pccp;

  std::vector<bool>v1(_monitores);
  std::vector<bool>v2(_branchs);
  monitores= v1;
  branchs= v2;
  custoTotal=_custoTotal;
  bSize=_bSize;
  //matriz: monitor x branch
  vetMxB = new int*[pccp->getNSize()];
  for (int i = 0; i < pccp->getNSize(); ++i){
    vetMxB[i] = new int[pccp->getNSize()];
    for(int j=0;j<pccp->getNSize();j++)
      vetMxB[i][j]= _vetMxB[i][j];
  }


}

THPareto::~THPareto(){
  //dtor
  for(int i=0; i<pccp->getNSize();i++){
    delete [] vetMxB[i];
  }
  delete [] vetMxB;

  monitores.clear();
  branchs.clear();

}

void THPareto::zerarSolucao(){
  monitores.clear();
  branchs.clear();
  bSize=0;
  custoTotal=0;

  for(int i=0; i<pccp->getNSize();i++){
    delete [] vetMxB[i];
  }
  delete [] vetMxB;

}

///Cria a lista de branchs candidatos
std::list<int*> THPareto::criarLcBranchs(){
  std::list<int*> lc;///lista inicialmente vaiza

  for(int i=0; i<pccp->getNSize(); i++){
    if(pccp->getBranch(i)==NULL || branchs[i]) continue;///ignora branchs que n�o fazem parte do problema ou que j� est�o cobertos
    int* v= new int[2];
    v[0] = i;///posi��o 1: id do branch
    v[1] = pccp->getBranch(i)->getCobertura();///posi��o 2: n� de pares que cobrem o branch
    lc.push_back(v);///insere branch na lista
  }

  lc.sort(ordBranchs());///ordena a lista

  return lc;
}

std::list<TPar*> THPareto::criarLcPares(int bId){
  std::list<TPar*> lc;///lista inicialmente vazia
  if(pccp->getBranch(bId)==NULL) return lc;

  ///loop nos pares do branch
  std::list<TPar*> pares= pccp->getBranch(bId)->getLstPares();
  for(std::list<TPar*>::iterator it=pares.begin(); it!=pares.end(); it++){
    TPar* p= *it;
    int custo= (monitores[p->getM1()])?0:pccp->getMonitor(p->getM1())->getCusto();
    custo+= (monitores[p->getM2()])?0:pccp->getMonitor(p->getM2())->getCusto();
    p->setCustoPar(custo);
    /*
    std::list<int> lB= p->getLstBranchs();
    int alcance=0;
    for(std::list<int>::iterator it2=lB.begin(); it2!=lB.end(); it2++){
      int b= *it2;
      alcance+= (branchs[b])?0:1;
    }
    p->setAlcance(alcance);
    */
  }



  TParesPareto* pp= new TParesPareto();///gera objeto de contru��o de pareto
  pp->criarLC(pccp->getBranch(bId)->getLstPares(), monitores);///cria a lista de pareto nos pares do branch
  return pp->pares;///retorna a lista de pares de pareto do branch
}

///seleciona um branch pra entrar na solu��o
int THPareto::selecionarBranch(float pRand, bool gravarLog){
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
  return v[0];///posi��o 1 da lista de canditatos: id do branch

}

///seleciona um par pra entrar na solu��o
TPar* THPareto::selecionarPar(int bId, float pRand, bool gravarLog){

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

/*
  if(pccp->getBranch(bId)==NULL) return NULL;

  TParesPareto* pp= new TParesPareto();
  pp->criarLC(pccp->getBranch(bId)->getLstPares(),monitores);
  return pp->selecionarRand(pRand);
*/
}


///marca na solu��o como cobertos os branchs de um determinado par
void THPareto::cobrirBranchsPar(int m1Id, int m2Id){
  TPar* p= pccp->getPar(m1Id,m2Id);
  if(p!=NULL){
    std::list<int> b= p->getLstBranchs();
    ///loop na lista de branchs que o par cobre
    for(std::list<int>::iterator it=b.begin();it!=b.end();it++){
      int ix= *it;
      if(branchs[ix]) continue;///ignora os branchs que j� est�o na solu��o
      branchs[ix]= true;///marca o branch como coberto
      bSize++;///incrementa o contador de branchs cobertos
    }
    p->setCustoPar(0);
    p->setAlcance(0);
  }
}


///atualiza a solu��o � partir de um novo par selecionado para ser colocado nela
void THPareto::atualizarS(int m1Id, int m2Id){
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
bool THPareto::verificarS(){
  return bSize>=pccp->getBSize();
}


///construtivo
void THPareto::construtivo(float pRandB, float pRandP, int corte, bool gravarLog){
  int limite= pccp->getNSize();///limite de tentativas
  if(corte<1) corte= INT_MAX;///valor de corte para interrup��o
  if(gravarLog){
    std::string nomeF= pccp->getNome()+"_log_exec_pareto.pccp";
    logFile.open(nomeF.c_str(), std::ios::trunc);///arquivo log detalhado
  }
  while(!verificarS() && limite>0 && custoTotal<corte){
    int b= selecionarBranch(pRandB, gravarLog);///seleciona o branch
    if(gravarLog) logFile<<"Escolheu B: "<<b<<"\n";
    TPar* p= selecionarPar(b,pRandP,gravarLog);///seleciona o par
    if(gravarLog)
      if(p!=NULL) logFile<<"Escolheu P: "<<p->getM1()<<","<<p->getM2()<<"\n";
      else logFile<<"NULL\n";
    if(p!=NULL)atualizarS(p->getM1(),p->getM2());///atualiza a solucao
    limite--;
  }

}


///retorna o nro de branchs cobertos na solu��o corrente
int THPareto::getBSize(){
  return bSize;
}

///retorna o nro de monitores ativos na solu��o corrente
int THPareto::getNroMonitores(){
  int c=0;
  for(int i=0; i<pccp->getNSize(); i++)
    if(monitores[i]) c++;
  return c;
}

///retorna o custo total da solu��o corrente
int THPareto::getCustoTot(){
  return custoTotal;
}

///retorna uma c�pia do vetor de monitores da solu��o
std::vector<bool> THPareto::getMonitores(){
  std::vector<bool> v(monitores);
  return v;
}

///retorna uma c�pia do vetor de monitores da solu��o
std::vector<bool> THPareto::getBranchs(){
  std::vector<bool> v(branchs);
  return v;
}

void THPareto::mudarUmMonitor(int mId){

  ///atualiza a matrix MxB na linha do monitor que ser� marcado como parte da solu��o
  for(int i=0; i<pccp->getNSize();i++){
    if(!monitores[mId] || !monitores[i] || pccp->getPar(mId,i)==NULL) continue;
    std::list<int> b= pccp->getPar(mId,i)->getLstBranchs();
    for(std::list<int>::iterator it=b.begin(); it!=b.end();++it){
      int ix= *it;
      int incremento= (monitores[mId])?-1:1;
      vetMxB[mId][ix]+=incremento;
      vetMxB[i][ix]+=incremento;
      vetMxB[0][ix]+=incremento;
    }//fim iterator
  }//fim para i

  bSize=0;
  for(int i=0; i<pccp->getNSize();i++){
    if(pccp->getBranch(i)==NULL) continue;
    branchs[i]= (vetMxB[0][i]!=0);
    if(branchs[i]) bSize++;
  }

  custoTotal+= monitores[mId]?(-1)*pccp->getMonitor(mId)->getCusto():pccp->getMonitor(mId)->getCusto();
  monitores[mId]= !monitores[mId];

  //exportar();
}

THPareto* THPareto::criarCopia(){
  THPareto* s= new THPareto(monitores,branchs,pccp,custoTotal,bSize,vetMxB);
  return s;
}

///marca como inativo um monitor na solu��o corrente
bool THPareto::removerMonitor(int mId, bool comInterrupt){

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

///marca como inativo um monitor na solu��o corrente
bool THPareto::removerMonitor2(int mId){

  if(!monitores[mId] || pccp->getMonitor(mId)==NULL) return true;

  for(int i=0; i<pccp->getNSize(); i++){
    if(pccp->getBranch(i)==NULL) continue;///ignora branchs que n�o fazem parte da instancia
    if((vetMxB[0][i]-vetMxB[mId][i])<=0) return false;
  }//fim para i

  monitores[mId]= false;
  custoTotal-=pccp->getMonitor(mId)->getCusto();

  for(int i=0; i<pccp->getNSize();i++){
    if(!monitores[i] || pccp->getPar(mId,i)==NULL) continue;
    std::list<int> b= pccp->getPar(mId,i)->getLstBranchs();
    for(std::list<int>::iterator it=b.begin(); it!=b.end();++it){
      int ix= *it;
      vetMxB[mId][ix]--;
      vetMxB[i][ix]--;
      vetMxB[0][ix]--;
    }

  }

  return true;

}


///atualiza o vetor de monitores da solu��o
void THPareto::setMonitores(std::vector<bool> v){
  for(int i=0; i<pccp->getNSize(); i++)
    monitores[i]= v[i];

  ///reset dos valores da instancia
  bSize=0;
  custoTotal=0;
  for(int i=0; i<pccp->getNSize();i++){
    branchs[i]= false;
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
/*
///executa uma limpeza gulosa na solu��o
void THPareto::limpezaS(bool comInterrupt){
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
    THPareto *s = new THPareto(pccp, monitores, branchs);///cria uma sol temporaria � partir da atual
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
*/
///executa uma limpeza gulosa na solu��o
void THPareto::limpeza2S(){

  ///atualiza��o da matriz MxB
  for(int i=0; i<pccp->getNSize(); i++){
    if(!monitores[i]) continue;
    for(int j=i; j<pccp->getNSize();j++){
      if(!monitores[j] || pccp->getPar(i,j)==NULL) continue;
      std::list<int> b= pccp->getPar(i,j)->getLstBranchs();
      for(std::list<int>::iterator it=b.begin(); it!=b.end();++it){
        int ix= *it;
        vetMxB[i][ix]++;
        vetMxB[j][ix]++;
        vetMxB[0][ix]++;
      }//fim para IT
    }//fim para J
  }//fim para I

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
    //std::cout<<m[0]<<" "<<removerMonitor2(m[0])<<"\n";///tenta remover o monitor da solu��o
    removerMonitor2(m[0]);
  }

}



///verificador completo da solu��o
bool THPareto::verificadorFull(){
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

void THPareto::exportar(){

  std::cout << "Exportando arquivo de log da saida" <<std::endl;

  std::string nomeF= pccp->getNome()+"_sol_pareto.pccp";
  std::ofstream fout(nomeF.c_str(), std::ios::trunc);///arquivo de destino

  if(fout.is_open() && fout.good()){
    ///vetor de monitores
    for(int i=0;i<pccp->getNSize();i++){
      //if(!(monitores[i])) continue;
      fout <<monitores[i];
    }
    fout<<" "<<custoTotal<<std::endl;


    ///vetMxB
    fout<<"\n\nVetMxB\n";
    for(int i=0;i<pccp->getNSize();i++){
      for(int j=0;j<pccp->getNSize();j++)
        fout <<vetMxB[i][j]<<"\t";
      fout<<std::endl;
    }

    ///fecha os arquivos
    fout.close();
    std::cout << ">>Exportacao concluida - result_log.pccp" <<std::endl;
  }else{///fim se abriu os arquivos
    std::cout << ">>Falha ao exportar o arquivo" <<std::endl;
  }

}

void THPareto::importar(char* path){

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
