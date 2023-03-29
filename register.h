#ifndef REGISTER_H
#define REGISTER_H
#include <QString>

class Register
{
public:
    Register();
    void setValue(int value);
    void setQ(QString q);
    int getValue();
    QString getQ();
private:
    int value;
    QString q;
};

#endif // REGISTER_H
