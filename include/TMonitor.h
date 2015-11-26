#ifndef TMONITOR_H
#define TMONITOR_H

#include <vector>
#include <list>
#include "TPar.h"


///-------------------- Classe: TMONITOR --------------------//
///
///  Representa um monitor com suas propriedades
///  e métodos de acesso
///
///---------------------------------------------------------//

class TMonitor
{
  public:
    TMonitor(int mId);
    virtual ~TMonitor();
    int getId();
    int getCusto();
    void setCusto(int c);
  protected:
  private:
    int id;///id do monitor
    int custo;///custo do monitor
};

#endif // TMONITOR_H
