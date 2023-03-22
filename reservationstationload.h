#ifndef RESERVATIONSTATIONLOAD_H
#define RESERVATIONSTATIONLOAD_H
#include <QString>

class ReservationStationLoad
{
public:
    ReservationStationLoad();
    void setBusy(bool busy);
    void setAddrReg(QString addrReg);
    void setImm(QString imm);
    void setAddr(int addr);
    void setAtCycle(int cycle);
    bool getBusy();
    QString getAddrReg();
    QString getImm();
    int getAddr();
    int getAtCycle();
private:
    bool busy;
    QString addrReg;
    QString imm;
    int addr;
    int atCycle;
};

#endif // RESERVATIONSTATIONLOAD_H
