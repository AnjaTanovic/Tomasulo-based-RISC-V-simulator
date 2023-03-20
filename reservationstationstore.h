#ifndef RESERVATIONSTATIONSTORE_H
#define RESERVATIONSTATIONSTORE_H
#include <QString>

class ReservationStationStore
{
public:
    ReservationStationStore();
    void setBusy(bool busy);
    void setAddr(int addr);
    void setVj(int vj);
    bool getBusy();
    int getAddr();
    int getVj();
private:
    bool busy;
    int addr;
    int vj;
};

#endif // RESERVATIONSTATIONSTORE_H
