#include <QtNetwork>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class Server: public QObject
{
  Q_OBJECT
  public:
    Server(QObject * parent = 0);
    ~Server();
    public slots:
      void acceptConnection();
    void startRead();
  private:
    QTcpServer server;
    QTcpSocket* client;
    quint16 blockSize;
    QMap<int, QString> data;
    QString answer;
    QString request;
    int key;
    QString value;
};
