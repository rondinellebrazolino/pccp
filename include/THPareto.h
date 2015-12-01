#ifndef THPARETO_H
#define THPARETO_H



#include "TInstancia.h"


#include <fstream>


///-------------------- Classe: THPARETO --------------------//
///
///  Representa a heuristica de melhor par pela análise
///  de pareto na lista de pares dos branchs
///
///----------------------------------------------------------//

class THPareto
{
  public:
    THPareto(TInstancia* _pccp);
    THPareto(std::vector<bool> _monitores,std::vector<bool> _branchs,TInstancia* _pccp,
    int _custoTotal,int _bSize,int** _vetMxB);
    virtual ~THPareto();

    void zerarSolucao();

    std::list<int*> criarLcBranchs();
    std::list<TPar*> criarLcPares(int bId);

    int selecionarBranch(float pRand, bool gravarLog);
    TPar* selecionarPar(int bId, float pRand, bool gravarLog);

    void cobrirBranchsPar(int m1Id, int m2Id);
    void atualizarPar(int m1Id, int m2Id);
    void atualizarS(int m1Id, int m2Id);
    bool verificarS();
    void construtivo(float pRandB, float pRandP, int corte, bool gravarLog);

    int getBSize();
    int getNroMonitores();
    int getCustoTot();
    std::vector<bool> getMonitores();
    std::vector<bool> getBranchs();

    THPareto* criarCopia();
    void mudarUmMonitor(int mId);

    bool removerMonitor(int mId, bool comInterrupt);
    bool removerMonitor2(int mId);

    void setMonitores(std::vector<bool> v);

    void limpezaS(bool comInterrupt);
    void limpeza2S();

    bool verificadorFull();

    void exportar();
    void importar(char* path);

  protected:
  private:
    ///vetor de monitores (true indica que o monitor está ativo na solução)
    std::vector<bool> monitores;
    ///vetor de branchs (true indica que o branchs está coberto na solução)
    std::vector<bool> branchs;
    ///Instancia original
    TInstancia* pccp;
    ///custo total corrente da solução
    int custoTotal;
    ///nº de branchs cobertos na solução corrente
    int bSize;

    ///Matriz de tamanho MxB, que indica quantas vezes o monitor mi "cobriu" o branch bj através de algum par da solução
    int** vetMxB;


    std::ofstream logFile;



    ///struct auxiliar de ordenação da lista de branchs candidatos
    struct ordBranchs{
      bool operator()(int* b1, int* b2){
        return b1[1]<b2[1];
      }
    };

    ///struct auxiliar de ordenação da lista de monitores
    struct ordMonitores{
      bool operator()(int* m1, int* m2){
        return m1[1]>m2[1];///mais caro na frente
      }
    };
};

#endif // THPARETO_H
