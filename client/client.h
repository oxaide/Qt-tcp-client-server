#include <QtNetwork>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QAbstractSocket>

class Client: public QObject
{
  Q_OBJECT
  public:
    Client(QObject* parent = 0);
    ~Client();
    void start(QString address, quint16 port);

  public slots:
    void startTransfer();
    void readData();

  private slots:
    void displayErrorSlot(QAbstractSocket::SocketError);

  private:
    QTcpSocket client;
    quint16 blockSize;
    QString ip;
    int port;
    QString request;
    int key;
    QString value;
    QString answer;
};
