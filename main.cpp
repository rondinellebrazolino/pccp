#include <iostream>


#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <ctime>

#include "include/TBranch.h"
#include "include/TMonitor.h"
#include "include/TPar.h"
#include "include/TInstancia.h"
#include "include/THMelhorPar.h"
#include "include/THPareto.h"
#include "include/THPareto2.h"
#include "include/TBuscaLocal.h"





void melhorPar(char *arquivo, int seed, float pRand, char* heurist, bool limpeza, bool buscaLoc, bool gravarLog){

  TInstancia *pccp = new TInstancia();

  clock_t t1;
  clock_t t2;

  t1 = clock();

  pccp->loadFile(arquivo);
  pccp->show();
  t2 = clock();
  long intervaloArq = ((t2 - t1) / (CLOCKS_PER_SEC / 1000));
  std::cout << "Tempo de carga da instancia " << intervaloArq <<"ms"<<std::endl<<std::endl;

  //pccp->exportar();


///***************** SOLUÇÃO ******************///
///**************** MELHOR PAR ***************///

  char* saida="result_log.pccp";

  std::ofstream fout(saida, std::ios::app);///arquivo de destino
  ///fim ajustes no nome do aquivo de log

  if(fout.is_open() && fout.good()){
    THMelhorPar *s = new THMelhorPar(pccp);
    std::cout <<"Iniciando construtivo\nRandomizacao: "<<pRand*100<<"%"<< std::endl;
    t1 = clock();
    std::string heuristica= heurist;
    s->construtivo(0,pRand,0, gravarLog);
    //s->construtivo(pRand,0,0, gravarLog);
    //s->exportar();
    if(limpeza){
      std::cout<<"\tExecutando limpeza\n";
      s->limpezaS(true);
      std::cout<<"\tFim da execucao da limpeza\n";
    }
    t2= clock();
    long intervalo = ((t2 - t1) / (CLOCKS_PER_SEC / 1000));

    fout<<"\t"<<heuristica<<"\t"<<arquivo<<"\t"<<intervaloArq<<"\t"<<seed<<"\t"<<pRand<<
    "\t"<<s->getBSize()/pccp->getBSize()<<"\t"<<intervalo<<
    "\t"<<s->getNroMonitores()<<"\t"<<s->getCustoTot()<<"\t"<<s->verificadorFull()<<"\n";
    std::cout << "Fim contrutivo: rand = "<<pRand*100<<"%"<< std::endl;

      ///BUSCA LOCAL
      if(buscaLoc){
        std::cout <<"\tTentando busca local 1: "<<pRand*100<<"%"<< std::endl;
        TBuscaLocal *bl1 = new TBuscaLocal(pccp);

        t1 = clock();
        //bl1->buscaLoc_HMPR(s,pRand);
        t2= clock();
        intervalo = ((t2 - t1) / (CLOCKS_PER_SEC / 1000));

        fout<<"\t"<<heuristica<<"+BL"<<"\t"<<arquivo<<"\t"<<intervaloArq<<"\t"<<seed<<"\t"<<pRand<<
        "\t"<<s->getBSize()/pccp->getBSize()<<"\t"<<intervalo<<
        "\t"<<s->getNroMonitores()<<"\t"<<s->getCustoTot()<<"\t"<<s->verificadorFull()<<"\n";
        std::cout << "\tFim tbusca local"<< std::endl;

        delete bl1;
      }
      ///fim BUSCA LOCAL

    //delete s;
    ///fecha os arquivos
    fout.close();
    std::cout << ">>Exportacao concluida - result_log.pccp\n\n" <<std::endl;
  }else{///fim se abriu os arquivos
    std::cout << ">>Falha ao exportar o arquivo\n\n" <<std::endl;
  }

  delete pccp;


}

void pareto2(char *arquivo, int seed, float pRand, char* heurist, bool limpeza, bool buscaLoc, bool gravarLog){

  TInstancia *pccp = new TInstancia();

  clock_t t1;
  clock_t t2;

  t1 = clock();

  pccp->loadFile(arquivo);
  pccp->show();
  t2 = clock();
  long intervaloArq = ((t2 - t1) / (CLOCKS_PER_SEC / 1000));
  std::cout << "Tempo de carga da instancia " << intervaloArq <<"ms"<<std::endl<<std::endl;

  //pccp->exportar();


///***************** SOLUÇÃO *****************///
///***************** PARETO *****************///


  char* saida="result_log.pccp";

  std::ofstream fout(saida, std::ios::app);///arquivo de destino
  ///fim ajustes no nome do aquivo de log

  if(fout.is_open() && fout.good()){
    THPareto2 *s = new THPareto2(pccp);
    std::cout <<"Iniciando construtivo\nRandomizacao: "<<pRand*100<<"%"<< std::endl;
    t1 = clock();
    std::string heuristica= heurist;
    s->construtivo(0,pRand,0, gravarLog);
    //s->construtivo(pRand,0,0, gravarLog);
    //s->exportar();
    if(limpeza){
      std::cout<<"\tExecutando limpeza\n";
      s->limpezaS(true);
      std::cout<<"\tFim da execucao da limpeza\n";
    }
    t2= clock();
    long intervalo = ((t2 - t1) / (CLOCKS_PER_SEC / 1000));

    fout<<"\t"<<heuristica<<"\t"<<arquivo<<"\t"<<intervaloArq<<"\t"<<seed<<"\t"<<pRand<<
    "\t"<<s->getBSize()/pccp->getBSize()<<"\t"<<intervalo<<
    "\t"<<s->getNroMonitores()<<"\t"<<s->getCustoTot()<<"\t"<<s->verificadorFull()<<"\n";
    std::cout << "Fim contrutivo: rand = "<<pRand*100<<"%"<< std::endl;
/*
      ///BUSCA LOCAL
      if(buscaLoc){
        std::cout <<"\tTentando busca local 1: "<<pRand*100<<"%"<< std::endl;
        TBuscaLocal *bl1 = new TBuscaLocal(pccp);

        t1 = clock();
        bl1->buscaLoc_HMPP(s,pRand);
        t2= clock();
        intervalo = ((t2 - t1) / (CLOCKS_PER_SEC / 1000));

        fout<<"\t"<<heuristica<<"+BL"<<"\t"<<arquivo<<"\t"<<intervaloArq<<"\t"<<seed<<"\t"<<pRand<<
        "\t"<<s->getBSize()/pccp->getBSize()<<"\t"<<intervalo<<
        "\t"<<s->getNroMonitores()<<"\t"<<s->getCustoTot()<<"\t"<<s->verificadorFull()<<"\n";
        std::cout << "\tFim tbusca local"<< std::endl;

        delete bl1;
      }
      ///fim BUSCA LOCAL
*/


    delete s;
    ///fecha os arquivos
    fout.close();
    std::cout << ">>Exportacao concluida - result_log.pccp\n\n" <<std::endl;
  }else{///fim se abriu os arquivos
    std::cout << ">>Falha ao exportar o arquivo\n\n" <<std::endl;
  }

  delete pccp;

}


















void pareto(char *arquivo, int seed, float pRand, char* heurist, bool limpeza, bool buscaLoc, bool gravarLog){

  TInstancia *pccp = new TInstancia();

  clock_t t1;
  clock_t t2;

  t1 = clock();

  pccp->loadFile(arquivo);
  pccp->show();
  t2 = clock();
  long intervaloArq = ((t2 - t1) / (CLOCKS_PER_SEC / 1000));
  std::cout << "Tempo de carga da instancia " << intervaloArq <<"ms"<<std::endl<<std::endl;

  //pccp->exportar();


///***************** SOLUÇÃO *****************///
///***************** PARETO *****************///


  char* saida="result_log.pccp";

  std::ofstream fout(saida, std::ios::app);///arquivo de destino
  ///fim ajustes no nome do aquivo de log

  if(fout.is_open() && fout.good()){
    THPareto *s = new THPareto(pccp);
    std::cout <<"Iniciando construtivo\nRandomizacao: "<<pRand*100<<"%"<< std::endl;
    t1 = clock();
    std::string heuristica= heurist;
    if(limpeza) heuristica.append("+L");
    s->construtivo(0,pRand,0, gravarLog);
    //s->construtivo(pRand,0,0, gravarLog);
    //s->exportar();
    if(limpeza){
      std::cout<<"\tExecutando limpeza\n";
      s->limpeza2S();
      std::cout<<"\tFim da execucao da limpeza\n";
    }
    if(buscaLoc) heuristica.append("+BL");
    t2= clock();
    long intervalo = ((t2 - t1) / (CLOCKS_PER_SEC / 1000));

    fout<<"\t"<<heuristica<<"\t"<<arquivo<<"\t"<<intervaloArq<<"\t"<<seed<<"\t"<<pRand<<
    "\t"<<s->getBSize()/pccp->getBSize()<<"\t"<<intervalo<<
    "\t"<<s->getNroMonitores()<<"\t"<<s->getCustoTot()<<"\t"<<s->verificadorFull()<<"\n";
    std::cout << "Fim contrutivo: rand = "<<pRand*100<<"%"<< std::endl;

      ///BUSCA LOCAL
      if(buscaLoc){
        std::cout <<"\tTentando busca local : "<<pRand*100<<"%"<< std::endl;
        TBuscaLocal *bl1 = new TBuscaLocal(pccp);

        t1 = clock();
        bl1->buscaLoc_HMPP(s,0,pRand);
        t2= clock();
        intervalo = ((t2 - t1) / (CLOCKS_PER_SEC / 1000));

        fout<<"\t"<<heuristica<<"\t"<<arquivo<<"\t"<<intervaloArq<<"\t"<<seed<<"\t"<<pRand<<
        "\t"<<s->getBSize()/pccp->getBSize()<<"\t"<<intervalo<<
        "\t"<<s->getNroMonitores()<<"\t"<<s->getCustoTot()<<"\t"<<s->verificadorFull()<<"\n";
        std::cout << "\tFim busca local: rand = "<<pRand*100<<"%"<< std::endl;

        delete bl1;
      }
      ///fim BUSCA LOCAL



    delete s;
    ///fecha os arquivos
    fout.close();
    std::cout << ">>Exportacao concluida - result_log.pccp\n\n" <<std::endl;
  }else{///fim se abriu os arquivos
    std::cout << ">>Falha ao exportar o arquivo\n\n" <<std::endl;
  }

  delete pccp;

}



int main(int argc, char *argv[])
{

  char *arquivos[65];
  arquivos[0] = "instancias/n300-i0-m150-b150-c100.cst";
  arquivos[1] = "instancias/n300-i0-m300-b150-c10.cst";
  arquivos[2] = "instancias/n300-i0-m300-b150-c1000.cst";
  arquivos[3] = "instancias/n300-i0-m300-b300-c10.cst";
  arquivos[4] = "instancias/n300-i0-m300-b300-c100.cst";
  arquivos[5] = "instancias/n300-i0-m300-b300-c1000.cst";
  arquivos[6] = "instancias/n300-i0-m300-b75-c100.cst";
  arquivos[7] = "instancias/n300-i0-m300-b75-c1000.cst";
  arquivos[8] = "instancias/n300-i1-m300-b150-c10.cst";
  arquivos[9] = "instancias/n300-i1-m300-b150-c1000.cst";
  arquivos[10] = "instancias/n300-i1-m300-b300-c100.cst";
  arquivos[11] = "instancias/n300-i1-m300-b75-c1000.cst";
  arquivos[12] = "instancias/n300-i1-m75-b75-c1000.cst";
  arquivos[13] = "instancias/n300-i2-m150-b150-c10.cst";
  arquivos[14] = "instancias/n300-i2-m150-b150-c100.cst";
  arquivos[15] = "instancias/n300-i2-m150-b150-c1000.cst";
  arquivos[16] = "instancias/n300-i2-m300-b150-c100.cst";
  arquivos[17] = "instancias/n300-i2-m300-b150-c1000.cst";
  arquivos[18] = "instancias/n300-i2-m300-b300-c100.cst";
  arquivos[19] = "instancias/n300-i3-m150-b150-c10.cst";
  arquivos[20] = "instancias/n300-i3-m300-b150-c10.cst";
  arquivos[21] = "instancias/n300-i3-m300-b150-c1000.cst";
  arquivos[22] = "instancias/n300-i3-m300-b300-c10.cst";
  arquivos[23] = "instancias/n300-i3-m300-b300-c100.cst";
  arquivos[24] = "instancias/n300-i3-m300-b300-c1000.cst";
  arquivos[25] = "instancias/n300-i3-m300-b75-c100.cst";
  arquivos[26] = "instancias/n300-i3-m300-b75-c1000.cst";
  arquivos[27] = "instancias/n300-i4-m150-b150-c10.cst";
  arquivos[28] = "instancias/n300-i4-m300-b150-c10.cst";
  arquivos[29] = "instancias/n300-i4-m300-b150-c100.cst";
  arquivos[30] = "instancias/n300-i4-m300-b150-c1000.cst";
  arquivos[31] = "instancias/n300-i4-m300-b300-c10.cst";
  arquivos[32] = "instancias/n300-i4-m300-b300-c100.cst";
  arquivos[33] = "instancias/n300-i4-m300-b300-c1000.cst";
  arquivos[34] = "instancias/n300-i5-m150-b150-c1000.cst";
  arquivos[35] = "instancias/n300-i5-m300-b150-c10.cst";
  arquivos[36] = "instancias/n300-i5-m300-b150-c100.cst";
  arquivos[37] = "instancias/n300-i6-m300-b150-c100.cst";
  arquivos[38] = "instancias/n300-i6-m300-b150-c1000.cst";
  arquivos[39] = "instancias/n300-i6-m300-b300-c10.cst";
  arquivos[40] = "instancias/n300-i6-m300-b300-c1000.cst";
  arquivos[41] = "instancias/n300-i6-m300-b75-c1000.cst";
  arquivos[42] = "instancias/n300-i7-m150-b150-c1000.cst";
  arquivos[43] = "instancias/n300-i7-m300-b150-c100.cst";
  arquivos[44] = "instancias/n300-i7-m300-b300-c10.cst";
  arquivos[45] = "instancias/n300-i7-m300-b300-c100.cst";
  arquivos[46] = "instancias/n300-i7-m300-b300-c1000.cst";
  arquivos[47] = "instancias/n300-i7-m300-b75-c100.cst";
  arquivos[48] = "instancias/n300-i8-m300-b150-c100.cst";
  arquivos[49] = "instancias/n300-i8-m300-b300-c1000.cst";
  arquivos[50] = "instancias/n300-i8-m300-b75-c100.cst";
  arquivos[51] = "instancias/n300-i9-m300-b150-c100.cst";
  arquivos[52] = "instancias/n300-i9-m300-b300-c10.cst";
  arquivos[53] = "instancias/n300-i9-m300-b300-c100.cst";
  arquivos[54] = "instancias/n300-i9-m300-b75-c100.cst";
  arquivos[55] = "instancias/n558-i0-m140-b140-c100.cst";
  arquivos[56] = "instancias/n558-i0-m279-b279-c10.cst";
  arquivos[57] = "instancias/n558-i0-m279-b279-c100.cst";
  arquivos[58] = "instancias/n558-i0-m279-b279-c1000.cst";
  arquivos[59] = "instancias/n558-i0-m558-b140-c10.cst";
  arquivos[60] = "instancias/n558-i0-m558-b140-c100.cst";
  arquivos[61] = "instancias/n558-i0-m558-b140-c1000.cst";
  arquivos[62] = "instancias/n558-i0-m558-b279-c10.cst";
  arquivos[63] = "instancias/n558-i0-m558-b279-c100.cst";
  arquivos[64] = "instancias/n558-i0-m558-b558-c1000.cst";

/*
  ///*** Bloco: Chamada manual para testes ***
  int vSeed[10]={1,27,791,3210,65602,900312,4915732,70391063,991003774,1009444382};
  int seed = vSeed[9];
  //int seed = time(NULL);
  srand(seed);///inicializa randomização
  float pRand = 0;
  bool limpeza= true;


  pareto(arquivos[58],seed,pRand,"hmpp",limpeza,false,true);
  //pareto2(arquivos[0],seed,pRand,"hmpp2",limpeza,false,true);
  //melhorPar(arquivos[0],seed,pRand,"hmpr",limpeza,false,true);
  ///*** FIM - Bloco: Chamada manual para testes ***
*/



/*
  ///*** Bloco: Chamada automática para testes ***
  if(argc>4){
    int ixArq = atoi(argv[1]);
    char* heu= argv[2];
    int seed= atoi(argv[3]);
    float pRand= (atof(argv[4]))/100;
    bool limpeza= false;
    bool buscaLoc= false;
    if(argc>5 && argv[5][0]=='s') limpeza= true;
    if(argc>6 && argv[6][0]=='s') buscaLoc= true;
    if(seed==0) seed= time(NULL);
    std::cout<<"Arq: "<<arquivos[ixArq]<<"\nHeuristica: "<<heu<<
    "\nSeed: "<<seed<<"\nRand: "<<pRand*100<<"%\n"<<"Limpeza: "<<
    limpeza<<"\n"<<"Busca local: "<<buscaLoc<<"\n";
    std::string heuristica = heu;
    srand(seed);
    if(heuristica.size()>4 && heuristica.compare(0,5,"hmpp2")==0)
      pareto2(arquivos[ixArq],seed,pRand,heu,limpeza,buscaLoc,false);
    else if(heuristica.compare(0,4,"hmpp")==0)
      pareto(arquivos[ixArq],seed,pRand,heu,limpeza,buscaLoc,false);
    else
      melhorPar(arquivos[ixArq],seed,pRand,heu, limpeza,buscaLoc,false);
  }
  ///*** FIM - Bloco: Chamada automática para testes ***
*/





  ///*** Bloco: testes extras (manuais) ***
  TInstancia* pccp= new TInstancia();
  pccp->loadFile(arquivos[2]);
  //pccp->exportar();
  THPareto* s= new THPareto(pccp);
  s->construtivo(0, 0.2,0,false);
  std::cout<<s->getNroMonitores()<<"\t"<<s->getCustoTot()<<"\t"<<s->verificadorFull()<<"\n";
  s->limpeza2S();
  std::cout<<s->getNroMonitores()<<"\t"<<s->getCustoTot()<<"\t"<<s->verificadorFull()<<"\n";
  TBuscaLocal* bl= new TBuscaLocal(pccp);
  bl->buscaLoc_HMPP(s,0,.2);
  std::cout<<s->getNroMonitores()<<"\t"<<s->getCustoTot()<<"\t"<<s->verificadorFull()<<"\n";

  delete s;
  delete pccp;
  ///*** FIM - Bloco: testes extras (manuais) ***





  std::cout << "\nFim" << std::endl;
  return 0;
}
