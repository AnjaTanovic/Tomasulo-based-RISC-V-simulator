#include "reservationstationload.h"

ReservationStationLoad::ReservationStationLoad()
{
    this->busy = false;
    this->addr = 0;
    this->atCycle = 0;
    this->working = false;
    this->port = "";
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

void ReservationStationLoad::setWorking(bool working)
{
    this->working = working;
}

void ReservationStationLoad::setPort(QString port)
{
    this->port = port;
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

bool ReservationStationLoad::getWorking()
{
    return working;
}

QString ReservationStationLoad::getPort()
{
    return port;
}

void ReservationStationLoad::read()
{
    busy = false;
    addr = 0;
    atCycle = 0;
    working = false;
    port = "";
}
