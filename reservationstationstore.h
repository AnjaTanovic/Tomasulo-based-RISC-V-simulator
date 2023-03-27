#ifndef RESERVATIONSTATIONSTORE_H
#define RESERVATIONSTATIONSTORE_H
#include <QString>

class ReservationStationStore
{
public:
    ReservationStationStore();
    void setBusy(bool busy);
    void setAddrReg(QString addrReg);
    void setImm(QString imm);
    void setAddr(int addr);
    void setVj(QString vj);
    void setAtCycle(int cycle);
    void setWorking(bool working);
    bool getBusy();
    QString getAddrReg();
    QString getImm();
    int getAddr();
    QString getVj();
    int getAtCycle();
    bool getWorking();
    void write();
private:
    bool busy;
    QString addrReg;
    QString imm;
    int addr;
    QString vj;
    int atCycle;
    bool working;
};

#endif // RESERVATIONSTATIONSTORE_H
