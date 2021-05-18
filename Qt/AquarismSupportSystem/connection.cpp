#include "connection.h"
#include "ui_connection.h"
#include <QSerialPortInfo>

Connection::Connection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Connection)
{
    ui->setupUi(this);
    FillComboBox();

    connect(this,SIGNAL(update_parent_values(quint16,quint16)),parent,SLOT(update_ids(quint16,quint16)));
}

Connection::~Connection()
{
    delete ui;
}

void Connection::FillComboBox()
{
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        ui->comboBox->addItem(serialPortInfo.description());
    }
}

void Connection::on_buttonBox_accepted()
{
    emit update_parent_values(vendorid,productid);
}

void Connection::on_comboBox_currentTextChanged(const QString &arg1)
{
    description = arg1;
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        if (serialPortInfo.description() == description){
            vendorid = serialPortInfo.vendorIdentifier();
            productid = serialPortInfo.productIdentifier();

            ui->description->setText(description);
            ui->vendorid->setText(QString::number(vendorid));
            ui->productid->setText(QString::number(productid));
        }
    }
}
