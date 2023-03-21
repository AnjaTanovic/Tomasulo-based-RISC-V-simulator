#ifndef RESERVATIONSTATIONLOAD_H
#define RESERVATIONSTATIONLOAD_H
#include <QString>

class ReservationStationLoad
{
public:
    ReservationStationLoad();
    void setBusy(bool busy);
    void setAddr(int addr);
    void setAtCycle(int cycle);
    bool getBusy();
    int getAddr();
    int getAtCycle();
private:
    bool busy;
    int addr;
    int atCycle;
};

#endif // RESERVATIONSTATIONLOAD_H
