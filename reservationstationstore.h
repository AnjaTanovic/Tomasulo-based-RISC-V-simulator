#ifndef RESERVATIONSTATIONSTORE_H
#define RESERVATIONSTATIONSTORE_H
#include <QString>

class ReservationStationStore
{
public:
    ReservationStationStore();
    void setBusy(bool busy);
    void setAddr(int addr);
    void setVj(int vj);
    void setAtCycle(int cycle);
    bool getBusy();
    int getAddr();
    int getVj();
    int getAtCycle();
private:
    bool busy;
    int addr;
    int vj;
    int atCycle;
};

#endif // RESERVATIONSTATIONSTORE_H
