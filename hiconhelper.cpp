#include "hiconhelper.h"
#include "hicontemplate.h"
#include "hbaseobj.h"
#include "hiconobj.h"

#include <QProcessEnvironment>

HIconHelper::HIconHelper()
{

}

HIconHelper* HIconHelper::m_pInstance = NULL;

HIconHelper* HIconHelper::Instance()
{
    if(!m_pInstance)
    {
        m_pInstance = new HIconHelper();
    }
    return m_pInstance;
}

QPixmap HIconHelper::iconPixmap(const QString& strType,const QString& uuid)
{
    HIconTemplate* pIconTemplate = new HIconTemplate(QString());
    Q_ASSERT( pIconTemplate );

    QString iconsPath  = QString(qgetenv("wfsystem_dir"));
#ifdef WIN32
    iconsPath = QProcessEnvironment::systemEnvironment().value("wfsystem_dir");
#else
    iconsPath = "/users/huangw";
#endif
    iconsPath.append("/icons");

    QString path = iconsPath +"/" + strType + "/" + uuid + ".xic";
    if ( uuid.isEmpty() && strType.contains(".xic") )
    {
        path = strType;
    }
    pIconTemplate->readXml(path);

    QSizeF sizeF = pIconTemplate->getDefaultSize();
    QImage image(sizeF.width()+1.00,sizeF.height(),QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    HIconComplexObj* pObj = new HIconComplexObj(pIconTemplate);
    //QPolygonF pf;
    //pf<<QPointF(0,0)<<QPointF(sizeF.width(),sizeF.height());
    //QRectF rectF = pf.boundingRect();
    QSizeF pt = pIconTemplate->getDefaultSize();
    double w1 = sizeF.width()/(pt.width()*20);
    double h1 = sizeF.height()/(pt.height()*20);
    pObj->resize(w1,h1);
    //应该还有一个move
    painter.translate(sizeF.width()/2.0,sizeF.height()/2.0);
    pObj->paint(&painter);
    delete pObj;

    return QPixmap().fromImage(image);
}
