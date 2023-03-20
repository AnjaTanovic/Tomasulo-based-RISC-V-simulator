#include "reservationstationaddsub.h"

ReservationStationAddSub::ReservationStationAddSub()
{
    this->busy = false;
    this->op = "";
    this->vj = 0;
    this->vk = 0;
}

void ReservationStationAddSub::setBusy(bool busy)
{
    this->busy = busy;
}

void ReservationStationAddSub::setOp(QString op)
{
    this->op = op;
}

void ReservationStationAddSub::setVj(int vj)
{
    this->vj = vj;
}

void ReservationStationAddSub::setVk(int vk)
{
    this->vk = vk;
}

bool ReservationStationAddSub::getBusy()
{
    return busy;
}

QString ReservationStationAddSub::getOp()
{
    return op;
}

int ReservationStationAddSub::getVj()
{
    return vj;
}

int ReservationStationAddSub::getVk()
{
    return vk;
}
