#include "reservationstationload.h"

ReservationStationLoad::ReservationStationLoad()
{
    this->busy = false;
    this->addr = 0;
}

void ReservationStationLoad::setBusy(bool busy)
{
    this->busy = busy;
}

void ReservationStationLoad::setAddr(int addr)
{
    this->addr = addr;
}

bool ReservationStationLoad::getBusy()
{
    return busy;
}

int ReservationStationLoad::getAddr()
{
    return addr;
}
