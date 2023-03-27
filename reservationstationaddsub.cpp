#include "reservationstationaddsub.h"

ReservationStationAddSub::ReservationStationAddSub()
{
    this->busy = false;
    this->op = "";
    this->vj = "";
    this->vk = "";
    this->resReg = "";
    this->atCycle = 0;
    this->working = false;
}

void ReservationStationAddSub::setBusy(bool busy)
{
    this->busy = busy;
}

void ReservationStationAddSub::setOp(QString op)
{
    this->op = op;
}

void ReservationStationAddSub::setVj(QString vj)
{
    this->vj = vj;
}

void ReservationStationAddSub::setVk(QString vk)
{
    this->vk = vk;
}

void ReservationStationAddSub::setResReg(QString resReg)
{
    this->resReg = resReg;
}

void ReservationStationAddSub::setAtCycle(int cycle)
{
    this->atCycle = cycle;
}

void ReservationStationAddSub::setWorking(bool working)
{
    this->working = working;
}

bool ReservationStationAddSub::getBusy()
{
    return busy;
}

QString ReservationStationAddSub::getOp()
{
    return op;
}

QString ReservationStationAddSub::getVj()
{
    return vj;
}

QString ReservationStationAddSub::getVk()
{
    return vk;
}

QString ReservationStationAddSub::getResReg()
{
    return resReg;
}

int ReservationStationAddSub::getAtCycle()
{
    return atCycle;
}

bool ReservationStationAddSub::getWorking()
{
    return working;
}

int ReservationStationAddSub::calculate()
{
    int result;
    if (op.toLower() == "add")
        result = vj.toInt() + vk.toInt();
    else
        result = vj.toInt() - vk.toInt();

    busy = false;
    op = "";
    vj = "";
    vk = "";
    resReg = "";
    atCycle = 0;
    working = false;

    return result;
}
