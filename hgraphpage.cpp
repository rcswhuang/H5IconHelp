#include "hgraphpage.h"
#include "ui_graphpage.h"

HGraphPage::HGraphPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HGraphPage)
{
    ui->setupUi(this);
}

HGraphPage::~HGraphPage()
{
    delete ui;
}
