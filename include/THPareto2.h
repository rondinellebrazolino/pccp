#ifndef THPARETO2_H
#define THPARETO2_H




#include "TInstancia.h"


#include <fstream>


///------------------- Classe: THPARETO2 --------------------//
///
///  Representa a heuristica de melhor par pela análise
///  de pareto na lista de pares da instancia
///
///----------------------------------------------------------//

class THPareto2
{
  public:
    THPareto2(TInstancia* _pccp);
    THPareto2(TInstancia* _pccp, std::vector<bool> vetM, std::vector<bool> vetB);
    virtual ~THPareto2();

    void criarLcPares();

    TPar* selecionarPar(float pRand, bool gravarLog);

    void cobrirBranchsPar(int m1Id, int m2Id);
    void atualizarPar(int m1Id, int m2Id);
    void atualizarS(int m1Id, int m2Id);
    bool verificarS();
    void construtivo(float pRandB, float pRandP, int corte, bool gravarLog);

    int getBSize();
    int getNroMonitores();
    int getCustoTot();
    std::vector<bool> getMonitores();

    bool removerMonitor(int mId, bool comInterrupt);

    void setMonitores(std::vector<bool> v);

    void limpezaS(bool comInterrupt);

    bool verificadorFull();

    void exportar();
    void importar(char* path);

  protected:
  private:
    ///vetor de monitores (true indica que o monitor está ativo na solução)
    std::vector<bool> monitores;
    ///vetor de branchs (true indica que o branchs está coberto na solução)
    std::vector<bool> branchs;
    ///vetor de cobertura (indica quantos pares o branch bi possui ativos na solução)--- AINDA N USADO
    std::vector<int> cobertura;
    ///Instancia original
    TInstancia* pccp;
    ///custo total corrente da solução
    int custoTotal;
    ///nº de branchs cobertos na solução corrente
    int bSize;

    ///
    std::list<TPar*> candidatos;


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

#endif // THPARETO2_H
