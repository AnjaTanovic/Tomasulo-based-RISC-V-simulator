#include "reservationstationload.h"

ReservationStationLoad::ReservationStationLoad()
{
    this->busy = false;
    this->addr = 0;
    this->atCycle = 0;
}

void ReservationStationLoad::setBusy(bool busy)
{
    this->busy = busy;
}

void ReservationStationLoad::setAddr(int addr)
{
    this->addr = addr;
}

void ReservationStationLoad::setAtCycle(int cycle)
{
    this->atCycle = cycle;
}

bool ReservationStationLoad::getBusy()
{
    return busy;
}

int ReservationStationLoad::getAddr()
{
    return addr;
}

int ReservationStationLoad::getAtCycle()
{
    return atCycle;
}
