/**********************************************************************
*  Copyright (c) 2008-2015, Alliance for Sustainable Energy.
*  All rights reserved.
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**********************************************************************/

#include "RunTabView.hpp"

#include "FileOperations.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "ScriptFolderListView.hpp"
#include <OpenStudio.hxx>

#include "../model/DaylightingControl.hpp"
#include "../model/DaylightingControl_Impl.hpp"
#include "../model/GlareSensor.hpp"
#include "../model/GlareSensor_Impl.hpp"
#include "../model/IlluminanceMap.hpp"
#include "../model/IlluminanceMap_Impl.hpp"
#include "../model/Model_Impl.hpp"
#include "../model/Space.hpp"
#include "../model/Space_Impl.hpp"
#include "../model/ThermalZone.hpp"
#include "../model/ThermalZone_Impl.hpp"
#include "../model/UtilityBill.hpp"
#include "../model/UtilityBill_Impl.hpp"
#include "../model/Building.hpp"
#include "../runmanager/lib/JobStatusWidget.hpp"
#include "../runmanager/lib/RubyJobUtils.hpp"
#include "../runmanager/lib/RunManager.hpp"

#include "../utilities/core/Application.hpp"
#include "../utilities/core/ApplicationPathHelpers.hpp"
#include "../utilities/sql/SqlFile.hpp"
#include "../utilities/core/Assert.hpp"

#include "../shared_gui_components/WorkflowTools.hpp"

#include <boost/filesystem.hpp>

#include "../energyplus/ForwardTranslator.hpp"
#include "../bec/ForwardTranslator.hpp"

#include "../model/Photovoltaic.hpp"
#include "../model/PhotovoltaicThermal.hpp"
#include "../model/Photovoltaic_Impl.hpp"
#include "../model/PhotovoltaicThermal_Impl.hpp"
#include "benchmarkdialog.hpp"

enum PVReportMode { PVReportMode_OPENSTUDIO, PVReportMode_BEC, PVReportMode_ENERGYPLUS};

#include <QButtonGroup>
#include <QDir>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPlainTextEdit>
#include <QProcess>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QStyleOption>
#include <QSysInfo>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

////////////////////////////////////////////////////////////////////////////////
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QTextStream>
#include <QProgressDialog>
#include <QDesktopServices>
#include <QInputDialog>

static double lastPV;
static QString bvName;
static double bvVal;
static double WholeNetEnergyConsumptionPerArea = 0.0;
static QPlainTextEdit* _log = NULL;

static void echo(const QString& msg){
    if(_log){
        _log->appendHtml(msg);
    }
}

static QString doubleToMoney(double val){
    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
    QString sValue = QString("%L1").arg(val ,12,'d',2);
    while(sValue.endsWith('0'))
        sValue.remove(sValue.length()-1, 1);

    if(sValue.endsWith('.'))
        sValue.append('0');

    return sValue;
}

static QString stringToMoney(const QString& val){
    bool isOK;
    double dval = val.toDouble(&isOK);
    if(isOK){
        return doubleToMoney(dval);
    }
    else{
        if(val.indexOf("m^2")){
            QString newVal = val;
            newVal.replace("m^2", "m<sup>2</sup>");
            return newVal;
        }else{
            return val;
        }
    }
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

QString Bold(const QString& text){
    return QString("<b>%1%2").arg(text).arg("</b>");
}

QString ParagraphBold(const QString& text){
    return QString("<p><b>%1%2").arg(text).arg("</b></p>");
}

QString PTag(const QString& text){
    return QString("<p>%1</p>").arg(text);
}

QString hn(size_t id, const QString& text){
    QString hx = QString("h") + QString::number(id);
    return QString("<%1>%2</%1>\n").arg(hx).arg(text);
}

static int levelsub=-1;

QString doHorizontalTable(QDomNode& root, QDomNode &node, int& level){

    level = 0;

    QString table =
        "<table border=\"1\" cellpadding=\"4\" cellspacing=\"0\">\n"
        "<tbody>\n";

    QString row1="<tr>", row2="<tr>";

    while(!node.isNull()) {
        QDomElement e = node.toElement();
        if(e.tagName() == "WholeNetEnergyConsumptionPerArea"){
            bool isOK;
            WholeNetEnergyConsumptionPerArea = e.text().toDouble(&isOK);
            if(!isOK){
                WholeNetEnergyConsumptionPerArea = 0;
            }
        }
        row1 += QString("<td align=\"right\" valign=\"top\"><b>%1</b></td>").arg(insertSpaceInTag(e.tagName()));
        row2 += QString("<td align=\"right\" valign=\"top\">%1</td>").arg(stringToMoney(e.text()));
        node = node.nextSibling();
    }
    row1 += "</tr>\n";
    row2 += "</tr>\n";

    table += row1;
    table += row2;

    level++;

    QDomElement re = root.toElement();
    QDomNode tmproot = root.nextSibling();
    while(1) {
        QDomElement enext = tmproot.toElement();
        if(re.tagName() == enext.tagName()){
            level++;
            node = tmproot.firstChild();
            QString rown="<tr>";
            while(!node.isNull()) {
                QDomElement e = node.toElement();
                if(!e.text().isEmpty()){
                    rown += QString("<td align=\"right\" valign=\"top\">%1</td>").arg(stringToMoney(e.text()));
                }
                node = node.nextSibling();
            }
            table += rown;
            tmproot = tmproot.nextSibling();
            levelsub = level-1;
        }
        else{
            root = tmproot.toElement();
            break;
        }
    }
    table += "</tbody>\n"
             "</table>\n";
    return table;
}

QString escapeTitle;

static QMap<QString, int> getTableNamesValues() {
    QMap<QString, int>map;
	map.insert("LightingSystemPerformance", 1);
    map.insert("LightingSystemByFloor", 1);
    map.insert("LightingSystemByZone", 1);
    return map;
}

static const QMap<QString, int> tableNames = getTableNamesValues();

QDomElement findElementOnPath(QDomNode& root, const QStringList& paths){
    QDomElement targetNode;
    for (int i = 0; i < paths.size(); ++i) {
        if(i==0){
            targetNode = root.firstChildElement(paths.at(0));
        }
        else{
            targetNode = targetNode.firstChildElement(paths.at(i));
        }

        if(targetNode.isNull())
            break;
    }
    return targetNode;
}

class IDoTable
{
public:
    virtual QString process(QDomNode& root, const QStringList& fullpaths)=0;
};

class doTableArrayOnPath : public IDoTable
{
public:
    doTableArrayOnPath() {}
    double TransparentComponentAreaSUM;
    double ComponentAreaComponentAreaSUM;
    void resetSUM(){
        TransparentComponentAreaSUM = 0.0f;
        ComponentAreaComponentAreaSUM = 0.0f;
    }
    double toWWR(){
        if(ComponentAreaComponentAreaSUM < 0.00000000001)
            return 0.0;
        //echo(QString("----------- TransparentComponentAreaSUM / ComponentAreaComponentAreaSUM : %1/%2").arg(TransparentComponentAreaSUM).arg(ComponentAreaComponentAreaSUM));
        return TransparentComponentAreaSUM / ComponentAreaComponentAreaSUM;
    }

    QString process(QDomNode& root, const QStringList& fullpaths){
        bool initHeader = false;
        QString arrayTags = fullpaths.at(fullpaths.size()-1);

        QStringList paths;
        for (int idx = 0; idx < fullpaths.size()-1; ++idx) {
            paths.append(fullpaths.at(idx));
        }
        QDomElement targetNode = findElementOnPath(root, paths);
        QString out;
        if(targetNode.isNull()){
            out = "<table border=\"1\" cellpadding=\"4\" cellspacing=\"0\">"
                            "<tbody>"
                            "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>"
                            "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>"
                            "</tbody>"
                            "</table>";
        }else{
            QString rowData;
            QString headerData;
            QDomNodeList rawNodes = targetNode.childNodes();
            if(rawNodes.size()>0){
                for (int rit = 0; rit < rawNodes.size(); ++rit) {
                    QDomNode nd = rawNodes.at(rit);
                    QDomElement erow = nd.toElement();

                    if(erow.tagName()!=arrayTags)
                        continue;

                    rowData.append("<tr>");
                    QDomNodeList colNodes = erow.childNodes();
                    for (int cit = 0; cit < colNodes.size(); ++cit) {

                        QDomNode nd = colNodes.at(cit);
                        QDomElement ecol = nd.toElement();

                        if(ecol.tagName().endsWith("Unit"))
                            continue;

                        if(ecol.tagName() == "TransparentComponentArea"){
                            TransparentComponentAreaSUM += ecol.text().toDouble();
                            //echo(QString("++++++++++++++++++++++++++++TransparentComponentAreaSUM [%1]:%2").arg(ecol.text()).arg(QString::number(TransparentComponentAreaSUM)));
                        }
                        if(ecol.tagName() == "ComponentAreaComponentArea"){
                            ComponentAreaComponentAreaSUM += ecol.text().toDouble();
                            //echo(QString("*****************************ComponentAreaComponentAreaSUM [%1]:%2").arg(ecol.text()).arg(QString::number(ComponentAreaComponentAreaSUM)));
                        }

                        if(!initHeader){
                            QString header = insertSpaceInTag(ecol.tagName());
                            QDomElement ecolnext = ecol.nextSiblingElement();
                            if(ecolnext.tagName().endsWith("Unit")){
                                headerData.append(QString("<td>%1(%2)</td>\n").arg(header).arg(ecolnext.text()));
                            }else{
                                headerData.append(QString("<td>%1</td>\n").arg(header));
                            }
                        }
                        rowData.append(QString("<td>%1</td>\n").arg(stringToMoney(ecol.text())));
                    }
                    initHeader = true;
                    rowData.append("</tr>\n");
                }

                if(rowData.isEmpty()&&headerData.isEmpty()){
                    out = "<table border=\"1\" cellpadding=\"4\" cellspacing=\"0\">\n"
                                    "<tbody>\n"
                                    "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>\n"
                                    "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>\n"
                                    "</tbody>\n"
                                    "</table>\n";
                }else{
                    out = QString("<table border=\"1\" cellpadding=\"4\" cellspacing=\"0\">\n"
                          "<tbody>\n"
                          "%1\n"
                          "%2\n"
                          "</tbody>\n"
                          "</table>\n").arg(headerData).arg(rowData);
                }
                return out;
            }else{
                out = "<table border=\"1\" cellpadding=\"4\" cellspacing=\"0\">\n"
                                "<tbody>\n"
                                "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>\n"
                                "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>\n"
                                "</tbody>\n"
                                "</table>\n";
                return out;
            }
        }
        return out;
    }
};

class doVTableOnPath : public IDoTable
{
public:
    doVTableOnPath() {}
    QString process(QDomNode& root, const QStringList& paths){
        QDomElement targetNode = findElementOnPath(root, paths);
        QString out;
        if(targetNode.isNull()){
            out = "<table border=\"1\" cellpadding=\"4\" cellspacing=\"0\">"
                            "<tbody>"
                            "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>"
                            "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>"
                            "</tbody>"
                            "</table>";
        }else{
            out += "<table border=\"1\" cellpadding=\"4\" cellspacing=\"0\">\n<tbody>\n";
            QDomNodeList nodes = targetNode.childNodes();
            if(nodes.size()>0){
                for (int idx = 0; idx < nodes.size(); ++idx) {
                    QDomNode nd = nodes.at(idx);
                    QDomElement element = nd.toElement();

                    if(element.tagName().endsWith("Unit"))
                        continue;

                    QString header = insertSpaceInTag(element.tagName());

                    QDomElement ecolnext = element.nextSiblingElement();
                    if(ecolnext.tagName().endsWith("Unit")){
                        out.append(QString("<tr>\n<td>%1(%2)</td><td>%3</td>\n</tr>\n")
                                   .arg(header)
                                   .arg(ecolnext.text())
                                   .arg(stringToMoney(element.text())));
                    }else{
                        out.append(QString("<tr>\n<td>%1</td><td>%2</td>\n</tr>\n")
                                   .arg(header)
                                   .arg(stringToMoney(element.text())));
                    }
                }
            }else{
                out += "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>\n";
                out += "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>\n";
            }
            out += "</tbody>\n</table>\n";
        }
        return out;
    }
};

class doTableOnPath : public IDoTable
{
public:
    doTableOnPath() {}
	double WWR;
    QString process(QDomNode& root, const QStringList& paths){
        QDomElement targetNode = findElementOnPath(root, paths);
        QString out;
        bool isOnBuildingOTTVAC = false;
        if(targetNode.isNull()){
            out = "<table border=\"1\" cellpadding=\"4\" cellspacing=\"0\">"
                            "<tbody>"
                            "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>"
                            "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>"
                            "</tbody>"
                            "</table>";
        }else{
            QString row2;
            out += "<table border=\"1\" cellpadding=\"4\" cellspacing=\"0\">\n<tbody>\n";
            QDomNodeList nodes = targetNode.childNodes();
            if(nodes.size()>0){
                out.append("<tr>");
                row2.append("<tr>");
                for (int idx = 0; idx < nodes.size(); ++idx) {
                    QDomNode nd = nodes.at(idx);
                    QDomElement element = nd.toElement();

                    if(element.tagName() == "WholeNetEnergyConsumptionPerArea"){
                        bool isOK = false;
                        WholeNetEnergyConsumptionPerArea = element.text().toDouble(&isOK);
                        if(!isOK){
                            WholeNetEnergyConsumptionPerArea = 0;
                        }
                    }

                    if(element.tagName() == "BuildingOTTVAC"){
                        bool isOK = false;
                        isOnBuildingOTTVAC = true;
                        WholeNetEnergyConsumptionPerArea = element.text().toDouble(&isOK);
                        if(!isOK){
                            WholeNetEnergyConsumptionPerArea = 0;
                        }
                    }

                    if(element.tagName().endsWith("Unit"))
                        continue;

                    QString header = insertSpaceInTag(element.tagName());
                    QDomElement ecolnext = element.nextSiblingElement();
                    if(ecolnext.tagName().endsWith("Unit")){
                        out.append(QString("<td>%1(%2)</td>\n").arg(header).arg(ecolnext.text()));
                    }else{
                        out.append(QString("<td>%1</td>\n").arg(header));
                    }

                    row2.append(QString("<td>%1</td>\n").arg(stringToMoney(element.text())));
                }
                if (isOnBuildingOTTVAC) {
					out.append("<td>WWR</td>\n");
                    row2.append(QString("<td>%1</td>\n").arg(QString::number(WWR, 'f', 2)));
				}
                row2.append("</tr>\n");
                out.append("</tr>\n");
                out.append(row2);
            }else{
                out += "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>\n";
                out += "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>\n";
            }
            out += "</tbody>\n</table>\n";
        }
        return out;
    }
};

class TableDataList
{
public:
    enum TABLETYPE { TABLE=0, VTABLE, ARRAYTABLE };
    doTableOnPath* _doTableOnPath;
    doVTableOnPath *_doVTableOnPath;
    doTableArrayOnPath *_doTableArrayOnPath;
    TableDataList(QDomNode root) {
        this->root = root;
        _doTableOnPath = new doTableOnPath;
        _doVTableOnPath = new doVTableOnPath;
        _doTableArrayOnPath = new doTableArrayOnPath;
        tableProcesser.insert(TABLE, _doTableOnPath);
        tableProcesser.insert(VTABLE, _doVTableOnPath);
        tableProcesser.insert(ARRAYTABLE, _doTableArrayOnPath);
    }

    ~TableDataList(){
        QHash<TABLETYPE, IDoTable*>::iterator it = tableProcesser.begin();
        while (it!=tableProcesser.end()) {
            delete it.value();
            it++;
        }
        tableProcesser.clear();
    }

    void append(const QStringList& path, const QString& title, TABLETYPE type){
        sls.append(path);
        titles.append(title);
        tableTypes.append(type);
    }

    QString toString(){
        _doTableArrayOnPath->resetSUM();
        _doTableArrayOnPath->process(root, QStringList()<<"EnvelopeSystem" << "BuildingOTTVwall" << "TransparentComponentWall" << "TransparentComponentByWall");
        _doTableArrayOnPath->process(root, QStringList()<<"EnvelopeSystem" << "BuildingOTTVwall" << "ComponentAreaWall" << "ComponentAreaPerWall");
        _doTableOnPath->WWR = _doTableArrayOnPath->toWWR();
        QString out;
        for (int idx = 0; idx < sls.size(); ++idx) {
            QStringList path = sls.at(idx);
            QString title = titles.at(idx);
            out += ParagraphBold(title);
            TABLETYPE type = tableTypes.at(idx);
            if(tableTypes.contains(type)){
                out.append(tableProcesser[type]->process(root, path));
            }
        }
        return out;
    }

    void clean(){
        sls.clear();
        titles.clear();
        tableTypes.clear();
    }

private:
    QList<QStringList> sls;
    QList<QString> titles;
    QList<TABLETYPE> tableTypes;
    QHash<TABLETYPE, IDoTable*> tableProcesser;
    QDomNode root;
};

static QString doTableV2(QDomNode& root){
    WholeNetEnergyConsumptionPerArea = 0.0;
    TableDataList tables(root);
    tables.append(QStringList()<<"BuildingInfo", "Building Info", TableDataList::TABLE);
    tables.append(QStringList()<<"EnvelopeSystem"<<"BuildingOTTVReport", insertSpaceInTag("BuildingOTTVReport"), TableDataList::TABLE);
    //tables.append(QStringList()<<"EnvelopeSystem"<<"BuildingOTTVReport", "Building Info V", TableDataList::TABLE);
    tables.append(QStringList()<<"EnvelopeSystem" << "BuildingOTTVwall" << "TotalWallOTTVReport", insertSpaceInTag("TotalWallOTTVReport"), TableDataList::ARRAYTABLE);
    tables.append(QStringList()<<"EnvelopeSystem" << "BuildingOTTVwall" << "WallOTTVBySection" << "WallOTTVSection", insertSpaceInTag("WallOTTVSection"), TableDataList::ARRAYTABLE);
    tables.append(QStringList()<<"EnvelopeSystem" << "BuildingOTTVwall" << "OpaqueComponentWall" << "OpaqueComponentByWall", insertSpaceInTag("OpaqueComponentByWall"), TableDataList::ARRAYTABLE);
    tables.append(QStringList()<<"EnvelopeSystem" << "BuildingOTTVwall" << "TransparentComponentWall" << "TransparentComponentByWall", insertSpaceInTag("TransparentComponentByWall"), TableDataList::ARRAYTABLE);
    tables.append(QStringList()<<"EnvelopeSystem" << "BuildingOTTVwall" << "ComponentAreaWall" << "ComponentAreaPerWall", insertSpaceInTag("ComponentAreaPerWall"), TableDataList::ARRAYTABLE);
    tables.append(QStringList()<<"LightingSystem" << "LightingSystemPerformance", insertSpaceInTag("LightingSystemPerformance"), TableDataList::TABLE);
    tables.append(QStringList()<<"LightingSystem" << "LightingSystemFloor" << "LightingSystemByFloor", insertSpaceInTag("LightingSystemByFloor"), TableDataList::ARRAYTABLE);
    tables.append(QStringList()<<"LightingSystem" << "LightingSystemZone" << "LightingSystemByZone", insertSpaceInTag("LightingSystemByZone"), TableDataList::ARRAYTABLE);
    tables.append(QStringList()<<"DXAirUnitSystem" << "DXAirUnit", insertSpaceInTag("DXAirUnit"), TableDataList::ARRAYTABLE);
    tables.append(QStringList()<<"CentralACSystem" << "WaterChilledReport", insertSpaceInTag("WaterChillerReport"), TableDataList::ARRAYTABLE);
    tables.append(QStringList()<<"CentralACSystem" << "CentralACOtherEQReport", insertSpaceInTag("CentralACOtherEQReport"), TableDataList::ARRAYTABLE);
    tables.append(QStringList()<<"CentralACSystem" << "CentralACEQList", insertSpaceInTag("CentralACEQList"), TableDataList::ARRAYTABLE);
    tables.append(QStringList()<<"PVSystem" << "PVSys", insertSpaceInTag("PVSystem"), TableDataList::ARRAYTABLE);
    tables.append(QStringList()<<"WholeBuildingEnergy"<<"WholeBuildingEnergyReport", insertSpaceInTag("WholeBuildingEnergyReport"), TableDataList::TABLE);
    tables.append(QStringList()<<"WholeBuildingEnergy" << "WholeBuildingEnergyReportFloor" << "WholeBuildingEnergyReportByFloor", insertSpaceInTag("WholeBuildingEnergyReportByFloor"), TableDataList::ARRAYTABLE);
    tables.append(QStringList()<<"WholeBuildingEnergy" << "WholeBuildingEnergyReportZone" << "WholeBuildingEnergyReportByZone", insertSpaceInTag("WholeBuildingEnergyReportByZone"), TableDataList::ARRAYTABLE);
    return tables.toString();
}

void doTable(const QString &title, QDomNode& root, QFile& file, int level){

    QHash<QString, bool> h1;
    h1["WholeBuildingEnergy"] = true;

    //if(level == 7)return;
    QDomNode node = root.firstChild();
    QDomElement elm = node.toElement();
    QDomElement fe = elm.firstChildElement();

    if(escapeTitle == title && !escapeTitle.isEmpty())
        return;
    else
        escapeTitle.clear();

    if(tableNames.contains(title)){
        int mylevel=0;
        QString table = doHorizontalTable(root, node, mylevel);
        file.write(table.toStdString().c_str());
        escapeTitle = title;
        return;
    }

    if(fe.isNull()){
        QString out = ParagraphBold(insertSpaceInTag(title));
        file.write(out.toStdString().c_str());
    }
    else{
        int ihn = fmin(level+1, 3);
        QString out = hn(ihn, insertSpaceInTag(title));
        file.write(out.toStdString().c_str());
    }

    //qDebug() << "------------------\n" << title << ":" << root.toElement().tagName() << ", " << level;

    while(!node.isNull()) {
        elm = node.toElement();
        { //AUTOMATIC GENERATE TABLE IN OTHER IS HARDCODE.
            fe = elm.firstChildElement();
            QDomElement fenx = elm.nextSibling().firstChildElement();

            if(fe.isNull()){
                if(level == 0){
                    QString table = ParagraphBold(insertSpaceInTag(elm.tagName()));
                    table +=    "<table border=\"1\" cellpadding=\"4\" cellspacing=\"0\">"
                                "<tbody>"
                                "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>"
                                "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>"
                                "</tbody>"
                                "</table>";
                    file.write(table.toStdString().c_str());
                }
                else if(fenx.tagName() != fe.tagName() && fe.tagName().isNull()){
                    QString table = ParagraphBold(insertSpaceInTag(elm.tagName()));
                    file.write(table.toStdString().c_str());
                    escapeTitle = title;
                }
                else{
                    int mylevel=0;
                    //QString table = Bold(insertSpaceInTag(elm.tagName()));
					QString table = doHorizontalTable(root, node, mylevel);
                    file.write(table.toStdString().c_str());
                    escapeTitle = title;
                    return;
                }
            }
            else{
                doTable(elm.tagName(), elm, file, level+1);
            }
        }
        if(!node.isNull())
            node = node.nextSibling();
        else
            break;
    }
}

static openstudio::path resourcesPath()
{
  if (openstudio::applicationIsRunningFromBuildDirectory())
  {
    return openstudio::getApplicationSourceDirectory() / openstudio::toPath("src/openstudio_app/Resources");
  }
  else
  {
    return openstudio::getApplicationRunDirectory() / openstudio::toPath("../share/openstudio-" + openStudioVersion() + "/OSApp");
  }
}

static QDomElement createTagWithText(QDomDocument* doc, QDomElement &parent, const QString &tag, const QString &text)
{
    QDomElement elm = doc->createElement(tag);
    if(!text.isEmpty())
        elm.appendChild(doc->createTextNode(text.toUtf8()));

    parent.appendChild(elm);
    return elm;
}

static bool doBecReport(const QString &path, QString& outpath, QString &err, float wwr_total){
    QDomDocument doc("becreport");

    QFileInfo fi(path);
    QString output = fi.absolutePath()+"/report.html";
    outpath = output;
    QFile file(output);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        file.write(
"<meta charset=\"UTF-8\">"
"<html>\n"
"<head>\n"
"<title>BEC Report</title>\n"
"</head>\n"
"<body>\n"
"<style>\n\
table {\n\
border-width: 1px;\n\
border-spacing: 0px;\n\
border-style: solid;\n\
border-color: green;\n\
border-collapse: separate;\n\
background-color: white;\n\
}\n\
h1 {\n\
background-color: #fbd4b3;\n\
}\n\
h2 {\n\
background-color: #fbd4b3;\n\
}\n\
h3 {\n\
background-color: #fbd4b3;\n\
}\n\
h4 {\n\
background-color: #fbd4b3;\n\
}\n\
p {\n\
background-color: #fbd4b3;\n\
}\n\
table th {\n\
border-width: 1px;\n\
padding: 1px;\n\
border-style: solid;\n\
border-color: black;\n\
background-color: white;\n\
-moz-border-radius: ;\n\
}\n\
\n\
table tr:first-child td {\n\
background-color: #b8dce8;\n\
text-align: center;\n\
}\n\
\n\
table td {\n\
border-width: 1px;\n\
padding: 1px;\n\
border-style: solid;\n\
border-color: black;\n\
-moz-border-radius: ;\n\
}\n\
\n\
table tr:nth-child(even){\n\
background-color: #ddd8c4;\n\
}\n\
\n\
table tr:hover {\n\
background-color: #ffff99;\n\
}\n\
</style>\n");

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
    //doTable(docElem.tagName(), docElem, file, 0);

    QString tables = doTableV2(docElem);
    file.write(tables.toUtf8());

    //BENCHMARK
    QString pass = "Failed";
    if(WholeNetEnergyConsumptionPerArea<bvVal){
        pass = "Passed";
    }

    QString bvTable = QString("<p><b>Analysis result</b></p>\n"
                            "<table id=\"%1\" border=\"1\" cellpadding=\"4\" cellspacing=\"0\">\n"
                            "  <tbody>\n"
                            "  <tr>\n"
                            "    <td align=\"center\" valign=\"top\"></td>\n"
                            "    <td align=\"center\" valign=\"top\">Type</td>\n"
                            "    <td align=\"center\" valign=\"top\">Ref Sector/Floor Area[kWh/m2]<sup>2</sup>]</td>\n"
                            "    <td align=\"center\" valign=\"top\">Result[kWh/m<sup>2</sup>]</td>\n"
                            "  <tr>\n"
                            "    <td align=\"right\" valign=\"top\">Benchmark</td>\n"
                            "    <td align=\"right\" valign=\"top\">%2</td>\n"
                            "    <td align=\"right\" valign=\"top\">%3</td>\n"
                            "    <td align=\"right\" valign=\"top\">%4</td>\n"
                            "  </tr>\n"
                            "</tbody></table><br><br>\n</body>")
            .arg("bv_table")
            .arg(bvName)
            .arg(doubleToMoney(bvVal))
            .arg(doubleToMoney(WholeNetEnergyConsumptionPerArea))
            .arg(pass);

    file.write(bvTable.toUtf8());

    QDate date = QDateTime::currentDateTime().date();
    int cryear, month, day;
    date.getDate(&cryear, &month, &day);
    cryear += 543;
    QLocale locale  = QLocale(QLocale::Thai, QLocale::Thailand); // set the locale you want here
    QString thaidate = locale.toString(date, "d MMMM");

    QString sign = QString("<br><div style=\"text-align: center;padding-bottom: 50px;padding-right: 50px;float: right;\"><br>\n"
                         "----- ----- ----- ----- ----- ----- ----- ----- -----<br><br>\n"
                         "(..... ..... ..... ..... ..... ..... ..... ..... ..... ..... .....)<br>\n"
                         "ผู้รับรองการประเมิน<br>\n"
                         "%1 %2\n"
                         "</div>\n").arg(thaidate).arg(cryear);
    file.write(sign.toUtf8());
    file.write("</body>\n"
               "</html>\n");

    file.close();
    return true;
}
////////////////////////////////////////////////////////////////////////////////

namespace openstudio {

RunTabView::RunTabView(const model::Model & model,
                       QWidget * parent)
  : MainTabView("Run Simulation",true,parent)
    //m_runView(new RunView(model)),
    //m_status(new openstudio::runmanager::JobStatusWidget(m_runView->runManager()))
{
  //addSubTab("Output", m_runView);
  //addSubTab("Tree", m_status);

  //connect(m_runView, SIGNAL(resultsGenerated(const openstudio::path &)),
  //    this, SIGNAL(resultsGenerated(const openstudio::path &)));
}

RunView::RunView(const model::Model & model,
                 const openstudio::path &t_modelPath,
                 const openstudio::path &t_tempFolder,
                 openstudio::runmanager::RunManager t_runManager)
  : m_model(model),
    m_modelPath(t_modelPath),
    m_tempFolder(t_tempFolder),
    m_canceling(false),
    becProcess(NULL)

{
  bool isConnected = t_runManager.connect(SIGNAL(statsChanged()), this, SLOT(runManagerStatsChanged()));
  OS_ASSERT(isConnected);

  QGridLayout *mainLayout = new QGridLayout();
  mainLayout->setContentsMargins(5,5,5,5);
  mainLayout->setSpacing(5);
  setLayout(mainLayout);

  // Run / Play button area
  m_playButton = new QToolButton();
  m_playButton->setText("     Run");
  m_playButton->setCheckable(true);
  m_playButton->setChecked(false);
  QIcon playbuttonicon(QPixmap(":/images/run_simulation_button.png"));
  playbuttonicon.addPixmap(QPixmap(":/images/run_simulation_button.png"), QIcon::Normal, QIcon::Off);
  playbuttonicon.addPixmap(QPixmap(":/images/cancel_simulation_button.png"), QIcon::Normal, QIcon::On);
  m_playButton->setStyleSheet("QToolButton { background:transparent; font: bold; }");
  m_playButton->setIconSize(QSize(35,35));
  m_playButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  m_playButton->setIcon(playbuttonicon);
  m_playButton->setLayoutDirection(Qt::RightToLeft);
//  m_playButton->setStyleSheet("QAbstractButton:!hover { border: none; }");

  mainLayout->addWidget(m_playButton, 0, 0);
  connect(m_playButton, &QToolButton::clicked, this, &RunView::playButtonClicked);

  // Progress bar area
  m_progressBar = new QProgressBar();
 
  QVBoxLayout *progressbarlayout = new QVBoxLayout();
  progressbarlayout->addWidget(m_progressBar);
  m_statusLabel = new QLabel("Ready");
  progressbarlayout->addWidget(m_statusLabel);
  mainLayout->addLayout(progressbarlayout, 0, 1);

  m_radianceGroup = new QButtonGroup(this);

  connect(m_radianceGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &RunView::on_radianceGroupClicked);

  int buttonCount = 0;

  m_energyPlusButton = new QRadioButton("EnergyPlus");
  m_radianceGroup->addButton(m_energyPlusButton,buttonCount++);

  m_radianceButton = new QRadioButton("Radiance");
  m_radianceGroup->addButton(m_radianceButton,buttonCount++);

  m_becButton = new QRadioButton("New BEC");
  m_radianceGroup->addButton(m_becButton, buttonCount++);

  // "Radiance" Button Layout
 
  QLabel *radianceLabel = new QLabel("<b>Select Daylight Simulation Engine</b>");

  QWidget *radianceWidget = new QWidget();
  radianceWidget->setObjectName("RunStatusViewRadiance");
  QHBoxLayout *radianceInteriorLayout = new QHBoxLayout();

  radianceWidget->setLayout(radianceInteriorLayout);
  radianceInteriorLayout->addWidget(radianceLabel);
  radianceInteriorLayout->addStretch();
  radianceInteriorLayout->addWidget(m_energyPlusButton);
  radianceInteriorLayout->addStretch();
  radianceInteriorLayout->addWidget(m_radianceButton);
  radianceInteriorLayout->addStretch();
  radianceInteriorLayout->addWidget(m_becButton);


/*
  radianceHLayout->addSpacing(100);
  radianceHLayout->addWidget(radianceWidget, 3);
  radianceHLayout->addStretch(2);
  */
  radianceWidget->setStyleSheet("QWidget#RunStatusViewRadiance {background: #DADADA; border: 1px solid #A5A5A5;}");



/*

  m_radianceWarningsAndErrorsButton = new QPushButton();
  m_radianceWarningsAndErrorsButton->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred);
  m_radianceWarningsAndErrorsButton->hide();

  connect(m_radianceWarningsAndErrorsButton, &QPushButton::clicked, this, &RunView::on_radianceWarningsAndErrorsClicked);

  QHBoxLayout * radianceHLayout = new QHBoxLayout();
  radianceHLayout->addWidget(m_radianceButton);
  radianceHLayout->addWidget(m_radianceWarningsAndErrorsButton);
  radianceHLayout->addStretch();

  QVBoxLayout * radianceVLayout = new QVBoxLayout();
  radianceVLayout->addWidget(m_energyPlusButton);
  radianceVLayout->addLayout(radianceHLayout);

  QGroupBox * groupBox = new QGroupBox("For Daylighting Calculation use");
  groupBox->setLayout(radianceVLayout);
*/

  mainLayout->addWidget(radianceWidget, 1, 1);

  if (usesRadianceForDaylightCalculations(t_runManager))
  {
    m_radianceButton->setChecked(true);
  } else {
    m_energyPlusButton->setChecked(true);
  }

  openstudio::runmanager::ToolVersion epversion = getRequiredEnergyPlusVersion();
  if( auto tag = epversion.getTag() ) {
    m_toolWarningLabel = new QLabel(openstudio::toQString("<b>Notice:</b> EnergyPlus " + 
      std::to_string(epversion.getMajor().get()) + "." +
      std::to_string(epversion.getMinor().get()) + "." +
      std::to_string(epversion.getBuild().get()) + " Build \"" +
      tag.get() + "\""
      " is required and not yet located."
      "  Run Preferences -> Scan For Tools to locate."));
  } else {
    m_toolWarningLabel = new QLabel(openstudio::toQString("<b>Notice:</b> EnergyPlus " + 
      std::to_string(epversion.getMajor().get()) + "." +
      std::to_string(epversion.getMinor().get()) + "." +
      std::to_string(epversion.getBuild().get()) +
      " is required and not yet located."
      "  Run Preferences -> Scan For Tools to locate."));
  }
  m_toolWarningLabel->hide();

  mainLayout->addWidget(m_toolWarningLabel, 2, 1);

  locateEnergyPlus();

  m_warningsLabel = new QLabel("<b>Warnings:</b> 0");
  m_errorsLabel = new QLabel("<b>Errors:</b> 0");
  mainLayout->addWidget(m_warningsLabel, 3, 1);
  mainLayout->addWidget(m_errorsLabel, 4, 1);
  mainLayout->addWidget(new QLabel("Output"), 5, 1);
  m_outputWindow = new QPlainTextEdit();
  _log = m_outputWindow;
  m_outputWindow->setReadOnly(true);
  mainLayout->addWidget(m_outputWindow, 6, 1);

  updateRunManagerStats(t_runManager);
}


void RunView::getRadiancePreRunWarningsAndErrors(std::vector<std::string> & warnings,
                                                 std::vector<std::string> & errors)
{
  openstudio::runmanager::RunManager rm = runManager();
  boost::optional<model::Model> model(m_model);
  openstudio::getRadiancePreRunWarningsAndErrors(warnings, errors, rm, model);
}

void RunView::locateEnergyPlus()
{
  openstudio::runmanager::ConfigOptions co(true);
  openstudio::runmanager::ToolVersion epversion = getRequiredEnergyPlusVersion();
  bool energyplus_not_installed = co.getTools().getAllByName("energyplus").getAllByVersion(epversion).tools().size() == 0;
  
  if (energyplus_not_installed){
    m_toolWarningLabel->show();
  } else {
    m_toolWarningLabel->hide();
  }
}

void RunView::updateToolsWarnings()
{
  LOG(Debug, "updateToolsWarnings called");
  
  //TODO:Implement BEC getRadiancePreRunWarningsAndErrors(m_becWarnings, m_becErrors);
  getRadiancePreRunWarningsAndErrors(m_radianceWarnings,m_radianceErrors);

  QString checkBoxText;
  QString buttonText;

  if(m_radianceErrors.size() > 0){
    m_energyPlusButton->setChecked(true);
  }

  locateEnergyPlus();
}

void RunView::outputDataAdded(const openstudio::UUID &, const std::string &t_data)
{
  QTextCursor cursor = m_outputWindow->textCursor();
  cursor.movePosition(QTextCursor::End);
  cursor.insertText(openstudio::toQString(t_data));
  m_outputWindow->ensureCursorVisible();
}

void RunView::updateRunManagerStats(openstudio::runmanager::RunManager t_runManager)
{
  double numberofjobs = 0;
  double completedjobs = 0;
  double totalerrors = 0;
  double totalwarnings = 0;

  std::map<std::string, double> stats = t_runManager.statistics();
  numberofjobs = stats["Number of Jobs"];
  completedjobs = stats["Completed Jobs"];
  totalerrors = stats["Total Errors"];
  totalwarnings = stats["Total Warnings"];

  if (numberofjobs == 0) numberofjobs = 1;

  m_progressBar->setRange(0, numberofjobs);
  m_progressBar->setValue(completedjobs);

  m_warningsLabel->setText(openstudio::toQString("<b>Warnings:</b> " + boost::lexical_cast<std::string>(int(totalwarnings))));
  m_errorsLabel->setText(openstudio::toQString("<b>Errors:</b> " + boost::lexical_cast<std::string>(int(totalerrors))));
}

bool RunView::doBecInput(const QString &path, const model::Model &model, QString &outpath, QString &err){
    QString output = path;

    outpath = output;

    bec::ForwardTranslator trans;
    m_outputWindow->appendPlainText(QString("Create input.xml at %1").arg(path));

    std::string bvn;
	bool success = trans.modelTobec(model, path.toStdString().c_str(), NULL, &bvn, &sunlits, wwr_totoal);
    bvName = bvn.c_str();

	std::string bvsdefault = resourcesPath().string() + "/" + "default_building_standard.bvs";
    BenchmarkDialog* bmdlg = new BenchmarkDialog(bvsdefault.c_str(), this);
    bvVal = bmdlg->getValueByName(bvName);
    bmdlg->accept();

    std::vector<LogMessage> translatorErrors = trans.errors();
    //std::vector<LogMessage> translatorWarnings = trans.warnings();

    for( std::vector<LogMessage>::iterator it = translatorErrors.begin();
         it < translatorErrors.end();
         ++it )
    {
        err.append(QString::fromStdString(it->logMessage()));
        err.append("\n");
    }
    //translatorWarnings = trans.warnings();

    m_outputWindow->appendPlainText(QString("Create input.xml at %1 success").arg(path));
    return success;
}

double RunView::getPV(openstudio::model::Model* model)
{
    double res = 0.0;
    std::vector<model::Photovoltaic> pvs = model->getModelObjects<model::Photovoltaic>();
    for (model::Photovoltaic& pv : pvs){
        res += pv.calculatePV();
    }

    std::vector<model::PhotovoltaicThermal> pvst = model->getModelObjects<model::PhotovoltaicThermal>();
    for (model::PhotovoltaicThermal& pv : pvst){
        res += pv.calculatePV();
    }
    return res;
}

static double sumArrayStringOfDouble(const QString& arrayStr, int begin){
    //qDebug() << QStringRef(&arrayStr, begin, 5).toString();
    QString res;
    double out=0;
    for(int i=begin;i<arrayStr.size();i++){
        QChar ch = arrayStr.at(i);
        if(ch.isNumber()){
            res.append(ch);
        }
        else if(ch == '.'){
            res.append(ch);
        }
        else if(ch==','){
            out += res.toDouble();
            //qDebug() << "in out sum: " << out << ", res:" <<res ;
            res.clear();
        }
        else if(ch==']'){
            out += res.toDouble();
            //qDebug() << "in out sum: " << out << ", res:" <<res ;
            res.clear();
            break;
        }
    }
    return out;
}

static double getDouble(const QString& str){
    QString res;
    for(int i=0;i<str.size();i++){
        QChar ch = str.at(i);
        if(ch.isNumber()){
            res.append(ch);
        }
        else if(ch == '.'){
            res.append(ch);
        }
    }
    bool isOk;
    double out = res.toDouble(&isOk);
    if(isOk)
        return out;
    else
        return 0;
}

static double findEnergyPlusPowerTotalkWh(const QString& str){
    const QString key1 = "<b>Site and Source Energy</b><br><br>";
    const QString key2 = "<td align=\"right\">Total Source Energy</td>";
    const QString tdend = "</td>";

    //qDebug() << "==============\n" << str;
    //qDebug() << "++++++++++++++\n" << key1;

    int idx = str.indexOf(key1);
    if(idx<0)
        return 0;

    idx = str.indexOf(key2, idx+key1.size());
    if(idx<0)
        return 0;

    int col2Begin = str.indexOf(tdend, idx+key2.size());
    if(col2Begin<0)
        return 0;

    int col2End = str.indexOf(tdend, col2Begin+tdend.size());
    if(col2End<0)
        return 0;

    QStringRef sub(&str, col2Begin, col2End-col2Begin);
    return getDouble(sub.toString())/3.6f;
}

static double findOpenStudioPowerTotal(const QString& str){
    const QString key0 = "\"Electricity Consumption\":{";

    QStringList sls;
    sls.push_back("\"Heating\":[");
    sls.push_back("\"Cooling\":[");
    sls.push_back("\"Interior Lighting\":[");
    sls.push_back("\"Exterior Lighting\":[");
    sls.push_back("\"Interior Equipment\":[");
    sls.push_back("\"Exterior Equipment\":[");
    sls.push_back("\"Fans\":[");
    sls.push_back("\"Pumps\":[");
    sls.push_back("\"Heat Rejection\":[");
    sls.push_back("\"Humidification\":[");
    sls.push_back("\"Heat Recovery\":[");
    sls.push_back("\"Water Systems\":[");
    sls.push_back("\"Refrigeration\":[");
    sls.push_back("\"Generators\":[");

    double out = 0;
    int begin = str.indexOf(key0);
    if(begin<0)
        return 0;

    begin = begin+key0.size();
    begin = str.indexOf(sls.at(0), begin);
    if(begin<0)
        return out;
    else
        out += sumArrayStringOfDouble(str, begin+sls.at(0).size());

    for(int i=1;i<sls.size();i++){
        begin = begin+sls.at(i-1).size();
        begin = str.indexOf(sls.at(i), begin);
        if(begin<0)
            return out;
        else{
            //qDebug() << QStringRef(&str, begin, 5).toString();
            out += sumArrayStringOfDouble(str, begin+sls.at(i).size());
            //qDebug() << "CURRENT OUT :" << out;
        }
    }
    return out;
}

void RunView::addPVAndBenchmarkToFile(const QString &fileName, int mode)
{
    double pv = lastPV;
    static QString pvid = "_Z_O_axz1d0_j_i_";
    static QString bvid = "_A_w08_B_p3_O_vv";

    QString fn = fileName;
    fn.replace("file:///", "");

    QByteArray fileData;
    QFile file(fn);

    if(!file.open(QIODevice::ReadWrite)){
        file.close();
        return;
    }
    fileData = file.readAll();
    QString text(fileData);

    bool firstPV = text.lastIndexOf(pvid)<0;

    switch (mode) {
    case PVReportMode_OPENSTUDIO:
    {
        const QString css_style = "\n\
table {\n\
border-width: 1px;\n\
border-spacing: 0px;\n\
border-style: solid;\n\
border-color: green;\n\
border-collapse: separate;\n\
background-color: white;\n\
}\n\
table tr:first-child td {\n\
background-color: #b8dce8;\n\
text-align: center;\n\
}\n\
table tr:last-child td {\n\
background-color: #91cf50;\n\
text-align: center;\n\
}\n\
table th {\n\
border-width: 1px;\n\
padding: 1px;\n\
border-style: solid;\n\
border-color: green;\n\
background-color: white;\n\
-moz-border-radius: ;\n\
}\n\
\n\
table th {\n\
background-color: #ffcc97;\n\
text-align: center;\n\
}\n\
\n\
table td {\n\
border-width: 1px;\n\
padding: 1px;\n\
border-style: solid;\n\
border-color: black;\n\
-moz-border-radius: ;\n\
}\n\
\n\
table tr:nth-child(even){\n\
background-color: #ddd8c4;\n\
}\n\
\n\
table tr:hover {\n\
background-color: #ffff99;\n\
}\n\
";
        double val = findOpenStudioPowerTotal(text);
        m_outputWindow->appendPlainText(QString("Power Total:%1, buildingArea:%2").arg(val).arg(buildingArea));
        val = val/buildingArea;
        m_outputWindow->appendPlainText(QString("Power Total/buildingArea=%1").arg(val));
        //PV
        QString table = QString("<h4>Photovoltaic(watt)</h4>\n"
                                "<table id=\"%1\">\n"
                                "	<thead>\n"
                                "		<tr>\n"
                                "			<th>&nbsp;</th>\n"
                                "			<th>watt</th>\n"
                                "		</tr>\n"
                                "	</thead>\n"
                                "	<tbody>\n"
                                "		<tr>\n"
                                "			<td>Photovoltaic</td>\n"
                                "			<td>%2</td>\n"
                                "		</tr>\n"
                                "	</tbody>\n"
                                "</table>\n"
                                "</body>").arg(pvid).arg(QString::number(pv, 'f', 2));
        if(firstPV){
            text.replace(QRegularExpression("body\\s+{\\s+font: 10px sans-serif;\\s+min-width: 1280px;\\s+}"), "");
            text.replace(QRegularExpression("table\\s+{\\s+max-width:800px;\\s+}"), css_style);
            text.replace("table table-striped table-bordered table-condensed", "");
            text.replace("</body>", table);
        }
        else{
            int start = text.lastIndexOf("<h4>Photovoltaic(watt)</h4>\n");
            int count = text.size()-start;
            text.replace(start, count+1, table);
            text.append("\n</html>\n");
        }
        ///////////////////////////////
        //BENCHMARK
        QString pass = "Failed";
        if(val<bvVal){
            pass = "Passed";
        }
        //TODO: CHANGE Value to Project name.
        //TODO: Add <sup>2</sup> to kwh

        std::shared_ptr<OSDocument> osdocument = OSAppBase::instance()->currentDocument();
        QString savePath = osdocument->savePath();
        QFileInfo savePathFile(savePath);
        table = QString("<h4>Benchmark</h4>\n"
                                "<table id=\"%1\">\n"
                                "	<thead>\n"
                                "		<tr>\n"
                                "			<th></th>\n"
                                "			<th>%2</th>\n"
                                "		</tr>\n"
                                "	</thead>\n"
                                "	<tbody>\n"
                                "		<tr>\n"
                                "			<td>Type</td>\n"
                                "			<td>%3</td>\n"
                                "		</tr>\n"
                                "		<tr>\n"
                                "			<td>Standard(kWh/m<sup>2</sup>)</td>\n"
                                "			<td>%4</td>\n"
                                "		</tr>\n"
                                "		<tr>\n"
                                "			<td>Result(kWh/m<sup>2</sup>)</td>\n"
                                "			<td>%5</td>\n"
                                "		</tr>\n"
                                "		<tr>\n"
                                "			<td>Status</td>\n"
                                "			<td>%6</td>\n"
                                "		</tr>\n"
                                "	</tbody>\n"
                                "</table>\n"
                                "</body>")
                .arg(bvid)
                .arg(savePathFile.baseName())
                .arg(bvName)
                .arg(doubleToMoney(bvVal))
                .arg(doubleToMoney(val))
                .arg(pass);

        text.replace("</body>", table);
    }
        break;
    case PVReportMode_ENERGYPLUS:
    {
        const QString css_style = "\n\
table {\n\
border-width: 1px;\n\
border-spacing: 0px;\n\
border-style: solid;\n\
border-color: black;\n\
border-collapse: separate;\n\
background-color: white;\n\
}\n\
table th {\n\
border-width: 1px;\n\
padding: 1px;\n\
border-style: solid;\n\
border-color: black;\n\
background-color: white;\n\
-moz-border-radius: ;\n\
}\n\
\n\
table th {\n\
background-color: #ffcc97;\n\
text-align: center;\n\
}\n\
table tr:first-child td {\n\
background-color: #b8dce8;\n\
text-align: center;\n\
}\n\
table td {\n\
border-width: 1px;\n\
padding: 1px;\n\
border-style: solid;\n\
border-color: black;\n\
-moz-border-radius: ;\n\
}\n\
\n\
table tr:nth-child(even){\n\
background-color: #ddd8c4;\n\
}\n\
\n\
table tr:hover {\n\
background-color: #ffff99;\n\
}\n\
";
        double val = findEnergyPlusPowerTotalkWh(text);
        if(text.indexOf("<meta charset=\"utf-8\">")<0){
            text.replace("<head>", "<head>\n<meta charset=\"utf-8\">");
        }
        QString table = QString("<b>Photovoltaic</b><br><br>\n"
                                "<table id=\"%1\" border=\"1\" cellpadding=\"4\" cellspacing=\"0\">\n"
                                "  <tbody>\n"
                                "  <tr><td></td><td align=\"right\">watt</td></tr>\n"
                                "  <tr>\n"
                                "    <td align=\"right\">Photovoltaic(watt)</td>\n"
                                "    <td align=\"right\">%2</td>\n"
                                "  </tr>\n"
                                "</tbody></table><br><br>\n</body>").arg(pvid).arg(QString::number(pv, 'f', 2));

        if(firstPV){
            text.replace("<html>", QString("<html><style type=\"text/css\">")+css_style+"</style>");
            text.replace("</body>", table);
        }
        else{
            int start = text.lastIndexOf("<b>Photovoltaic</b><br><br>");
            int count = text.size()-start;
            text.replace(start, count+1, table);
            text.append("\n</html>\n");
        }

        ///////////////////////////////
        //BENCHMARK
        QString pass = "Failed";
        if(val<bvVal){
            pass = "Passed";
        }
        table.clear();
        table = QString("<b>Benchmark</b><br><br>\n"
                                "<table id=\"%1\" border=\"1\" cellpadding=\"4\" cellspacing=\"0\">\n"
                                "  <tbody>\n"
                                "  <tr>\n"
                                "    <td align=\"center\"></td>\n"
                                "    <td align=\"center\">Type</td>\n"
                                "    <td align=\"center\">Standard[kWh/m<sup>2</sup>]</td>\n"
                                "    <td align=\"center\">Result[kWh/m<sup>2</sup>]</td>\n"
                                "    <td align=\"center\">Status</td>\n"
                                "  <tr>\n"
                                "    <td align=\"right\">Benchmark</td>\n"
                                "    <td align=\"right\">%2</td>\n"
                                "    <td align=\"right\">%3</td>\n"
                                "    <td align=\"right\">%4</td>\n"
                                "    <td align=\"right\">%5</td>\n"
                                "  </tr>\n"
                                "</tbody></table><br><br>\n</body>")
                .arg(bvid)
                .arg(bvName)
                .arg(doubleToMoney(bvVal))
                .arg(doubleToMoney(val))
                .arg(pass);

        text.replace("</body>", table);
    }
        break;
    }
    file.seek(0);
    file.write(text.toUtf8());
    file.flush();
    file.close();
}

//TODO:CHANGE TO POSTPORCESSING
void RunView::updatePVInfile()
{
    QString outpath = (m_tempFolder/"resources").string().c_str();
    QString opsReportPath = outpath + "/run/6-UserScript-0/report.html";
    QString eReportPath = outpath + "/run/5-EnergyPlus-0/eplustbl.htm";
    QString becReportPath = outpath + "/run/9-BEC-0/eplustbl.htm";

    QFileInfo opsCheckFile(opsReportPath);
    if (opsCheckFile.exists() && opsCheckFile.isFile()) {
        m_outputWindow->appendPlainText(QString("Update OpenStudio report pv value is %1 at %2").arg(lastPV).arg(opsReportPath));
        addPVAndBenchmarkToFile(opsReportPath, PVReportMode_OPENSTUDIO);
    }

    QFileInfo eReportFile(eReportPath);
    if (eReportFile.exists() && eReportFile.isFile()) {
        m_outputWindow->appendPlainText(QString("Update EnergyPlus report pv value is %1 at %2").arg(lastPV).arg(eReportPath));
        addPVAndBenchmarkToFile(eReportPath, PVReportMode_ENERGYPLUS);
    }

    QFileInfo becReportFile(becReportPath);
    if (becReportFile.exists() && becReportFile.isFile()) {

    }
}

void RunView::callRealBEC(const QString &dir){
    QString outpath = dir;
    outpath.replace("\\", "/");
    if(!outpath.endsWith("//")){
        outpath.append("/");
    }
    openstudio::path path = resourcesPath();
    std::string program = path.string() + "/newBEC.exe";

    QStringList arguments;
    arguments << outpath;

    if(becProcess)
        becProcess->deleteLater();

    becProcess = new QProcess(this);
    becProcess->start(program.c_str(), arguments);

    //BEC SLOT
    connect(becProcess, SIGNAL(error(QProcess::ProcessError))
            , SLOT(becError(QProcess::ProcessError)));
    connect(becProcess, SIGNAL(finished(int, QProcess::ExitStatus))
            , SLOT(becFinished(int, QProcess::ExitStatus)));
    connect(becProcess, SIGNAL(readyReadStandardError())
            , SLOT(becReadyReadStandardError()));
    connect(becProcess, SIGNAL(readyReadStandardOutput())
            , SLOT(becReadyReadStandardOutput()));
    connect(becProcess, SIGNAL(started())
            , SLOT(becStarted()));
    connect(becProcess, SIGNAL(stateChanged(QProcess::ProcessState))
            , SLOT(becStateChanged(QProcess::ProcessState)));
}

void RunView::runManagerStatsChanged()
{
    updateRunManagerStats(runManager());
}

void RunView::runFinished(const openstudio::path &t_sqlFile, const openstudio::path &t_radianceOutputPath)
{
    if (m_canceling)
    {
        m_statusLabel->setText("Canceled");
    }

    std::shared_ptr<OSDocument> osdocument = OSAppBase::instance()->currentDocument();

    // DLM: should we attach the sql file to the model here?
    // DLM: if model is re-opened with results they will not be added here, better to do this on results tab
  //if (exists(t_sqlFile)){
  //  SqlFile sqlFile(t_sqlFile);
  //  if (sqlFile.connectionOpen()){
  //    osdocument->model().setSqlFile(sqlFile);
  //  }
  //}
  updatePVInfile();
  m_canceling = false;
  LOG(Debug, "Emitting results generated for sqlfile: " << openstudio::toString(t_sqlFile) << " and radiance file " << openstudio::toString(t_radianceOutputPath));
  emit resultsGenerated(t_sqlFile, t_radianceOutputPath);

  // needed so save of osm file does not trigger out of date and start running again
  runManager().setPaused(true);
  m_playButton->setChecked(false);
  osdocument->enableTabsAfterRun();
}

void RunView::treeChanged(const openstudio::UUID &t_uuid)
{
  std::string statusstr = "Ready";

  try {
    openstudio::runmanager::Job j = runManager().getJob(t_uuid);
    while (j.parent())
    {
      j = j.parent().get();
    }

    openstudio::runmanager::TreeStatusEnum status = j.treeStatus();
    LOG(Debug, "Tree finished, status is: " << status.valueName());
    statusstr = status.valueDescription();

    openstudio::path sqlpath;
    openstudio::path radianceOutPath;

    if (status == openstudio::runmanager::TreeStatusEnum::Finished
        || status == openstudio::runmanager::TreeStatusEnum::Failed
        || status == openstudio::runmanager::TreeStatusEnum::Canceled)
    {
      if (status == openstudio::runmanager::TreeStatusEnum::Failed && m_canceling)
      {
        statusstr = "Canceled";
      }

      try {
        sqlpath = j.treeAllFiles().getLastByFilename("eplusout.sql").fullPath;
      } catch (const std::exception &e) {
        LOG(Debug, "Tree finished, error getting sql file: " << e.what());
      } catch (...) {
        LOG(Debug, "Tree finished, error getting sql file");
        // no sql file exists
      }

      try {
        radianceOutPath = j.treeOutputFiles().getLastByFilename("radout.sql").fullPath;
      } catch (const std::exception &e) {
        LOG(Debug, "Tree finished, error getting radout.sql file: " << e.what());
      } catch (...) {
        LOG(Debug, "Tree finished, error getting radout.sql file");
        // no sql file exists
      }

      runFinished(sqlpath, radianceOutPath);
    } else { 
      m_canceling = false;
    }
  } catch (const std::exception &e) {
    LOG(Debug, "Tree finished, error getting status: " << e.what());
    runFinished(openstudio::path(), openstudio::path());

  } catch (...) {
    LOG(Debug, "Tree finished, error getting status");
    runFinished(openstudio::path(), openstudio::path());
    // no sql file exists
  }

  m_statusLabel->setText(openstudio::toQString(statusstr));
}

openstudio::runmanager::ToolVersion RunView::getRequiredEnergyPlusVersion()
{
  std::string sha = energyPlusBuildSHA();
  if( ! sha.empty() ) {
    return openstudio::runmanager::ToolVersion(energyPlusVersionMajor(),energyPlusVersionMinor(),energyPlusVersionPatch(),sha);
  } else {
    return openstudio::runmanager::ToolVersion(energyPlusVersionMajor(),energyPlusVersionMinor(),energyPlusVersionPatch());
  }
}

openstudio::path resourcesPath()
{
  if (openstudio::applicationIsRunningFromBuildDirectory())
  {
    return openstudio::getApplicationSourceDirectory() / openstudio::toPath("src/openstudio_app/Resources");
  }
  else
  {
    return openstudio::getApplicationRunDirectory() / openstudio::toPath("../share/openstudio-" + openStudioVersion() + "/OSApp");
  }
}

void RunView::playButtonClicked(bool t_checked)
{
  LOG(Debug, "playButtonClicked " << t_checked);
  
  std::shared_ptr<OSDocument> osdocument = OSAppBase::instance()->currentDocument();

  if(osdocument->modified())
  {
    osdocument->save();
    // save dialog was canceled
    if(osdocument->modified()) {
      return;
    }
  }
  buildingArea = 0.000001;
  if(boost::optional<openstudio::model::Building> building = osdocument->model().building()){
      openstudio::model::Building & bd = building.get();
      buildingArea = bd.floorArea();
  }

  lastPV = getPV(&osdocument->model());

  if(m_becButton->isChecked()){

      QString outpath = (m_tempFolder/"resources").string().c_str();
      if(!outpath.isEmpty()){

          //GEN INPUT
          QString filePath;
          QString err;
          outpath += "/run/9-BEC-0/";
          m_outputWindow->appendPlainText(QString("Temp output : '%1'").arg(outpath));

          QDir dir(outpath);
          if (!dir.exists()) {
              dir.mkpath(".");
          }

          //////////////////////////
          QFile file(outpath+"../5-EnergyPlus-0/eplustbl.htm");
          sunlits.clear();
		  wwr_totoal = 0.0f;
          if (file.open(QIODevice::ReadOnly | QIODevice::Text))
          {
              m_outputWindow->appendHtml(QString("<font color=\"blue\">Start read eblustbl.htm</font>"));
              QTextStream in(&file);
              QString str = in.readAll();
			  QString gwstr = str;
              int start = str.indexOf("<b>Sunlit Fraction</b>");
              int table_start_idx = str.indexOf("<table", start);
              int table_end_idx = str.indexOf("</table>", table_start_idx+6);

              m_outputWindow->appendPlainText(QString("table sidx:%1->%2:%3").arg(start).arg(table_start_idx).arg(table_end_idx));

              str = str.mid(table_start_idx, table_end_idx-table_start_idx);
              str = str.replace("\r", "");
			  str = str.replace("\n", "");
              str = str.replace(QRegExp("<table[^>]*>"), "");
              str = str.replace("</table>", "");
              str = str.replace(QRegExp("<td[^>]*>"), "");
              str = str.replace(QRegExp("<tr[^>]*>"), "");

              QStringList rowls = str.split("</tr>");

              m_outputWindow->appendHtml("<h3>Sunlit Fraction</h3>");
              for (int irow = 1; irow < rowls.size(); ++irow) {
                  QStringList cols = rowls.at(irow).split("</td>");

                  if(cols.size()<1)
                      break;

                  QString sfname = cols.at(0);
                  QString msg = sfname+":";
                  for (int icol = 1; icol < cols.size(); ++icol) {
                      double num = cols.at(icol).trimmed().toDouble();
                      sunlits[sfname.toUpper().trimmed()].append(num);
                      msg += QString::number(num)+",";
                  }
                  m_outputWindow->appendHtml(QString("<font color=\"green\">%1</font>\n").arg(msg));
              }
              m_outputWindow->appendHtml(QString("<font color=\"blue\">SF:%1</font>").arg(QString::number(sunlits.size())));

			  //Read Conditioned Window-Wall Ratio --> Gross Window-Wall Ratio [%] --> <td align="right">       35.20</td>
			  int idx_of_wwrAll = gwstr.indexOf("Conditioned Window-Wall Ratio");
			  int idx_of_wwrAll_end = -1;
			  if (idx_of_wwrAll > -1){
				  idx_of_wwrAll = gwstr.indexOf("Gross Window-Wall Ratio [%]", idx_of_wwrAll);
				  const QString tdBegin("<td align=\"right\"> ");
				  const QString tdEnd("</td>");
				  idx_of_wwrAll = gwstr.indexOf(tdBegin, idx_of_wwrAll);
				  if (idx_of_wwrAll > -1){
					  idx_of_wwrAll += tdBegin.length();
					  idx_of_wwrAll_end = gwstr.indexOf(tdEnd, idx_of_wwrAll);
					  m_outputWindow->appendHtml(QString("idx0:%1, idx1:%2").arg(idx_of_wwrAll).arg(idx_of_wwrAll_end));
					  if (idx_of_wwrAll_end > -1){
						  gwstr = gwstr.mid(idx_of_wwrAll, idx_of_wwrAll_end - idx_of_wwrAll);
						  m_outputWindow->appendHtml(QString("gwstr:[%1]").arg(gwstr));
						  gwstr = gwstr.trimmed();
                          wwr_totoal = gwstr.toFloat();
                          wwr_totoal = wwr_totoal/100.0f;
						  m_outputWindow->appendHtml(QString("wwr_totoal:[%1]").arg(wwr_totoal));
						  m_outputWindow->appendHtml(QString("<font color=\"blue\"><b>wwr totoal:%1</b></font>").arg(gwstr));
					  }
				  }
			  }
          }else{
            m_outputWindow->appendHtml(QString("<font color=\"red\">ERROR:Can't read eblustbl.htm</font>"));
          }
          ///////////////////////////

          becoutputPath = outpath+"output.xml";

          bool success = doBecInput(outpath+"input.xml", osdocument->model(), filePath, err);

          if(!err.isEmpty())
              m_outputWindow->appendPlainText(err);

          if(!success){
              osdocument->enableTabsAfterRun();
              runFinished(openstudio::path(), openstudio::path());
              return;
          }
          else
              m_outputWindow->appendPlainText("Call newBEC.");

          callRealBEC(outpath);
      }
      return;
  }else if(m_energyPlusButton->isChecked()){
      bvName = QString();
      bvVal = 0.0f;
      QInputDialog inputBuildingType;
      inputBuildingType.setOption(QInputDialog::UseListViewForComboBoxItems);
      inputBuildingType.setWindowTitle("What is building type.");
      inputBuildingType.setLabelText("Selection:");
      QStringList types;

      std::string bvsdefault = resourcesPath().string() +"/"+ "default_building_standard.bvs";
      BenchmarkDialog* bmdlg = new BenchmarkDialog(bvsdefault.c_str(), this);

      for(size_t idx=0;idx<bmdlg->valuesCount();idx++){
          BenchmarkValue* bv = bmdlg->valueAt(idx);
          if(bv){
              types << bv->name();
          }
      }

      inputBuildingType.setComboBoxItems(types);
      int ret = inputBuildingType.exec();

      if(ret == QDialog::Rejected){
          osdocument->enableTabsAfterRun();
          runFinished(openstudio::path(), openstudio::path());
          return;
      }

      bvName = inputBuildingType.textValue();
      bvVal = bmdlg->getValueByName(bvName);
      bmdlg->accept();
  }

  updateToolsWarnings();

  if (!t_checked)
  {
    m_playButton->setChecked(true);

    if (!m_canceling)
    {
      // we are pausing the simulations
      m_statusLabel->setText("Canceling");
      m_canceling = true;
      openstudio::Application::instance().processEvents();
      runmanager::RunManager rm = runManager();
      pauseRunManager(rm);
    } else {
      LOG(Debug, "Already canceling, not doing it again");
    }
  } else {
    runmanager::ConfigOptions co(true);
    co.findTools(true, true, false, true);
    co.saveQSettings();

    updateToolsWarnings();

    openstudio::runmanager::ToolVersion epver = getRequiredEnergyPlusVersion();
    if (co.getTools().getAllByName("energyplus").getAllByVersion(epver).tools().size() == 0)
    {
      if( auto tag = epver.getTag() ) {
        QMessageBox::information(this, 
            "Missing EnergyPlus",
            QString::fromStdString("EnergyPlus " +
            std::to_string(epver.getMajor().get()) + "." +
            std::to_string(epver.getMinor().get()) + "." +
            std::to_string(epver.getBuild().get()) + " Build \"" +
            tag.get() + "\" could not be located, simulation aborted."),
            QMessageBox::Ok);
      } else {
        QMessageBox::information(this, 
            "Missing EnergyPlus",
            QString::fromStdString("EnergyPlus " +
            std::to_string(epver.getMajor().get()) + "." +
            std::to_string(epver.getMinor().get()) + "." +
            std::to_string(epver.getBuild().get()) + 
            " could not be located, simulation aborted."),
            QMessageBox::Ok);
      }
      m_playButton->setChecked(false);
      osdocument->enableTabsAfterRun();
      return;
    }

    if (co.getTools().getAllByName("ruby").tools().size() == 0)
    {
      QMessageBox::information(this,
          "Missing Ruby",
          "Ruby could not be located, simulation aborted.",
          QMessageBox::Ok);
      m_playButton->setChecked(false);
      osdocument->enableTabsAfterRun();
      return;
    }

    // TODO call Dan's ModelToRad translator to determine if there are problems
    if(m_radianceButton->isChecked() && (!m_radianceWarnings.empty() || !m_radianceErrors.empty())) {
      showRadianceWarningsAndErrors(m_radianceWarnings, m_radianceErrors);
      if(m_radianceErrors.size()){
        return;
      }
      else{
        // check messageBox return value to run with warnings
      }
    }

    m_canceling = false;
    m_outputWindow->clear();

    // reset the model's sqlFile
    osdocument->model().resetSqlFile();

    // Tell OSDoc that great things are happening
    osdocument->disableTabsDuringRun();

    // we are starting the simulations
    QTimer::singleShot(0, this, SLOT(requestStartRunManager()));
  }
}

void RunView::requestStartRunManager()
{
  // we are starting the simulations
  std::shared_ptr<OSDocument> osdocument = OSAppBase::instance()->currentDocument();
  bool requireCalibrationReports = (osdocument->model().getConcreteModelObjects<model::UtilityBill>().size() > 0);
  openstudio::runmanager::RunManager rm = runManager();
  startRunManager(rm, m_modelPath, m_tempFolder, m_radianceButton->isChecked(), requireCalibrationReports, this);
}

void RunView::becFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_outputWindow->appendPlainText("BEC Finished.");
    m_outputWindow->appendPlainText("Generate Report.");
    QString outpath, err;
    if(!doBecReport(becoutputPath, outpath, err, wwr_totoal)){
        m_outputWindow->appendPlainText("Error BEC Report.");
        m_outputWindow->appendPlainText(err);
    }
    else{
        std::shared_ptr<OSDocument> osdocument = OSAppBase::instance()->currentDocument();
        m_outputWindow->appendPlainText("Generate bec complete.");
        m_playButton->setChecked(false);
        //updatePVInfile();
    }

    if(m_progressBar->value()!=50)
        m_progressBar->setValue(100);

    runFinished(openstudio::path(), openstudio::path());
    becProcess->deleteLater();
    becProcess = NULL;
}

void RunView::becReadyReadStandardError()
{
    QByteArray array = becProcess->readAllStandardError();
    QString str = QString("<font color=\"red\">%1</font><br>").arg(QString(array));
    m_outputWindow->appendHtml(str);
    m_progressBar->setValue(50);
}

void RunView::becReadyReadStandardOutput()
{
    QByteArray array = becProcess->readAllStandardOutput();
    QString str = QString("%1").arg(QString(array));
    m_outputWindow->appendPlainText(str);
}

void RunView::becStarted()
{
    m_progressBar->setRange(0,100);
    m_outputWindow->appendPlainText("Start running bec.");
}

void RunView::becStateChanged(QProcess::ProcessState newState)
{
    //TODO:IMPLEMENT becStateChanged
    (void)newState;
}

void RunView::becError(QProcess::ProcessError error)
{
    //TODO:IMPLEMENT becError
    m_progressBar->setValue(50);
    (void)error;
}

openstudio::runmanager::RunManager RunView::runManager()
{
  return OSAppBase::instance()->project()->runManager();
}

void RunView::showRadianceWarningsAndErrors(const std::vector<std::string> & warnings,
                                            const std::vector<std::string> & errors)
{
  QString errorsAndWarnings;
  QString text;
  
  if(warnings.size()){
    errorsAndWarnings += "WARNINGS:\n";
    for (std::string warning : warnings){
      text = warning.c_str();
      errorsAndWarnings += text;
      errorsAndWarnings += '\n';
    }
    errorsAndWarnings += '\n';
  }

  if(errors.size()){
    errorsAndWarnings += "ERRORS:\n";
    for (std::string error : errors){
      text = error.c_str();
      errorsAndWarnings += text;
      errorsAndWarnings += '\n';
    }
  }

  QMessageBox::critical(this, "Radiance Warnings and Errors", errorsAndWarnings);
}

void RunView::on_radianceWarningsAndErrorsClicked(bool /*checked*/)
{
  showRadianceWarningsAndErrors(m_radianceWarnings,m_radianceErrors);
}

void RunView::on_radianceGroupClicked(int idx)
{
  QAbstractButton * button = m_radianceGroup->button(idx);
  OS_ASSERT(button);
  if(button == m_radianceButton){
    emit useRadianceStateChanged(true);
    updateToolsWarnings();
    if(m_radianceErrors.size()){
      showRadianceWarningsAndErrors(m_radianceWarnings,m_radianceErrors);
    }
  }
  else if(button == m_becButton){
     //TODO:IMPLEMENT.
  }
  else{
    emit useRadianceStateChanged(false);
  }
}

} // openstudio
