#include "dialog.h"
#include "ui_dialog.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <string>
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>

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
    ui->Date_Value->setFont(formation);
    ui->Time_Value->setFont(formation);
    ui->Max_Temp_Def->setText("30");
    ui->Min_Temp_Def->setText("27");
    updateTemperatureMessage();

    clean_days  = 30;
    clean_hours = 0;
    feed_days   = 0;
    feed_hours  = 8;

    food_alarm = 0;
    clean_alarm = 0;
    temp_alarm = 0;
    hit_median = 1;
    rising = 0;

    arduino_connected = false;

    refreshConnection();
    writeSerial();
}



Dialog::~Dialog()
{
    delete ui;
}

void Dialog::refreshConnection()
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

    if(arduino_available && !arduino_connected){

        qDebug() << "Found the arduino port...\n";
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::ReadWrite);
        qDebug() << arduino->isOpen();
        arduino->setBaudRate(QSerialPort::Baud19200);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        arduino_connected = true;
        QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT( decode()) );
        QObject::connect(this, SIGNAL(messsageReceived(QByteArray)), this, SLOT(readSerial(QByteArray)) );
        qDebug() << arduino->isOpen();
        serialData.clear();
    }else if(arduino_connected)
    {
        QMessageBox::information(this,"Serial Port Already Connected", "Serial port to Arduino Uno is already open.");
    }else{
        qDebug() << "Couldn't find the correct port for arduino.\n";
        QMessageBox::information(this,"Serial Port Error", "Couldn't open serial port to Arduino Uno.");
    }
}

void Dialog::decode()
{
    QByteArray bytes = arduino->readAll();
    for (char c : bytes)
    {
        if (c == '$')
        {
            emit messsageReceived(m_buffer);
            m_buffer.clear();
        }
        else
        {
             m_buffer.append(c);
        }
    }
}

void Dialog::readSerial(QByteArray message)
{
    qDebug() << "Serialport works\n";

    serialData = message;
    serialBuffer = QString::fromStdString( serialData.toStdString() );
    qDebug() << serialBuffer;

    QStringList bufferSplit = serialBuffer.split("|");

        QString temperature = bufferSplit[0].split(":")[1];
        qDebug() << temperature;
        updateLCD(temperature);
        //get Date
        QString date = bufferSplit[1].split(":")[1];
        QString week = bufferSplit[2].split(":")[1];
        QString hour = bufferSplit[3].split(":")[1].split("\r\n")[0];

        QString day = QStringLiteral("%1").arg( date.split("/")[0].toInt() , 2, 10, QLatin1Char('0'));
        QString mon = QStringLiteral("%1").arg( date.split("/")[1].toInt() , 2, 10, QLatin1Char('0'));
        QString year = QStringLiteral("%1").arg( date.split("/")[2].toInt() , 4, 10, QLatin1Char('0'));

        QString h = QStringLiteral("%1").arg( hour.split("/")[0].toInt() , 2, 10, QLatin1Char('0'));
        QString m = QStringLiteral("%1").arg( hour.split("/")[1].toInt() , 2, 10, QLatin1Char('0'));

        update_date_values(day,mon,year);
        update_time_values(h,m);
        ui->WeekDay->setText(week);

        qDebug() << date;
        qDebug() << week;
        qDebug() << hour;
        serialData.clear();
        update_Clean_Message();
        update_Feed_Message();

        writeSerial();
}

void Dialog::writeSerial()
{
    QString m;
    m = "TA:";
    m += QString::number(temp_alarm);
    m += "|FA:";
    m += QString::number(food_alarm);
    m += "|CA:";
    m += QString::number(clean_alarm);
    m += "|HM:";
    m += QString::number(hit_median);
    m += "$";
    qDebug() << m;
    QByteArray message = m.toUtf8();

    arduino->write(message);
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
        temp_alarm = 0;
    }else if(disp_msg <= ui->Max_Temp_Def->text().toFloat()
             && disp_msg >= ui->Min_Temp_Def->text().toFloat()){
        ui->TemperatureInfo->setStyleSheet("color : black");
        ui->TemperatureInfo->setText("The temperature is within the standards.");
        temp_alarm = 0;
        if(rising){
            if(disp_msg >= (ui->Min_Temp_Def->text().toFloat()+ui->Max_Temp_Def->text().toFloat())/2)
                hit_median = 1;
        }else{
            if(disp_msg <= (ui->Min_Temp_Def->text().toFloat()+ui->Max_Temp_Def->text().toFloat())/2)
                hit_median = 1;
        }
    }else{
        ui->TemperatureInfo->setStyleSheet("color : red");
        ui->TemperatureInfo->setText("The temperature is NOT within the standards.");
        hit_median = 0;
        if(disp_msg <= ui->Min_Temp_Def->text().toFloat()){
            temp_alarm = 1;
            rising = 1;
        }
        else{
            temp_alarm = 2;
            rising = 0;
        }
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

void Dialog::update_date_values(QString day, QString mon, QString yea)
{
    QString value = day + "/" + mon + "/" + yea;
    ui->Date_Value->setText(value);
}

void Dialog::update_time_values(QString hour, QString min)
{
    QString value = hour + ":" + min;
    ui->Time_Value->setText(value);
}

void Dialog::on_pushButton_clicked( )
{
    refreshConnection();
}


void Dialog::on_Set_Clean_Period_clicked()
{
    if(ui->Days_Clean_Value->value()>0 || (ui->Days_Clean_Value->value()==0 && ui->Hours_Clean_Value->value()>0))
    {
        clean_days  = ui->Days_Clean_Value->value();
        clean_hours = ui->Hours_Clean_Value->value();
        //atualizar o horário da próxima vez que deve ser limpo
        refresh_next_clean_time();
        update_Clean_Message();
    }
}

void Dialog::on_Set_Feed_Period_clicked()
{
    if(ui->Days_Feed_Value->value()>0 || (ui->Days_Feed_Value->value()==0 && ui->Hours_Feed_Value->value()>0))
    {
        feed_days  = ui->Days_Feed_Value->value();
        feed_hours = ui->Hours_Feed_Value->value();
        //atualizar o horário da proxima vez que deve alimentar
        refresh_next_feed_time();
        update_Feed_Message();
    }
}

void Dialog::on_pushButton_Clean_clicked()
{
    //atualizar a data e hora da última vez em que o aquário foi limpo
    ui->Last_Clean_Value->setText(ui->Date_Value->text() + " " + ui->Time_Value->text());
    //atualizar a próxima data e hora que o aquário deve ser limpo
    refresh_next_clean_time();
    //atualizar mensagem do aquário para limpo
    update_Clean_Message();
    //ui->Clean_Message->setText("Cleaned Aquarium");
}

void Dialog::update_Clean_Message()
{
    QString d_value = ui->Date_Value->text();
    int d_day = d_value.split(" ")[0].split("/")[0].toInt();
    int d_mon = d_value.split(" ")[0].split("/")[1].toInt();
    int d_yea = d_value.split(" ")[0].split("/")[2].toInt();
    QString t_value = ui->Time_Value->text();
    int t_h = t_value.split(" ")[0].split(":")[0].toInt();
    int t_m = t_value.split(" ")[0].split(":")[1].toInt();
    QString l_value = ui->Next_Clean_Value->text();
    int l_day = l_value.split(" ")[0].split("/")[0].toInt();
    int l_mon = l_value.split(" ")[0].split("/")[1].toInt();
    int l_yea = l_value.split(" ")[0].split("/")[2].toInt();
    int l_h = l_value.split(" ")[1].split(":")[0].toInt();
    int l_m = l_value.split(" ")[1].split(":")[1].toInt();
    if((d_yea > l_yea) || (d_yea == l_yea && d_mon > l_mon) || (d_yea == l_yea && d_mon == l_mon && d_day > l_day) || (d_yea == l_yea && d_mon == l_mon && d_day == l_day && t_h > l_h) || (d_yea == l_yea && d_mon == l_mon && d_day == l_day && t_h == l_h && t_m >= l_m))
    {
        //atualizar mensagem de alimentar peixes
        ui->Clean_Message->setStyleSheet("color : red");
        ui->Clean_Message->setText("It's time to clean the aquarium");
        clean_alarm = 1;
    } else {
        ui->Clean_Message->setStyleSheet("color : black");
        ui->Clean_Message->setText("Cleaned Aquarium");
        clean_alarm = 0;
    }
}

void Dialog::on_pushButton_Feed_clicked()
{
    //atualizar a data e hora da última vez em que os peixes foram alimentados
    ui->Last_Feed_Value->setText(ui->Date_Value->text() + " " + ui->Time_Value->text());
    //atualizar a próxima data e hora em que os peixes devem ser alimentados
    refresh_next_feed_time();
    //atualizar mensagem dos peixes alimentados
    update_Feed_Message();
    //ui->Feed_Message->setText("Fish Fed");
}

void Dialog::update_Feed_Message()
{
    QString d_value = ui->Date_Value->text();
    int d_day = d_value.split(" ")[0].split("/")[0].toInt();
    int d_mon = d_value.split(" ")[0].split("/")[1].toInt();
    int d_yea = d_value.split(" ")[0].split("/")[2].toInt();
    QString t_value = ui->Time_Value->text();
    int t_h = t_value.split(" ")[0].split(":")[0].toInt();
    int t_m = t_value.split(" ")[0].split(":")[1].toInt();
    QString l_value = ui->Next_Feed_Value->text();
    int l_day = l_value.split(" ")[0].split("/")[0].toInt();
    int l_mon = l_value.split(" ")[0].split("/")[1].toInt();
    int l_yea = l_value.split(" ")[0].split("/")[2].toInt();
    int l_h = l_value.split(" ")[1].split(":")[0].toInt();
    int l_m = l_value.split(" ")[1].split(":")[1].toInt();
    if((d_yea > l_yea) || (d_yea == l_yea && d_mon > l_mon) || (d_yea == l_yea && d_mon == l_mon && d_day > l_day) || (d_yea == l_yea && d_mon == l_mon && d_day == l_day && t_h > l_h) || (d_yea == l_yea && d_mon == l_mon && d_day == l_day && t_h == l_h && t_m >= l_m))
    {
        //atualizar mensagem de alimentar peixes
        ui->Feed_Message->setStyleSheet("color : red");
        ui->Feed_Message->setText("It's time to feed the fish");
        food_alarm = 1;
    } else {
        ui->Feed_Message->setStyleSheet("color : black");
        ui->Feed_Message->setText("Fish Fed");
        food_alarm = 0;
    }
}

void Dialog::refresh_next_feed_time()
{
    QString l_value = ui->Last_Feed_Value->text();
    int l_day = l_value.split(" ")[0].split("/")[0].toInt();
    int l_mon = l_value.split(" ")[0].split("/")[1].toInt();
    int l_yea = l_value.split(" ")[0].split("/")[2].toInt();
    int l_h = l_value.split(" ")[1].split(":")[0].toInt();
    int l_m = l_value.split(" ")[1].split(":")[1].toInt();
    QDateTime n_date = QDateTime(QDate(l_yea,l_mon,l_day),QTime(l_h,l_m));
    n_date = n_date.addDays(feed_days).addSecs(60*60*(feed_hours));
    QString day = QStringLiteral("%1").arg( n_date.date().day() , 2, 10, QLatin1Char('0'));
    QString mon = QStringLiteral("%1").arg( n_date.date().month() , 2, 10, QLatin1Char('0'));
    QString yea = QStringLiteral("%1").arg( n_date.date().year() , 4, 10, QLatin1Char('0'));
    QString h = QStringLiteral("%1").arg( n_date.time().hour() , 2, 10, QLatin1Char('0'));
    QString m = QStringLiteral("%1").arg( n_date.time().minute() , 2, 10, QLatin1Char('0'));
    QString date = day+"/"+mon+"/"+yea+" "+h+":"+m;
    ui->Next_Feed_Value->setText(date);
}

void Dialog::refresh_next_clean_time()
{
    QString l_value = ui->Last_Clean_Value->text();
    int l_day = l_value.split(" ")[0].split("/")[0].toInt();
    int l_mon = l_value.split(" ")[0].split("/")[1].toInt();
    int l_yea = l_value.split(" ")[0].split("/")[2].toInt();
    int l_h = l_value.split(" ")[1].split(":")[0].toInt();
    int l_m = l_value.split(" ")[1].split(":")[1].toInt();
    QDateTime n_date = QDateTime(QDate(l_yea,l_mon,l_day),QTime(l_h,l_m));
    n_date = n_date.addDays(clean_days).addSecs(60*60*(clean_hours));
    QString day = QStringLiteral("%1").arg( n_date.date().day() , 2, 10, QLatin1Char('0'));
    QString mon = QStringLiteral("%1").arg( n_date.date().month() , 2, 10, QLatin1Char('0'));
    QString yea = QStringLiteral("%1").arg( n_date.date().year() , 4, 10, QLatin1Char('0'));
    QString h = QStringLiteral("%1").arg( n_date.time().hour() , 2, 10, QLatin1Char('0'));
    QString m = QStringLiteral("%1").arg( n_date.time().minute() , 2, 10, QLatin1Char('0'));
    QString date = day+"/"+mon+"/"+yea+" "+h+":"+m;
    ui->Next_Clean_Value->setText(date);
}
