#include "serverworker.h"
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>

ServerWorker::ServerWorker(QObject *parent): QObject{parent}
{
    m_serverSocket=new QTcpSocket(this);
    connect(m_serverSocket,&QTcpSocket::readyRead,this,&ServerWorker::onReadyRead);

}

bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor)
{
    return m_serverSocket->setSocketDescriptor(socketDescriptor);
}

void ServerWorker::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_12); //

    for(;;){
        socketStream.startTransaction();
        socketStream >> jsonData; //

        if(socketStream.commitTransaction()){
            // 解析 JSON
            QJsonDocument doc = QJsonDocument::fromJson(jsonData);
            if (!doc.isNull() && doc.isObject()) {
                QJsonObject obj = doc.object();
                QString type = obj["type"].toString();
                QString text = obj["text"].toString();

                // 格式化输出到服务器日志
                emit logMessage(QString("[%1] %2").arg(type, text));
            } else {
                emit logMessage("收到非 JSON 格式数据: " + QString::fromUtf8(jsonData));
            }
            sendMessage("我收到消息了"); //
        } else {
            break;
        }
    }
}

void ServerWorker::sendMessage(const QString &text, const QString &type)
{
    if(m_serverSocket->state()!=QAbstractSocket::ConnectedState){
        return;
    }
    if(!text.isEmpty()){
        QDataStream serverStream(m_serverSocket);
        serverStream.setVersion(QDataStream::Qt_5_12);

        QJsonObject message;
        message["type"]=type;
        message["text"]=text;

        serverStream<< QJsonDocument(message).toJson();
    }
}
