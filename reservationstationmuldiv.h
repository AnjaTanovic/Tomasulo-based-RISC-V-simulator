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
    void setAtCycle(int cycle);
    bool getBusy();
    QString getOp();
    QString getVj();
    QString getVk();
    int getAtCycle();
    int calculate();
private:
    bool busy;
    QString op;
    QString vj;
    QString vk;
    int atCycle;
};

#endif // RESERVATIONSTATIONMULDIV_H
