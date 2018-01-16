#ifndef NINJAMJAMROOMCONTROLLER_H
#define NINJAMJAMROOMCONTROLLER_H

#include <QObject>
#include <QMutex>
#include "ninjam/User.h"
#include "ninjam/Server.h"
#include "audio/Encoder.h"

#include <QThread>

class NinjamTrackNode;

namespace audio {
class MetronomeTrackNode;
class SamplesBuffer;
}

namespace controller {
class MainController;

// ++++++++++++++++++++

class NinjamController : public QObject
{
    Q_OBJECT

public:
    explicit NinjamController(controller::MainController *mainController);
    virtual ~NinjamController();
    virtual void process(const audio::SamplesBuffer &in, audio::SamplesBuffer &out, int sampleRate);
    void start(const ninjam::Server &server);
    void stop(bool emitDisconnectedSignal);
    bool isRunning() const;

    void setMetronomeBeatsPerAccent(int beatsPerAccent, int currentBpi);
    void setMetronomeAccentBeats(QList<int> accentBeats);
    QList<int> getMetronomeAccentBeats();

    int getCurrentBpi() const;
    int getCurrentBpm() const;

    void voteBpi(int newBpi);
    void voteBpm(int newBpm);

    void setBpm(int newBpm);

    void sendChatMessage(const QString &msg);

    static const long METRONOME_TRACK_ID = 123456789; // just a number :)

    void recreateEncoders();

    QByteArray encode(const audio::SamplesBuffer &buffer, uint channelIndex);
    QByteArray encodeLastPartOfInterval(uint channelIndex);

    void scheduleEncoderChangeForChannel(int channelIndex);
    void removeEncoder(int groupChannelIndex);

    void scheduleXmitChange(int channelID, bool transmiting); // schedule the change for the next interval

    void setSampleRate(int newSampleRate);

    void reset(bool keepRecentIntervals); // discard downloaded intervals and reset intervalPosition

    bool isPreparedForTransmit() const;

    void recreateMetronome(int newSampleRate);

    void blockUserInChat(const ninjam::User &user);
    void blockUserInChat(const QString &userNameToBlock);

    void unblockUserInChat(const ninjam::User &user);
    void unblockUserInChat(const QString &userNameToBlock);

    bool userIsBlockedInChat(const QString &userName) const;

    bool userIsBot(const QString userName) const;

    ninjam::User getUserByName(const QString &userName) const;

    uint getSamplesPerInterval() const;

    QList<NinjamTrackNode *> getTrackNodes() const;

signals:
    void currentBpiChanged(int newBpi); // emitted when a scheduled bpi change is processed in interval start (first beat).
    void currentBpmChanged(int newBpm);

    void intervalBeatChanged(int intervalBeat);
    void startingNewInterval();
    void startProcessing(int intervalPosition);
    void channelAdded(const ninjam::User &user, const ninjam::UserChannel &channel, long channelID);
    void channelRemoved(const ninjam::User &user, const ninjam::UserChannel &channel, long channelID);
    void channelNameChanged(const ninjam::User &user, const ninjam::UserChannel &channel, long channelID);
    void channelXmitChanged(long channelID, bool transmiting);
    void channelAudioChunkDownloaded(long channelID);
    void channelAudioFullyDownloaded(long channelID);
    void userLeave(const QString &userName);
    void userEnter(const QString &userName);

    void publicChatMessageReceived(const ninjam::User &user, const QString &message );
    void privateChatMessageReceived(const ninjam::User &user, const QString &message );
    void topicMessageReceived(const QString &message);

    void encodedAudioAvailableToSend(const QByteArray &encodedAudio, quint8 channelIndex, bool isFirstPart, bool isLastPart);

    void userBlockedInChat(const QString &userName);
    void userUnblockedInChat(const QString &userName);

    void preparingTransmission(); // waiting for start transmission
    void preparedToTransmit(); // this signal is emmited one time, when Jamtaba is ready to transmit (after wait some complete itervals)

    void started();

protected:
    long intervalPosition;
    long samplesInInterval;

    QMap<QString, NinjamTrackNode *> trackNodes; // the other users channels

    controller::MainController *mainController;

    audio::MetronomeTrackNode *metronomeTrackNode;

private:
    static QString getUniqueKeyForChannel(const ninjam::UserChannel &channel);
    static QString getUniqueKeyForUser(const ninjam::User& user);

    void addTrack(const ninjam::User &user, const ninjam::UserChannel &channel);
    void removeTrack(const ninjam::User &user, const ninjam::UserChannel &channel);

    static bool userIsBlockedInChat(const ninjam::User &user);

    static QList<QString> chatBlockedUsers; // using static to keep the blocked users list until Jamtaba is closed.

    bool running;
    int lastBeat;

    int currentBpi;
    int currentBpm;

    QMutex mutex;
    QMutex encodersMutex;

    long computeTotalSamplesInInterval();
    long getSamplesPerBeat();

    void processScheduledChanges();
    bool hasScheduledChanges() const;

    static long generateNewTrackID();

    audio::MetronomeTrackNode *createMetronomeTrackNode(int sampleRate);

    QMap<int, AudioEncoder *> encoders;
    AudioEncoder *getEncoder(quint8 channelIndex);

    void handleNewInterval();
    void recreateEncoderForChannel(int channelIndex);

    void setXmitStatus(int channelID, bool transmiting);

    // ++++++++++++++++++++ nested classes to handle scheduled events +++++++++++++++++

    class SchedulableEvent; // the interface for all events
    class BpiChangeEvent;
    class BpmChangeEvent;
    class InputChannelChangedEvent;// user change the channel input selection from mono to stereo or vice-versa, or user added a new channel, both cases requires a new encoder in next interval
    QList<SchedulableEvent *> scheduledEvents;

    class EncodingThread;

    EncodingThread *encodingThread;

    bool preparedForTransmit;
    int waitingIntervals;
    static const int TOTAL_PREPARED_INTERVALS = 2; // how many intervals Jamtaba will wait to start trasmiting?

private slots:
    // ninjam events
    void scheduleBpmChangeEvent(quint16 newBpm);
    void scheduleBpiChangeEvent(quint16 newBpi, quint16 oldBpi);
    void handleIntervalCompleted(const ninjam::User &user, quint8 channelIndex, const QByteArray &encodedAudioData);
    void handleIntervalDownloading(const ninjam::User &user, quint8 channelIndex, int downloadedBytes);
    void addNinjamRemoteChannel(const ninjam::User &user, const ninjam::UserChannel &channel);
    void removeNinjamRemoteChannel(const ninjam::User &user, const ninjam::UserChannel &channel);
    void updateNinjamRemoteChannel(const ninjam::User &user, const ninjam::UserChannel &channel);
    void handleNinjamUserExiting(const ninjam::User &user);
    void handleNinjamUserEntering(const ninjam::User &user);
    void handleReceivedPublicChatMessage(const ninjam::User &user, const QString &message);
    void handleReceivedPrivateChatMessage(const ninjam::User &user, const QString &message);

}; // end of class

inline QList<NinjamTrackNode *> NinjamController::getTrackNodes() const
{
    return trackNodes.values();
}

inline bool NinjamController::hasScheduledChanges() const
{
    return !scheduledEvents.isEmpty();
}

inline bool NinjamController::isPreparedForTransmit() const
{
    return preparedForTransmit;
}

inline int NinjamController::getCurrentBpi() const
{
    return currentBpi;
}

inline int NinjamController::getCurrentBpm() const
{
    return currentBpm;
}

inline bool NinjamController::isRunning() const
{
    return running;
}

inline uint NinjamController::getSamplesPerInterval() const
{
    return samplesInInterval;
}

}
#endif // NINJAMJAMROOMCONTROLLER_H
