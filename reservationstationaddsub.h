#ifndef RESERVATIONSTATIONADDSUB_H
#define RESERVATIONSTATIONADDSUB_H
#include <QString>

class ReservationStationAddSub
{
public:
    ReservationStationAddSub();
    void setBusy(bool busy);
    void setOp(QString op);
    void setVj(int vj);
    void setVk(int vk);
    void setAtCycle(int cycle);
    bool getBusy();
    QString getOp();
    int getVj();
    int getVk();
    int getAtCycle();
private:
    bool busy;
    QString op;
    int vj;
    int vk;
    int atCycle;
};

#endif // RESERVATIONSTATIONADDSUB_H
