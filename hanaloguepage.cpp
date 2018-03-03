#include "hanaloguepage.h"
#include "ui_analoguepage.h"
#include "hbaseobj.h"
#include "hiconobj.h"
#include "hiconrectobj.h"
#include "hiconcomplexobj.h"
#include "hdynamicobj.h"
#include "hstation.h"
#include "hiconhelper.h"
#include <QColorDialog>
#include <QFontDialog>
extern ATTRINFO AnaAttrInfo[];
HAnaloguePage::HAnaloguePage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::analoguePage)
{
    ui->setupUi(this);
    wStation = (ushort)-1;
    wPoint = (ushort)-1;
    wAttrib = 0;
}

HAnaloguePage::HAnaloguePage(HBaseObj* pObj,QWidget *parent):
    QDialog(parent),ui(new Ui::analoguePage)
{
    ui->setupUi(this);
    wStation = (ushort)-1;
    wPoint = (ushort)-1;
    wAttrib = 0;
    pCurObj = (HIconComplexObj*)pObj;

    connect(ui->okBtn,SIGNAL(clicked()),this,SLOT(onOk()));
    connect(ui->cancelBtn,SIGNAL(clicked()),this,SLOT(onCancel()));
}

HAnaloguePage::~HAnaloguePage()
{
    delete ui;
}

void HAnaloguePage::setIconComplexObj(HBaseObj* pObj)
{
    pCurObj = (HIconComplexObj*)pObj;
}

void HAnaloguePage::initDataProperty()
{
    connect(ui->stComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onStationCurrentIndexChanged(int)));
    connect(ui->jgComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onIntervalCurrentIndexChanged(int)));
    connect(ui->ptListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(onListWidgetItemDoubleClicked(QListWidgetItem*)));

    wStation = pCurObj->getDynamicObj()->getDBStation();
    wPoint = pCurObj->getDynamicObj()->getDBPoint();
    wAttrib = pCurObj->getDynamicObj()->getDBAttr();

    ui->stComboBox->clear();
    QList<HStation*> stationList = HStationHelper::Instance()->getStationList();
    for(int i = 0; i < stationList.count();i++)
    {
        HStation* station = (HStation*)stationList[i];
        ui->stComboBox->addItem(QString(station->getName()),QVariant(station->getNo()));
    }
    if(wStation == (ushort)-1)
        ui->stComboBox->setCurrentIndex(-1);//默认设置为0
    else
    {
        int index = ui->stComboBox->findData(wStation);
        ui->stComboBox->setCurrentIndex(index);
    }
}

void HAnaloguePage::onStationCurrentIndexChanged(int index)
{
    if(-1 == index) return;
    int stationID = ui->stComboBox->itemData(index).toInt();
    HStation *pStation = HStationHelper::Instance()->getStation(stationID);
    int wTotalGroup = pStation->getTotalGroup();
    HGroup* pGroup = pStation->getGroup();
    ui->jgComboBox->clear();
    for(int i = 0; i < wTotalGroup;i++,pGroup++)
    {
        ui->jgComboBox->addItem(QString(pGroup->getName()),QVariant(pGroup->getNo()));
    }
    if(wPoint != (ushort)-1)
    {
        pGroup = pStation->getGroupByDigital(wPoint);
        int index1 = ui->jgComboBox->findData(pGroup->getGroupID());
        ui->jgComboBox->setCurrentIndex(index1);
        ui->jgLineEdit->setText(pGroup->getName());
    }
}

void HAnaloguePage::onIntervalCurrentIndexChanged(int index)
{
    ui->ptListWidget->clear();
    int stationID = ui->stComboBox->currentData().toInt();
    int groupID = ui->jgComboBox->itemData(index).toInt();
    HStation *pStation = HStationHelper::Instance()->getStation(stationID);

    int wTotalAnalogue = pStation->getTotalAnalogue();
    HAnalogue* pAna = pStation->getAnalogue();
    int curIndex = 0;
    for(int i = 0; i < wTotalAnalogue;i++,pAna++)
    {
        if(!pAna && pAna->getGroupID() == groupID)
        {
            QListWidgetItem* item = new QListWidgetItem(QString(pAna->getName()),ui->ptListWidget);
            item->setData(Qt::UserRole,pAna->getNo());
            ui->ptListWidget->addItem(item);
            if(wPoint == pAna->getNo())
                curIndex = i;
        }
    }

    ui->ptListWidget->setCurrentRow(curIndex);
    ui->ptLineEdit->setText(ui->ptListWidget->currentItem()->text());
    //ui->ptNameLineEdit->setText(ui->ptListWidget->currentItem()->text());
    setAnaAttrib(pAna);
}

void HAnaloguePage::onListWidgetItemDoubleClicked(QListWidgetItem* item)
{
    if(!item) return;
    wPoint = item->data(Qt::UserRole).toUInt();
    QString strPointName = item->text();
    ui->ptNameLineEdit->setText(strPointName);
    ui->ptLineEdit->setText(strPointName);
}

void HAnaloguePage::setAnaAttrib(HAnalogue* pAna)
{
    if(NULL == pAna)
        return;
    ui->attrComboBox->clear();
    for(int i = 0;AnaAttrInfo[i].szName != 0;i++)
    {
        ui->attrComboBox->addItem(QString(AnaAttrInfo[i].szName),AnaAttrInfo[i].wAttrib);
    }
    ui->attrComboBox->setCurrentIndex(ui->attrComboBox->findData(wAttrib));
}

//基本属性
void HAnaloguePage::initBaseProperty()
{

    connect(ui->fontBtn,SIGNAL(clicked(bool checked)),this,SLOT(fontBtn_clicked()));
    connect(ui->textClrBtn,SIGNAL(clicked(bool checked)),this,SLOT(textClrBtn_clicked()));
    connect(ui->bgClrBtn,SIGNAL(clicked(bool checked)),this,SLOT(bgClrBtn_clicked()));

    /*-------------------------基本设置-------------------------------*/
    ui->jgLineEdit->setText("");
    ui->ptLineEdit->setText("");
    ui->jgLineEdit->setReadOnly(true);
    ui->ptLineEdit->setReadOnly(true);

    /*-------------------------文字设置-------------------------------*/
    //内容
    ui->ptNameLineEdit->setText("");
    //字体点击之后再显示
    //对齐
    ui->hComboBox->addItem(QStringLiteral("左对齐"),Qt::AlignLeft);
    ui->hComboBox->addItem(QStringLiteral("居中对齐"),Qt::AlignHCenter);
    ui->hComboBox->addItem(QStringLiteral("右对齐"),Qt::AlignRight);
    ui->vComboBox->addItem(QStringLiteral("顶端对齐"),Qt::AlignTop);
    ui->vComboBox->addItem(QStringLiteral("垂直居中"),Qt::AlignVCenter);
    ui->vComboBox->addItem(QStringLiteral("底部对齐"),Qt::AlignBottom);

    /*************************颜色设置************************************/
    QString strColor = "#000000"; //文字颜色
    strTextColor = QString("background-color:")+ strColor; //文字颜色
    ui->textClrBtn->setStyleSheet(strTextColor);
    strBgColor = QString("background-color:")+ strColor; //背景颜色
    ui->bgClrBtn->setStyleSheet(strBgColor);
    ui->transparentCheckBox->setCheckState(Qt::Unchecked);

    /**********************************格式设置******************************/
    ui->pointFormatComboBox->addItem(".",TEXT_NO_POINT);
    ui->pointFormatComboBox->addItem(".x",TEXT_ONE_POINT);
    ui->pointFormatComboBox->addItem(".xx",TEXT_TWO_POINT);
    ui->pointFormatComboBox->addItem(".xxx",TEXT_THREE_POINT);
    ui->prefixLineEdit->setText("");
    ui->suffixLineEdit->setText("");


    HTextObj* pTextObj = pCurObj->getIconSymbol()->getFirstTextObj();//获取文字信息
    if(pCurObj && pTextObj)
    {
        ui->ptNameLineEdit->setText(pCurObj->getObjName());
        ui->hComboBox->setCurrentIndex(ui->hComboBox->findData(pTextObj->getHorizontalAlign()));
        ui->vComboBox->setCurrentIndex(ui->vComboBox->findData(pTextObj->getVerticalAlign()));


        strColor = pTextObj->getTextColorName();
        strTextColor = QString("background-color:")+ strColor;//文字颜色
        ui->textClrBtn->setStyleSheet(strTextColor);
        strColor = pCurObj->getFillColorName();
        strBgColor = QString("background-color:")+ strColor; //填充颜色
        ui->bgClrBtn->setStyleSheet(strBgColor);

        //无颜色填充即透明
        if(pCurObj->getFillStyle() == 0)
            ui->transparentCheckBox->setCheckState(Qt::Checked);
        else
            ui->transparentCheckBox->setCheckState(Qt::Unchecked);

        //格式
        ui->pointFormatComboBox->setCurrentIndex(ui->pointFormatComboBox->findData(pTextObj->getTextFormat()));
        ui->prefixLineEdit->setText(pTextObj->getTextPrefix());
        ui->suffixLineEdit->setText(pTextObj->getTextSuffix());


        wStation = pCurObj->getDynamicObj()->getDBStation();
        wPoint = pCurObj->getDynamicObj()->getDBPoint();
        HStation* pStation = HStationHelper::Instance()->getStation(wStation);
        if(pStation)
        {
            HGroup* pGroup = pStation->getGroupByDigital(wPoint);
            if(pGroup)
            {
                ui->jgLineEdit->setText(pGroup->getName());
                HAnalogue* pAna = pStation->getAnalogue(wPoint);
                if(pAna)
                {
                    ui->ptNameLineEdit->setText(pAna->getName());
                    ui->ptLineEdit->setText(pAna->getName());
                }
            }
        }
    }
}

void HAnaloguePage::fontBtn_clicked()
{
    bool ok;
    HTextObj* pTextObj = pCurObj->getIconSymbol()->getFirstTextObj();//获取文字信息
    font.setFamily(pTextObj->getTextFontName());
    font.setPointSize(pTextObj->getPointSize());
    font.setWeight(pTextObj->getWeigth());
    font.setItalic(pTextObj->getItalic());
    QFont newFont = QFontDialog::getFont(&ok,font,this,QStringLiteral("选择字体"));
    if(ok)
    {
        font = newFont;
    }
}

void HAnaloguePage::textClrBtn_clicked()
{
    QColor curColor = QColor(Qt::black);
    HTextObj* pTextObj = pCurObj->getIconSymbol()->getFirstTextObj();//获取文字信息
    if(pCurObj)
    {
        strTextColor = pTextObj->getTextColorName();
        curColor = QColor(strTextColor);
    }
    const QColor color = QColorDialog::getColor(curColor, this, QStringLiteral("选择颜色"));
    if(!color.isValid()) return;
    strTextColor = color.name(QColor::HexRgb);
    ui->textClrBtn->setStyleSheet(QString("background-color:")+strTextColor);
}

void HAnaloguePage::bgClrBtn_clicked()
{
    QColor curColor = QColor(Qt::black);
    if(pCurObj)
    {
        strBgColor = pCurObj->getFillColorName();
        curColor = QColor(strBgColor);
    }
    const QColor color = QColorDialog::getColor(curColor, this, QStringLiteral("选择颜色"));
    if(!color.isValid()) return;
    strBgColor = color.name(QColor::HexRgb);
    ui->bgClrBtn->setStyleSheet(QString("background-color:")+strBgColor);
}

void HAnaloguePage::onOk()
{
    pCurObj->getDynamicObj()->setDBStation(ui->stComboBox->currentData().toUInt());
    pCurObj->getDynamicObj()->setDBPoint(ui->ptListWidget->currentItem()->data(Qt::UserRole).toUInt());
    pCurObj->getDynamicObj()->setDBAttr(ui->attrComboBox->currentData().toUInt());

    HTextObj* pTextObj = pCurObj->getIconSymbol()->getFirstTextObj();//获取文字信息
    pTextObj->setTextContent(ui->ptNameLineEdit->text());
    pTextObj->setHorizontalAlign(ui->hComboBox->currentData().toInt());
    pTextObj->setVerticalAlign(ui->vComboBox->currentData().toInt());
    pTextObj->setTextFontName(font.family());
    pTextObj->setPointSize(font.pointSize());
    pTextObj->setWeight(font.weight());
    pTextObj->setItalic(font.italic());

    pTextObj->setTextColorName(strTextColor);
    pCurObj->setFillColorName(strBgColor);//填充颜色
    if(ui->transparentCheckBox->isChecked())
        pCurObj->setFillWay(0);
    else
        pCurObj->setFillWay(1);

    pTextObj->setTextFormat(ui->pointFormatComboBox->currentData().toUInt());
    pTextObj->setTextPrefix(ui->prefixLineEdit->text());
    pTextObj->setTextSuffix(ui->suffixLineEdit->text());

    QDialog::accept();
}

void HAnaloguePage::onCancel()
{
    QDialog::reject();
}
