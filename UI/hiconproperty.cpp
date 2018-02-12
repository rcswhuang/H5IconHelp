#include "hiconproperty.h"
#include "ui_iconproperty.h"

HIconProperty::HIconProperty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HIconProperty)
{
    ui->setupUi(this);
}

HIconProperty::~HIconProperty()
{
    delete ui;
}
