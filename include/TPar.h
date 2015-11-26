#ifndef TPAR_H
#define TPAR_H

#include <vector>
#include <list>

class TBranch;
class TMonitor;

///-------------------- Classe: TPAR -----------------------//
///
///  Representa um par com suas propriedades
///  e métodos de acesso
///
///--------------------------------------------------------//

class TPar
{
  public:
    TPar(int m1, int m2);
    virtual ~TPar();

    int getM1();
    int getM2();
    std::list<int> getLstBranchs();
    int getAlcance();
    int getCustoPar();
    void setCustoPar(int c);
    void setAlcance(int a);

    void addBranch(int bId);

    void ordenarLstBranchs();


  protected:
  private:
    int m1Id;
    int m2Id;
    int custo;
    int alcance;
    std::list<int> branchs;


    struct ordenarBranchs{
      bool operator()(int b1, int b2){
        return b1<=b2;
      }
    };

    struct compararBranchs{
      bool operator()(int b1, int b2){
        return b1==b2;
      }
    };
};

#endif // TPAR_H
