#ifndef MAINFRAMEVST_H
#define MAINFRAMEVST_H

#include "MainWindow.h"

class MainWindowStandalone : public MainWindow
{
    Q_OBJECT
public:
    MainWindowStandalone(Controller::MainController *controller);

protected:
    void showEvent(QShowEvent *ent) override;

    void closeEvent(QCloseEvent *);

    NinjamRoomWindow *createNinjamWindow(Login::RoomInfo, Controller::MainController *) override;

    void initializePluginFinder() override;

    void showPreferencesDialog(int initialTab) override;

protected slots:
    void handleServerConnectionError(QString msg);

    void setGlobalPreferences(QList<bool>, int audioDevice, int firstIn, int lastIn, int firstOut,
                              int lastOut, int sampleRate, int bufferSize);
};

#endif // MAINFRAMEVST_H