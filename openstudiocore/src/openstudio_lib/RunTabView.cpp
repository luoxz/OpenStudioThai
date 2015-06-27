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
        if(!e.text().isEmpty()){
            row1 += QString("<td align=\"left\">%1</td>").arg(insertSpaceInTag(e.tagName()));
            row2 += QString("<td align=\"right\">%1</td>").arg(e.text());
        }
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
                    rown += QString("<td align=\"right\">%1</td>").arg(e.text());
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

void doTable(const QString &title, QDomNode& root, QFile& file, int level){

    //if(level == 7)return;
    QDomNode node = root.firstChild();
    QDomElement elm = node.toElement();
    QDomElement fe = elm.firstChildElement();

    if(escapeTitle == title && !escapeTitle.isEmpty())
        return;
    else
        escapeTitle.clear();

    if(fe.isNull()){
        QString out = Bold(insertSpaceInTag(title))+"<br>";
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
        fe = elm.firstChildElement();
        if(fe.isNull()){
            int mylevel=0;
            QString table = doHorizontalTable(root, node, mylevel);
            //qDebug() << "mylevel:" << mylevel;
            file.write(table.toStdString().c_str());
            escapeTitle = title;
            return;
        }
        else{
            doTable(elm.tagName(), elm, file, level+1);
        }
        if(!node.isNull())
            node = node.nextSibling();
        else
            break;
    }
}

const QString bec_xml
=
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<BuildingReport>\
  <BuildingInfo>\
    <BuildingName>BEC_Store</BuildingName>\
    <BuildingType>Department Store</BuildingType>\
    <BuildingLocation>Bangkok</BuildingLocation>\
  </BuildingInfo>\
  <EnvelopeSystem>\
    <BuildingOTTVReport>\
      <BuildingOTTVStatus>Failed</BuildingOTTVStatus>\
      <BuildingRTTVStatus>Passed</BuildingRTTVStatus>\
      <BuildingOTTVAC>31.117</BuildingOTTVAC>\
      <BuildingOTTVACUnit>W&#47;m^2</BuildingOTTVACUnit>\
      <BuildingCodeOTTV>40</BuildingCodeOTTV>\
      <BuildingCodeOTTVUnit>W&#47;m^2</BuildingCodeOTTVUnit>\
      <BuildingOTTVAll>41</BuildingOTTVAll>\
      <BuildingOTTVAllUnit>W&#47;m^2</BuildingOTTVAllUnit>\
      <BuildingRTTVAC>9.2</BuildingRTTVAC>\
      <BuildingRTTVACUnit>W&#47;m^2</BuildingRTTVACUnit>\
      <BuildingCodeRTTV>11</BuildingCodeRTTV>\
      <BuildingCodeRTTVUnit>W&#42;m^2</BuildingCodeRTTVUnit>\
    </BuildingOTTVReport>\
    <BuildingOTTVwall>\
      <TotalWallOTTVReport>\
        <TotalWallName>ผนังทิศตะวันตก</TotalWallName>\
        <TotalWallType>Wall</TotalWallType>\
        <TotalWallOTTV>21.63</TotalWallOTTV>\
        <TotalWallOTTVUnit>W&#47;m^2</TotalWallOTTVUnit>\
        <TotalWallWWR>0.00</TotalWallWWR>\
      </TotalWallOTTVReport>\
      <WallOTTVBySection>\
        <WallOTTVSection>\
          <WallWallName>ผนังทิศตะวันตก</WallWallName>\
          <WallSectionName>ผนังแบบที่ 2</WallSectionName>\
          <WallSectionOTTV>22.629</WallSectionOTTV>\
          <WallSectionOTTVUnit>W&#47;m^2</WallSectionOTTVUnit>\
          <WallWWR>0.00</WallWWR>\
        </WallOTTVSection>\
      </WallOTTVBySection>\
      <OpaqueComponentWall>\
        <OpaqueComponentByWall>\
          <OpaqueWallName>ผนังทิศตะวันตก</OpaqueWallName>\
          <OpaqueSectionName>ผนังแบบที่ 2</OpaqueSectionName >\
          <OpaqueComponentName>ผนังคอนกรีตมวลเบาฉาบปูน</OpaqueComponentName>\
          <OpaqueComponentArea>28</OpaqueComponentArea>\
          <OpaqueComponentAreaUnit>m^2</OpaqueComponentAreaUnit>\
          <OpaqueComponentUw>2.294</OpaqueComponentUw>\
          <OpaqueComponentDSH>199.68</OpaqueComponentDSH>\
          <OpaqueComponentDSHUnit>kJ&#47;m^3</OpaqueComponentDSHUnit>\
          <OpaqueComponentColor>Surface of pale color</OpaqueComponentColor>\
          <OpaqueComponentSolarAbsorbtance>0.5</OpaqueComponentSolarAbsorbtance>\
          <OpaqueComponentTDeq>19.916</OpaqueComponentTDeq>\
          <OpaqueComponentQ>829.621</OpaqueComponentQ>\
        </OpaqueComponentByWall>\
      </OpaqueComponentWall>\
      <TransparentComponentWall>\
        <TransparentComponentByWall>\
          <TransparentWallName>ผนังทิศตะวันออก</TransparentWallName>\
          <TransparentSectionName>ผนังแบบที่ 1</TransparentSectionName >\
          <TransparentComponentName>กระจกใส 10 มม.</TransparentComponentName>\
          <TransparentComponentArea>20</TransparentComponentArea>\
          <TransparentComponentAreaUnit>m^2</TransparentComponentAreaUnit>\
          <TransparentComponentUt>5.78</TransparentComponentUt>\
          <TransparentComponentDt>5</TransparentComponentDt>\
          <TransparentComponentSHGC>0.73</TransparentComponentSHGC>\
          <TransparentComponentSC>1</TransparentComponentSC>\
          <TransparentComponentESR>162.039</TransparentComponentESR>\
          <TransparentComponentQ>2943.769</TransparentComponentQ>\
        </TransparentComponentByWall>\
      </TransparentComponentWall>\
      <ComponentAreaWall>\
        <ComponentAreaPerWall>\
          <ComponentAreaWallName>ผนังทิศตะวันออก</ComponentAreaWallName>\
          <ComponentAreaSectionName>ผนังแบบที่ 1</ComponentAreaSectionName >\
          <ComponentAreaComponentName>กระจกใส 10 มม.</ComponentAreaComponentName>\
          <ComponentAreaComponentArea>79.97</ComponentAreaComponentArea>\
          <ComponentAreaComponentAreaUnit>m^2</ComponentAreaComponentAreaUnit>\
          <ComponentAreaPercentage>15.87</ComponentAreaPercentage>\
        </ComponentAreaPerWall>\
        <ComponentAreaPerWall>\
          <ComponentAreaWallName>ผนังทิศตะวันออก</ComponentAreaWallName>\
          <ComponentAreaSectionName>ผนังแบบที่ 1</ComponentAreaSectionName >\
          <ComponentAreaComponentName>ผนังคอนกรีตมวลเบาฉาบปูน</ComponentAreaComponentName>\
          <ComponentAreaComponentArea>424.03</ComponentAreaComponentArea>\
          <ComponentAreaComponentAreaUnit>m^2</ComponentAreaComponentAreaUnit>\
          <ComponentAreaPercentage>84.13</ComponentAreaPercentage>\
        </ComponentAreaPerWall>\
      </ComponentAreaWall>\
    </BuildingOTTVwall>\
  </EnvelopeSystem>\
  <LightingSystem>\
    <LightingSystemPerformance>\
      <LightingSystemStatus>Passed</LightingSystemStatus>\
      <LightingSystemTotalPower>113016.00</LightingSystemTotalPower>\
      <LightingSystemTotalPowerUnit>Watts</LightingSystemTotalPowerUnit>\
      <LightingSystemTotalBuildingArea>8064.00</LightingSystemTotalBuildingArea>\
      <LightingSystemTotalBuildingAreaUnit>m^2</LightingSystemTotalBuildingAreaUnit>\
      <LightingSystemPowerDensity>14.015</LightingSystemPowerDensity>\
      <LightingSystemPowerDensityUnit>W&#47;m^2</LightingSystemPowerDensityUnit>\
      <LightingSystemCompliance>18</LightingSystemCompliance>\
      <LightingSystemComplianceUnit>W&#47;m^2</LightingSystemComplianceUnit>\
    </LightingSystemPerformance>\
    <LightingSystemFloor>\
      <LightingSystemByFloor>\
        <LightingFloorName>1</LightingFloorName>\
        <LightingFloorTotalPower>36984.00</LightingFloorTotalPower>\
        <LightingFloorTotalPowerUnit>Watts</LightingFloorTotalPowerUnit>\
        <LightingFloorTotalArea>2688.00</LightingFloorTotalArea>\
        <LightingFloorTotalAreaUnit>m^2</LightingFloorTotalAreaUnit>\
        <LightingFloorPowerDensity>13.759</LightingFloorPowerDensity>\
        <LightingFloorPowerDensityUnit>W&#47;m^2</LightingFloorPowerDensityUnit>\
      </LightingSystemByFloor>\
      <LightingSystemByFloor>\
        <LightingFloorName>2</LightingFloorName>\
        <LightingFloorTotalPower>38016.00</LightingFloorTotalPower>\
        <LightingFloorTotalPowerUnit>Watts</LightingFloorTotalPowerUnit>\
        <LightingFloorTotalArea>2688.00</LightingFloorTotalArea>\
        <LightingFloorTotalAreaUnit>m^2</LightingFloorTotalAreaUnit>\
        <LightingFloorPowerDensity>14.143</LightingFloorPowerDensity>\
        <LightingFloorPowerDensityUnit>W&#47;m^2</LightingFloorPowerDensityUnit>\
      </LightingSystemByFloor>\
      <LightingSystemByFloor>\
        <LightingFloorName>3</LightingFloorName>\
        <LightingFloorTotalPower>38016.00</LightingFloorTotalPower>\
        <LightingFloorTotalPowerUnit>Watts</LightingFloorTotalPowerUnit>\
        <LightingFloorTotalArea>2688.00</LightingFloorTotalArea>\
        <LightingFloorTotalAreaUnit>m^2</LightingFloorTotalAreaUnit>\
        <LightingFloorPowerDensity>14.143</LightingFloorPowerDensity>\
        <LightingFloorPowerDensityUnit>W&#47;m^2</LightingFloorPowerDensityUnit>\
      </LightingSystemByFloor>\
    </LightingSystemFloor>"
"    <LightingSystemZone>\
      <LightingSystemByZone>\
        <LightingZoneName>1Z-01</LightingZoneName>\
        <LightingZoneFloorName>1</LightingZoneFloorName>\
        <LightingZoneArea>1792</LightingZoneArea>\
        <LightingZoneAreaUnit>m^2</LightingZoneAreaUnit>\
        <LightingZonePower>96.00</LightingZonePower>\
        <LightingZonePowerUnit>Watts/Unit</LightingZonePowerUnit>\
        <LightingZoneQuantity>310</LightingZoneQuantity>\
        <LightingZoneQuantityDayLighted>0</LightingZoneQuantityDayLighted>\
        <LightingZoneTotalPower>28760.00</LightingZoneTotalPower>\
        <LightingZoneTotalPowerUnit>Watt</LightingZoneTotalPowerUnit>\
        <LightingZonePowerDensity>16.607</LightingZonePowerDensity>\
        <LightingZonePowerDensityUnit>W&#47;m^2</LightingZonePowerDensityUnit>\
      </LightingSystemByZone>\
      <LightingSystemByZone>\
        <LightingZoneName>1Z-02</LightingZoneName>\
        <LightingZoneFloorName>1</LightingZoneFloorName>\
        <LightingZoneArea>640</LightingZoneArea>\
        <LightingZoneAreaUnit>m^2</LightingZoneAreaUnit>\
        <LightingZonePower>97.00</LightingZonePower>\
        <LightingZonePowerUnit>Watts/Unit</LightingZonePowerUnit>\
        <LightingZoneQuantity>42</LightingZoneQuantity>\
        <LightingZoneQuantityDayLighted>0</LightingZoneQuantityDayLighted>\
        <LightingZoneTotalPower>4032.00</LightingZoneTotalPower>\
        <LightingZoneTotalPowerUnit>Watt</LightingZoneTotalPowerUnit>\
        <LightingZonePowerDensity>6.3</LightingZonePowerDensity>\
        <LightingZonePowerDensityUnit>W&#47;m^2</LightingZonePowerDensityUnit>\
      </LightingSystemByZone>\
      <LightingSystemByZone>\
        <LightingZoneName>1Z-03</LightingZoneName>\
        <LightingZoneFloorName>1</LightingZoneFloorName>\
        <LightingZoneArea>256</LightingZoneArea>\
        <LightingZoneAreaUnit>m^2</LightingZoneAreaUnit>\
        <LightingZonePower>84.00</LightingZonePower>\
        <LightingZonePowerUnit>Watts/Unit</LightingZonePowerUnit>\
        <LightingZoneQuantity>38</LightingZoneQuantity>\
        <LightingZoneQuantityDayLighted>0</LightingZoneQuantityDayLighted>\
        <LightingZoneTotalPower>3192.00</LightingZoneTotalPower>\
        <LightingZoneTotalPowerUnit>Watt</LightingZoneTotalPowerUnit>\
        <LightingZonePowerDensity>12.469</LightingZonePowerDensity>\
        <LightingZonePowerDensityUnit>W&#47;m^2</LightingZonePowerDensityUnit>\
      </LightingSystemByZone>\
      <LightingSystemByZone>\
        <LightingZoneName>2Z-01</LightingZoneName>\
        <LightingZoneFloorName>2</LightingZoneFloorName>\
        <LightingZoneArea>2048</LightingZoneArea>\
        <LightingZoneAreaUnit>m^2</LightingZoneAreaUnit>\
        <LightingZonePower>96.00</LightingZonePower>\
        <LightingZonePowerUnit>Watts/Unit</LightingZonePowerUnit>\
        <LightingZoneQuantity>354</LightingZoneQuantity>\
        <LightingZoneQuantityDayLighted>0</LightingZoneQuantityDayLighted>\
        <LightingZoneTotalPower>33984.00</LightingZoneTotalPower>\
        <LightingZoneTotalPowerUnit>Watt</LightingZoneTotalPowerUnit>\
        <LightingZonePowerDensity>16.594</LightingZonePowerDensity>\
        <LightingZonePowerDensityUnit>W&#47;m^2</LightingZonePowerDensityUnit>\
      </LightingSystemByZone>\
      <LightingSystemByZone>\
        <LightingZoneName>2Z-02</LightingZoneName>\
        <LightingZoneFloorName>2</LightingZoneFloorName>\
        <LightingZoneArea>640</LightingZoneArea>\
        <LightingZoneAreaUnit>m^2</LightingZoneAreaUnit>\
        <LightingZonePower>96.00</LightingZonePower>\
        <LightingZonePowerUnit>Watts/Unit</LightingZonePowerUnit>\
        <LightingZoneQuantity>42</LightingZoneQuantity>\
        <LightingZoneQuantityDayLighted>0</LightingZoneQuantityDayLighted>\
        <LightingZoneTotalPower>4032.00</LightingZoneTotalPower>\
        <LightingZoneTotalPowerUnit>Watt</LightingZoneTotalPowerUnit>\
        <LightingZonePowerDensity>6.3</LightingZonePowerDensity>\
        <LightingZonePowerDensityUnit>W&#47;m^2</LightingZonePowerDensityUnit>\
      </LightingSystemByZone>\
      <LightingSystemByZone>\
        <LightingZoneName>3Z-01</LightingZoneName>\
        <LightingZoneFloorName>3</LightingZoneFloorName>\
        <LightingZoneArea>2048</LightingZoneArea>\
        <LightingZoneAreaUnit>m^2</LightingZoneAreaUnit>\
        <LightingZonePower>96.00</LightingZonePower>\
        <LightingZonePowerUnit>Watts/Unit</LightingZonePowerUnit>\
        <LightingZoneQuantity>354</LightingZoneQuantity>\
        <LightingZoneQuantityDayLighted>0</LightingZoneQuantityDayLighted>\
        <LightingZoneTotalPower>33984.00</LightingZoneTotalPower>\
        <LightingZoneTotalPowerUnit>Watt</LightingZoneTotalPowerUnit>\
        <LightingZonePowerDensity>16.594</LightingZonePowerDensity>\
        <LightingZonePowerDensityUnit>W&#47;m^2</LightingZonePowerDensityUnit>\
      </LightingSystemByZone>\
      <LightingSystemByZone>\
        <LightingZoneName>3Z-02</LightingZoneName>\
        <LightingZoneFloorName>3</LightingZoneFloorName>\
        <LightingZoneArea>640</LightingZoneArea>\
        <LightingZoneAreaUnit>m^2</LightingZoneAreaUnit>\
        <LightingZonePower>96.00</LightingZonePower>\
        <LightingZonePowerUnit>Watts/Unit</LightingZonePowerUnit>\
        <LightingZoneQuantity>42</LightingZoneQuantity>\
        <LightingZoneQuantityDayLighted>0</LightingZoneQuantityDayLighted>\
        <LightingZoneTotalPower>4032.00</LightingZoneTotalPower>\
        <LightingZoneTotalPowerUnit>Watt</LightingZoneTotalPowerUnit>\
        <LightingZonePowerDensity>6.3</LightingZonePowerDensity>\
        <LightingZonePowerDensityUnit>W&#47;m^2</LightingZonePowerDensityUnit>\
      </LightingSystemByZone>\
    </LightingSystemZone>\
  </LightingSystem>\
  <DXAirUnitSystem>\
    <DXAirUnit>\
      <DXACCode>PAU08.5</DXACCode>\
      <DXACType>Package Air Cooled</DXACType>\
      <DXCoolingCapacity>8.50</DXCoolingCapacity>\
      <DXCoolingCapacityUnit>TR</DXCoolingCapacityUnit>\
      <DXPowerConsumption>14.00</DXPowerConsumption>\
      <DXPowerConsumptionUnit>kW</DXPowerConsumptionUnit>\
      <DXACCOP>2.135</DXACCOP>\
      <DXACComplianceCOP>0</DXACComplianceCOP>\
      <DXACStatus>n/a</DXACStatus>\
    </DXAirUnit>\
    <DXAirUnit>\
      <DXACCode>PAU10.0</DXACCode>\
      <DXACType>Package Air Cooled</DXACType>\
      <DXCoolingCapacity>10.00</DXCoolingCapacity>\
      <DXCoolingCapacityUnit>TR</DXCoolingCapacityUnit>\
      <DXPowerConsumption>15.50</DXPowerConsumption>\
      <DXPowerConsumptionUnit>kW</DXPowerConsumptionUnit>\
      <DXACCOP>2.269</DXACCOP>\
      <DXACComplianceCOP>0</DXACComplianceCOP>\
      <DXACStatus>n/a</DXACStatus>\
    </DXAirUnit>\
    <DXAirUnit>\
      <DXACCode>PAU14.0</DXACCode>\
      <DXACType>Package Air Cooled</DXACType>\
      <DXCoolingCapacity>14.00</DXCoolingCapacity>\
      <DXCoolingCapacityUnit>TR</DXCoolingCapacityUnit>\
      <DXPowerConsumption>18.90</DXPowerConsumption>\
      <DXPowerConsumptionUnit>kW</DXPowerConsumptionUnit>\
      <DXACCOP>2.605</DXACCOP>\
      <DXACComplianceCOP>0</DXACComplianceCOP>\
      <DXACStatus>n/a</DXACStatus>\
    </DXAirUnit>\
  </DXAirUnitSystem>\
  <CentralACSystem>\
    <WaterChillerReport>\
      <CentralACSystemName>Central 1</CentralACSystemName>\
      <CentralACChillerName>CH120.0</CentralACChillerName>\
      <CentralACEquipmentType>Air Cooled Water Chiller</CentralACEquipmentType>\
      <CentralACQuantity>1</CentralACQuantity>\
      <CentralACChillerType>All</CentralACChillerType>\
      <CentralACCoolingCapacity>120</CentralACCoolingCapacity>\
      <CentralACCoolingCapacityUnit>TR</CentralACCoolingCapacityUnit>\
      <CentralACPowerConsumption>132</CentralACPowerConsumption>\
      <CentralACPowerConsumptionUnit>kW</CentralACPowerConsumptionUnit>\
      <CentralACPerformanceCOP>3.197</CentralACPerformanceCOP>\
      <CentralACComplianceCOP>2.64</CentralACComplianceCOP>\
      <CentralACStatus>Passed</CentralACStatus>\
    </WaterChillerReport>\
    <CentralACOtherEQReport>\
      <CentralACSystemName>Central 1</CentralACSystemName>\
      <CentralOtherEQChillerCapacity>422.004</CentralOtherEQChillerCapacity>\
      <CentralOtherEQChillerCapacityUnit>kWth</CentralOtherEQChillerCapacityUnit>\
      <CentralOtherEQTotalPower>28.5</CentralOtherEQTotalPower>\
      <CentralOtherEQTotalPowerUnit>kW</CentralOtherEQTotalPowerUnit>\
      <CentralOtherEQPerformanceCOP>14.807</CentralOtherEQPerformanceCOP>\
      <CentralOtherEQComplianceCOP>7.03</CentralOtherEQComplianceCOP>\
      <CentralOtherEQStatus>Passed</CentralOtherEQStatus>\
    </CentralACOtherEQReport>\
    <CentralACEQList>\
      <CentralACSystemName>Central 1</CentralACSystemName>\
      <CentralEQListEQName>CH120.0</CentralEQListEQName>\
      <CentralEQListEQType>Air Cooled Water Chiller</CentralEQListEQType>\
      <CentralEQListQuantity>1</CentralEQListQuantity>\
      <CentralEQListCoolingCapacity>120</CentralEQListCoolingCapacity>\
      <CentralEQListCoolingCapacityUnit>TR</CentralEQListCoolingCapacityUnit>\
      <CentralEQListPowerRatingPU>132</CentralEQListPowerRatingPU>\
      <CentralEQListPowerRatingPUUnit>kW</CentralEQListPowerRatingPUUnit>\
      <CentralEQListPerformance>---</CentralEQListPerformance>\
      <CentralEQListAbsorptionCompliance>n/a</CentralEQListAbsorptionCompliance>\
    </CentralACEQList>\
  </CentralACSystem>"
"  <PVSystem>\
    <PVSys>\
      <PVSystemName>PV1</PVSystemName>\
      <PVEfficiency>11</PVEfficiency>\
      <PVEfficiencyUnit>%</PVEfficiencyUnit>\
      <PVModuleArea>20</PVModuleArea>\
      <PVModuleAreaUnit>m^2</PVModuleAreaUnit>\
      <PVAzimuthAngle>0</PVAzimuthAngle>\
      <PVAzimuthAngleUnit>degrees</PVAzimuthAngleUnit>\
      <PVInclinationAngle>15</PVInclinationAngle>\
      <PVInclinationAngleUnit>degrees</PVInclinationAngleUnit>\
      <PVESR>441.623</PVESR>\
      <PVTotalEnergy>3191.609</PVTotalEnergy>\
      <PVTotalEnergyUnit>kWhr/year</PVTotalEnergyUnit>\
    </PVSys>\
    <PVSys>\
      <PVSystemName>PV2</PVSystemName>\
      <PVEfficiency>11</PVEfficiency>\
      <PVEfficiencyUnit>%</PVEfficiencyUnit>\
      <PVModuleArea>20</PVModuleArea>\
      <PVModuleAreaUnit>m^2</PVModuleAreaUnit>\
      <PVAzimuthAngle>45</PVAzimuthAngle>\
      <PVAzimuthAngleUnit>degrees</PVAzimuthAngleUnit>\
      <PVInclinationAngle>15</PVInclinationAngle>\
      <PVInclinationAngleUnit>degrees</PVInclinationAngleUnit>\
      <PVESR>438.903</PVESR>\
      <PVTotalEnergy>1585.976</PVTotalEnergy>\
      <PVTotalEnergyUnit>kWhr/year</PVTotalEnergyUnit>\
    </PVSys>\
  </PVSystem>\
  <HWSystem>\
    <HWSys>\
      <HWSystemName>Hotwater1</HWSystemName>\
      <HWBoilerType>Oil Fired Steam Boiler</HWBoilerType>\
      <HWBoilerEfficiency>87</HWBoilerEfficiency>\
      <HWBoilerEfficiencyUnit>%</HWBoilerEfficiencyUnit>\
      <HWHeatPumpType>None</HWHeatPumpType>\
      <HWHeatPumpEfficiencyCOP>---</HWHeatPumpEfficiencyCOP>\
      <HWBoilerCompliance>Passed</HWBoilerCompliance>\
      <HWHeatPumpCompliance>---</HWHeatPumpCompliance>\
    </HWSys>\
  </HWSystem>\
  <WholeBuildingEnergy>\
    <WholeBuildingEnergyReport>\
      <WholeBuildingEnergyConsumption>2032584.01</WholeBuildingEnergyConsumption>\
      <WholeBuildingEnergyConsumptionUnit>kWh/Year</WholeBuildingEnergyConsumptionUnit>\
      <WholeEnergyPVSystem>4777.50</WholeEnergyPVSystem>\
      <WholeEnergyPVSystemUnit>kWh/Year</WholeEnergyPVSystemUnit>\
      <WholeNetEnergyConsumption>2027806.42</WholeNetEnergyConsumption>\
      <WholeNetEnergyConsumptionUnit>kWh/Year</WholeNetEnergyConsumptionUnit>\
      <WholeNetEnergyConsumptionRef>2282616.37</WholeNetEnergyConsumptionRef>\
      <WholeNetEnergyConsumptionRefUnit>kWh/Year</WholeNetEnergyConsumptionRefUnit>\
      <WholeBuildingEnergyCodeCompliance>Passed</WholeBuildingEnergyCodeCompliance>\
    </WholeBuildingEnergyReport>\
    <WholeBuildingEnergyReportFloor>\
      <WholeBuildingEnergyReportByFloor>\
        <WholeFloorName>1</WholeFloorName>\
        <WholeFloorArea>2688.00</WholeFloorArea>\
        <WholeFloorAreaUnit>m^2</WholeFloorAreaUnit>\
        <WholeFloorWallArea>616.00</WholeFloorWallArea>\
        <WholeFloorWallAreaUnit>m^2</WholeFloorWallAreaUnit>\
        <WholeFloorRoofArea>0.00</WholeFloorRoofArea>\
        <WholeFloorRoofAreaUnit>m^2</WholeFloorRoofAreaUnit>\
        <WholeFloorOTTV>66.85</WholeFloorOTTV>\
        <WholeFloorOTTVUnit>W&#47;m^2</WholeFloorOTTVUnit>\
        <WholeFloorRTTV>0.00</WholeFloorRTTV>\
        <WholeFloorRTTVUnit>W&#47;m^2</WholeFloorRTTVUnit>\
        <WholeFloorCOP>2.52</WholeFloorCOP>\
        <WholeFloorLPD>13.76</WholeFloorLPD>\
        <WholeFloorLPDUnit>W&#47;m^2</WholeFloorLPDUnit>\
        <WholeFloorEPD>20.00</WholeFloorEPD>\
        <WholeFloorEPDUnit>W&#47;m^2</WholeFloorEPDUnit>\
        <WholeFloorOCCU>0.10</WholeFloorOCCU>\
        <WholeFloorOCCUUnit>Head&#47;m^2</WholeFloorOCCUUnit>\
        <WholeFloorVENT>0.25</WholeFloorVENT>\
        <WholeFloorVENTUnit>l/s&#47;m^2</WholeFloorVENTUnit>\
        <WholeFloorTotalEnergy>677455.47</WholeFloorTotalEnergy>\
        <WholeFloorTotalEnergyUnit>kWhr/Year</WholeFloorTotalEnergyUnit>\
      </WholeBuildingEnergyReportByFloor>\
      <WholeBuildingEnergyReportByFloor>\
        <WholeFloorName>2</WholeFloorName>\
        <WholeFloorArea>2688.00</WholeFloorArea>\
        <WholeFloorAreaUnit>m^2</WholeFloorAreaUnit>\
        <WholeFloorWallArea>728.00</WholeFloorWallArea>\
        <WholeFloorWallAreaUnit>m^2</WholeFloorWallAreaUnit>\
        <WholeFloorRoofArea>0.00</WholeFloorRoofArea>\
        <WholeFloorRoofAreaUnit>m^2</WholeFloorRoofAreaUnit>\
        <WholeFloorOTTV>28.81</WholeFloorOTTV>\
        <WholeFloorOTTVUnit>W&#47;m^2</WholeFloorOTTVUnit>\
        <WholeFloorRTTV>0.00</WholeFloorRTTV>\
        <WholeFloorRTTVUnit>W&#47;m^2</WholeFloorRTTVUnit>\
        <WholeFloorCOP>2.52</WholeFloorCOP>\
        <WholeFloorLPD>14.14</WholeFloorLPD>\
        <WholeFloorLPDUnit>W&#47;m^2</WholeFloorLPDUnit>\
        <WholeFloorEPD>20.00</WholeFloorEPD>\
        <WholeFloorEPDUnit>W&#47;m^2</WholeFloorEPDUnit>\
        <WholeFloorOCCU>0.10</WholeFloorOCCU>\
        <WholeFloorOCCUUnit>Head&#47;m^2</WholeFloorOCCUUnit>\
        <WholeFloorVENT>0.25</WholeFloorVENT>\
        <WholeFloorVENTUnit>l/s&#47;m^2</WholeFloorVENTUnit>\
        <WholeFloorTotalEnergy>666111.06</WholeFloorTotalEnergy>\
        <WholeFloorTotalEnergyUnit>kWhr/Year</WholeFloorTotalEnergyUnit>\
      </WholeBuildingEnergyReportByFloor>\
      <WholeBuildingEnergyReportByFloor>\
        <WholeFloorName>3</WholeFloorName>\
        <WholeFloorArea>2688.00</WholeFloorArea>\
        <WholeFloorAreaUnit>m^2</WholeFloorAreaUnit>\
        <WholeFloorWallArea>728.00</WholeFloorWallArea>\
        <WholeFloorWallAreaUnit>m^2</WholeFloorWallAreaUnit>\
        <WholeFloorRoofArea>1344</WholeFloorRoofArea>\
        <WholeFloorRoofAreaUnit>m^2</WholeFloorRoofAreaUnit>\
        <WholeFloorOTTV>28.81</WholeFloorOTTV>\
        <WholeFloorOTTVUnit>W&#47;m^2</WholeFloorOTTVUnit>\
        <WholeFloorRTTV>9.16</WholeFloorRTTV>\
        <WholeFloorRTTVUnit>W&#47;m^2</WholeFloorRTTVUnit>\
        <WholeFloorCOP>2.52</WholeFloorCOP>\
        <WholeFloorLPD>14.14</WholeFloorLPD>\
        <WholeFloorLPDUnit>W&#47;m^2</WholeFloorLPDUnit>\
        <WholeFloorEPD>20.00</WholeFloorEPD>\
        <WholeFloorEPDUnit>W&#47;m^2</WholeFloorEPDUnit>\
        <WholeFloorOCCU>0.10</WholeFloorOCCU>\
        <WholeFloorOCCUUnit>Head&#47;m^2</WholeFloorOCCUUnit>\
        <WholeFloorVENT>0.25</WholeFloorVENT>\
        <WholeFloorVENTUnit>l/s&#47;m^2</WholeFloorVENTUnit>\
        <WholeFloorTotalEnergy>689017.48</WholeFloorTotalEnergy>\
        <WholeFloorTotalEnergyUnit>kWhr/Year</WholeFloorTotalEnergyUnit>\
      </WholeBuildingEnergyReportByFloor>\
    </WholeBuildingEnergyReportFloor>\
    <WholeBuildingEnergyReportZone>\
      <WholeBuildingEnergyReportByZone>\
        <WholeZoneName>1Z-01</WholeZoneName>\
        <WholeZoneFloorName>1</WholeZoneFloorName>\
        <WholeZoneArea>1792.00</WholeZoneArea>\
        <WholeZoneAreaUnit>m^2</WholeZoneAreaUnit>\
        <WholeZoneWallArea>616.00</WholeZoneWallArea>\
        <WholeZoneWallAreaUnit>m^2</WholeZoneWallAreaUnit>\
        <WholeZoneRoofArea>0.00</WholeZoneRoofArea>\
        <WholeZoneRoofAreaUnit>m^2</WholeZoneRoofAreaUnit>\
        <WholeZoneOTTV>66.85</WholeZoneOTTV>\
        <WholeZoneOTTVUnit>W&#47;m^2</WholeZoneOTTVUnit>\
        <WholeZoneRTTV>0.00</WholeZoneRTTV>\
        <WholeZoneRTTVUnit>W&#47;m^2</WholeZoneRTTVUnit>\
        <WholeZoneCOP>2.34</WholeZoneCOP>\
        <WholeZoneLPD>16.61</WholeZoneLPD>\
        <WholeZoneLPDUnit>W&#47;m^2</WholeZoneLPDUnit>\
        <WholeZoneEPD>20</WholeZoneEPD>\
        <WholeZoneEPDUnit>W&#47;m^2</WholeZoneEPDUnit>\
        <WholeZoneOCCU>0.10</WholeZoneOCCU>\
        <WholeZoneOCCUUnit>Head&#47;m^2</WholeZoneOCCUUnit>\
        <WholeZoneVENT>0.25</WholeZoneVENT>\
        <WholeZoneVENTUnit>l/s&#47;m^2</WholeZoneVENTUnit>\
        <WholeZoneHeatGainWall>17582.04</WholeZoneHeatGainWall>\
        <WholeZoneHeatGainWallUnit>W</WholeZoneHeatGainWallUnit>\
        <WholeZoneHeatGainRoof>0.00</WholeZoneHeatGainRoof>\
        <WholeZoneHeatGainRoofUnit>W</WholeZoneHeatGainRoofUnit>\
        <WholeZoneInteriorHeat>36765.75</WholeZoneInteriorHeat>\
        <WholeZoneInteriorHeatUnit>W</WholeZoneInteriorHeatUnit>\
        <WholeZoneACEnergyConsumption>238043.31</WholeZoneACEnergyConsumption>\
        <WholeZoneACEnergyConsumptionUnit>kWhr/Year</WholeZoneACEnergyConsumptionUnit>\
        <WholeZoneLightingEnergyConsumption>130347.68</WholeZoneLightingEnergyConsumption>\
        <WholeZoneLightingEnergyConsumptionUnit>kWhr/Year</WholeZoneLightingEnergyConsumptionUnit>\
        <WholeZoneEQEnergyConsumption>156979.20</WholeZoneEQEnergyConsumption>\
        <WholeZoneEQEnergyConsumptionUnit>kWhr/Year</WholeZoneEQEnergyConsumptionUnit>\
        <WholeZoneTotalEnergy>525370.19</WholeZoneTotalEnergy>\
        <WholeZoneTotalEnergyUnit>kWhr/Year</WholeZoneTotalEnergyUnit>\
      </WholeBuildingEnergyReportByZone>\
      <WholeBuildingEnergyReportByZone>\
        <WholeZoneName>1Z-02</WholeZoneName>\
        <WholeZoneFloorName>1</WholeZoneFloorName>\
        <WholeZoneArea>640</WholeZoneArea>\
        <WholeZoneAreaUnit>m^2</WholeZoneAreaUnit>\
        <WholeZoneWallArea>0.00</WholeZoneWallArea>\
        <WholeZoneWallAreaUnit>m^2</WholeZoneWallAreaUnit>\
        <WholeZoneRoofArea>0.00</WholeZoneRoofArea>\
        <WholeZoneRoofAreaUnit>m^2</WholeZoneRoofAreaUnit>\
        <WholeZoneOTTV>0.00</WholeZoneOTTV>\
        <WholeZoneOTTVUnit>W&#47;m^2</WholeZoneOTTVUnit>\
        <WholeZoneRTTV>0.00</WholeZoneRTTV>\
        <WholeZoneRTTVUnit>W&#47;m^2</WholeZoneRTTVUnit>\
        <WholeZoneCOP>2.63</WholeZoneCOP>\
        <WholeZoneLPD>6.30</WholeZoneLPD>\
        <WholeZoneLPDUnit>W&#47;m^2</WholeZoneLPDUnit>\
        <WholeZoneEPD>20</WholeZoneEPD>\
        <WholeZoneEPDUnit>W&#47;m^2</WholeZoneEPDUnit>\
        <WholeZoneOCCU>0.10</WholeZoneOCCU>\
        <WholeZoneOCCUUnit>Head&#47;m^2</WholeZoneOCCUUnit>\
        <WholeZoneVENT>0.25</WholeZoneVENT>\
        <WholeZoneVENTUnit>l/s&#47;m^2</WholeZoneVENTUnit>\
        <WholeZoneHeatGainWall>0.00</WholeZoneHeatGainWall>\
        <WholeZoneHeatGainWallUnit>W</WholeZoneHeatGainWallUnit>\
        <WholeZoneHeatGainRoof>0.00</WholeZoneHeatGainRoof>\
        <WholeZoneHeatGainRoofUnit>W</WholeZoneHeatGainRoofUnit>\
        <WholeZoneInteriorHeat>9578.60</WholeZoneInteriorHeat>\
        <WholeZoneInteriorHeatUnit>W</WholeZoneInteriorHeatUnit>\
        <WholeZoneACEnergyConsumption>41954.28</WholeZoneACEnergyConsumption>\
        <WholeZoneACEnergyConsumptionUnit>kWhr/Year</WholeZoneACEnergyConsumptionUnit>\
        <WholeZoneLightingEnergyConsumption>17660.16</WholeZoneLightingEnergyConsumption>\
        <WholeZoneLightingEnergyConsumptionUnit>kWhr/Year</WholeZoneLightingEnergyConsumptionUnit>\
        <WholeZoneEQEnergyConsumption>56064.00</WholeZoneEQEnergyConsumption>\
        <WholeZoneEQEnergyConsumptionUnit>kWhr/Year</WholeZoneEQEnergyConsumptionUnit>\
        <WholeZoneTotalEnergy>115678.44</WholeZoneTotalEnergy>\
        <WholeZoneTotalEnergyUnit>kWhr/Year</WholeZoneTotalEnergyUnit>\
      </WholeBuildingEnergyReportByZone>\
      <WholeBuildingEnergyReportByZone>\
        <WholeZoneName>1Z-03</WholeZoneName>\
        <WholeZoneFloorName>1</WholeZoneFloorName>\
        <WholeZoneArea>256</WholeZoneArea>\
        <WholeZoneAreaUnit>m^2</WholeZoneAreaUnit>\
        <WholeZoneWallArea>0.00</WholeZoneWallArea>\
        <WholeZoneWallAreaUnit>m^2</WholeZoneWallAreaUnit>\
        <WholeZoneRoofArea>0.00</WholeZoneRoofArea>\
        <WholeZoneRoofAreaUnit>m^2</WholeZoneRoofAreaUnit>\
        <WholeZoneOTTV>0.00</WholeZoneOTTV>\
        <WholeZoneOTTVUnit>W&#47;m^2</WholeZoneOTTVUnit>\
        <WholeZoneRTTV>0.00</WholeZoneRTTV>\
        <WholeZoneRTTVUnit>W&#47;m^2</WholeZoneRTTVUnit>\
        <WholeZoneCOP>0.00</WholeZoneCOP>\
        <WholeZoneLPD>12.47</WholeZoneLPD>\
        <WholeZoneLPDUnit>W&#47;m^2</WholeZoneLPDUnit>\
        <WholeZoneEPD>20</WholeZoneEPD>\
        <WholeZoneEPDUnit>W&#47;m^2</WholeZoneEPDUnit>\
        <WholeZoneOCCU>0.10</WholeZoneOCCU>\
        <WholeZoneOCCUUnit>Head&#47;m^2</WholeZoneOCCUUnit>\
        <WholeZoneVENT>0.25</WholeZoneVENT>\
        <WholeZoneVENTUnit>l/s&#47;m^2</WholeZoneVENTUnit>\
        <WholeZoneHeatGainWall>0.00</WholeZoneHeatGainWall>\
        <WholeZoneHeatGainWallUnit>W</WholeZoneHeatGainWallUnit>\
        <WholeZoneHeatGainRoof>0.00</WholeZoneHeatGainRoof>\
        <WholeZoneHeatGainRoofUnit>W</WholeZoneHeatGainRoofUnit>\
        <WholeZoneInteriorHeat>0.00</WholeZoneInteriorHeat>\
        <WholeZoneInteriorHeatUnit>W</WholeZoneInteriorHeatUnit>\
        <WholeZoneACEnergyConsumption>0.00</WholeZoneACEnergyConsumption>\
        <WholeZoneACEnergyConsumptionUnit>kWhr/Year</WholeZoneACEnergyConsumptionUnit>\
        <WholeZoneLightingEnergyConsumption>13981.24</WholeZoneLightingEnergyConsumption>\
        <WholeZoneLightingEnergyConsumptionUnit>kWhr/Year</WholeZoneLightingEnergyConsumptionUnit>\
        <WholeZoneEQEnergyConsumption>22425.60</WholeZoneEQEnergyConsumption>\
        <WholeZoneEQEnergyConsumptionUnit>kWhr/Year</WholeZoneEQEnergyConsumptionUnit>\
        <WholeZoneTotalEnergy>36406.84</WholeZoneTotalEnergy>\
        <WholeZoneTotalEnergyUnit>kWhr/Year</WholeZoneTotalEnergyUnit>\
      </WholeBuildingEnergyReportByZone>\
      <WholeBuildingEnergyReportByZone>\
        <WholeZoneName>2Z-01</WholeZoneName>\
        <WholeZoneFloorName>2</WholeZoneFloorName>\
        <WholeZoneArea>2048.00</WholeZoneArea>\
        <WholeZoneAreaUnit>m^2</WholeZoneAreaUnit>\
        <WholeZoneWallArea>728.00</WholeZoneWallArea>\
        <WholeZoneWallAreaUnit>m^2</WholeZoneWallAreaUnit>\
        <WholeZoneRoofArea>0.00</WholeZoneRoofArea>\
        <WholeZoneRoofAreaUnit>m^2</WholeZoneRoofAreaUnit>\
        <WholeZoneOTTV>28.81</WholeZoneOTTV>\
        <WholeZoneOTTVUnit>W&#47;m^2</WholeZoneOTTVUnit>\
        <WholeZoneRTTV>0.00</WholeZoneRTTV>\
        <WholeZoneRTTVUnit>W&#47;m^2</WholeZoneRTTVUnit>\
        <WholeZoneCOP>2.35</WholeZoneCOP>\
        <WholeZoneLPD>16.59</WholeZoneLPD>\
        <WholeZoneLPDUnit>W&#47;m^2</WholeZoneLPDUnit>\
        <WholeZoneEPD>20</WholeZoneEPD>\
        <WholeZoneEPDUnit>W&#47;m^2</WholeZoneEPDUnit>\
        <WholeZoneOCCU>0.10</WholeZoneOCCU>\
        <WholeZoneOCCUUnit>Head&#47;m^2</WholeZoneOCCUUnit>\
        <WholeZoneVENT>0.25</WholeZoneVENT>\
        <WholeZoneVENTUnit>l/s&#47;m^2</WholeZoneVENTUnit>\
        <WholeZoneHeatGainWall>8912.97</WholeZoneHeatGainWall>\
        <WholeZoneHeatGainWallUnit>W</WholeZoneHeatGainWallUnit>\
        <WholeZoneHeatGainRoof>0.00</WholeZoneHeatGainRoof>\
        <WholeZoneHeatGainRoofUnit>W</WholeZoneHeatGainRoofUnit>\
        <WholeZoneInteriorHeat>41812.07</WholeZoneInteriorHeat>\
        <WholeZoneInteriorHeatUnit>W</WholeZoneInteriorHeatUnit>\
        <WholeZoneACEnergyConsumption>222175.66</WholeZoneACEnergyConsumption>\
        <WholeZoneACEnergyConsumptionUnit>kWhr/Year</WholeZoneACEnergyConsumptionUnit>\
        <WholeZoneLightingEnergyConsumption>148852.16</WholeZoneLightingEnergyConsumption>\
        <WholeZoneLightingEnergyConsumptionUnit>kWhr/Year</WholeZoneLightingEnergyConsumptionUnit>\
        <WholeZoneEQEnergyConsumption>179404.80</WholeZoneEQEnergyConsumption>\
        <WholeZoneEQEnergyConsumptionUnit>kWhr/Year</WholeZoneEQEnergyConsumptionUnit>\
        <WholeZoneTotalEnergy>550432.63</WholeZoneTotalEnergy>\
        <WholeZoneTotalEnergyUnit>kWhr/Year</WholeZoneTotalEnergyUnit>\
      </WholeBuildingEnergyReportByZone>"
"     <WholeBuildingEnergyReportByZone>\
        <WholeZoneName>2Z-02</WholeZoneName>\
        <WholeZoneFloorName>2</WholeZoneFloorName>\
        <WholeZoneArea>640</WholeZoneArea>\
        <WholeZoneAreaUnit>m^2</WholeZoneAreaUnit>\
        <WholeZoneWallArea>0.00</WholeZoneWallArea>\
        <WholeZoneWallAreaUnit>m^2</WholeZoneWallAreaUnit>\
        <WholeZoneRoofArea>0.00</WholeZoneRoofArea>\
        <WholeZoneRoofAreaUnit>m^2</WholeZoneRoofAreaUnit>\
        <WholeZoneOTTV>0.00</WholeZoneOTTV>\
        <WholeZoneOTTVUnit>W&#47;m^2</WholeZoneOTTVUnit>\
        <WholeZoneRTTV>0.00</WholeZoneRTTV>\
        <WholeZoneRTTVUnit>W&#47;m^2</WholeZoneRTTVUnit>\
        <WholeZoneCOP>2.63</WholeZoneCOP>\
        <WholeZoneLPD>6.30</WholeZoneLPD>\
        <WholeZoneLPDUnit>W&#47;m^2</WholeZoneLPDUnit>\
        <WholeZoneEPD>20</WholeZoneEPD>\
        <WholeZoneEPDUnit>W&#47;m^2</WholeZoneEPDUnit>\
        <WholeZoneOCCU>0.10</WholeZoneOCCU>\
        <WholeZoneOCCUUnit>Head&#47;m^2</WholeZoneOCCUUnit>\
        <WholeZoneVENT>0.25</WholeZoneVENT>\
        <WholeZoneVENTUnit>l/s&#47;m^2</WholeZoneVENTUnit>\
        <WholeZoneHeatGainWall>0.00</WholeZoneHeatGainWall>\
        <WholeZoneHeatGainWallUnit>W</WholeZoneHeatGainWallUnit>\
        <WholeZoneHeatGainRoof>0.00</WholeZoneHeatGainRoof>\
        <WholeZoneHeatGainRoofUnit>W</WholeZoneHeatGainRoofUnit>\
        <WholeZoneInteriorHeat>9578.60</WholeZoneInteriorHeat>\
        <WholeZoneInteriorHeatUnit>W</WholeZoneInteriorHeatUnit>\
        <WholeZoneACEnergyConsumption>41954.28</WholeZoneACEnergyConsumption>\
        <WholeZoneACEnergyConsumptionUnit>kWhr/Year</WholeZoneACEnergyConsumptionUnit>\
        <WholeZoneLightingEnergyConsumption>17660.16</WholeZoneLightingEnergyConsumption>\
        <WholeZoneLightingEnergyConsumptionUnit>kWhr/Year</WholeZoneLightingEnergyConsumptionUnit>\
        <WholeZoneEQEnergyConsumption>56064.00</WholeZoneEQEnergyConsumption>\
        <WholeZoneEQEnergyConsumptionUnit>kWhr/Year</WholeZoneEQEnergyConsumptionUnit>\
        <WholeZoneTotalEnergy>115678.44</WholeZoneTotalEnergy>\
        <WholeZoneTotalEnergyUnit>kWhr/Year</WholeZoneTotalEnergyUnit>\
      </WholeBuildingEnergyReportByZone>\
      <WholeBuildingEnergyReportByZone>\
        <WholeZoneName>3Z-01</WholeZoneName>\
        <WholeZoneFloorName>2</WholeZoneFloorName>\
        <WholeZoneArea>2048.00</WholeZoneArea>\
        <WholeZoneAreaUnit>m^2</WholeZoneAreaUnit>\
        <WholeZoneWallArea>728.00</WholeZoneWallArea>\
        <WholeZoneWallAreaUnit>m^2</WholeZoneWallAreaUnit>\
        <WholeZoneRoofArea>1344.00</WholeZoneRoofArea>\
        <WholeZoneRoofAreaUnit>m^2</WholeZoneRoofAreaUnit>\
        <WholeZoneOTTV>28.81</WholeZoneOTTV>\
        <WholeZoneOTTVUnit>W&#47;m^2</WholeZoneOTTVUnit>\
        <WholeZoneRTTV>9.16</WholeZoneRTTV>\
        <WholeZoneRTTVUnit>W&#47;m^2</WholeZoneRTTVUnit>\
        <WholeZoneCOP>2.35</WholeZoneCOP>\
        <WholeZoneLPD>16.59</WholeZoneLPD>\
        <WholeZoneLPDUnit>W&#47;m^2</WholeZoneLPDUnit>\
        <WholeZoneEPD>20</WholeZoneEPD>\
        <WholeZoneEPDUnit>W&#47;m^2</WholeZoneEPDUnit>\
        <WholeZoneOCCU>0.10</WholeZoneOCCU>\
        <WholeZoneOCCUUnit>Head&#47;m^2</WholeZoneOCCUUnit>\
        <WholeZoneVENT>0.25</WholeZoneVENT>\
        <WholeZoneVENTUnit>l/s&#47;m^2</WholeZoneVENTUnit>\
        <WholeZoneHeatGainWall>8912.97</WholeZoneHeatGainWall>\
        <WholeZoneHeatGainWallUnit>W</WholeZoneHeatGainWallUnit>\
        <WholeZoneHeatGainRoof>5229.78</WholeZoneHeatGainRoof>\
        <WholeZoneHeatGainRoofUnit>W</WholeZoneHeatGainRoofUnit>\
        <WholeZoneInteriorHeat>41812.07</WholeZoneInteriorHeat>\
        <WholeZoneInteriorHeatUnit>W</WholeZoneInteriorHeatUnit>\
        <WholeZoneACEnergyConsumption>245082.08</WholeZoneACEnergyConsumption>\
        <WholeZoneACEnergyConsumptionUnit>kWhr/Year</WholeZoneACEnergyConsumptionUnit>\
        <WholeZoneLightingEnergyConsumption>148852.16</WholeZoneLightingEnergyConsumption>\
        <WholeZoneLightingEnergyConsumptionUnit>kWhr/Year</WholeZoneLightingEnergyConsumptionUnit>\
        <WholeZoneEQEnergyConsumption>179404.80</WholeZoneEQEnergyConsumption>\
        <WholeZoneEQEnergyConsumptionUnit>kWhr/Year</WholeZoneEQEnergyConsumptionUnit>\
        <WholeZoneTotalEnergy>573339.04</WholeZoneTotalEnergy>\
        <WholeZoneTotalEnergyUnit>kWhr/Year</WholeZoneTotalEnergyUnit>\
      </WholeBuildingEnergyReportByZone>\
      <WholeBuildingEnergyReportByZone>\
        <WholeZoneName>3Z-02</WholeZoneName>\
        <WholeZoneFloorName>3</WholeZoneFloorName>\
        <WholeZoneArea>640</WholeZoneArea>\
        <WholeZoneAreaUnit>m^2</WholeZoneAreaUnit>\
        <WholeZoneWallArea>0.00</WholeZoneWallArea>\
        <WholeZoneWallAreaUnit>m^2</WholeZoneWallAreaUnit>\
        <WholeZoneRoofArea>0.00</WholeZoneRoofArea>\
        <WholeZoneRoofAreaUnit>m^2</WholeZoneRoofAreaUnit>\
        <WholeZoneOTTV>0.00</WholeZoneOTTV>\
        <WholeZoneOTTVUnit>W&#47;m^2</WholeZoneOTTVUnit>\
        <WholeZoneRTTV>0.00</WholeZoneRTTV>\
        <WholeZoneRTTVUnit>W&#47;m^2</WholeZoneRTTVUnit>\
        <WholeZoneCOP>2.63</WholeZoneCOP>\
        <WholeZoneLPD>6.30</WholeZoneLPD>\
        <WholeZoneLPDUnit>W&#47;m^2</WholeZoneLPDUnit>\
        <WholeZoneEPD>20</WholeZoneEPD>\
        <WholeZoneEPDUnit>W&#47;m^2</WholeZoneEPDUnit>\
        <WholeZoneOCCU>0.10</WholeZoneOCCU>\
        <WholeZoneOCCUUnit>Head&#47;m^2</WholeZoneOCCUUnit>\
        <WholeZoneVENT>0.25</WholeZoneVENT>\
        <WholeZoneVENTUnit>l/s&#47;m^2</WholeZoneVENTUnit>\
        <WholeZoneHeatGainWall>0.00</WholeZoneHeatGainWall>\
        <WholeZoneHeatGainWallUnit>W</WholeZoneHeatGainWallUnit>\
        <WholeZoneHeatGainRoof>0.00</WholeZoneHeatGainRoof>\
        <WholeZoneHeatGainRoofUnit>W</WholeZoneHeatGainRoofUnit>\
        <WholeZoneInteriorHeat>9578.60</WholeZoneInteriorHeat>\
        <WholeZoneInteriorHeatUnit>W</WholeZoneInteriorHeatUnit>\
        <WholeZoneACEnergyConsumption>41954.28</WholeZoneACEnergyConsumption>\
        <WholeZoneACEnergyConsumptionUnit>kWhr/Year</WholeZoneACEnergyConsumptionUnit>\
        <WholeZoneLightingEnergyConsumption>17660.16</WholeZoneLightingEnergyConsumption>\
        <WholeZoneLightingEnergyConsumptionUnit>kWhr/Year</WholeZoneLightingEnergyConsumptionUnit>\
        <WholeZoneEQEnergyConsumption>56064.00</WholeZoneEQEnergyConsumption>\
        <WholeZoneEQEnergyConsumptionUnit>kWhr/Year</WholeZoneEQEnergyConsumptionUnit>\
        <WholeZoneTotalEnergy>115678.44</WholeZoneTotalEnergy>\
        <WholeZoneTotalEnergyUnit>kWhr/Year</WholeZoneTotalEnergyUnit>\
      </WholeBuildingEnergyReportByZone>\
    </WholeBuildingEnergyReportZone>\
  </WholeBuildingEnergy >\
</BuildingReport>";

void callBEC(const QString &path, QPlainTextEdit * log){

    //Start bec for gen xml.
    //QString program = QApplication::applicationDirPath()+"//FeakBEC.exe";
    //QStringList arguments;
    //arguments << ui->txtParam->text();
    //becProcess = new QProcess(this);
    //becProcess->start(program, arguments);
    //connect(becProcess, SIGNAL(finished(int, QProcess::ExitStatus))
    //        , this, SLOT(becFinished(int, QProcess::ExitStatus)));

    int numFiles = 60000;
    QProgressDialog progress("Generate BEC output....", "Abort", 0, numFiles, 0);
    progress.setWindowModality(Qt::WindowModal);

    QString output = path;

    QFile file(output);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        file.write(bec_xml.toStdString().c_str());
    }

    for (int i = 0; i < numFiles; i++) {
        progress.setValue(i);
        QCoreApplication::processEvents();
        if (progress.wasCanceled())
            break;
    }
    progress.setValue(numFiles);
    QCoreApplication::processEvents();
    log->appendPlainText("call bec...");
}

bool doBecInput(const QString &path, QString& outpath, QString &err){
    QString output = path;
    outpath = output;

    //TODO:Use this code.
    //#include <ForwardTranslator.hpp>
    //bec::ForwardTranslator trans;
    //trans.modelToBEC(m, outDir);
    //translatorErrors = trans.errors();
    //translatorWarnings = trans.warnings();

    QFile file(output);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        file.write(
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
            <root>\
            <title>\
            BEC INPUT FILE\
            </title>\
            </root>");

        file.close();
        return true;
    }
    else{
        err = file.errorString();
    }
    return false;
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
    m_canceling(false)

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

  if(m_radianceErrors.size()>0 && m_becWarnings.size()>0){
      QMessageBox::information(0, "AAAAAAAAAAAAAA", "AAAAAAAAAAAA" );
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

void RunView::playButtonClicked(bool t_checked)
{
    if(m_becButton->isChecked()){

    }

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
          m_playButton->setChecked(false);
      } else {
          m_playButton->setChecked(false);
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

    //TODO IMPLEMENT BEC.
    //if(m_becButton->isChecked() && (!m_becWarnings.empty() || !m_becErrors.empty())) {
    //    showBECWarningsAndErrors(m_becWarnings, m_becErrors);
    //    if(m_becErrors.size()){
    //        return;
    //    }
    //    else{
    //        // check messageBox return value to run with warnings
    //    }
    //}

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

            QString becoutputPath = outpath+"bec.xml";

            doBecInput(outpath+"input.xml", filePath, err);

            if(!err.isEmpty())
                m_outputWindow->appendPlainText(err);

            callBEC(becoutputPath, m_outputWindow);

            if(!doBecReport(becoutputPath, outpath, err)){
                m_outputWindow->appendPlainText("Error BEC Report.");
                m_outputWindow->appendPlainText(err);
            }
            else{
                //QString foutpath = QString("file:///") + outpath + "report.html";
                //QUrl url(foutpath);
                //QDesktopServices::openUrl(url);
                std::shared_ptr<OSDocument> osdocument = OSAppBase::instance()->currentDocument();
                m_outputWindow->appendPlainText("Generate bec complete.");
                m_playButton->setChecked(false);
                osdocument->runComplete();
                osdocument->enableTabsAfterRun();
            }
        }
    }
    else{
        // we are starting the simulations
        std::shared_ptr<OSDocument> osdocument = OSAppBase::instance()->currentDocument();
        bool requireCalibrationReports = (osdocument->model().getConcreteModelObjects<model::UtilityBill>().size() > 0);
        openstudio::runmanager::RunManager rm = runManager();
        startRunManager(rm, m_modelPath, m_tempFolder, m_radianceButton->isChecked(), requireCalibrationReports, this);
    }
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
      emit useRadianceStateChanged(true);
      //TODO:IMPLEMENT Below.
      //updateToolsWarnings();
      //if(m_radianceErrors.size()){
      // showRadianceWarningsAndErrors(m_radianceWarnings,m_radianceErrors);
      //}
  }
  else{
    emit useRadianceStateChanged(false);
  }
}

} // openstudio
