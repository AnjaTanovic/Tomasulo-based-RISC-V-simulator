#include "reservationstationaddsub.h"

ReservationStationAddSub::ReservationStationAddSub()
{
    this->busy = false;
    this->op = "";
    this->vj = "";
    this->vk = "";
    this->atCycle = 0;
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

void ReservationStationAddSub::setAtCycle(int cycle)
{
    this->atCycle = cycle;
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

int ReservationStationAddSub::getAtCycle()
{
    return atCycle;
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
    atCycle = 0;

    return result;
}
