#include "login.h"
#include "clientcontroller.h"
#include "chatclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ClientController controller;
    ChatClient chatClient;

    auto connectionTask = chatClient.connectToServer("127.0.0.1", 54321);

    Login w(nullptr, &controller,  &chatClient);
    w.show();

    return a.exec();
}
