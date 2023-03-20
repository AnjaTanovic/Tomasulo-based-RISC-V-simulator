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

#endif // RESERVATIONSTATIONADDSUB_H
