#include "login.h"
#include "clientcontroller.h"
#include "chatclient.h"
#include "tcpchattransport.h"
#include "theme.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Theme::loadFromSettings();

    ClientController controller;
    TcpChatTransport transport;
    ChatClient chatClient(&transport);
    transport.setClient(&chatClient);

    transport.connectToServer("127.0.0.1", 54321);

    Login w(nullptr, &controller,  &chatClient);
    w.show();

    return a.exec();
}
