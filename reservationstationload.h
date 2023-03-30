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
    void setWorking(bool working);
    bool getBusy();
    int getAddr();
    int getAtCycle();
    bool getWorking();
    void read();
private:
    bool busy;
    int addr;
    int atCycle;
    bool working;
};

#endif // RESERVATIONSTATIONLOAD_H
