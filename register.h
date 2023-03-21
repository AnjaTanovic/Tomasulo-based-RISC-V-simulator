#ifndef REGISTER_H
#define REGISTER_H


class Register
{
public:
    Register();
    void setValue(int value);
    void setBusy(bool busy);
    int getValue();
    bool getBusy();
private:
    int value;
    bool busy;
};

#endif // REGISTER_H
