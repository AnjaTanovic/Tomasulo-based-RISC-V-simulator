#ifndef REGISTER_H
#define REGISTER_H
#include <QString>

class Register
{
public:
    Register();
    void setValue(int value);
    void setBusy(bool busy);
    void setStation(QString station);
    int getValue();
    bool getBusy();
    QString getStation();
private:
    int value;
    bool busy;
    QString station;
};

#endif // REGISTER_H
