#include "reservationstationstore.h"

ReservationStationStore::ReservationStationStore()
{
    this->busy = false;
    this->addrReg = "";
    this->imm = "";
    this->addr = 0;
    this->vj = "";
    this->atCycle = 0;
}

void ReservationStationStore::setBusy(bool busy)
{
    this->busy = busy;
}

void ReservationStationStore::setAddrReg(QString addrReg)
{
    this->addrReg = addrReg;
}

void ReservationStationStore::setImm(QString imm)
{
    this->imm = imm;
}

void ReservationStationStore::setAddr(int addr)
{
    this->addr = addr;
}

void ReservationStationStore::setVj(QString vj)
{
    this->vj = vj;
}

void ReservationStationStore::setAtCycle(int cycle)
{
    this->atCycle = cycle;
}

bool ReservationStationStore::getBusy()
{
    return busy;
}

QString ReservationStationStore::getAddrReg()
{
    return addrReg;
}

QString ReservationStationStore::getImm()
{
    return imm;
}

int ReservationStationStore::getAddr()
{
    return addr;
}

QString ReservationStationStore::getVj()
{
    return vj;
}

int ReservationStationStore::getAtCycle()
{
    return atCycle;
}
