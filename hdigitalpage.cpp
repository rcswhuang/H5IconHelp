#if defined(_MSC_VER) &&(_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "hdigitalpage.h"
#include "ui_digitalpage.h"
#include "hiconsymbol.h"
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
    :QDialog(parent),ui(new Ui::digitalPage)
{
    ui->setupUi(this);
    pCurObj = (HIconObj*)pObj;
    wStation = pCurObj->getDynamicObj()->getDBStation();
    wPoint = pCurObj->getDynamicObj()->getDBPoint();
    wAttrib = pCurObj->getDynamicObj()->getDBAttr();
    connect(ui->okBtn,SIGNAL(clicked()),this,SLOT(onOk()));
    connect(ui->cancelBtn,SIGNAL(clicked()),this,SLOT(onCancel()));

    initBaseProper();
    initDataProper();

    //遥信属性设置--{uuid}红绿圆
    setWindowTitle(QStringLiteral("遥信属性设置"));
}

HDigitalPage::~HDigitalPage()
{
    delete ui;
}

void HDigitalPage::initDataProper()
{
    connect(ui->stComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onStationCurrentIndexChanged(int)));
    connect(ui->jgComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onIntervalCurrentIndexChanged(int)));
    connect(ui->ptListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(onPointList_clicked()));
    //对厂站信息初始化
    ui->stComboBox->clear();
    QList<HStation*> stationList = HStationHelper::Instance()->getStationList();
    for(int i = 0; i < stationList.count();i++)
    {
        HStation* station = (HStation*)stationList[i];
        ui->stComboBox->addItem(QString(station->getName()),QVariant(station->getNo()));
    }
    if(wStation == (ushort)-1)
    {
        ui->stComboBox->setCurrentIndex(0);//默认设置为0
        ui->stLineEdit->setText(ui->stComboBox->currentText());
    }
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
    ui->ptX->setText(QString("%1").arg(pCurObj->getOX(),0,'f',2));
    ui->ptY->setText(QString("%1").arg(pCurObj->getOY(),0,'f',2));
    ui->ptWidth->setText(QString("%1").arg(pCurObj->getRectWidth(),0,'f',2));
    ui->ptHeight->setText(QString("%1").arg(pCurObj->getRectHeight(),0,'f',2));
    ui->ptRotate->setText(QString("%1°").arg(pCurObj->getRotateAngle()));
    //connect(ui->ptRotate,SIGNAL(),this,SLOT());
    //图符 可以用hiconhelper操作
    //QIcon openIcon,closeIcon,wrongOpenIcon,wrongCloseIcon;
    if(!pCurObj->iconTemplate() || !pCurObj->iconTemplate()->getSymbol())
        return;

    QSizeF defaultSize = pCurObj->iconTemplate()->getDefaultSize();
    QSizeF uiSize = QSizeF(40,40);
    QSizeF sizeF = uiSize;
    if(uiSize.width() > defaultSize.width())
    {
        sizeF.setWidth(defaultSize.width());
        if(uiSize.height() > defaultSize.height())
            sizeF.setHeight(defaultSize.height());
    }
    else
    {
        if(uiSize.height() > defaultSize.height())
            sizeF.setHeight(defaultSize.height());
    }
    HIconSymbol* pSymbol = pCurObj->iconTemplate()->getSymbol();
    pSymbol->setCurrentPattern(0);//分
    QString strType,strUuid;
    strType = pCurObj->getCatalogName();
    strUuid = pCurObj->getUuid();
    //分
    ui->open->setAlignment(Qt::AlignCenter);
    ui->open->setPixmap(HIconHelper::Instance()->iconPixmap(strType,strUuid,sizeF,0));
    //合
    ui->close->setAlignment(Qt::AlignCenter);
    ui->close->setPixmap(HIconHelper::Instance()->iconPixmap(strType,strUuid,sizeF,1));
    //分错
    ui->wrongOpen->setAlignment(Qt::AlignCenter);
    ui->wrongOpen->setPixmap(HIconHelper::Instance()->iconPixmap(strType,strUuid,sizeF,2));
    //合错
    ui->wrongClose->setAlignment(Qt::AlignCenter);
    ui->wrongClose->setPixmap(HIconHelper::Instance()->iconPixmap(strType,strUuid,sizeF,3));
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
    {
        ui->jgComboBox->setCurrentIndex(0);
        ui->jgLineEdit->setText(ui->jgComboBox->currentText());
    }
    else
    {
        pGroup = pStation->getGroupByDigital(wPoint);
        int index1 = ui->jgComboBox->findData(pGroup->getNo());
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
        if(pDigital && pDigital->getGroupID() == groupID)
        {
            QListWidgetItem* item = new QListWidgetItem(QString(pDigital->getName()),ui->ptListWidget);
            item->setData(Qt::UserRole,pDigital->getNo());
            ui->ptListWidget->addItem(item);
            if(wPoint == pDigital->getNo())
                curIndex = i;
        }
    }

    ui->ptListWidget->setCurrentRow(curIndex);
    if(ui->ptListWidget->currentItem())
        ui->ptLineEdit->setText(ui->ptListWidget->currentItem()->text());
    setDigitalAttr(pStation->findDigital(curIndex));
}

void HDigitalPage::setDigitalAttr(HDigital* pDigital)
{
    if(NULL == pDigital)
        return;
    ui->attrComboBox->clear();
    ushort wAttrib = pCurObj->getDynamicObj()->getDBAttr();
    for(int i = 0;DgtAttrInfo[i].szName != 0;i++)
    {
        if(ATTR_DGT_4_STATE_VALUE == DgtAttrInfo[i].wAttrib)
        {
            if((ushort)-1 != pDigital->getDoubleDNo())
            {
                ui->attrComboBox->addItem(QStringLiteral("%1").arg(DgtAttrInfo[i].szName),DgtAttrInfo[i].wAttrib);
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

                ui->attrComboBox->addItem(QStringLiteral("%1").arg(DgtAttrInfo[i].szName),DgtAttrInfo[i].wAttrib);
                if(pCurObj->getDynamicObj()->getDBAttr() != DgtAttrInfo[i].wAttrib)
                {
                    wAttrib = DgtAttrInfo[i].wAttrib;
                    pCurObj->getDynamicObj()->setDBAttr(DgtAttrInfo[i].wAttrib);
                }
            }
        }
        else
        {
            ui->attrComboBox->addItem(QStringLiteral("%1").arg(QString(DgtAttrInfo[i].szName)),DgtAttrInfo[i].wAttrib);
            if(pCurObj->getDynamicObj()->getDBAttr() != DgtAttrInfo[i].wAttrib)
            {
                wAttrib = DgtAttrInfo[i].wAttrib;
                pCurObj->getDynamicObj()->setDBAttr(DgtAttrInfo[i].wAttrib);
            }
        }
    }
    ui->attrComboBox->setCurrentIndex(ui->attrComboBox->findData(wAttrib));
}

void HDigitalPage::onPointList_clicked()
{
    QListWidgetItem* pCurItem = ui->ptListWidget->currentItem();
    if(!pCurItem) return;
    ushort wNo = pCurItem->data(Qt::UserRole).toUInt();
    int stationID = ui->stComboBox->itemData(ui->stComboBox->currentIndex()).toInt();
    HStation *pStation = HStationHelper::Instance()->getStation(stationID);
    if(NULL == pStation) return;
    HDigital *pDigital = pStation->getDigital(wNo);
    if(NULL == pDigital) return;
    setDigitalAttr(pDigital);
    ui->ptLineEdit->setText(pCurItem->text());
}

void HDigitalPage::onOk()
{
    //主要就是保存点的数据
    if((int)-1 != ui->stComboBox->currentIndex())
    {
        wStation = ui->stComboBox->currentData().toInt();
        pCurObj->getDynamicObj()->setDBStation(wStation);
    }
    if(NULL != ui->ptListWidget->currentItem())
    {
        wPoint = ui->ptListWidget->currentItem()->data(Qt::UserRole).toUInt();
        pCurObj->getDynamicObj()->setDBPoint(wPoint);
        pCurObj->getDynamicObj()->setValueType(TYPE_DIGITAL);
    }
    QDialog::accept();
}

void HDigitalPage::onCancel()
{
    QDialog::reject();
}
