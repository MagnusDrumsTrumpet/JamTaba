#include "joyctrl.h"
#include <QMessageBox>
#include <QDebug>
//Basic implementation of a test class
//JoyCtrl will allow the user to use the Jamtaba's interface with a koystick
JoyCtrl::JoyCtrl()
{

SDL_Init( SDL_INIT_EVERYTHING );
QMessageBox msgBox;
msgBox.setText("SDL Init = Ok !");
msgBox.exec();

qDebug() << "new Joystick" << this;

    index = -1;
    name = "";
    numAxes = 0;
    numButtons = 0;
    numHats = 0;
    joy = NULL;
    axes.clear();
    buttons.clear();
    hats.clear();

}
#include <QObject>
#include <QTimer>
#include <QList>
#include <QListIterator>
void JoyCtrl::scan()
{
   //qDeleteAll(joys.begin(), joys.end());
   //joys.clear();

   /*if (SDL_WasInit(SDL_INIT_JOYSTICK) != 0) {
       qDebug() << "SDL_Quit";
       SDL_Quit();
   }
*/
   if (SDL_Init(SDL_INIT_JOYSTICK) != 0) {
       qCritical() << "SDL_INIT_JOYSTICK is fail";
       return;
   }

   int count = SDL_NumJoysticks();
   //Joystick *j;
   for (int i = 0; i < count; ++i) {
      // j = new Joystick();

       index = i;

       joy = SDL_JoystickOpen(i);
       if (!joy) {
           qCritical() << "SDL_JoystickOpen is fail, index:" << i;
           return;
       }
       name.append(SDL_JoystickName(joy));
       quint8 k;

       numAxes = SDL_JoystickNumAxes(joy);
       for (k = 0; k < numAxes; ++k)
           axes[k] = 0;

       numButtons = SDL_JoystickNumButtons(joy);
       for (k = 0; k < numButtons; ++k)
           buttons[k] = 0;

       numHats = SDL_JoystickNumHats(joy);
       for (k = 0; k < numHats; ++k)
          hats[k] = 0;



       qDebug("idx: %d, name: %s (axes: %d, buttons: %d, hats: %d)",
              index,
              name.data(),
              numAxes,
              numButtons,
              numHats);
   }

   //QListIterator<Joystick *> i(joys);
  //emit joysChanged(count, i);
}

JoyCtrl::~JoyCtrl()
{
    SDL_Quit();
    QMessageBox msgBox;
    msgBox.setText("SDL QUIT !");
    msgBox.exec();
}

