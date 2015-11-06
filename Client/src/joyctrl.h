#ifndef JOYCTRL_H
#define JOYCTRL_H
#include <QMap>
#include "SDL.h"

 struct InputJoystick
{
    char buttons[32];
    int axes[2];
    int hat[1];

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
    inline const char* getName(){return name;}
    inline  int getNumButtons(){return numButtons;}
    inline  int getNumHats(){return numHats;}
    inline  int getNumAxis(){return numAxes;}
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
