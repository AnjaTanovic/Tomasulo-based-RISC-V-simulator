#ifndef RESERVATIONSTATIONMULDIV_H
#define RESERVATIONSTATIONMULDIV_H
#include <QString>

class ReservationStationMulDiv
{
public:
    ReservationStationMulDiv();
    void setBusy(bool busy);
    void setOp(QString op);
    void setVj(QString vj);
    void setVk(QString vk);
    void setQj(QString qj);
    void setQk(QString qk);
    void setResReg(QString resReg);
    void setAtCycle(int cycle);
    void setWorking(bool working);
    bool getBusy();
    QString getOp();
    QString getVj();
    QString getVk();
    QString getQj();
    QString getQk();
    QString getResReg();
    int getAtCycle();
    bool getWorking();
    int calculate();
private:
    bool busy;
    QString op;
    QString vj;
    QString vk;
    QString qj;
    QString qk;
    QString resReg;
    int atCycle;
    bool working;
};

#endif // RESERVATIONSTATIONMULDIV_H
