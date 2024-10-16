#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    nextBlockSize = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    socket->connectToHost("127.0.0.1", 2323);
}

void MainWindow::sendToServer(QString str)
{
    dataByteArray.clear();
    QDataStream out(&dataByteArray, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << uint16_t(0) << QTime::currentTime()<< str;
    out.device()->seek(0);
    out << uint16_t(dataByteArray.size() - sizeof(uint16_t));
    socket->write(dataByteArray);
    ui->lineEdit->clear();
}

void MainWindow::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_2);
    if (in.status() == QDataStream::Ok)
    {
        qDebug() << "read...";
        for (;;)
        {
            if (nextBlockSize == 0)
            {
                if (socket->bytesAvailable() < 2)
                {
                    break;
                }
                in >> nextBlockSize;
            }
            if (socket->bytesAvailable() < nextBlockSize)
            {
                break;
            }

            QString str;
            QTime time;
            in >> time >> str;
            nextBlockSize = 0;
            ui->textBrowser->append(time.toString() + " " + str);
        }
    }
    else
    {
        ui->textBrowser->append("read error");
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    sendToServer(ui->lineEdit->text());
}


void MainWindow::on_lineEdit_returnPressed()
{
     sendToServer(ui->lineEdit->text());
}

