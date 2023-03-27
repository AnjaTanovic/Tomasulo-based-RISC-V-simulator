#include "reservationstationmuldiv.h"

ReservationStationMulDiv::ReservationStationMulDiv()
{
    this->busy = false;
    this->op = "";
    this->vj = "";
    this->vk = "";
    this->resReg = "";
    this->atCycle = 0;
    this->working = false;
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

void ReservationStationMulDiv::setResReg(QString resReg)
{
    this->resReg = resReg;
}

void ReservationStationMulDiv::setAtCycle(int cycle)
{
    this->atCycle = cycle;
}

void ReservationStationMulDiv::setWorking(bool working)
{
    this->working = working;
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

QString ReservationStationMulDiv::getResReg()
{
    return resReg;
}

int ReservationStationMulDiv::getAtCycle()
{
    return atCycle;
}

bool ReservationStationMulDiv::getWorking()
{
    return working;
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
    resReg = "";
    atCycle = 0;
    working = false;

    return result;
}
