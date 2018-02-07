#include "hdatapropertydlg.h"
#include "ui_datapropertydlg.h"

HDataPropertyDlg::HDataPropertyDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataPropertyDlg)
{
    ui->setupUi(this);
}

HDataPropertyDlg::HDataPropertyDlg(HBaseObj* pObj,QWidget *parent)
    :QDialog(parent)
{
   pCurObj = pObj;
}

HDataPropertyDlg::~HDataPropertyDlg()
{
    delete ui;
}

void HDataPropertyDlg::initDataProperDlg()
{

}
