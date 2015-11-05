#ifndef JOYCTRL_H
#define JOYCTRL_H
#include <QMap>
#include "SDL.h"

class Joystick
{
public:
    Joystick(){};
    Joystick(int index);
    ~Joystick();
    bool enabled;
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
