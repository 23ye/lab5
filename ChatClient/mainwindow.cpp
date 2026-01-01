#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->LoginPage);
    m_chatClient=new ChatClient(this);

    connect(m_chatClient, &ChatClient::connected, this, &MainWindow::connectedToServer);
    connect(m_chatClient,&ChatClient::messageReceived,this,&MainWindow::messageReceived);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_LoginButton_clicked()
{
    m_chatClient->connectToServer(QHostAddress(ui->Server->text()),1967);

}


void MainWindow::on_SendButton_clicked()
{
    if(!ui->sayLineEdit->text().isEmpty())
        m_chatClient->sendMessage(ui->sayLineEdit->text());

}


void MainWindow::on_ExitButton_clicked()
{

    ui->stackedWidget->setCurrentWidget(ui->LoginPage);
}

void MainWindow::connectedToServer()
{
    ui->stackedWidget->setCurrentWidget(ui->ChatPage);
    ui->roomTextEdit->append("新用户已连接");
    m_chatClient->sendMessage(ui->UserName->text(),"login");

}

void MainWindow::messageReceived(const QString &text)
{
    ui->roomTextEdit->append(text);

}

