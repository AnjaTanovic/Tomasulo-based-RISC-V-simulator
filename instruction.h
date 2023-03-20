#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include <QString>

class Instruction
{
public:
    Instruction(QString op, QString reg1, QString reg2, QString reg3);
    QString getOp();
    QString getReg1();
    QString getReg2();
    QString getReg3();
private:
    QString op;
    QString reg1;
    QString reg2;
    QString reg3;
};

#endif // INSTRUCTION_H
