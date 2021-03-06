#include "Server.h"
#include "User.h"
#include <QDebug>
#include <QDataStream>

using namespace Ninjam;

//QMap<QString, std::shared_ptr<Server>> Server::servers;

Server::Server(QString host, int port, int maxChannels)
    :port(port), host(host),
      maxUsers(0), bpm(120), bpi(16),
      activeServer(true),
      streamUrl(""),
      topic(""),
      containBot(false),
      maxChannels(maxChannels)
{

    //qDebug() << "criou server " << host <<":" << port;
}

Server::Server(QString host, int port, int maxChannels, int maxUsers)
    :port(port), host(host),
      maxUsers(maxUsers), bpm(120), bpi(16),
      activeServer(true),
      streamUrl(""),
      topic(""),
      containBot(false),
      maxChannels(maxChannels)
{

    //qDebug() << "criou server " << host <<":" << port;
}

Server::~Server(){
    //qDeleteAll(users);
}

//QString Server::getUniqueName(QString host, int port) {
//    return host + ":" + QString::number(port);
//}

bool Server::containsUser(QString userFullName) const{
    return users.contains(userFullName);
}


bool Server::containsUser(const User &user) const{
    return containsUser(user.getFullName());
}


//Server* Server::getServer(QString host, int port) {
//    QString key = getUniqueName(host, port);
//    if (!servers.contains(key)) {
//        servers.insert(key, std::shared_ptr<Server>(new Server(host, port)));
//    }
//    return servers[key].get();
//}

void Server::addUser(User user) {
    if (!users.contains(user.getFullName())) {
        users.insert(user.getFullName(), new User(user.getFullName()));
        if (user.isBot()) {
            containBot = true;
        }
    }
}

User *Server::getUser(QString userFullName) const{
    if(users.contains(userFullName)){
        return users[userFullName];
    }
    return nullptr;
}

QList<User*> Server::getUsers() const{
    return users.values();
}

bool Server::containsBotOnly() const{
    if (users.size() == 1 && containBot) {
        return true;
    }
    return false;
}

QString Server::getUniqueName() const
{
     return host + ":" + QString::number(port);
}

bool Server::setBpm(short bpm) {
    if (bpm == this->bpm) {
        return false;
    }

    if (bpm >= Server::MIN_BPM && bpm <= Server::MAX_BPM) {
        this->bpm = bpm;
        return true;
    }
    return false;
}

bool Server::setBpi(short bpi) {
    if (bpi == this->bpi) {
        return false;
    }

    if (bpi >= Server::MIN_BPI && bpi <= Server::MAX_BPI) {
        this->bpi = bpi;
        return true;
    }
    return false;
}

void Server::refreshUserList(QSet<QString> onlineUsers) {
    QList<QString> toRemove;

    foreach (QString onlineUserName , onlineUsers) {
        addUser(User(onlineUserName));
    }

    QList<User*> currentUsers= users.values();
    foreach (const User* user , currentUsers) {
        if (!onlineUsers.contains(user->getFullName())) {
            toRemove.append(user->getFullName());
        }
    }

    foreach (QString fullName , toRemove) {
        users.remove(fullName);
    }

}

QDataStream & Ninjam::operator<<(QDataStream &out, const Server &server){
    out << "NinjamServer{" << "port="  <<  server.getPort()  <<  ", host="
        <<  server.getHostName() <<  ", stream="  <<  server.getStreamUrl()
        <<" maxUsers="  <<  server.getMaxUsers() <<  ", bpm="
        <<  server.getBpm()  <<  ", bpi="  <<  server.getBpi()
        <<  ", isActive="  <<  server.isActive() <<  "}\n";
    for (const User* user : server.getUsers()) {
        out << "\t" << user->getName()  << "\n";
    }
    return out;
}
