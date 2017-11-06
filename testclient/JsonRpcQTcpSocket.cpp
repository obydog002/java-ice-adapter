#include "JsonRpcQTcpSocket.h"

namespace faf {

JsonRpcQTcpSocket::JsonRpcQTcpSocket(QObject* parent):
  QObject(parent),
  mSocket(new QTcpSocket),
  mOwnSocket(true)
{
  init();
}

JsonRpcQTcpSocket::JsonRpcQTcpSocket(QTcpSocket* socket, QObject* parent):
  QObject(parent),
  mSocket(socket),
  mOwnSocket(false)
{
  init();
}

JsonRpcQTcpSocket::~JsonRpcQTcpSocket()
{
  if (mOwnSocket)
  {
    delete mSocket;
  }
}

QTcpSocket* JsonRpcQTcpSocket::socket() const
{
  return mSocket;
}

void JsonRpcQTcpSocket::init()
{
  QObject::connect(mSocket,
                   &QTcpSocket::readyRead,
                   [this]()
  {
    mBuffer += mSocket->readAll();
    mBuffer = JsonRpcProtocol::processBuffer(this, mBuffer.trimmed());
  });
}

bool JsonRpcQTcpSocket::send(std::string const& msg)
{
  if (mSocket->state() != QAbstractSocket::ConnectedState)
  {
    qCritical() << "not connected for sending msg " << QString::fromStdString(msg);
  }
  auto bytesSent = mSocket->write(msg.c_str(), msg.size());
  return bytesSent == msg.size();
}

bool JsonRpcQTcpSocket::sendJson(Socket* socket, std::string const& message)
{
  if (socket && this != socket)
  {
    qCritical() << "this != socket";
    return false;
  }
  return send(message);
}


} // namespace faf
