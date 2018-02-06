#include "hfonthelper.h"

HFontHelper::HFontHelper()
{

}

HFontHelper::~HFontHelper()
{
    //if(pFont)
}

HFontHelper* HFontHelper::m_pInstance = NULL;
HFontHelper* HFontHelper::Instance()
{
    if(!m_pInstance)
    {
        m_pInstance = new HFontHelper;
    }

    return m_pInstance;
}

QStringList HFontHelper::fontFamilies() const
{
    QStringList familiesList;
    familiesList<<"Arial"<<"Arial Black"<<"Arial Narrow"<<"Courier"<<"Courier New"<<"Times New Roman";
    familiesList<<QStringLiteral("仿宋")<<QStringLiteral("宋体")<<QStringLiteral("幼圆")<<QStringLiteral("微软雅黑")<<QStringLiteral("新宋体")<<QStringLiteral("方正姚体")
               <<QStringLiteral("方正等线")<<QStringLiteral("方正舒体")<<QStringLiteral("楷体")<<QStringLiteral("隶书")<<QStringLiteral("黑体");
    return familiesList;
}

bool HFontHelper::bold(const QString &family, const QString &style) const
{
    return fontDatabase.bold(family,style);
}

QFont HFontHelper::font(const QString &family, const QString &style, int pointSize) const
{
    return fontDatabase.font(family,style,pointSize);
}

bool HFontHelper::italic(const QString &family, const QString &style) const
{
    return fontDatabase.italic(family,style);
}

QList<int> HFontHelper::pointSizes(const QString &family, const QString &styleName )
{
    return fontDatabase.pointSizes(family,styleName);
}

QList<int> HFontHelper::smoothSizes(const QString &family, const QString &styleName)
{
    return fontDatabase.smoothSizes(family,styleName);
}

QString HFontHelper::styleString(const QFont &font)
{
    return fontDatabase.styleString(font);
}

QString HFontHelper::styleString(const QFontInfo &fontInfo)
{
    return fontDatabase.styleString(fontInfo);
}

QStringList HFontHelper::styles(const QString &family) const
{
    return fontDatabase.styles(family);
}

int HFontHelper::weight(const QString &family, const QString &style) const
{
    return fontDatabase.weight(family,style);
}
