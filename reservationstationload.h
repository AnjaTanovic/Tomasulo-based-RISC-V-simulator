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
    void setPort(QString port);
    bool getBusy();
    int getAddr();
    int getAtCycle();
    bool getWorking();
    QString getPort();
    void read();
private:
    bool busy;
    int addr;
    int atCycle;
    bool working;
    QString port;
};

#endif // RESERVATIONSTATIONLOAD_H
