#include "instruction.h"

Instruction::Instruction(QString op, QString reg1, QString reg2, QString reg3)
{
    this->op = op.toLower();
    this->reg1 = reg1.toLower();
    this->reg2 = reg2.toLower();
    this->reg3 = reg3.toLower();
}

QString Instruction::getOp()
{
    return op;
}
QString Instruction::getReg1()
{
    return reg1;
}

QString Instruction::getReg2()
{
    return reg2;
}

QString Instruction::getReg3()
{
    return reg3;
}
