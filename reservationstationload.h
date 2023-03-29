#ifndef RESERVATIONSTATIONLOAD_H
#define RESERVATIONSTATIONLOAD_H
#include <QString>

class ReservationStationLoad
{
public:
    ReservationStationLoad();
    void setBusy(bool busy);
    void setAddrReg(QString addrReg);
    void setQ(QString q);
    void setImm(QString imm);
    void setAddr(int addr);
    void setAtCycle(int cycle);
    void setWorking(bool working);
    bool getBusy();
    QString getAddrReg();
    QString getQ();
    QString getImm();
    int getAddr();
    int getAtCycle();
    bool getWorking();
    void read();
private:
    bool busy;
    QString addrReg;
    QString q;
    QString imm;
    int addr;
    int atCycle;
    bool working;
};

#endif // RESERVATIONSTATIONLOAD_H
