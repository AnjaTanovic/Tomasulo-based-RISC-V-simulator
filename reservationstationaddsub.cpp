#include "reservationstationaddsub.h"

ReservationStationAddSub::ReservationStationAddSub()
{
    this->busy = false;
    this->op = "";
    this->vj = "";
    this->vk = "";
    this->qj = "";
    this->qk = "";
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

void ReservationStationAddSub::setQj(QString qj)
{
    this->qj = qj;
}

void ReservationStationAddSub::setQk(QString qk)
{
    this->qk = qk;
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

QString ReservationStationAddSub::getQj()
{
    return qj;
}

QString ReservationStationAddSub::getQk()
{
    return qk;
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
    if (op == "add" || op == "addi")
        result = vj.toInt() + vk.toInt();
    else
        result = vj.toInt() - vk.toInt();

    busy = false;
    op = "";
    vj = "";
    vk = "";
    qj = "";
    qk = "";
    atCycle = 0;
    working = false;

    return result;
}
