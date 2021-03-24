#include "edit_temperature_dialog.h"
#include "ui_edit_temperature_dialog.h"
#include <QDebug>

Edit_Temperature_Dialog::Edit_Temperature_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Edit_Temperature_Dialog)
{
    ui->setupUi(this);
    connect(this,SIGNAL(update_parent_values(QString,QString)),parent,SLOT(update_temperature_values(QString,QString)));
}

Edit_Temperature_Dialog::~Edit_Temperature_Dialog()
{
    delete ui;
}

void Edit_Temperature_Dialog::set_Max_Min_Value(float max, float min)
{
    ui->maxTemp->setValue(max);
    ui->minTemp->setValue(min);
}

void Edit_Temperature_Dialog::on_buttonBox_accepted()
{
    float max_temp_changed = ui->maxTemp->value();
    float min_temp_changed = ui->minTemp->value();
    QString M, m;
    QString max_value_parent;
    QString min_value_parent;

    if( max_temp_changed > min_temp_changed ){

        max_value_parent = M.setNum(max_temp_changed);
        min_value_parent = m.setNum(min_temp_changed);
        qDebug() << max_value_parent << " " << min_value_parent << "\n";
        emit update_parent_values(max_value_parent,min_value_parent);
    }
}
