#include "../include/TInstancia.h"
#include "../include/TBranch.h"
#include "../include/TMonitor.h"
#include "../include/TPar.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <cstdlib>


TInstancia::TInstancia(){
  //ctor
  bSize = 0;///quantos branchs existem na instancia
}

TInstancia::~TInstancia(){
  //dtor
  for(int i=0; i<nSize;i++){
    delete [] vetMxB[i];
  }
  delete [] vetMxB;

  vetB.clear();
  vetM.clear();
  vetP.clear();
}

///Impressão dos dados da instancia
void TInstancia::show(){
  std::cout <<"Size "<< vetB.size() <<" "<< vetM.size() <<" "<< vetP.size() <<std::endl;
}


///obtém um par do vector de pares. Retorna um ponteiro para o par caso exista no vector e null em caso oposto
TPar* TInstancia::getPar(int m1, int m2){
  int aux= m1;
  if(m1>m2){
    m1= m2;
    m2= aux;
  }
  int ix = m1*(nSize)+m2;
  if(ix<vetP.size()) return vetP.at(ix);
  return NULL;

}
///obtém um monitor do vector de monitores. Retorna um ponteiro para o monitor caso exista no vector e null em caso oposto
TMonitor* TInstancia::getMonitor(int mId){
  if(mId<vetM.size()) return vetM.at(mId);
  return NULL;
}
///obtém um branch do vector de branchs. Retorna um ponteiro para o branch caso exista no vector e null em caso oposto
TBranch* TInstancia::getBranch(int bId){
  if(bId<vetB.size()) return vetB.at(bId);
  return NULL;
}

///retorna o vetor de branchs da instancia
std::vector<TBranch*> TInstancia::getVetBranchs(){
  return vetB;
}
///retorna o vetor de monitores da instancia
std::vector<TMonitor*> TInstancia::getVetMonitores(){
  return vetM;
}
///retorna o vetor de pares da instancia
std::vector<TPar*> TInstancia::getVetPares(){
  return vetP;
}

///retorna o valor de id máximo da instancia
int TInstancia::getNSize(){
  return nSize;
}

///retorna o nro de branchs que precisam ser cobertos na instancia
int TInstancia::getBSize(){
  return bSize;
}

///insere um branch no vetor de branchs. Retorna um ponteiro para o elemento incluído
TBranch* TInstancia::addBranch(int bId){
  if(bId<nSize){
    if(vetB.at(bId)==NULL){
      vetB.at(bId) = new TBranch(bId);;
      bSize++;
      return vetB.at(bId);
    }
    return vetB.at(bId);
  }
  return NULL;
}
///insere um monitor no vetor de monitores. Retorna um ponteiro para o elemento incluído
TMonitor* TInstancia::addMonitor(int mId){
  if(mId<nSize){
    if(vetM.at(mId)==NULL){
      vetM.at(mId) = new TMonitor(mId);
      return vetM.at(mId);
    }
    return vetM.at(mId);
  }
  return NULL;
}
///insere um par no vetor de pares. Retorna um ponteiro para o elemento incluído
TPar* TInstancia::addPar(int m1, int m2){
  int aux= m1;
  if(m1>m2){
    m1= m2;
    m2= aux;
  }
  int ix = m1*(nSize);
  ix+=m2;

  if(ix<vetP.size()){
    if(vetP.at(ix)==NULL){
      vetP.at(ix) = new TPar(m1,m2);
      return vetP.at(ix);
    }
    return vetP.at(ix);
  }

  return NULL;
}

///Retorna True/False sobre a informação: monitor "atinge" o branch
bool TInstancia::monitorCobreBranch(int mId, int bId){
  return vetMxB[mId][bId];
}

///Existencia de um par na instancia
bool TInstancia::existePar(int m1Id, int m2Id){
  return getPar(m1Id,m2Id)!=NULL;
}


///leitura do arquivo de instancia
void TInstancia::loadFile(std::string path){
  int n=0, m=0, b=0;
  if(path.size()==0) exit(0);

  ///obtem o valor de n no nome do arquivo
  std::size_t n1 = path.find("/n");
  std::size_t n2 = path.find("-i");
  std::string aux = path.substr(n1+2,n2-n1-2);
  n = atoi((char*)aux.c_str());
  nSize = n+1;
  nome="n"+aux+"-i";

  ///obtem o valor de i no nome do arquivo
  n1 = path.find("-i");
  n2 = path.find("-m");
  aux = path.substr(n1+2,n2-n1-2);
  nome=nome+aux+"-m";

  ///obtem o valor de m no nome do arquivo
  n1 = path.find("-m");
  n2 = path.find("-b");
  aux = path.substr(n1+2,n2-n1-2);
  m = atoi((char*)aux.c_str());
  nome=nome+aux+"-b";


  ///obtem o valor de b no nome do arquivo
  n1 = path.find("-b");
  n2 = path.find("-c");
  aux = path.substr(n1+2,n2-n1-2);
  b = atoi((char*)aux.c_str());
  nome=nome+aux+"-c";

  ///obtem o valor de c no nome do arquivo
  n1 = path.find("-c");
  n2 = path.find(".cst");
  aux = path.substr(n1+2,n2-n1-2);
  nome=nome+aux;


  //std::cout<<n<<" " <<m<<" "<<b<<std::endl;

  ///aloca o tamanho correto dos vector
  vetB.resize(nSize,NULL);
  vetM.resize(nSize,NULL);
  vetP.resize(nSize*nSize,NULL);

  //matriz: monitor x branch
  vetMxB = new bool*[nSize];
  for (int i = 0; i < nSize; ++i){
    vetMxB[i] = new bool[nSize];
    for(int j=0;j<nSize;j++)
      vetMxB[i][j]=false;
  }



  std::ifstream fin;
  fin.open((char*)path.c_str(), std::ios::in);

  std::cout <<"Lendo arquivo "<< path <<std::endl;
  if(fin.is_open() && fin.good()){

    while(!fin.fail()){
      std::string linha;
      getline(fin,linha);

      char *p;

      p= strtok((char*)linha.c_str()," ");
      if(p!=NULL && p[0]=='t'){
        ///obtém os ids de branchd e monitores da linha lida
        p= strtok(NULL," ");
        int m1Id = atoi(p);
        p= strtok(NULL," ");
        int m2Id = atoi(p);
        p= strtok(NULL," ");
        int bId=atoi(p);

        ///inclusão dos ids na instancia
        TBranch* b= addBranch(bId);
        TMonitor* m1= addMonitor(m1Id);
        TMonitor* m2= addMonitor(m2Id);
        TPar* p= addPar(m1Id,m2Id);
        b->addPar(p);
        p->addBranch(bId);

        vetMxB[m1Id][bId]=true;
        vetMxB[m2Id][bId]=true;

      }else if(p!=NULL && p[0]=='c'){
        ///inclusão dos custos dos monitores
        p= strtok(NULL," ");
        int mId = atoi(p);
        p= strtok(NULL," ");
        int c = atoi(p);
        getMonitor(mId)->setCusto(c);
      }

    }//fim enquanto
    fin.close();//fecha o arquivo

    //std::cout<<vetB.size()<<" "<<vetM.size()<<" "<<vetP.size()<<"\n";

    ///loop nos branchs
    for(int i=0; i<nSize; i++)
      if(getBranch(i)!=NULL)
        ///corrige a lista de pares que cobre o branch (ordena e remove duplicações)
        getBranch(i)->ordenarLstPares();

    ///loop nos pares
    for(int i=0; i<nSize; i++)
      for(int j=i; j<nSize; j++)
        if(getPar(i,j)!=NULL){
          ///corrige a lista de branchs que o par cobre(ordena e remove duplicações)
          getPar(i,j)->ordenarLstBranchs();
          getPar(i,j)->setAlcance(getPar(i,j)->getLstBranchs().size());
        }

    std::cout<<"Leitura concluida\n";

  }else{//fim-se abriu o arquivo
    std::cout << ">>Falha ao tentar abrir o arquivo "<< path <<std::endl;
  }

}


///Gera um log da instancia, no diretório corrente
void TInstancia::exportar(){

  std::cout << "Exportando arquivo de log da instancia" <<std::endl;

  std::ofstream fout("instancia_log.pccp", std::ios::trunc);///arquivo de destino

  if(fout.is_open() && fout.good()){
    //cout <<"ok"<<endl;
    int i;

    ///branchs
    for(i=0;i<vetB.size();i++){
      TBranch*b = vetB.at(i);
      if(b==NULL) continue;
      fout << "B " << b->getId()<<"\tp: "<<b->getCobertura()<<std::endl;
      std::list<TPar*> p= b->getLstPares();
      for(std::list<TPar*>::iterator it=p.begin();it!=p.end();it++){
        TPar* v= *it;
        fout<<v->getM1()<<","<<v->getM2()<<"\t";
      }
      fout<<"\n";
    }

    ///monitores
    for(i=0;i<vetM.size();i++){
      TMonitor *m = vetM.at(i);
      if(m==NULL) continue;
      fout <<"M "<< m->getId()<<"\tc:"<<m->getCusto()<<std::endl;
    }

    ///pares
    for(i=0;i<vetP.size();i++){
      TPar *p = vetP.at(i);
      if(p==NULL) continue;
      fout << "P[" << p->getM1()<<","<<p->getM2()<<"]\tb: "<<p->getAlcance()<<std::endl;
      std::list<int> b= p->getLstBranchs();
      for(std::list<int>::iterator it=b.begin();it!=b.end();it++){
        int ix= *it;
        fout<<ix<<"\t";
      }
      fout<<"\n";
    }

    ///vetMxB
    fout<<"VetMxB\n";
    for(i=0;i<nSize;i++){
      for(int j=0;j<nSize;j++)
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

std::string TInstancia::getNome(){
  return nome;
}
