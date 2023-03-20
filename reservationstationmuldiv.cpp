#include "reservationstationmuldiv.h"

ReservationStationMulDiv::ReservationStationMulDiv()
{
    this->busy = false;
    this->op = "";
    this->vj = 0;
    this->vk = 0;
}

void ReservationStationMulDiv::setBusy(bool busy)
{
    this->busy = busy;
}

void ReservationStationMulDiv::setOp(QString op)
{
    this->op = op;
}

void ReservationStationMulDiv::setVj(int vj)
{
    this->vj = vj;
}

void ReservationStationMulDiv::setVk(int vk)
{
    this->vk = vk;
}

bool ReservationStationMulDiv::getBusy()
{
    return busy;
}

QString ReservationStationMulDiv::getOp()
{
    return op;
}

int ReservationStationMulDiv::getVj()
{
    return vj;
}

int ReservationStationMulDiv::getVk()
{
    return vk;
}
