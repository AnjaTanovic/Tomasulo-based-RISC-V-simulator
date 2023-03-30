#ifndef RESERVATIONSTATIONSTORE_H
#define RESERVATIONSTATIONSTORE_H
#include <QString>

class ReservationStationStore
{
public:
    ReservationStationStore();
    void setBusy(bool busy);
    void setAddr(int addr);
    void setVj(QString vj);
    void setQj(QString qj);
    void setAtCycle(int cycle);
    void setWorking(bool working);
    bool getBusy();
    int getAddr();
    QString getVj();
    QString getQj();
    int getAtCycle();
    bool getWorking();
    void write();
private:
    bool busy;
    int addr;
    QString vj;
    QString qj;
    int atCycle;
    bool working;
};

#endif // RESERVATIONSTATIONSTORE_H
