#include "register.h"

Register::Register()
{
    this->value = 0;
    this->busy = false;
}

void Register::setValue(int value)
{
    this->value = value;
}

void Register::setBusy(bool busy)
{
    this->busy = busy;
}

int Register::getValue()
{
    return value;
}

bool Register::getBusy()
{
    return busy;
}
