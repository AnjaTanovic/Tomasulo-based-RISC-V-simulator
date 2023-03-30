#include "reservationstationstore.h"

ReservationStationStore::ReservationStationStore()
{
    this->busy = false;
    this->addr = 0;
    this->vj = "";
    this->qj = "";
    this->atCycle = 0;
    this->working = false;
}

void ReservationStationStore::setBusy(bool busy)
{
    this->busy = busy;
}

void ReservationStationStore::setAddr(int addr)
{
    this->addr = addr;
}

void ReservationStationStore::setVj(QString vj)
{
    this->vj = vj;
}

void ReservationStationStore::setQj(QString qj)
{
    this->qj = qj;
}

void ReservationStationStore::setAtCycle(int cycle)
{
    this->atCycle = cycle;
}

void ReservationStationStore::setWorking(bool working)
{
    this->working = working;
}

bool ReservationStationStore::getBusy()
{
    return busy;
}

int ReservationStationStore::getAddr()
{
    return addr;
}

QString ReservationStationStore::getVj()
{
    return vj;
}

QString ReservationStationStore::getQj()
{
    return qj;
}

int ReservationStationStore::getAtCycle()
{
    return atCycle;
}

bool ReservationStationStore::getWorking()
{
    return working;
}

void ReservationStationStore::write()
{
    busy = false;
    addr = 0;
    vj = "";
    qj = "";
    atCycle = 0;
    working = false;
}
