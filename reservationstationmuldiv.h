#ifndef RESERVATIONSTATIONMULDIV_H
#define RESERVATIONSTATIONMULDIV_H
#include <QString>

class ReservationStationMulDiv
{
public:
    ReservationStationMulDiv();
    void setBusy(bool busy);
    void setOp(QString op);
    void setVj(int vj);
    void setVk(int vk);
    bool getBusy();
    QString getOp();
    int getVj();
    int getVk();
private:
    bool busy;
    QString op;
    int vj;
    int vk;
};

#endif // RESERVATIONSTATIONMULDIV_H
