#include <QApplication>
#include <iostream>
#include <string>
#include "server.h"
using namespace std;

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  // Проверяем количество аргументов
  if(argc < 2){
    cout << "Error: Bad Syntax" << endl;
    cout << "Usage: ./server port" << endl;
    exit(0);
  }
  Server server;
  return app.exec();
}

Server::Server(QObject* parent): QObject(parent)
{
  // Добавим немного данных для более удобной проверки работоспособности
  data[1] = "123";
  data[2] = "456";
  data[3] = "789";

  connect(&server, SIGNAL(newConnection()),
      this, SLOT(acceptConnection()));

  // Запускаем сервер
  if(server.listen(QHostAddress::Any, qApp->arguments().at(1).toInt())){
    cout << "Server started" << endl;
  }
  else{
    cout << "Server not started" << endl;
  }
}

Server::~Server()
{
  server.close();
}

void Server::acceptConnection()
{
  client = server.nextPendingConnection();

  connect(client, SIGNAL(readyRead()),
      this, SLOT(startRead()));
}

void Server::startRead()
{
  // Получаем данные
  QDataStream in(client);
  blockSize = 0;
  if (blockSize == 0) {
    if (client->bytesAvailable() < (int)sizeof(quint16))
      return;
    in >> blockSize;
  }
  if (client->bytesAvailable() < blockSize)
    return;

  in >> request;
  in >> key;

  // Проверяем тип запроса и количество аргументов
  if(request.toStdString() == "set"){
    in >> value;
    cout << "Receive from client: " << request.toStdString() << " " << key << " " << value.toStdString() << endl;
    data[key] = value;
    answer = "Successfully set";
  }

  if(request.toStdString() == "get"){
    cout << "Receive from client: " << request.toStdString() << " " << key << " " << endl;
    value = data.value(key, "No value");
    answer = value;
  }

  cout << "Send to client: " << answer.toStdString() << endl;
  cout << endl;
  QMap<int,QString>::iterator it = data.begin();
  for(;it != data.end(); ++it)
  {
    qDebug() << "Key:" << it.key() << "Value:" << it.value();
  }
  cout << endl;

  // Отправляем ответ серверу
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out << (quint16)0;
  out << answer;
  out.device()->seek(0);
  out << (quint16)(block.size() - sizeof(quint16));
  client->write(block);

  // После отправки ответа клиенту закрываем соединение
  client->close();
}
