#include "hgraph.h"
#include <QFile>
#include <QTextStream>
#include <QDomDocument>
#include <QDomElement>
#include <QTextCodec>

HGraph::HGraph(const QString& name)
    :sName(name)
{
    nGraphID = 0;
    nGraphWidth = 1000;
    nGraphHeight = 1000;
    nRefreshInterval = 3000;
    strFillColor = "#000000";
    bModify = false;
    bStart = false;
    btType = 0;
    nGraphID = (int)-1;
}

HGraph::~HGraph()
{

}


void HGraph::setGraphName(const QString& name)
{
    sName = name;
}

QString HGraph::getGraphName()
{
    return sName;
}

void HGraph::setGraphID(int id)
{
    nGraphID = id;
}

//获取ID
int HGraph::getGraphID()
{
    return nGraphID;
}

void HGraph::setGraphWidth(int width)
{
    nGraphWidth = width;
}

int HGraph::getGraphWidth()
{
    return nGraphWidth;
}

void HGraph::setGraphHeight(int height)
{
    nGraphHeight = height;
}

int HGraph::getGraphHeight()
{
    return nGraphHeight;
}

void HGraph::setFillColor(const QString& clr)
{
    strFillColor = clr;
}

QString HGraph::getFillColor()
{
    return strFillColor;
}

void HGraph::setFillPicture(const QString& strPic)
{
    strFillPicture = strPic;
}

QString HGraph::getFillPicture()
{
    return strFillPicture;
}

void HGraph::setRefreshInterval(int val)
{
    nRefreshInterval = val;
}

int HGraph::getRefreshInterval()
{
    return nRefreshInterval;
}

void HGraph::setStartFlag(bool start)
{
    bStart = start;
}

bool HGraph::getStartFlag()
{
    return bStart;
}

void HGraph::setGraphType(ushort type)
{
    btType = type;
}

ushort HGraph::getGraphType()
{
    return btType;
}

QString HGraph::getStationName()
{
    return "";
}

//画面文件需要同时保存到二进制文件和xml文件?
void HGraph::readDataFile(const QString&)
{

}

 void HGraph::writeDateFile(const QString&)
{

}

//可以通过datafile保存到二进制文件 也可以作为复制拷贝功能来用
void HGraph::readData(int v,QDataStream *d)
{
    QString s;
    *d>>s;
    sName = s;
    int n;
    *d>>n;
    nGraphID = n;
    *d>>n;
    nGraphWidth=n;
    *d>>n;
    nGraphHeight=n;
    *d>>n;
    nRefreshInterval = n;
    *d>>s;
    strFillColor = s;

    *d>>n;
    int nCount = n;
    for(int i = 0; i < nCount;i++)
    {
        *d>>s;
        QUuid uuid(s);
        HIconTemplate* iconTemplate = new HIconTemplate(uuid);
        if(iconTemplate)
            iconTemplate->readData(v,d);
    }

    *d>>n;
    nCount = n;
    for(int i = 0; i < nCount;i++)
    {
        quint8 type;
        *d>>type;
        QString uuid = QString();
        if(type == enumComplex)
        {
           *d>>s;
            uuid = s;
        }
        HBaseObj* pObj = newObj(type,uuid);
        pObj->readData(d);
    }
}

void HGraph::writeData(int n,QDataStream *d)
{
    *d<<sName;
    *d<<nGraphID;
    *d<<nGraphWidth;
    *d<<nGraphHeight;
    *d<<nRefreshInterval;
    *d<<strFillColor;

    *d<<pIconTemplateList.count();
    for(int i = 0; i < pIconTemplateList.count();i++)
    {
        HIconTemplate* iconTemplate = (HIconTemplate*)pIconTemplateList[i];
        QString struuid;
        if(!iconTemplate)
        {
            continue;
        }
        struuid = iconTemplate->getUuid().toString();
        *d<<struuid;
        iconTemplate->writeData(n,d);
    }

    *d<<pObjList.count();
    for(int i = 0; i < pObjList.count();i++)
    {
        HBaseObj* pObj = (HBaseObj*)pObjList[i];
        quint8 type = pObj->getShapeType();
        *d<<type;
        if(type == enumComplex)
        {
            QString struuid = ((HIconObj*)pObj)->iconTemplate()->getUuid().toString();
            *d<<struuid;
        }
        pObj->writeData(d);
    }
}

 //先打开文件
bool HGraph::readXmlFile(const QString& fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return false;
    QTextStream dsm(&file);
    QDomDocument doc;
    dsm.setCodec("GB2312");
    QString errorString;
    int errorLine;
    int errorColumn;
    if(!doc.setContent(&file,false,&errorString,&errorLine,&errorColumn))
    {
        file.close();
        return false;
    }
    QDomElement root = doc.documentElement();
    if(root.isNull())
        return false;
    readXml(&root);
    file.close();
    return true;
}

//读具体的信息
void HGraph::readXml(QDomElement *d)
{
  //分为本身属性，模板部分，动态数据部分
    if(!d)
        return;
    sName = d->attribute("GraphName");
    nGraphID = d->attribute("GraphID").toInt();
    nGraphWidth = d->attribute("GraphWidth").toInt();
    nGraphHeight = d->attribute("GraphHeight").toInt();
    nRefreshInterval = d->attribute("RefreshInterval").toInt();
    strFillColor = d->attribute("FillColor");

    //读取模板的信息
    //clear();
    QDomElement templateDom = d->namedItem("IconTemplates").toElement();
    QDomNode n = templateDom.firstChild();
    for(int i = 0;!n.isNull();n=n.nextSibling(),i++)
    {
        QDomElement e = n.toElement();
        QUuid uuid = QUuid(e.attribute("Uuid"));
        HIconTemplate *pIconTemplate = new HIconTemplate(uuid);//需要定义一个不带uuid的参数
        if(!pIconTemplate) continue;
        pIconTemplate->readXml(&e);
        pIconTemplateList.append(pIconTemplate);
    }


    QDomElement elementsDom = d->namedItem("Elements").toElement();
    QDomNode n1 = elementsDom.firstChild();
    for(int i = 0;!n1.isNull();n1=n1.nextSibling(),i++)
    {
        QDomElement e = n1.toElement();
        QString strTagName = e.tagName();
        QString strUuid = e.attribute("Uuid");
        HBaseObj* pObj = newObj(strTagName,strUuid);
        if(!pObj) continue;
        pObj->readXml(&e);
        pObjList.append(pObj);
    }
}

bool HGraph::writeXmlFile(const QString& fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
        return false;
    QTextStream dsm(&file);
    QDomDocument doc;
    QTextCodec* c = QTextCodec::codecForLocale();
    QString strLocal = QString("version=\"1.0\" encoding=\"GB2312\"");
    QDomProcessingInstruction instruct = doc.createProcessingInstruction("xml",strLocal);
    doc.appendChild(instruct);
    QDomElement root = doc.createElement("Graph");
    if(root.isNull())
        return false;
    doc.appendChild(root);
    writeXml(&root);
    dsm.setCodec("GB2312");
    doc.save(dsm,1);
    file.close();
    return true;
}

void HGraph::writeXml(QDomElement *d)
{
    if(!d)
        return;
    d->setAttribute("GraphName",sName);
    d->setAttribute("GraphID",nGraphID);
    d->setAttribute("GraphWidth",nGraphWidth);
    d->setAttribute("GraphHeight",nGraphHeight);
    d->setAttribute("RefreshInterval",nRefreshInterval);
    d->setAttribute("FillColor",strFillColor);

    //再创建模板的xml结构
    QDomElement templateDom = d->ownerDocument().createElement("IconTemplates");
    d->appendChild(templateDom);
    for(int i = 0; i < pIconTemplateList.count();i++)
    {
        HIconTemplate* iconTemplate = (HIconTemplate*)pIconTemplateList[i];
        if(!iconTemplate) continue;
        QDomElement tempDom = templateDom.ownerDocument().createElement("IconTemplate");
        templateDom.appendChild(tempDom);
        iconTemplate->writeXml(&tempDom);
    }

    //创建复杂和简单图元
    QDomElement elementDom = d->ownerDocument().createElement("Elements");
    d->appendChild(elementDom);
    for(int i = 0; i < pObjList.count();i++)
    {
        HBaseObj* pObj = (HBaseObj*)pObjList[i];
        if(!pObj) continue;
        QDomElement tempDom = elementDom.ownerDocument().createElement(pObj->TagName());
        elementDom.appendChild(tempDom);
        pObj->writeXml(&tempDom);
    }
}


void HGraph::Draw(QPainter* p)
{

}

void HGraph::clear()
{
    QList<HIconTemplate*>::iterator tempIt = pIconTemplateList.begin();
    for(; tempIt < pIconTemplateList.end();tempIt++)
    {
        HIconTemplate* icontemp = (HIconTemplate*)pIconTemplateList.takeFirst();
        if(!icontemp) continue;
        delete icontemp;
        icontemp = NULL;
    }
    pIconTemplateList.clear();

    //所有图元信息
    while(!pObjList.isEmpty())
    {
        HBaseObj* pObj = (HBaseObj*)pObjList.takeFirst();
        if(!pObj) continue;
        delete pObj;
        pObj = NULL;
    }
    pObjList.clear();
}

void HGraph::copyTo(HGraph* graph)
{
    if(!graph) return;
    graph->clear();
    graph->sName = sName;
    graph->nGraphID = nGraphID;
    graph->nGraphWidth = nGraphWidth;
    graph->nGraphHeight = nGraphHeight;
    graph->nRefreshInterval = nRefreshInterval;
    graph->strFillColor = strFillColor;
    graph->bModify = bModify;


    //所有模板信息
    for(int i = 0; i < pIconTemplateList.count();i++)
    {
        HIconTemplate* icontemp = (HIconTemplate*)pIconTemplateList[i];
        if(!icontemp) continue;
        HIconTemplate* newtemp = new HIconTemplate(icontemp->getUuid());
        icontemp->copyTo(newtemp);
        graph->pIconTemplateList.append(newtemp);//拷贝HIconTemplate到新HGraph里面
    }

    //所有图元信息
    for (int i = 0; i < pObjList.count();i++)
    {
        HBaseObj* pObj = (HBaseObj*)pObjList[i];
        if(pObj->getShapeType() == enumLine)
        {
            HLine* pLineObj = new HLine;
            pObj->clone(pLineObj);
            graph->addObj(pLineObj);
        }
        else if(pObj->getShapeType() == enumRectangle)
        {
            HRectangle* pRectObj = new HRectangle;
            pObj->clone(pRectObj);
            graph->addObj(pRectObj);
        }
        else if(pObj->getShapeType() == enumEllipse)
        {
            HEllipse* pEllipseObj = new HEllipse;
            pObj->clone(pEllipseObj);
            graph->addObj(pEllipseObj);
        }
        else if(pObj->getShapeType() == enumCircle)
        {
            HCircle* pCircleObj = new HCircle;
            pObj->clone(pCircleObj);
            graph->addObj(pCircleObj);
        }
        else if(pObj->getShapeType() == enumPolyline)
        {
            HPolyline* pPolylineObj = new HPolyline;
            pObj->clone(pPolylineObj);
            graph->addObj(pPolylineObj);
        }
        else if(pObj->getShapeType() == enumPolygon)
        {
            HPolygon* pPolygonObj = new HPolygon;
            pObj->clone(pPolygonObj);
            graph->addObj(pPolygonObj);
        }
        else if(pObj->getShapeType() == enumPie)
        {
            HPie* pPieObj = new HPie;
            pObj->clone(pPieObj);
            graph->addObj(pPieObj);
        }
        else if(pObj->getShapeType() == enumArc)
        {
            HArc* pArcObj = new HArc;
            pObj->clone(pArcObj);
            graph->addObj(pArcObj);
        }
        else if(pObj->getShapeType() == enumText)
        {
            HText* pTextObj = new HText;
            pObj->clone(pTextObj);
            graph->addObj(pTextObj);
        }
        else if(pObj->getShapeType() == enumGroup)
        {
            HIconTemplate* icontemp = graph->findIconTemplate(QUuid(pObj1->getUuid()));
            HGroupObj* pGroupObj = new HGroupObj(icontemp->getSymbol());
            pObj->clone(pGroupObj);
            sp->addObj(pGroupObj);
        }
        else if(pObj->getShapeType() == enumComplex)
        {
            HIconObj* pObj1 = (HIconObj*)pObj;
            //要从graph里面去寻找
            HIconTemplate* icontemp = graph->findIconTemplate(QUuid(pObj1->getUuid()));
            HIconObj* pComplexObj = new HIconObj(icontemp);
            pObj1->clone(pComplexObj);
            graph->addObj(pComplexObj);
        }
    }
}


HBaseObj* HGraph::newObj(QString tagName,const QString &strUuid)
{
    quint8 drawShape = enumNo;
    if(tagName == "Line")
        drawShape = enumLine;
    else if(tagName == "Rectangle")
        drawShape = enumRectangle;
    else if(tagName == "Ellipse")
        drawShape = enumEllipse;
    else if(tagName == "Circle")
        drawShape = enumCircle;
    else if(tagName == "Polyline")
        drawShape = enumPolyline;
    else if(tagName == "Arc")
        drawShape = enumArc;
    else if(tagName == "Pie")
        drawShape = enumPie;
    else if(tagName == "Text")
        drawShape = enumText;
    else if(tagName == "Polygon")
        drawShape = enumPolygon;
    else if(tagName ==  "Group")
        drawShape = enumGroup;
    else if(tagName == "WfPointObj")
        drawShape = enumComplex;
    return newObj(drawShape,strUuid);
}

HBaseObj* HGraph::newObj(int nObjType,const QString &strUuid)
{
    HBaseObj* pObj = NULL;
    if(nObjType == enumLine)
    {
        pObj = new HLine();
    }
    else if(nObjType == enumRectangle)
    {
        pObj = new HRectangle();
    }
    else if(nObjType == enumEllipse)
    {
        pObj = new HEllipse();
    }
    else if(nObjType == enumCircle)
    {
        pObj = new HCircle();
    }
    else if(nObjType == enumPolygon)
    {
        pObj = new HPolygon();
    }
    else if(nObjType == enumPolyline)
    {
        pObj = new HPolyline();
    }
    else if(nObjType == enumArc)
    {
        pObj = new HArc();
    }
    else if(nObjType == enumPie)
    {
        pObj = new HPie();
    }
    else if(nObjType == enumText)
    {
        pObj = new HText();
    }
    else if(nObjType == enumGroup)
    {
        HIconTemplate* icontemp = findIconTemplate(QUuid(strUuid));
        pObj = new HGroupObj(icontemp->getSymbol());
    }
    else if(nObjType == enumComplex)
    {
        //先获取icontemplate
        HIconTemplate* icontemp = findIconTemplate(QUuid(strUuid));
        pObj = new HIconObj(icontemp);
    }
    pObj->setShapeType((DRAWSHAPE)nObjType);
    if(pObj)
    {
        int objID = getObjID();
        pObj->setObjID(objID);
    }
    return pObj;
}

//获取ObjID
int HGraph::getObjID()
{
    int nObjID = 1;
    while(findObjID(nObjID))
        nObjID++;
    return nObjID;
}

bool HGraph::findObjID(int nObjID)
{
    if(pObjList.count() == 0) return false;
    for(int i = 0;i < pObjList.count();i++)
    {
        HBaseObj* pObj = (HBaseObj*)pObjList[i];
        if(pObj && pObj->getObjID() == nObjID)
            return true;
    }
    return false;
}

void HGraph::addObj(HBaseObj* pObj)
{
    if(!pObj) return;
    QString strObjName = QString("%1_%2_%3").arg(pObj->TagName()).arg(pObj->getShapeType()).arg(pObj->getObjID());
    if(pObj->getShapeType() == enumComplex)
    {
        HIconObj* pIconObj = (HIconObj*)pObj;
        if(pIconObj->iconTemplate() && pIconObj->getIconSymbol())
            strObjName = pIconObj->iconTemplate()->getCatalogName() + " " + pIconObj->getIconSymbol()->getSymolName();
    }
    pObj->setObjName(strObjName);
    pObjList.append(pObj);
}

void HGraph::removeObj(HBaseObj* pObj)
{
    if(!pObj) return;
    pObjList.removeOne(pObj);
}

void HGraph::delObj(HBaseObj *pObj)
{
    if(!pObj) return;
    if(pObjList.removeOne(pObj))
    {
        delete pObj;
        pObj = NULL;
    }
}

QList<HBaseObj*> HGraph::getObjList()
{
    return pObjList;
}

//模板部分
int HGraph::iconTemplateNum()
{
    return pIconTemplateList.count();
}

HIconTemplate* HGraph::IconTemplateAt(int index)
{
    return pIconTemplateList[index];
}

HIconTemplate* HGraph::findIconTemplate(const QUuid& uid)
{
    QString struuid = uid.toString();
    for(int i = 0; i < pIconTemplateList.count();i++)
    {
        HIconTemplate* iconTemplate = (HIconTemplate*)pIconTemplateList[i];
        if(iconTemplate->getUuid() == struuid)
            return iconTemplate;
    }
    return NULL;
}

void HGraph::addIconTemplate(HIconTemplate* temp)
{
    if(!temp)
        return;
    pIconTemplateList.append(temp);
}

void HGraph::removeIconTemplate(HIconTemplate* temp)
{
    HIconTemplate *iconTemplate = NULL;
    iconTemplate = findIconTemplate(temp->getUuid());
    if(iconTemplate)
    {
        pIconTemplateList.removeOne(iconTemplate);
        delete iconTemplate;
        iconTemplate = NULL;
    }
}

void HGraph::clearIconTemplate()
{
    while(!pIconTemplateList.isEmpty())
        delete pIconTemplateList.takeFirst();
    pIconTemplateList.clear();
}

void HGraph::resetIconTemplate()
{
    clearIconTemplate();
}

void HGraph::setModify(bool modify)
{
    bModify = modify;
}

bool HGraph::getModify()
{
    return bModify;
}
