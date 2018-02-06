#include "hdatapropertydlg.h"
#include "ui_hdatapropertydlg.h"

HDataPropertyDlg::HDataPropertyDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HDataPropertyDlg)
{
    ui->setupUi(this);
}

HDataPropertyDlg::~HDataPropertyDlg()
{
    delete ui;
}
