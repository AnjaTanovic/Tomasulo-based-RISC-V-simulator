#include "reservationstationload.h"

ReservationStationLoad::ReservationStationLoad()
{
    this->busy = false;
    this->addrReg = "";
    this->imm = "";
    this->addr = 0;
    this->atCycle = 0;
}

void ReservationStationLoad::setBusy(bool busy)
{
    this->busy = busy;
}

void ReservationStationLoad::setAddrReg(QString addrReg)
{
    this->addrReg = addrReg;
}

void ReservationStationLoad::setImm(QString imm)
{
    this->imm = imm;
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

QString ReservationStationLoad::getAddrReg()
{
    return addrReg;
}

QString ReservationStationLoad::getImm()
{
    return imm;
}

int ReservationStationLoad::getAddr()
{
    return addr;
}

int ReservationStationLoad::getAtCycle()
{
    return atCycle;
}

void ReservationStationLoad::read()
{
    busy = false;
    addrReg = "";
    imm = "";
    addr = 0;
    atCycle = 0;
}
