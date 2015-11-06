#ifndef JOYCTRL_H
#define JOYCTRL_H
#include <QMap>
#include "SDL.h"

 struct InputJoystick
{
    char *buttons;
    int *axes;
    int *hat;

};
typedef InputJoystick JOYINPUT;
class Joystick
{
public:
    Joystick(){};
    Joystick(int index);
    ~Joystick();
    JOYINPUT input;
    bool enabled;
    void updateInput();
private:
    bool init(int index);
    int index;

       const char* name;
       int numAxes;
       int numButtons;
       int numHats;
       SDL_Joystick *joy;
       QMap<quint8, Sint16> axes;
       QMap<quint8, Uint8> buttons;
       QMap<quint8, Uint8> hats;

};

#endif // JOYCTRL_H
