#include "register.h"

Register::Register()
{
    this->value = 0;
    this->q = "";
}

void Register::setValue(int value)
{
    this->value = value;
}

void Register::setQ(QString q)
{
    this->q = q;
}

int Register::getValue()
{
    return value;
}

QString Register::getQ()
{
    return q;
}
