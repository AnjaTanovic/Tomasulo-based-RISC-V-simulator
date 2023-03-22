#include "reservationstationmuldiv.h"

ReservationStationMulDiv::ReservationStationMulDiv()
{
    this->busy = false;
    this->op = "";
    this->vj = "";
    this->vk = "";
    this->atCycle = 0;
}

void ReservationStationMulDiv::setBusy(bool busy)
{
    this->busy = busy;
}

void ReservationStationMulDiv::setOp(QString op)
{
    this->op = op;
}

void ReservationStationMulDiv::setVj(QString vj)
{
    this->vj = vj;
}

void ReservationStationMulDiv::setVk(QString vk)
{
    this->vk = vk;
}

void ReservationStationMulDiv::setAtCycle(int cycle)
{
    this->atCycle = cycle;
}

bool ReservationStationMulDiv::getBusy()
{
    return busy;
}

QString ReservationStationMulDiv::getOp()
{
    return op;
}

QString ReservationStationMulDiv::getVj()
{
    return vj;
}

QString ReservationStationMulDiv::getVk()
{
    return vk;
}

int ReservationStationMulDiv::getAtCycle()
{
    return atCycle;
}

int ReservationStationMulDiv::calculate()
{
    int result;
    if (op.toLower() == "mul")
        result = vj.toInt() * vk.toInt();
    else
        result = vj.toInt() / vk.toInt();

    busy = false;
    op = "";
    vj = "";
    vk = "";
    atCycle = 0;

    return result;
}
