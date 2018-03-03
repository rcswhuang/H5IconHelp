#include "hdigitalpage.h"
#include "ui_digitalpage.h"
#include "hbaseobj.h"
#include "hiconobj.h"
#include "hiconrectobj.h"
#include "hiconcomplexobj.h"
#include "hdynamicobj.h"
#include "hstation.h"
#include "hiconhelper.h"
extern ATTRINFO DgtAttrInfo[];

HDigitalPage::HDigitalPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::digitalPage)
{
    ui->setupUi(this);
}

HDigitalPage::HDigitalPage(HBaseObj* pObj,QWidget *parent)
    :QDialog(parent)
{
   pCurObj = (HIconComplexObj*)pObj;
   wStation = pCurObj->getDynamicObj()->getDBStation();
   wPoint = pCurObj->getDynamicObj()->getDBPoint();
   wAttrib = pCurObj->getDynamicObj()->getDBAttr();
   connect(ui->stComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onStationCurrentIndexChanged(int)));
   connect(ui->jgComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onIntervalCurrentIndexChanged(int)));
}

HDigitalPage::~HDigitalPage()
{
    delete ui;
}

void HDigitalPage::initDataProper()
{
    //对厂站信息初始化
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
        ui->stLineEdit->setText(HStationHelper::Instance()->getStation(wStation)->getName());
    }
}

void HDigitalPage::initBaseProper()
{
    //布局
    ui->ptX->setText(QString("%1").arg(pCurObj->getOX(),0,'g',2));
    ui->ptY->setText(QString("%1").arg(pCurObj->getOY(),0,'g',2));
    ui->ptWidth->setText(QString("%1").arg(pCurObj->getRectWidth(),0,'g',2));
    ui->ptHeight->setText(QString("%1").arg(pCurObj->getRectHeight(),0,'g',2));
    ui->ptRotate->setText(QString("%1").arg(pCurObj->getRotateAngle()));

    //图符 可以用hiconhelper操作
    QIcon openIcon,closeIcon,wrongOpenIcon,wrongCloseIcon;
    if(!pCurObj->iconTemplate() || !pCurObj->iconTemplate()->getSymbol())
        return;
    HIconSymbol* pSymbol = pCurObj->iconTemplate()->getSymbol();
    pSymbol->setCurrentPattern(0);//分
    QString strType,strUuid;
    strType = pCurObj->getCatalogName();
    strUuid = pCurObj->getUuid();
    QSizeF sizeF = QSizeF(ui->open->width()-2,ui->open->height()-2);
    openIcon = QIcon(HIconHelper::Instance()->iconPixmap(strType,strUuid,sizeF,0));
    ui->open->setIcon(openIcon);
    sizeF = QSizeF(ui->close->width()-2,ui->close->height()-2);
    closeIcon = QIcon(HIconHelper::Instance()->iconPixmap(strType,strUuid,sizeF,1));
    ui->close->setIcon(closeIcon);
    sizeF = QSizeF(ui->wrongOpen->width()-2,ui->wrongOpen->height()-2);
    wrongOpenIcon = QIcon(HIconHelper::Instance()->iconPixmap(strType,strUuid,sizeF,2));
    ui->wrongOpen->setIcon(wrongOpenIcon);
    sizeF = QSizeF(ui->wrongClose->width()-2,ui->wrongClose->height()-2);
    wrongCloseIcon = QIcon(HIconHelper::Instance()->iconPixmap(strType,strUuid,sizeF,3));
    ui->wrongClose->setIcon(wrongCloseIcon);

}

void HDigitalPage::onStationCurrentIndexChanged(int index)
{
    //加间隔
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
    if(wPoint == (ushort)-1)
        ui->jgComboBox->setCurrentIndex(0);
    else
    {
        pGroup = pStation->getGroupByDigital(wPoint);
        int index1 = ui->jgComboBox->findData(pGroup->getGroupID());
        ui->jgComboBox->setCurrentIndex(index1);
        ui->jgLineEdit->setText(pGroup->getName());
    }
}

void HDigitalPage::onIntervalCurrentIndexChanged(int index)
{
    ui->ptListWidget->clear();
    int stationID = ui->stComboBox->currentData().toInt();
    int groupID = ui->jgComboBox->itemData(index).toInt();
    HStation *pStation = HStationHelper::Instance()->getStation(stationID);

    int wTotalDigital = pStation->getTotalDigital();
    HDigital* pDigital = pStation->getDigital();
    int curIndex = 0;
    for(int i = 0; i < wTotalDigital;i++,pDigital++)
    {
        if(!pDigital && pDigital->getGroupID() == groupID)
        {
            QListWidgetItem* item = new QListWidgetItem(QString(pDigital->getName()),ui->ptListWidget);
            item->setData(Qt::UserRole,pDigital->getNo());
            ui->ptListWidget->addItem(item);
            if(wPoint == pDigital->getNo())
                curIndex = i;
        }
    }

    ui->ptListWidget->setCurrentRow(curIndex);
    ui->ptLineEdit->setText(ui->ptListWidget->currentItem()->text());
    setDigitalAttr(pDigital);
}

void HDigitalPage::setDigitalAttr(HDigital* pDigital)
{
    if(NULL == pDigital)
        return;
    ui->attrComboBox->clear();
    ushort wAttrib = 0;
    for(int i = 0;DgtAttrInfo[i].szName != 0;i++)
    {
        if(ATTR_DGT_4_STATE_VALUE == DgtAttrInfo[i].wAttrib)
        {
            if((ushort)-1 != pDigital->getDoubleDNo())
            {
                ui->attrComboBox->addItem(QString(DgtAttrInfo[i].szName),DgtAttrInfo[i].wAttrib);
                if(pCurObj->getDynamicObj()->getDBAttr() != DgtAttrInfo[i].wAttrib)
                {
                    wAttrib = DgtAttrInfo[i].wAttrib;
                    pCurObj->getDynamicObj()->setDBAttr(DgtAttrInfo[i].wAttrib);
                }
            }
        }
        else if(ATTR_DGT_VALUE == DgtAttrInfo[i].wAttrib)
        {
            if((ushort)-1 == pDigital->getDoubleDNo())
            {
                ui->attrComboBox->addItem(QString(DgtAttrInfo[i].szName),DgtAttrInfo[i].wAttrib);
                if(pCurObj->getDynamicObj()->getDBAttr() != DgtAttrInfo[i].wAttrib)
                {
                    wAttrib = DgtAttrInfo[i].wAttrib;
                    pCurObj->getDynamicObj()->setDBAttr(DgtAttrInfo[i].wAttrib);
                }
            }
        }
        else
        {
            ui->attrComboBox->addItem(QString(DgtAttrInfo[i].szName),DgtAttrInfo[i].wAttrib);
            if(pCurObj->getDynamicObj()->getDBAttr() != DgtAttrInfo[i].wAttrib)
            {
                wAttrib = DgtAttrInfo[i].wAttrib;
                pCurObj->getDynamicObj()->setDBAttr(DgtAttrInfo[i].wAttrib);
            }
        }
    }
    ui->attrComboBox->setCurrentIndex(ui->attrComboBox->findData(wAttrib));
}

void HDigitalPage::onOk()
{
    //主要就是保存点的数据
    wStation = ui->stComboBox->currentData().toInt();
    wPoint = ui->ptListWidget->currentItem()->data(Qt::UserRole).toInt();
    pCurObj->getDynamicObj()->setDBStation(wStation);
    pCurObj->getDynamicObj()->setDBPoint(wPoint);
    QDialog::accept();
}

void HDigitalPage::onCancel()
{
    QDialog::reject();
}
