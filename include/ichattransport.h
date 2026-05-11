#ifndef ICHATTRANSPORT_H
#define ICHATTRANSPORT_H

#include <QByteArray>

// Pure abstract — no Q_OBJECT — so gmock can subclass it freely.
// The transport is the only thing that knows about real sockets;
// everything else in the client talks to this interface.
class IChatTransport
{
public:
    virtual ~IChatTransport() = default;
    virtual void send(const QByteArray &bytes) = 0;
};

#endif
