#ifndef RESERVATIONSTATIONLOAD_H
#define RESERVATIONSTATIONLOAD_H
#include <QString>

class ReservationStationLoad
{
public:
    ReservationStationLoad();
    void setBusy(bool busy);
    void setAddr(int addr);
    bool getBusy();;
    int getAddr();
private:
    bool busy;
    int addr;
};

#endif // RESERVATIONSTATIONLOAD_H
