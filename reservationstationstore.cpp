#include "reservationstationstore.h"

ReservationStationStore::ReservationStationStore()
{
    this->busy = false;
    this->addr = 0;
    this->vj = 0;
}

void ReservationStationStore::setBusy(bool busy)
{
    this->busy = busy;
}

void ReservationStationStore::setAddr(int addr)
{
    this->addr = addr;
}

void ReservationStationStore::setVj(int vj)
{
    this->vj = vj;
}

bool ReservationStationStore::getBusy()
{
    return busy;
}

int ReservationStationStore::getAddr()
{
    return addr;
}

int ReservationStationStore::getVj()
{
    return vj;
}
