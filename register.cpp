#include "register.h"

Register::Register()
{
    this->value = 0;
    this->busy = false;
    this->station = "";
}

void Register::setValue(int value)
{
    this->value = value;
}

void Register::setBusy(bool busy)
{
    this->busy = busy;
}

void Register::setStation(QString station)
{
    this->station = station;
}

int Register::getValue()
{
    return value;
}

bool Register::getBusy()
{
    return busy;
}

QString Register::getStation()
{
    return station;
}
