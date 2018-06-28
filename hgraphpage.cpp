#if defined(_MSC_VER) &&(_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif
#include "hgraphpage.h"
#include "ui_graphpage.h"
#include "hstation.h"
#include "hgraph.h"
#include <QColorDialog>
#include <QFileDialog>
HGraphPage::HGraphPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::graphPage)
{
    ui->setupUi(this);
}


HGraphPage::HGraphPage(HGraph* graph,QWidget *parent):
    QDialog(parent),ui(new Ui::graphPage)
{
    ui->setupUi(this);
    pGraph = graph;
    initGraphInfo();
    setWindowTitle("设置画面属性");
}


HGraphPage::~HGraphPage()
{
    delete ui;
}

/*
void HGraphPage::copyGraphInfo(GRAPHINFO* pGraphInfo)
{
    if(!pGraphInfo)
        return;
    if(graphInfo)
    {
        delete graphInfo;
        graphInfo = new GRAPHINFO;
    }
    graphInfo->strGraphName = pGraphInfo->strGraphName;
    graphInfo->nGraphID  = pGraphInfo->nGraphID;;//画面ID
    graphInfo->nGraphWidth = pGraphInfo->nGraphWidth; //长度，
    graphInfo->nGraphHeight = pGraphInfo->nGraphHeight; //高度
    graphInfo->nRefreshInterval = pGraphInfo->nRefreshInterval; //刷新间隔
    graphInfo->strFillColor = pGraphInfo->strFillColor;//背景填充颜色
    graphInfo->strFillPicture = pGraphInfo->strFillPicture;
    graphInfo->bStartGraph = pGraphInfo->bStartGraph;//是不是启动画面
    graphInfo->btType = pGraphInfo->btType;//画面类型
    graphInfo->wStationNo = pGraphInfo->wStationNo;//厂站ID
    graphInfo->nReserver1 = pGraphInfo->nReserver1;
    graphInfo->nReserver2 = pGraphInfo->nReserver2;
    graphInfo->nReserver3 = pGraphInfo->nReserver3;
    graphInfo->nReserver4 = pGraphInfo->nReserver4;
}
*/

void HGraphPage::initGraphInfo()
{
    connect(ui->okBtn,SIGNAL(clicked(bool)),this,SLOT(onOk()));
    connect(ui->cancleBtn,SIGNAL(clicked(bool)),this,SLOT(onCancle()));
    connect(ui->bgClrBtn,SIGNAL(clicked(bool)),this,SLOT(onBgColorBtn_clicked()));
    connect(ui->picPathBtn,SIGNAL(clicked(bool)),this,SLOT(onPicPathBtn_clicked()));
    ui->startCheckBox->setChecked(false);
    ui->wSpinBox->setRange(300,9999);
    ui->hSpinBox->setRange(300,9999);
    ui->wSpinBox->setValue(1000);
    ui->hSpinBox->setValue(1000);
    ui->typeComboBox->addItem("",0);
    ui->typeComboBox->addItem(QStringLiteral("接线图"),1);
    ui->typeComboBox->addItem(QStringLiteral("其他图"),2);
    ui->typeComboBox->setCurrentIndex(0);
    QString strColor = QString("background-color:")+ QString("#000000"); //文字颜色
    ui->bgClrBtn->setStyleSheet(strColor);
    ui->refreshSpinBox->setRange(500,15000);
    ui->refreshSpinBox->setValue(3000);
    if(pGraph)
    {
        pGraph->setModify(true);
        ui->graphNameLineEdit->setText(pGraph->getGraphName());
        ui->startCheckBox->setChecked(pGraph->getStartFlag());
        ui->wSpinBox->setValue(pGraph->getGraphWidth());
        ui->hSpinBox->setValue(pGraph->getGraphHeight());
        ui->picPathLineEdit->setText(pGraph->getFillPicture());
        ui->refreshSpinBox->setValue(pGraph->getRefreshInterval());
        strFillColor = QString("background-color:")+ pGraph->getFillColor(); //文字颜色
        ui->bgClrBtn->setStyleSheet(strFillColor);
        ui->stLineEdit->setText(pGraph->getStationName());
        //设置类型
        ui->typeComboBox->setCurrentIndex(ui->typeComboBox->findData(pGraph->getGraphType()));
    }
}

void HGraphPage::onBgColorBtn_clicked()
{
    QColor curColor = QColor(Qt::black);
    if(pGraph)
    {
        strFillColor = pGraph->getFillColor();
        curColor = QColor(strFillColor);
    }
    const QColor color = QColorDialog::getColor(curColor, this, QStringLiteral("选择颜色"));
    if(!color.isValid()) return;
    strFillColor = color.name(QColor::HexRgb);
    ui->bgClrBtn->setStyleSheet(QString("background-color:")+strFillColor);
}

void HGraphPage::onPicPathBtn_clicked()
{
    static QString path = "../";

    QString fileName = QFileDialog::getOpenFileName(this,QStringLiteral("打开图片"),path,"Images(*.png *.jpg *.bmp)",NULL,    QFileDialog::DontUseNativeDialog);
    if(!fileName.isEmpty() || !fileName.isNull())
    {
        ui->picPathBtn->setText(fileName);
    }
}

void HGraphPage::onOk()
{
    if(!pGraph)
        return;
    pGraph->setGraphName(ui->graphNameLineEdit->text());
    pGraph->setStartFlag(ui->startCheckBox->isChecked());
    pGraph->setGraphWidth(ui->wSpinBox->value());
    pGraph->setGraphHeight(ui->hSpinBox->value());
    pGraph->setFillColor(strFillColor);
    pGraph->setFillPicture(ui->picPathLineEdit->text());
    pGraph->setRefreshInterval(ui->refreshSpinBox->value());
    pGraph->setGraphType(ui->typeComboBox->currentData().toUInt());

    QDialog::accept();
}

void HGraphPage::onCancle()
{
    QDialog::reject();
}
