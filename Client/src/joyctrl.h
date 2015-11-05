#ifndef JOYCTRL_H
#define JOYCTRL_H
#include <QMap>
#include "SDL.h"

class JoyCtrl
{
public:
    JoyCtrl();
    ~JoyCtrl();
    void scan();
    int index;

       QString name;
       int numAxes;
       int numButtons;
       int numHats;
       SDL_Joystick *joy;
       QMap<quint8, Sint16> axes;
       QMap<quint8, Uint8> buttons;
       QMap<quint8, Uint8> hats;

};

#endif // JOYCTRL_H
