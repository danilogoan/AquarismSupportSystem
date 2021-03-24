#include "dialog.h"
#include "ui_dialog.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <string>
#include <QDebug>
#include <QMessageBox>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->tempLcdNumber->display("-------");

    arduino = new QSerialPort(this);

    QFont formation("Arial",18);
    ui->Max_Temp_Def->setFont(formation);
    ui->Min_Temp_Def->setFont(formation);
    ui->Max_Temp_Def->setText("30");
    ui->Min_Temp_Def->setText("27");
    updateTemperatureMessage();

    refreshConnection(this);
}



Dialog::~Dialog()
{
    delete ui;
}

void Dialog::refreshConnection(QDialog *Di)
{
    serialBuffer = "";

    qDebug() << "Number of ports: " << QSerialPortInfo::availablePorts().length() << "\n";
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        qDebug() << "Description: "    << serialPortInfo.description()          << "\n";
        qDebug() << "Has vendor ID? "  << serialPortInfo.hasVendorIdentifier()  << "\n";
        qDebug() << "Vendor ID: "      << serialPortInfo.vendorIdentifier()     << "\n";
        qDebug() << "Has product ID? " << serialPortInfo.hasProductIdentifier() << "\n";
        qDebug() << "Product ID: "     << serialPortInfo.productIdentifier()    << "\n";
    }

    //identify if the arduino uno port is on:
    bool arduino_available = false;
    QString arduino_port_name;

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){
            if((serialPortInfo.vendorIdentifier()==arduino_uno_vendor_id) && (serialPortInfo.productIdentifier()==arduino_uno_product_id)){
                arduino_available = true;
                arduino_port_name = serialPortInfo.portName();
            }
        }
    }

    if(arduino_available){
        qDebug() << "Found the arduino port...\n";
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::ReadOnly);
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        QObject::connect(arduino, SIGNAL(readyRead()), Di, SLOT(readSerial()));
    }else{
        qDebug() << "Couldn't find the correct port for arduino.\n";
        QMessageBox::information(this,"Serial Port Error", "Couldn't open serial port to Arduino Uno.");
    }
}

void Dialog::readSerial()
{
    qDebug() << "Serialport works\n";
    serialData = arduino->readAll();
    serialBuffer = QString::fromStdString( serialData.toStdString() );
    qDebug() << serialBuffer;

    QStringList bufferSplit = serialBuffer.split(": ");
    if(bufferSplit[0] == "Temperatura"){
        QString value = bufferSplit[1].split("\r\n")[0];
        qDebug() << value;
        updateLCD(value);
    }else{
        updateLCD("-------");
    }
}



void Dialog::updateLCD(const QString sensor_reading){
    ui->tempLcdNumber->display(sensor_reading);
    updateTemperatureMessage();
}

void Dialog::updateTemperatureMessage()
{
    float disp_msg = ui->tempLcdNumber->value();
    QString minus = "-------";
    float trying = minus.toFloat();

    if(disp_msg == trying){
        ui->TemperatureInfo->setStyleSheet("color : red");
        ui->TemperatureInfo->setText("No temperature info.");
    }else if(disp_msg <= ui->Max_Temp_Def->text().toFloat()
             && disp_msg >= ui->Min_Temp_Def->text().toFloat()){
        ui->TemperatureInfo->setStyleSheet("color : black");
        ui->TemperatureInfo->setText("The temperature is within the standards.");
    }else{
        ui->TemperatureInfo->setStyleSheet("color : red");
        ui->TemperatureInfo->setText("The temperature is NOT within the standards.");
    }
}

void Dialog::on_EditTemperature_clicked()
{
    Edit_Temperature_Dialog *subdialog = new Edit_Temperature_Dialog(this);
    subdialog->setFixedSize(344,126);
    subdialog->setWindowTitle("Edit Maximum and Minimum Temperatures");
    qDebug() << ui->Max_Temp_Def->text() << "\n";
    qDebug() << ui->Min_Temp_Def->text() << "\n";
    QString max_temp_s = ui->Max_Temp_Def->text();
    QString min_temp_s = ui->Min_Temp_Def->text();

    float maxtemp = max_temp_s.toFloat();
    float mintemp = min_temp_s.toFloat();

    subdialog->set_Max_Min_Value(maxtemp,mintemp);
    subdialog->show();
    subdialog->set_Max_Min_Value(maxtemp,mintemp);
    qDebug() << max_temp_s << " " << min_temp_s << "\n";

}

void Dialog::update_temperature_values(QString max, QString min)
{

    ui->Max_Temp_Def->setText(max);
    ui->Min_Temp_Def->setText(min);

    qDebug() << ui->Max_Temp_Def->text() << " " << ui->Min_Temp_Def->text() << "\n";

    updateTemperatureMessage();
}

void Dialog::on_pushButton_clicked(QDialog *Di)
{
    refreshConnection(Di);
}
