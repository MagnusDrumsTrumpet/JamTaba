#pragma once

#include <QDataStream>
#include <QByteArray>
#include <QDataStream>
#include <QMap>
#include <memory>

namespace Ninjam {



class ServerMessage;
enum class ServerMessageType : std::uint8_t;

class ServerMessageParser  {


private:
    //useri shared_pointer porque o unique_ptr estava dando muito problema com o map
    static QMap<ServerMessageType, std::shared_ptr<ServerMessageParser>> parsers;

    static ServerMessageParser* createInstance(ServerMessageType messageType);

protected:
    static QString extractString(QDataStream& stream) ;

public:
    virtual ServerMessage* parse(QDataStream& stream, quint32 payloadLenght) = 0;

    static ServerMessageParser *getParser(ServerMessageType messageType) ;


};

//+++++++++++++++++++++++++++++++++++++++++++++++
class AuthChallengeParser : public ServerMessageParser{


public:
    virtual ServerMessage* parse(QDataStream& stream, quint32 payloadLenght) ;

};
//++++++++++++++++++++++++++++++++++++++++++++++++++
class AuthReplyParser : public ServerMessageParser{

public:
    virtual ServerMessage *parse(QDataStream &stream, quint32 payloadLenght);
};

//++++++++++++++++++++++++++++++++++++++++++++++++++
class ServerKeepAliveMessage;
class KeepAliveParser : public ServerMessageParser{

public:
    virtual ServerMessage* parse(QDataStream &stream, quint32 payloadLenght);
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++
class ConfigChangeNotifyParser : public ServerMessageParser{

public:
    virtual ServerMessage* parse(QDataStream &stream, quint32 payloadLenght);
};
//++++++++++++++++++++++
class UserInfoChangeNotifyParser : public ServerMessageParser {

public:
    virtual ServerMessage *parse(QDataStream &stream, quint32 payloadLenght);

};
//+++++++++++++++++++=

class ChatMessageParser : public ServerMessageParser {

    virtual ServerMessage *parse(QDataStream &stream, quint32 payloadLenght);

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
 Offset Type        Field
 0x0    uint8_t[16] GUID (binary)
 0x10   uint32_t    Estimated Size
 0x14   uint8_t[4]  FourCC
 0x18   uint8_t     Channel Index
 0x19   ...         Username (NUL-terminated)
 */
class DownloadIntervalBeginParser : public ServerMessageParser {

    virtual ServerMessage* parse(QDataStream& stream, quint32 payload);

};

//++++++++++++++++++++
/*
 Offset Type        Field
 0x0    uint8_t[16] GUID (binary)
 0x10   uint8_t     Flags
 0x11   ...         Audio Data
 */
class DownloadIntervalWriteParser : public ServerMessageParser {

    //max payload recebido do reaper foi 2076

public:
    virtual ServerMessage* parse(QDataStream& stream, quint32 payloadLenght);

};

}