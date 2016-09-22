#include <QApplication>
#include <QHostAddress>
#include <QtNetwork>
#include <QtGui>
#include <QHash>
#include <iostream>
#include "client.h"
using namespace std;

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  if(argc < 5){
    cout << "Error: Bad Syntax" << endl;
    cout << "Usage: ./client ip port request key value" << endl;
    exit(0);
  }
  QString ip = QString(argv[1]);
  int port = QString(argv[2]).toInt();

  Client client;
  client.start(ip, port);

  return app.exec();
}

Client::Client(QObject* parent): QObject(parent)
{
  connect(&client, SIGNAL(connected()),
      this, SLOT(startTransfer()));
  connect(&client, SIGNAL(readyRead()),
      this, SLOT(readData()));
  connect(&client, SIGNAL(error(QAbstractSocket::SocketError)),
      this, SLOT(displayErrorSlot(QAbstractSocket::SocketError)));
}

Client::~Client()
{
  client.close();
}

void Client::start(QString address, quint16 port)
{
  QHostAddress addr(address);
  client.connectToHost(addr, port);
  cout << "Connecting to " << address.toStdString() << ":" << port << "..." << endl;
}

void Client::startTransfer()
{
  // Можно задать запрос вручную
  // QString request = "get";
  // int key         = 2;
  // QString value   = "abc";
  cout << "Connected!" << endl;

  request = qApp->arguments().at(3);
  key = qApp->arguments().at(4).toInt();

  // Отправляем данные на сервер
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out << (quint16)0;
  out << request;
  out << key;
  // Если запрос "set" отправляем на сервер значение для ключа
  if(request == "set"){
    // Проверяем указан ли последний аргумент
    if(qApp->arguments().length() == 6){
      value = qApp->arguments().at(5);
      out << value;
    }
    // Если последний аргумент не указан выводим сообщение об ошибке
    else{
      cout << "Error: Bad Syntax" << endl;
      cout << "Usage: ./client ip port request key value" << endl;
      exit(0);
    }
  }
  out.device()->seek(0);
  out << (quint16)(block.size() - sizeof(quint16));
  client.write(block);

  cout << "Send to server: " << request.toStdString() << " " << key << " " << ((value != "") ? value.toStdString() : "") << endl;
}

void Client::readData()
{
  // Получаем ответ от сервера
  QDataStream in(&client);
  blockSize = 0;
  if (blockSize == 0) {
    if (client.bytesAvailable() < (int)sizeof(quint16))
      return;
    in >> blockSize;
  }
  if (client.bytesAvailable() < blockSize)
    return;
  in >> answer;
  cout << "Answer from server: " << answer.toStdString() << endl;

  // Как только ответ от сервера получен выходим из приложения
  qApp->exit();
}

// Обрабатываем возможные ошибки
void Client::displayErrorSlot(QAbstractSocket::SocketError socketError)
{
  switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
      qApp->exit();
      break;
    case QAbstractSocket::HostNotFoundError:
      cout << "Error: Host not found" << endl;
      qApp->exit();
      break;
    case QAbstractSocket::ConnectionRefusedError:
      cout << "Error: Connection refused" << endl;
      qApp->exit();
      break;
    default:
      cout << "Error: " << client.errorString().toStdString() << endl;
      qApp->exit();
      break;
  }
}
