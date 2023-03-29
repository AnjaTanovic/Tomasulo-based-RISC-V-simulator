#include "reservationstationload.h"

ReservationStationLoad::ReservationStationLoad()
{
    this->busy = false;
    this->addrReg = "";
    this->q = "";
    this->imm = "";
    this->addr = 0;
    this->atCycle = 0;
    this->working = false;
}

void ReservationStationLoad::setBusy(bool busy)
{
    this->busy = busy;
}

void ReservationStationLoad::setAddrReg(QString addrReg)
{
    this->addrReg = addrReg;
}

void ReservationStationLoad::setQ(QString q)
{
    this->q = q;
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

void ReservationStationLoad::setWorking(bool working)
{
    this->working = working;
}

bool ReservationStationLoad::getBusy()
{
    return busy;
}

QString ReservationStationLoad::getAddrReg()
{
    return addrReg;
}

QString ReservationStationLoad::getQ()
{
    return q;
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

bool ReservationStationLoad::getWorking()
{
    return working;
}

void ReservationStationLoad::read()
{
    busy = false;
    addrReg = "";
    imm = "";
    addr = 0;
    atCycle = 0;
    working = false;
}
