#ifndef THMELHORPAR_H
#define THMELHORPAR_H

#include <vector>
#include <list>
#include <cstdlib>
#include <fstream>

#include "TInstancia.h"


///-------------------- Classe: THMELHORPAR --------------------//
///
///  Representa a heuristica de melhor par pela razão
///  cobertura/custo
///
///-------------------------------------------------------------//

class THMelhorPar
{
  public:
    THMelhorPar(TInstancia* _pccp);
    THMelhorPar(TInstancia* _pccp, std::vector<bool> vetM, std::vector<bool> vetB);
    virtual ~THMelhorPar();

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

    std::ofstream logFile;


    ///struct auxiliar de ordenação da lista de branchs candidatos
    struct ordBranchs{
      bool operator()(int* b1, int* b2){
        return b1[1]<b2[1];///menor nro de pares cobrindo o branch primeiro
      }
    };

    struct ordPares{
      bool operator()(TPar* p1, TPar* p2){
        float c1= p1->getCustoPar()==0? 9999999999: p1->getCustoPar();
        float c2= p2->getCustoPar()==0? 9999999999: p2->getCustoPar();
        return (c1/p1->getAlcance())<(c2/p2->getAlcance());
      }
    };

    ///struct auxiliar de ordenação da lista de monitores
    struct ordMonitores{
      bool operator()(int* m1, int* m2){
        return m1[1]>m2[1];///mais caro na frente
      }
    };
};

#endif // THMELHORPAR_H
