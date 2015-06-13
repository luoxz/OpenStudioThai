#include "genbecreport.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QFileInfo>
#include <QDebug>
#include <QDomElement>
#include <QColor>
#include <vector>
#include <math.h>

bool isSupportTitle(const QString& title){
    if(title == "BEC Report")return true;
    if(title == "Results | OpenStudio")return true;
    if(title.indexOf("EnergyPlus")>=0) return true;
    return false;
}

QString insertSpaceInTag(const QString& tagName){
    QChar ch0 = 'A';
    QString out;
    for(int i=0;i<tagName.length();i++){
        QChar ch = tagName.at(i);
        if(ch.isUpper()){
            if(ch0.isLower()){
                out+= " ";
            }
            else if(ch0.isUpper() && i!=0){
                int i1=i+1;
                if(i1 < tagName.length()){
                    QChar ch1 = tagName.at(i1);
                    if(ch1.isLower())
                        out += " ";
                }
            }
        }
        ch0 = ch;
        out += ch;
    }
    return out;
}

struct Dic{
    static QHash<QString,QString> create_map()
    {
        QHash<QString,QString> m;
        m["BuildingName"]           = "Building Name";
        m["BuildingType"]           = "BuildingType";
        m["BuildingLocation"]       = "Building Location";
        m["BuildingOTTVStatus"]     = "Building OTTV Status";
        m["BuildingRTTVStatus"]     = "Building RTTV Status";
        m["BuildingOTTVAC"]         = "Building OTTVAC";
        m["BuildingOTTVACUnit"]     = "Building OTTVAC Unit";
        m["BuildingCodeOTTV"]       = "Building Code OTTV";
        m["BuildingCodeOTTVUnit"]   = "Building Code OTTV Unit";
        m["BuildingOTTVAll"]        = "Building OTTV All";
        m["BuildingOTTVAllUnit"]    = "Building OTTV AllUnit";
        m["BuildingRTTVAC"]         = "Building RTTVAC";
        m["BuildingRTTVACUnit"]     = "Building RTTVAC Unit";
        m["BuildingCodeRTTV"]       = "Building Code RTTV";
        m["BuildingCodeRTTVUnit"]   = "Building Code RTTV Unit";
        m["BuildingOTTVAC"]         = "Building OTTVAC";
        m["BuildingOTTVACUnit"]     = "Building OTTVAC Unit";
        m["BuildingCodeOTTV"]       = "Building Code OTTV";
        m["BuildingCodeOTTVUnit"]   = "Building Code OTTV Unit";
        m["BuildingOTTVAll"]        = "Building OTTVAll";
        m["BuildingOTTVAllUnit"]    = "Building OTTVAll Unit";
        m["BuildingRTTVAC"]         = "Building RTTVAC";
        m["BuildingRTTVACUnit"]     = "Building RTTVAC Unit";
        m["BuildingCodeRTTV"]       = "Building Code RTTV";
        m["BuildingCodeRTTVUnit"]   = "Building Code RTTV Unit";
        m["TotalWallName"]          = "Total Wall Name";
        m["TotalWallType"]          = "Total Wall Type";
        m["TotalWallOTTV"]          = "Total Wall OTTV";
        m["TotalWallOTTVUnit"]      = "Total Wall OTTV Unit";
        m["TotalWallWWR"]           = "Total Wall WWR";
        m["xxxxxx"] = "";
        m["xxxxxx"] = "";

        return m;
    }
    static const QHash<QString,QString> map;
};

const QHash<QString,QString> Dic::map =  Dic::create_map();

QString Bold(const QString& text){
    return QString("<b>%1%2").arg(text).arg("</b>");
}

QString PTag(const QString& text){
    return QString("<p>%1</p>").arg(text);
}

QString hn(size_t id, const QString& text){
    QString hx = QString("h") + QString::number(id);
    return QString("<%1>%2</%1>\n").arg(hx).arg(text);
}

QString getWord(const QString& word){
    QHash<QString,QString>::const_iterator find = Dic::map.find(word);
    if(find!=Dic::map.end()){
        return (*find);
    }
    else{
        return word;
    }
}

void writeBuildingInfo(QDomElement& root, QFile& file){
    QString out = hn(2, "Building Info ");
    QDomNode node = root.firstChild();
    while(!node.isNull()) {
        QDomElement elm = node.toElement();
        out += Bold(QString(getWord(elm.tagName())+" : %1<br>").arg(elm.text()));
        node = node.nextSibling();
    }
    file.write(out.toStdString().c_str());
}

QString colorText(const QString& str, const QColor& color){
    return QString("<font color=\"%1\">%2</font>")
            .arg(color.name()).arg(str);
}

QString colorTrueFalse(const QString& text){
    QString tlower = text.toLower();
    if(tlower == "false" || tlower == "failed" || tlower == "fail"){
        return colorText(text, Qt::red);
    }
    else if(tlower == "true" || tlower == "passed" || tlower == "pass"){
        return colorText(text, Qt::green);
    }
    else return colorText(text, Qt::black);
}

void writeBuildingOTTVReport(QDomElement& root, QFile& file){
    QString out = hn(2, "Building OTTV Report");
    QDomNode node = root.firstChild();
    while(!node.isNull()) {
        QDomElement elm = node.toElement();
        if(elm.tagName() == "BuildingOTTVStatus"){
            QString text = colorTrueFalse(elm.text());
            out += Bold(QString(getWord(elm.tagName())+" : %1<br>").arg(text));
        }
        else if(elm.tagName() == "BuildingRTTVStatus"){
            QString text = colorTrueFalse(elm.text());
            out += Bold(QString(getWord(elm.tagName())+" : %1<br>").arg(text));
        }
        else{
            if(elm.tagName().endsWith("Unit"))
                out += Bold(elm.text()+"<br>");
            else
                out += Bold(QString(getWord(elm.tagName())+" : %1 ").arg(elm.text()));
        }
        node = node.nextSibling();
    }
    file.write(out.toStdString().c_str());
}

QString doTableTagHeaderTextNodeVal(QDomElement& root){
    QString sumary = "<table border=\"1\" cellpadding=\"4\" cellspacing=\"0\">\n<tbody>\n";
    QDomNode node = root.firstChild();
    QString vals;
    sumary += "<tr>\n";
    while(!node.isNull()){
        QDomElement elm = node.toElement();
        QString tag = getWord(elm.tagName());
        sumary += QString("<td align=\"center\">%1</td>").arg(tag);
        vals += QString("<td align=\"right\">%1</td>").arg(elm.text());
        node = node.nextSibling();
    }
    sumary += "</tr>\n";
    sumary += QString("<tr>\n%1</tr>\n").arg(vals);
    sumary += "	</tbody>\n</table>\n";
    return sumary;
}

void writeBuildingOTTVwall(QDomElement& root, QFile& file){
    QString out = hn(2, "Building OTTV Wall");
    file.write(out.toStdString().c_str());
    QDomNode node = root.firstChild();
    while(!node.isNull()) {
        QDomElement elm = node.toElement();
        if(elm.tagName() == "TotalWallOTTVReport"){
            QString table = doTableTagHeaderTextNodeVal(elm);
            file.write(table.toStdString().c_str());
        }
        else if(elm.tagName() == "WallOTTVBySection"){
            QDomNode node2 = node.firstChild();
            while (!node2.isNull()) {
                //TODO:Create WallOTTVSection logic.
                //TODO:Do class for adda multi data row with data compare.
                QDomElement elm2 = node2.toElement();
                qDebug() << "999999999+ : " << elm2.tagName();
                node2 = node2.nextSibling();
            }
        }
        else {
            qDebug() << "555555+ : " << elm.tagName();
        }
        node = node.nextSibling();
    }
}

void writeEnvelopeSystem(QDomElement& root, QFile& file){
    QString out = hn(1, "Envelope System");
    file.write(out.toStdString().c_str());
    QDomNode node = root.firstChild();
    while(!node.isNull()) {
        QDomElement elm = node.toElement();
        if(elm.tagName() == "BuildingOTTVReport"){
            writeBuildingOTTVReport(elm, file);
        }
        else if(elm.tagName() == "BuildingOTTVwall"){
            writeBuildingOTTVwall(elm, file);
        }
        else if(elm.tagName() == "BuildingOTTVReport"){

        }
        node = node.nextSibling();
    }
}

QString doVerticalTable(QDomNode &node){
    QString table =
        "<table border=\"1\" cellpadding=\"4\" cellspacing=\"0\">\n"
        "<tbody>\n";

    while(!node.isNull()) {
        QDomElement e = node.toElement();
        if(!e.text().isEmpty()){
            table += QString("<tr><td align=\"left\">%1</td>"
                             "<td align=\"right\">%2</td></tr>\n")
                            .arg(insertSpaceInTag(e.tagName()))
                            .arg(e.text());
        }
        node = node.nextSibling();
    }
    table += "</tr>\n"
             "</tbody>\n"
             "</table>\n";
    return table;
}

void doTable(const QString &title, QDomElement& root, QFile& file, int level){
    if(title == "LightingSystem")
        return;

    //if(level == 7)return;
    QDomNode node = root.firstChild();
    QDomElement e = node.toElement();
    QDomElement fe = e.firstChildElement();

    if(fe.isNull()){
        QString out = PTag(Bold(insertSpaceInTag(title)));
        file.write(out.toStdString().c_str());
    }
    else{
        int ihn = fmin(level+1, 3);
        QString out = hn(ihn, insertSpaceInTag(title));
        file.write(out.toStdString().c_str());
    }

    while(!node.isNull()) {
        e = node.toElement();
        fe = e.firstChildElement();
        if(fe.isNull()){
            QString table = doVerticalTable(node);
            file.write(table.toStdString().c_str());
        }
        else{
            //qDebug()<< "Title:" << out << ", tagname:" << e.tagName() << ", type:" << e.nodeType() << " , val:" << e.nodeValue();
            doTable(e.tagName(), e, file, level+1);
        }
        if(!node.isNull())
            node = node.nextSibling();
        else
            break;
    }
}

bool doBecReport(const QString &path, QString& outpath, QString &err){
    QDomDocument doc("becreport");

    QFileInfo fi(path);
    QString output = fi.absolutePath()+"/report.html";
    outpath = output;
    QFile file(output);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        file.write("<!DOCTYPE html>\n"
                   "<meta charset=\"UTF-8\">"
                   "<html>\n"
                   "<head>\n"
                   "<title>BEC Report</title>\n"
                   "</head>\n"
                   "<body>\n");

    }
    else{
        err = "Can't create report name " +output;
        return false;
    }

    QFile xmlfile(path);
    if (!xmlfile.open(QIODevice::ReadOnly))
        return false;
    if (!doc.setContent(&xmlfile)) {
        xmlfile.close();
        err = "Can't set content xml file.";
        return false;
    }
    xmlfile.close();

    QDomElement docElem = doc.documentElement();
    doTable(docElem.tagName(), docElem, file, 0);

    file.write("</body>\n"
               "</html>\n");
    file.close();
    return true;
}

void testInsertSpaceInTag(){
    QHash<QString,QString>::const_iterator it = Dic::map.begin();
    while (it!=Dic::map.end()) {
        qDebug() << (*it) << ":" << insertSpaceInTag((*it)) << "\n";
        it++;
    }
}
