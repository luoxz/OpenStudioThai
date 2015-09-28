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

#include "ForwardTranslator.hpp"

#include "../model/Model.hpp"
#include "../model/ModelObject.hpp"
#include "../model/ModelObject_Impl.hpp"
#include "../model/Material.hpp"
#include "../model/Material_Impl.hpp"
#include "../model/ConstructionBase.hpp"
#include "../model/ConstructionBase_Impl.hpp"
#include "../model/Construction.hpp"
#include "../model/Construction_Impl.hpp"
#include "../model/Facility.hpp"
#include "../model/Facility_Impl.hpp"
#include "../model/Building.hpp"
#include "../model/Building_Impl.hpp"
#include "../model/BuildingStory.hpp"
#include "../model/BuildingStory_Impl.hpp"
#include "../model/ElectricEquipment.hpp"
#include "../model/ElectricEquipmentDefinition.hpp"
#include "../model/ElectricEquipmentDefinition_Impl.hpp"
#include "../model/LightsDefinition.hpp"
#include "../model/LightsDefinition_Impl.hpp"
#include "../model/Lights.hpp"
#include "../model/Lights_Impl.hpp"
#include "../model/OtherEquipment.hpp"
#include "../model/OtherEquipment_Impl.hpp"
#include "../model/OtherEquipmentDefinition.hpp"
#include "../model/OtherEquipmentDefinition_Impl.hpp"
#include "../model/People.hpp"
#include "../model/People_Impl.hpp"
#include "../model/PeopleDefinition.hpp"
#include "../model/PeopleDefinition_Impl.hpp"
#include "../model/GasEquipment.hpp"
#include "../model/GasEquipment_Impl.hpp"
#include "../model/GasEquipmentDefinition.hpp"
#include "../model/GasEquipmentDefinition_Impl.hpp"
#include "../model/Luminaire.hpp"
#include "../model/Luminaire_Impl.hpp"
#include "../model/LuminaireDefinition.hpp"
#include "../model/LuminaireDefinition_Impl.hpp"
#include "../model/Space.hpp"
#include "../model/Space_Impl.hpp"
#include "../model/SpaceType.hpp"
#include "../model/SpaceType_Impl.hpp"
#include "../model/StandardOpaqueMaterial.hpp"
#include "../model/StandardOpaqueMaterial_Impl.hpp"
#include "../model/Surface.hpp"
#include "../model/Surface_Impl.hpp"
#include "../model/SubSurface.hpp"
#include "../model/SubSurface_Impl.hpp"
#include "../model/WaterUseEquipment.hpp"
#include "../model/WaterUseEquipment_Impl.hpp"
#include "../model/WaterUseEquipmentDefinition.hpp"
#include "../model/WaterUseEquipmentDefinition_Impl.hpp"
#include "../model/Surface.hpp"
#include "../model/Surface_Impl.hpp"
#include "../model/RenderingColor.hpp"
#include "../model/RenderingColor_Impl.hpp"
#include "../model/BoilerHotWater.hpp"
#include "../model/BoilerHotWater_Impl.hpp"
#include "../model/SimpleGlazing.hpp"
#include "../model/SimpleGlazing_Impl.hpp"
#include "../model/AirGap.hpp"
#include "../model/AirGap_Impl.hpp"
#include "../model/HotWaterEquipment.hpp"
#include "../model/ThermalZone.hpp"
#include "../model/ThermalZone_Impl.hpp"
#include "../model/Photovoltaic.hpp"
#include "../model/Photovoltaic_Impl.hpp"
#include "../model/PhotovoltaicThermal.hpp"
#include "../model/PhotovoltaicThermal_Impl.hpp"
#include "../model/Splitter.hpp"
#include "../model/Splitter_Impl.hpp"
#include "../model/AirLoopHVAC.hpp"
#include "../model/AirLoopHVAC_Impl.hpp"
#include "../model/CoilHeatingDXSingleSpeed.hpp"
#include "../model/CoilHeatingDXSingleSpeed_Impl.hpp"
#include "../model/CoilHeatingElectric.hpp"
#include "../model/CoilHeatingElectric_Impl.hpp"
#include "../model/FanConstantVolume.hpp"
#include "../model/FanConstantVolume_Impl.hpp"
#include "../model/CoolingTowerSingleSpeed.hpp"
#include "../model/CoolingTowerSingleSpeed_Impl.hpp"
#include "../model/CoolingTowerTwoSpeed.hpp"
#include "../model/CoolingTowerTwoSpeed_Impl.hpp"
#include "../model/CoolingTowerVariableSpeed.hpp"
#include "../model/CoolingTowerVariableSpeed_Impl.hpp"
#include "../model/PumpVariableSpeed.hpp"
#include "../model/PumpVariableSpeed_Impl.hpp"
#include "../model/ChillerElectricEIR.hpp"
#include "../model/ChillerElectricEIR_Impl.hpp"


///////////////////////////////////////////////////////
#include "../model/CoolingTowerVariableSpeed_Impl.hpp"
#include "../model/CoolingTowerVariableSpeed_Impl.hpp"
#include "../model/FanVariableVolume.hpp"
#include "../model/FanVariableVolume_Impl.hpp"
#include "../model/CoilCoolingDXTwoSpeed.hpp"
#include "../model/CoilCoolingDXTwoSpeed_Impl.hpp"
#include "../model/CoilCoolingDXMultiSpeed.hpp"
#include "../model/CoilCoolingDXMultiSpeed_Impl.hpp"
#include "../model/CoilCoolingWater.hpp"
#include "../model/CoilCoolingWater_Impl.hpp"

#include "../model/SubSurface.hpp"
#include "../model/SubSurface_Impl.hpp"
#include "../model/Surface.hpp"
#include "../model/Surface_Impl.hpp"

#include "../model/AirConditionerVariableRefrigerantFlow.hpp"
#include "../model/AirConditionerVariableRefrigerantFlow_Impl.hpp"
#include "../model/CoilCoolingDXSingleSpeed.hpp"
#include "../model/CoilCoolingDXSingleSpeed_Impl.hpp"
#include "../model/CoilCoolingDXVariableRefrigerantFlow.hpp"
#include "../model/CoilCoolingDXVariableRefrigerantFlow_Impl.hpp"
#include "../model/CoilCoolingWaterToAirHeatPumpEquationFit.hpp"
#include "../model/CoilCoolingWaterToAirHeatPumpEquationFit_Impl.hpp"
#include "../model/CoilCoolingDXSingleSpeed.hpp"
#include "../model/CoilCoolingDXSingleSpeed_Impl.hpp"
#include "../model/RefrigerationCase.hpp"
#include "../model/RefrigerationCase_Impl.hpp"

#include "../model/FanConstantVolume.hpp"
#include "../model/FanConstantVolume_Impl.hpp"
#include "../model/AirLoopHVAC.hpp"
#include "../model/PlantLoop.hpp"
#include "../model/AirLoopHVAC_Impl.hpp"
#include "../model/PlantLoop_Impl.hpp"

#include "../utilities/geometry/Transformation.hpp"
#include "../utilities/geometry/EulerAngles.hpp"
#include "../utilities/geometry/BoundingBox.hpp"
#include "../utilities/geometry/Geometry.hpp"
#include "../utilities/plot/ProgressBar.hpp"
#include "../utilities/core/Assert.hpp"

#include <utilities/idd/IddEnums.hxx>

#include <boost/math/constants/constants.hpp>

#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QThread>
#include <QInputDialog>
#include <QDebug>
#include <math.h>

double rtod(double rad){
    return rad*180/3.141592653589793;
}

namespace openstudio {
namespace bec {

ForwardTranslator::ForwardTranslator()
{
    m_logSink.setLogLevel(Warn);
    m_logSink.setChannelRegex(boost::regex("openstudio\\.bec\\.ForwardTranslator"));
    m_logSink.setThreadId(QThread::currentThread());
}

ForwardTranslator::~ForwardTranslator()
{
}

bool ForwardTranslator::modelTobec(const openstudio::model::Model& model
                                   , const openstudio::path& path
                                   , ProgressBar* progressBar
                                   , std::string *bvName)
{
    QInputDialog inputBuildingType;
    inputBuildingType.setOption(QInputDialog::UseListViewForComboBoxItems);
    inputBuildingType.setWindowTitle("What is building type.");
    inputBuildingType.setLabelText("Selection:");
    QStringList types;
        types <<"Office building"
        <<"Department store"
        <<"Community building"
        <<"Hotel"
        <<"Condominium"
        <<"Medical Center"
        <<"Educational Institution"
        <<"Service Facility"
        <<"Theater";

    inputBuildingType.setComboBoxItems(types);

    int ret = inputBuildingType.exec();

    if (ret == QDialog::Accepted){
        (*bvName) = inputBuildingType.textValue().toStdString();
        m_progressBar = progressBar;
        m_logSink.setThreadId(QThread::currentThread());
        m_logSink.resetStringStream();

        boost::optional<QDomDocument> doc = this->translateModel(model, inputBuildingType.textValue());
        if (!doc){
            return false;
        }

        QFile file(toQString(path));
        if (file.open(QFile::WriteOnly)){
            QTextStream textStream(&file);
            textStream.setCodec("UTF-8");
            textStream << doc->toString(2);
            file.close();
            return true;
        }
        return false;
    }
    return false;
}

std::vector<LogMessage> ForwardTranslator::warnings() const
{
    std::vector<LogMessage> result;

    for (LogMessage logMessage : m_logSink.logMessages()){
        if (logMessage.logLevel() == Warn){
            result.push_back(logMessage);
        }
    }

    return result;
}

std::vector<LogMessage> ForwardTranslator::errors() const
{
    std::vector<LogMessage> result;

    for (LogMessage logMessage : m_logSink.logMessages()){
        if (logMessage.logLevel() > Warn){
            result.push_back(logMessage);
        }
    }

    return result;
}

QString ForwardTranslator::escapeName(const std::string& name)
{
    QString result = toQString(name);
    result.replace(" ", "_");
    result.replace("(", "_");
    result.replace(")", "_");
    return result;
}

QDomElement ForwardTranslator::createTagWithText(QDomElement &parent, const QString &tag, const QString &text)
{
    QDomElement elm = _doc->createElement(tag);
    if(!text.isEmpty())
        elm.appendChild(_doc->createTextNode(text.toUtf8()));

    parent.appendChild(elm);
    return elm;
}

void ForwardTranslator::doEnvelope(const model::Model &model, QDomElement &parent)
{
    QDomElement Envelope = createTagWithText(parent, "Envelope");


    doMaterial(model, Envelope);

    //TODO:IMPLEMENT ComponentOfSection
    doComponentLoop(model, Envelope);

    //TODO:IMPLEMENT SectionOfWall and Wall
    doSectionOfWall(model, Envelope);

    //TODO:IMPLEMENT Wall
    //doWall(model, parent);
}

void ForwardTranslator::doMaterial(const model::Model &model, QDomElement &parent)
{
    QDomElement Material = createTagWithText(parent, "Material");
    QDomElement OpaqueMaterial = createTagWithText(Material, "OpaqueMaterial");
    QDomElement TransparentMaterial = createTagWithText(Material, "TransparentMaterial");
    QDomElement AirGapMaterial = createTagWithText(Material, "AirGapMaterial");

    QDomElement UNKNOW_MATERIAL;

    std::vector<model::Material> materials = model.getModelObjects<model::Material>();

    for (const model::Material& material : materials){
        if (material.optionalCast<model::StandardOpaqueMaterial>()){

            QDomElement OpaqueMat = createTagWithText(OpaqueMaterial, "OpaqueMat");
            createTagWithText(OpaqueMat, "OpaqueMaterialName", material.name().get().c_str());
            //!- Conductivity{ W / m - K }
            model::StandardOpaqueMaterial m = material.cast<model::StandardOpaqueMaterial>();
            //WARNING: <OpaqueMaterialType>Wall</OpaqueMaterialType> Not in Openstudio.
            createTagWithText(OpaqueMat, "OpaqueMaterialType", "Wall");
            createTagWithText(OpaqueMat, "OpaqueMaterialThermalConductivity", QString::number(m.conductivity()));
            createTagWithText(OpaqueMat, "OpaqueMaterialThermalConductivityUnit", "W/mK");

            //!- Density{ kg / m3 }
            createTagWithText(OpaqueMat, "OpaqueMaterialDensity", QString::number(m.density()));
            createTagWithText(OpaqueMat, "OpaqueMaterialDensityUnit", "kg/m^3");

            //!- Specific Heat{ J / kg - K }
            createTagWithText(OpaqueMat, "OpaqueMaterialSpecificHeat", QString::number(m.specificHeat()));
            createTagWithText(OpaqueMat, "OpaqueMaterialSpecificHeatUnit", "kJ/kgK");
        }
        else if(material.optionalCast<model::SimpleGlazing>())       {
            model::SimpleGlazing glass = material.cast<model::SimpleGlazing>();
            QDomElement TranMat = createTagWithText(TransparentMaterial, "TranMat");
            createTagWithText(TranMat, "TransparentMaterialName", glass.name().get().c_str());
            double thickness = qMax(0.012, glass.thickness());
            createTagWithText(TranMat, "TransparentMaterialThickness", QString::number(thickness));
            createTagWithText(TranMat, "TransparentMaterialThicknessUnit", "m");
            createTagWithText(TranMat, "TransparentMaterialVisibleRayReflectance", "");//WARNING: NOTING
            createTagWithText(TranMat, "TransparentMaterialVisibleRayTransmittance", QString::number(glass.visibleTransmittance().get()));
            createTagWithText(TranMat, "TransparentMaterialSolarEnergyReflectance", "");//WARNING: NOTING
            createTagWithText(TranMat, "TransparentMaterialSolarEnergyTransmittance", "");//WARNING: NOTING
            createTagWithText(TranMat, "TransparentMaterialSolarEnergyAbsorption", "");//WARNING: NOTING
            createTagWithText(TranMat, "TransparentMaterialUvalue", QString::number(glass.uFactor()));
            createTagWithText(TranMat, "TransparentMaterialUvalueUnit", "w/M^2k");
            createTagWithText(TranMat, "TransparentMaterialSHGC", QString::number(glass.solarHeatGainCoefficient()));//WARNING: NOTING
        }
        else if(material.optionalCast<model::AirGap>()){
            model::AirGap airgap = material.cast<model::AirGap>();
            QDomElement AirGapMat = createTagWithText(AirGapMaterial, "AirGapMat");
            createTagWithText(AirGapMat, "AirGapMaterialName", airgap.name().get().c_str());
            createTagWithText(AirGapMat, "AirGapMaterialType", ""); //WARNING: NOTING
            createTagWithText(AirGapMat, "AirGapMaterialSurfaceType", ""); //WARNING: NOTING
            createTagWithText(AirGapMat, "AirGapMaterialThickness", QString::number(airgap.thickness()));
            createTagWithText(AirGapMat, "AirGapMaterialThicknessUnit", "m");
            createTagWithText(AirGapMat, "AirGapMaterialAirGapResistance", QString::number(airgap.thermalResistance()));
        }
        else{
            if(UNKNOW_MATERIAL.isNull()){
                UNKNOW_MATERIAL = createTagWithText(Material, "UNKNOW_MATERIALS");
            }
            QDomElement UNKNOW = createTagWithText(UNKNOW_MATERIAL, "UNKNOW");
            createTagWithText(UNKNOW, "NAME", material.name().get().c_str());
            createTagWithText(UNKNOW, "TYPE", material.iddObject().name().c_str());
        }
    }
}

void ForwardTranslator::doTransparentMaterial(const model::Model &model, QDomElement &parent)
{
    model::SimpleGlazingVector glazings = model.getModelObjects<model::SimpleGlazing>();
    model::SimpleGlazingVector::iterator it = glazings.begin();

    createTagWithText(parent, "TransparentSize", QString::number(glazings.size()));

    while (it!=glazings.end()) {
        model::SimpleGlazing& glass = (*it);
        QDomElement TranMat = _doc->createElement("TranMat");
        createTagWithText(TranMat, "TransparentMaterialName", glass.name().get().c_str());
        createTagWithText(TranMat, "TransparentMaterialThickness", QString::number(glass.thickness()));
        createTagWithText(TranMat, "TransparentMaterialThicknessUnit", "m");
        createTagWithText(TranMat, "TransparentMaterialVisibleRayReflectance", "1");//WARNING: NOTING
        createTagWithText(TranMat, "TransparentMaterialVisibleRayTransmittance", QString::number(glass.visibleTransmittance().get()));
        createTagWithText(TranMat, "TransparentMaterialSolarEnergyReflectance", "1");//WARNING: NOTING
        createTagWithText(TranMat, "TransparentMaterialSolarEnergyTransmittance", "1");//WARNING: NOTING
        createTagWithText(TranMat, "TransparentMaterialSolarEnergyAbsorption", "1");//WARNING: NOTING
        createTagWithText(TranMat, "TransparentMaterialUvalue", QString::number(glass.uFactor()));
        createTagWithText(TranMat, "TransparentMaterialUvalueUnit", "w/M^2k");
        createTagWithText(TranMat, "TransparentMaterialSHGC", QString::number(glass.solarHeatGainCoefficient()));//WARNING: NOTING
        parent.appendChild(TranMat);
        it++;
    }
}

void ForwardTranslator::doAirGapMaterial(const model::Model &model, QDomElement &parent)
{
    model::AirGapVector airGaps = model.getModelObjects<model::AirGap>();
    model::AirGapVector::iterator it = airGaps.begin();

    createTagWithText(parent, "AirGapMaterialSize", QString::number(airGaps.size()));

    while (it!=airGaps.end()) {
        model::AirGap& airgap = (*it);
        QDomElement TranMat = _doc->createElement("AirGapMat");
        createTagWithText(TranMat, "AirGapMaterialName", airgap.name().get().c_str());
        createTagWithText(TranMat, "AirGapMaterialType", "Roof"); //WARNING: NOTING
        createTagWithText(TranMat, "AirGapMaterialSurfaceType", "????"); //WARNING: NOTING
        createTagWithText(TranMat, "AirGapMaterialThickness", QString::number(airgap.thickness()));//WARNING: NOTING
        createTagWithText(TranMat, "AirGapMaterialThicknessUnit", "m");
        createTagWithText(TranMat, "AirGapMaterialAirGapResistance", QString::number(airgap.thermalResistance()));//WARNING: NOTING
        parent.appendChild(TranMat);
        it++;
    }
}

void ForwardTranslator::doComponentLoop(const model::Model &model, QDomElement &parent)
{
    //Start doComponentOfSection
    QDomElement componentOfSection = createTagWithText(parent, "ComponentOfSection");
    QDomElement OpaqueComponentList = createTagWithText(componentOfSection, "OpaqueComponentList");
    QDomElement OpaqueComponentDetail = createTagWithText(componentOfSection, "OpaqueComponentDetail");
    QDomElement transparentComponentList = createTagWithText(componentOfSection, "TransparentComponentList");
    QHash<QString, int> componentCheck;
    //END doComponentOfSection

    std::vector<openstudio::model::BuildingStory> stories = model.getConcreteModelObjects<openstudio::model::BuildingStory>();

    for (const openstudio::model::BuildingStory& buildingStory : stories) {

        std::vector<openstudio::model::Space> spaces = buildingStory.spaces();
        for (openstudio::model::Space& space : spaces){

            openstudio::model::SurfaceVector surfaces = space.surfaces();
            for (openstudio::model::Surface& surface : surfaces){

                doComponentOfSection(surface
                                     , OpaqueComponentList
                                     , OpaqueComponentDetail
                                     , transparentComponentList
                                     , componentCheck);
            }
        }
    }
}

void ForwardTranslator::doComponentOfSection(openstudio::model::Surface& surface
                                             , QDomElement &OpaqueComponentList
                                             , QDomElement &OpaqueComponentDetail
                                             , QDomElement &transparentComponentList
                                             , QHash<QString, int>& componentCheck)
{
    std::string name = surface.construction().get().name().get();
    std::string stype = surface.surfaceType();
    if(stype == "RoofCeiling"){
        stype = "Roof";
    }
    if(!((name.empty())|| componentCheck.contains(name.c_str()))){
        componentCheck.insert(name.c_str(), 1);
        boost::optional<model::ConstructionBase> scon = surface.construction();
        if (scon){
            boost::optional<model::Construction> construction = scon->optionalCast<model::Construction>();
            if (construction){
                std::vector<model::Material> layers = construction->layers();
                if (!layers.empty()){
                    if (layers[0].optionalCast<model::StandardOpaqueMaterial>()){
                        model::StandardOpaqueMaterial outerMaterial = layers[0].cast<model::StandardOpaqueMaterial>();

                        QDomElement OpaqueDetail = createTagWithText(OpaqueComponentDetail, "OpaqueDetail");
                        createTagWithText(OpaqueDetail, "OpaqueComponentDetailListName", name.c_str());
                        createTagWithText(OpaqueDetail, "OpaqueComponentDetailMaterialName", outerMaterial.name().get().c_str());
                        createTagWithText(OpaqueDetail, "OpaqueComponentDetailThickness", QString::number(outerMaterial.thickness()));
                        //createTagWithText(OpaqueDetail, "IsOpaque", QString(layers[0]->isOpaque()));
                        createTagWithText(OpaqueDetail, "OpaqueComponentDetailThicknessUnit", "m");

                        QDomElement OpaqueList = createTagWithText(OpaqueComponentList, "OpaqueList");
                        createTagWithText(OpaqueList, "OpaqueComponentListName", name.c_str());
                        createTagWithText(OpaqueList, "OpaqueComponentListType", stype.c_str());

                        QString colorName = "White";
                        if(construction->renderingColor()){
                            QColor color;
                            color.setRed(construction->renderingColor()->renderingRedValue());
                            color.setGreen(construction->renderingColor()->renderingGreenValue());
                            color.setBlue(construction->renderingColor()->renderingBlueValue());
                            color.setAlpha(construction->renderingColor()->renderingAlphaValue());
                            colorName = color.name();
                        }
                        createTagWithText(OpaqueList, "OpaqueComponentListOuterSurfaceColor", colorName);
                        createTagWithText(OpaqueList, "OpaqueComponentListInnerSurfaceType", "UNKNOW");
                        createTagWithText(OpaqueList, "OpaqueComponentListDescription", "???");
                    }
                    else if (layers[0].optionalCast<model::SimpleGlazing>()){
                        model::SimpleGlazing glass = layers[0].cast<model::SimpleGlazing>();

                        QDomElement TransparentList = createTagWithText(transparentComponentList, "TransparentList");
                        createTagWithText(TransparentList, "TransparentComponentListName", glass.name().get().c_str());
                        createTagWithText(TransparentList, "TransparentComponentListType", stype.c_str());
                        createTagWithText(TransparentList, "TransparentComponentListSurfaceName", surface.name().get().c_str());
                        createTagWithText(TransparentList, "TransparentComponentListSHGC", QString::number(glass.solarHeatGainCoefficient()));
                        createTagWithText(TransparentList, "TransparentComponentListTransmittance", QString::number(glass.visibleTransmittance().get()));
                        //createTagWithText(TransparentList, "IsOpaque", QString(layers[0]->isOpaque()));
                        createTagWithText(TransparentList, "TransparentComponentListDescription", "???");
                    }
                    else{
                        createTagWithText(OpaqueComponentList, "ERROR", name.c_str());
                    }
                }
            }
        }
    }
    std::vector<model::SubSurface> subSurfaces = surface.subSurfaces();
    std::vector<model::SubSurface>::iterator it = subSurfaces.begin();
    while (it!=subSurfaces.end()) {
        model::SubSurface sub = (*it);
        std::string name = sub.construction().get().name().get();
        std::string stype = sub.subSurfaceType();
        if(!((name.empty())|| componentCheck.contains(name.c_str()))){
            componentCheck.insert(name.c_str(), 1);
            boost::optional<model::ConstructionBase> scon = sub.construction();
            if (scon){
                boost::optional<model::Construction> construction = scon->optionalCast<model::Construction>();
                if (construction){
                    std::vector<model::Material> layers = construction->layers();
                    if (!layers.empty()){
                        if (layers[0].optionalCast<model::StandardOpaqueMaterial>()){
                            model::StandardOpaqueMaterial outerMaterial = layers[0].cast<model::StandardOpaqueMaterial>();

                            QDomElement OpaqueDetail = createTagWithText(OpaqueComponentDetail, "OpaqueDetail");
                            createTagWithText(OpaqueDetail, "OpaqueComponentDetailListName", name.c_str());
                            createTagWithText(OpaqueDetail, "OpaqueComponentDetailMaterialName", outerMaterial.name().get().c_str());
                            createTagWithText(OpaqueDetail, "OpaqueComponentDetailThickness", QString::number(outerMaterial.thickness()));
                            //createTagWithText(OpaqueDetail, "IsOpaque", QString(layers[0]->isOpaque()));
                            createTagWithText(OpaqueDetail, "OpaqueComponentDetailThicknessUnit", "m");

                            QDomElement OpaqueList = createTagWithText(OpaqueComponentList, "OpaqueList");
                            createTagWithText(OpaqueList, "OpaqueComponentListName", name.c_str());
                            createTagWithText(OpaqueList, "OpaqueComponentListType", stype.c_str());

                            QString colorName = "White";
                            if(construction->renderingColor()){
                                QColor color;
                                color.setRed(construction->renderingColor()->renderingRedValue());
                                color.setGreen(construction->renderingColor()->renderingGreenValue());
                                color.setBlue(construction->renderingColor()->renderingBlueValue());
                                color.setAlpha(construction->renderingColor()->renderingAlphaValue());
                                colorName = color.name();
                            }
                            createTagWithText(OpaqueList, "OpaqueComponentListOuterSurfaceColor", colorName);
                            createTagWithText(OpaqueList, "OpaqueComponentListInnerSurfaceType", "UNKNOW");
                            createTagWithText(OpaqueList, "OpaqueComponentListDescription", "???");
                        }
                        else if (layers[0].optionalCast<model::SimpleGlazing>()){
                            model::SimpleGlazing glass = layers[0].cast<model::SimpleGlazing>();

                            QDomElement TransparentList = createTagWithText(transparentComponentList, "TransparentList");
                            createTagWithText(TransparentList, "TransparentComponentListName", glass.name().get().c_str());
                            createTagWithText(TransparentList, "TransparentComponentListType", stype.c_str());
                            createTagWithText(TransparentList, "TransparentComponentListSHGC", QString::number(glass.solarHeatGainCoefficient()));
                            createTagWithText(TransparentList, "TransparentComponentListTransmittance", QString::number(glass.visibleTransmittance().get()));
                            //createTagWithText(TransparentList, "IsOpaque", QString(layers[0]->isOpaque()));
                            createTagWithText(TransparentList, "TransparentComponentListDescription", "???");
                        }
                        else{
                            createTagWithText(OpaqueComponentList, "ERROR", name.c_str());
                        }
                    }
                }
            }
        }
        it++;
    }
}

void ForwardTranslator::doOpaqueComponentList(const model::Model &model, QDomElement &parent)
{
    QDomElement opaqueComponentList = _doc->createElement("OpaqueComponentList");
    parent.appendChild(opaqueComponentList);
    //      boost::optional<openstudio::model::Building> building = model.building();
    //      if(building){
    //          std::vector<openstudio::model::Space>::iterator it = building->spaces().begin();
    //          while (it!=building->spaces().end()) {
    //              doOpaqueList((*it), doc, opaqueComponentList);
    //              it++;
    //          }
    //      }
}

void ForwardTranslator::doOpaqueComponentDetail(const model::Model &model, QDomElement &parent)
{

}

void ForwardTranslator::doTransparentComponentList(const model::Model &model, QDomElement &parent)
{

}

void ForwardTranslator::doOpaqueList(const openstudio::model::Space &model, QDomElement &parent)
{

}

void ForwardTranslator::doSectionOfWall(const model::Model &model, QDomElement &Envelope)
{
    QDomElement sectionOfWall = createTagWithText(Envelope, "SectionOfWall");
    QDomElement SectionList = createTagWithText(sectionOfWall, "SectionList");
    QDomElement SectionDetail = createTagWithText(sectionOfWall, "SectionDetail");

    QDomElement Wall = createTagWithText(Envelope, "Wall");
    QDomElement WallList = createTagWithText(Wall, "WallList");
    QDomElement WallDetail = createTagWithText(Wall, "WallDetail");

    std::vector<openstudio::model::BuildingStory> stories = model.getConcreteModelObjects<openstudio::model::BuildingStory>();

    for (const openstudio::model::BuildingStory& buildingStory : stories) {

        std::vector<openstudio::model::Space> spaces = buildingStory.spaces();
        for (openstudio::model::Space& space : spaces){

            openstudio::model::SurfaceVector surfaces = space.surfaces();
            for (model::Surface& surface : surfaces){
                if(surface.surfaceType() == "Wall" || surface.surfaceType() == "RoofCeiling"){

                    QString surfaceType;
                    if(surface.surfaceType() == "RoofCeiling"){
                        surfaceType = "Roof";
                    }
                    else
                        surfaceType = "Wall";

                    QDomElement SectionL = createTagWithText(SectionList,"SectionL");
                    createTagWithText(SectionL, "SectionListName", surface.name().get().c_str());
                    createTagWithText(SectionL, "SectionListType", surfaceType);
                    createTagWithText(SectionL, "SectionListDescription", "???");

                    QDomElement SectionD = createTagWithText(SectionDetail,"SectionD");
                    createTagWithText(SectionD, "SectionDetailSectionListName", surface.name().get().c_str());

                    QString componentName = surface.construction().get().name().get().c_str();
                    boost::optional<model::ConstructionBase> scon = surface.construction();
                    if (scon){
                        boost::optional<model::Construction> construction = scon->optionalCast<model::Construction>();
                        if (construction){
                            std::vector<model::Material> layers = construction->layers();
                            if (!layers.empty()){
                                if (layers[0].optionalCast<model::SimpleGlazing>()){
                                    model::SimpleGlazing glass = layers[0].cast<model::SimpleGlazing>();
                                    componentName = glass.name().get().c_str();
                                }
                            }
                        }
                    }

                    createTagWithText(SectionD, "SectionDetailComponentName", componentName);
                    createTagWithText(SectionD, "SectionDetailArea", QString::number(surface.netArea()));
                    createTagWithText(SectionD, "SectionDetailAreaUnit", "m^2");

                    QDomElement WallL = createTagWithText(WallList,"WallL");
                    createTagWithText(WallL, "WallListName", surface.name().get().c_str());
                    createTagWithText(WallL, "WallListType", surfaceType);
                    createTagWithText(WallL, "WallListPlanAzimuth", QString::number(rtod(surface.azimuth())));
                    createTagWithText(WallL, "WallListInclination", QString::number(rtod(surface.tilt())));
                    createTagWithText(WallL, "WallListDescription", "???");

                    QDomElement WallD = createTagWithText(WallDetail,"WallD");
                    createTagWithText(WallD, "WallDetailWallListName", surface.name().get().c_str());
                    createTagWithText(WallD, "WallDetailSectionName", surface.name().get().c_str());
                    createTagWithText(WallD, "WallDetailSC", "1");

                    openstudio::model::SubSurfaceVector subs = surface.subSurfaces();

                    for (openstudio::model::SubSurface& sub : subs){
                        QDomElement SectionD = createTagWithText(SectionDetail,"SectionD");
                        createTagWithText(SectionD, "SectionDetailSectionListName", surface.name().get().c_str());

                        QString componentName = sub.construction().get().name().get().c_str();
                        boost::optional<model::ConstructionBase> scon = sub.construction();
                        if (scon){
                            boost::optional<model::Construction> construction = scon->optionalCast<model::Construction>();
                            if (construction){
                                std::vector<model::Material> layers = construction->layers();
                                if (!layers.empty()){
                                    if (layers[0].optionalCast<model::SimpleGlazing>()){
                                        model::SimpleGlazing glass = layers[0].cast<model::SimpleGlazing>();
                                        componentName = glass.name().get().c_str();
                                    }
                                }
                            }
                        }

                        createTagWithText(SectionD, "SectionDetailComponentName", componentName);
                        createTagWithText(SectionD, "SectionDetailArea", QString::number(sub.netArea()));
                        createTagWithText(SectionD, "SectionDetailAreaUnit", "m^2");
                    }
                }
                else{
                    QDomElement SectionUNKNOW = createTagWithText(SectionList,"SectionUNKNOW");
                    createTagWithText(SectionUNKNOW,"SectionUNKNOW_TYPE", surface.surfaceType().c_str());
                    createTagWithText(SectionUNKNOW,"SectionUNKNOW_NAME", surface.name().get().c_str());
                }
            }
        }
    }
}

void ForwardTranslator::doModelLoop(const model::Model &model, QDomElement &becInput)
{
    QDomElement LightingSystem = createTagWithText(becInput, "LightingSystem");
    QDomElement ACSystem = createTagWithText(becInput, "ACSystem");
    QDomElement HotWaterSystem = createTagWithText(becInput, "HotWaterSystem");
    QDomElement OtherEquipment = createTagWithText(becInput, "OtherEquipment");
    doPV(model, becInput);

    doHotWaterSystem(model, HotWaterSystem);
    doACSystem(model, ACSystem);
    //doOtherEquipment(model, OtherEquipment);

    std::vector<openstudio::model::BuildingStory> stories = model.getConcreteModelObjects<openstudio::model::BuildingStory>();

    for (const openstudio::model::BuildingStory& buildingStory : stories) {

        std::vector<openstudio::model::Space> spaces = buildingStory.spaces();
        for (openstudio::model::Space& space : spaces){

            doLightingSystem(space, LightingSystem);

            openstudio::model::SurfaceVector surfaces = space.surfaces();
            for (openstudio::model::Surface& surface : surfaces){

            }
        }
    }

}

void ForwardTranslator::doLightingSystem(const model::Space &space, QDomElement &LightingSystem)
{
    if(true){
        std::vector<model::Lights> lights = space.spaceType().get().lights();
        for (const model::Lights& light : lights){
            if(light.lightingLevel()){
                QDomElement Lighting = createTagWithText(LightingSystem, "Lighting");
                createTagWithText(Lighting, "LightingSystemName"
                                  , light.lightsDefinition().name().get().c_str());
                createTagWithText(Lighting, "LightingSystemPower", QString::number(light.lightingLevel().get()));
                createTagWithText(Lighting, "LightingSystemPowerUnit", "watt");
                createTagWithText(Lighting, "LightingSystemDescription", "???");
            }
        }
    }
}

void ForwardTranslator::doOtherEquipment(const model::Space &space, QDomElement &OtherEquipment)
{
    if(true){
        std::vector<model::OtherEquipment> others = space.spaceType().get().otherEquipment();
        for (const model::OtherEquipment& other : others){
            QDomElement OtherEQ = createTagWithText(OtherEquipment, "OtherEQ");
            createTagWithText(OtherEQ, "OtherEQName"
                              , other.otherEquipmentDefinition().name().get().c_str());
            if(other.otherEquipmentDefinition().designLevel()){
                createTagWithText(OtherEQ, "OtherEQPower", QString::number(other.otherEquipmentDefinition().designLevel().get()));
            }else if(other.otherEquipmentDefinition().wattsperPerson()){
                createTagWithText(OtherEQ, "OtherEQPower", QString::number(other.otherEquipmentDefinition().wattsperPerson().get()));
            }else if(other.otherEquipmentDefinition().wattsperSpaceFloorArea()){
                createTagWithText(OtherEQ, "OtherEQPower", QString::number(other.otherEquipmentDefinition().wattsperSpaceFloorArea().get()));
            }
            createTagWithText(OtherEQ, "OtherEQPowerUnit", "watt");
            createTagWithText(OtherEQ, "OtherEQDescription", "???");
        }
    }
}

bool isSkip(model::ModelObject model){
    if(       model.iddObject().type().value() == IddObjectType::OS_Node
              || model.iddObject().type().value() == IddObjectType::OS_Fan_ConstantVolume
              || model.iddObject().type().value() == IddObjectType::OS_Pipe_Adiabatic
              || model.iddObject().type().value() == IddObjectType::OS_AirLoopHVAC_OutdoorAirSystem
              || model.iddObject().type().value() == IddObjectType::OS_Connector_Splitter
              || model.iddObject().type().value() == IddObjectType::OS_SetpointManager_SingleZone_Reheat
              || model.iddObject().type().value() == IddObjectType::OS_Connector_Mixer
              || model.iddObject().type().value() == IddObjectType::OS_SetpointManager_Scheduled
              || model.iddObject().type().value() == IddObjectType::OS_AirLoopHVAC_ZoneSplitter
              || model.iddObject().type().value() == IddObjectType::OS_AirLoopHVAC_ZoneMixer
              || model.iddObject().type().value() == IddObjectType::OS_Fan_OnOff
              || model.iddObject().type().value() == IddObjectType::OS_Fan_VariableVolume)
    {
        return true;
    }
    return false;
}

void ForwardTranslator::doACSystem(const model::Model &model, QDomElement &ACSystem)
{
    QDomElement SplitTypeSystem = createTagWithText(ACSystem, "SplitTypeSystem");
    QDomElement PackagedAirCooledUnit = createTagWithText(ACSystem, "PackagedAirCooledUnit");
    QDomElement PackagedWaterCooledUnit = createTagWithText(ACSystem, "PackagedWaterCooledUnit");
    QDomElement CentralACSystem = createTagWithText(ACSystem, "CentralACSystem");
    //////////////////////////////////////////////////////////////////////////
    QDomElement CentralACList = createTagWithText(CentralACSystem, "CentralACList");
    std::vector<openstudio::model::AirLoopHVAC> airLoops = model.getConcreteModelObjects<openstudio::model::AirLoopHVAC>();
    std::vector<openstudio::model::PlantLoop> plantLoops = model.getConcreteModelObjects<openstudio::model::PlantLoop>();
    QDomElement CentralACDetail = createTagWithText(CentralACSystem, "CentralACDetail");

    for (model::AirLoopHVAC &airLoop : airLoops)
    {
        std::vector<model::ModelObject> hvacs = airLoop.supplyComponents();
        QString listName = airLoop.name().get().c_str();
        if(listName.startsWith("Thai Packaged Air Conditioner"))
        {
            double scapacity = 0;
            double spower = 0;
            for (const model::ModelObject & hvac : hvacs)
            {
                if (isSkip(hvac)){
                    continue;
                }
                if(hvac.iddObject().type().value() == openstudio::IddObjectType::OS_Coil_Cooling_DX_SingleSpeed)
                {
                    model::CoilCoolingDXSingleSpeed coil = hvac.cast<model::CoilCoolingDXSingleSpeed>();
                    boost::optional<double> coolingcap = coil.ratedTotalCoolingCapacity();
                    if(coolingcap)
                        scapacity += coolingcap.get_value_or(0.0f);

                    double rc = coil.ratedCOP().get_value_or(0.0f);

                    if(rc == 0.0)
                        rc = 0.000000001f;

                    boost::optional<double> power = coolingcap.get_value_or(0.0f)/rc;
                    if(power)
                        spower += power.get_value_or(0.0f);
                }
                else if(hvac.iddObject().type().value() == openstudio::IddObjectType::OS_Coil_Cooling_DX_TwoSpeed)
                {
                    model::CoilCoolingDXTwoSpeed coil = hvac.cast<model::CoilCoolingDXTwoSpeed>();
                    boost::optional<double> coolingcap = coil.ratedHighSpeedTotalCoolingCapacity();
                    if(coolingcap)
                        scapacity += coolingcap.get_value_or(0.0f);

                    double rc = coil.ratedHighSpeedCOP().get_value_or(0.0f);

                    if(rc == 0.0)
                        rc = 0.000000001f;

                    boost::optional<double> power = coolingcap.get_value_or(0.0f)/rc;
                    if(power)
                        spower += power.get_value_or(0.0f);
                }
            }


            QDomElement PackagedAirCooled = createTagWithText(PackagedAirCooledUnit
                                                              , "PackagedAirCooled");
            createTagWithText(PackagedAirCooled, "PackagedAirCooledName", listName);
            createTagWithText(PackagedAirCooled, "PackagedAirCooledCoolingCapacity", QString::number(scapacity));
            createTagWithText(PackagedAirCooled, "PackagedAirCooledCoolingCapacityUnit", "TR");
            createTagWithText(PackagedAirCooled, "PackagedAirCooledPower", QString::number(spower));
            createTagWithText(PackagedAirCooled, "PackagedAirCooledPowerUnit", "kW");
            createTagWithText(PackagedAirCooled, "PackagedAirCooledDescription", "???");
            createTagWithText(PackagedAirCooled, "PackagedAirCooledCOP", "0");
            createTagWithText(PackagedAirCooled, "PackagedAirCooledkWth", "0");
            continue;
        }
        else if(listName.startsWith("Thai Split Type Air"))
        {
            for (const model::ModelObject & hvac : hvacs){
                if(isSkip(hvac)){
                    continue;
                }
                switch(hvac.iddObject().type().value())
                {
                case openstudio::IddObjectType::OS_Coil_Cooling_DX_SingleSpeed :
                {
                    model::CoilCoolingDXSingleSpeed coil = hvac.cast<model::CoilCoolingDXSingleSpeed>();
                    QDomElement SplitType = createTagWithText(SplitTypeSystem, "SplitType");
                    //////////////////////////////////////////////////
                    std::string name = coil.name().get();
                    boost::optional<double> cop = coil.ratedCOP();
                    boost::optional<double> coolingcap = coil.ratedTotalCoolingCapacity();
                    //boost::optional<double> power2 = coil.evaporativeCondenserPumpRatedPowerConsumption();
                    boost::optional<double> power = coil.ratedEvaporatorFanPowerPerVolumeFlowRate();

                    createTagWithText(SplitType, "SplitTypeName", name.c_str());
                    createTagWithText(SplitType, "SplitTypeCoolingCapacity"
                                      , QString::number(coolingcap.get_value_or(0.0f)));
                    createTagWithText(SplitType, "SplitTypeCoolingCapacityUnit"
                                      , "TR");
                    createTagWithText(SplitType, "SplitTypePower"
                                      , QString::number(power.get_value_or(0.0f)/1000));
                    //createTagWithText(SplitType, "SplitTypePower2"
                    //                  , QString::number(power2.get_value_or(0.0f)));
                    createTagWithText(SplitType, "SplitTypePowerUnit", "kW");
                    createTagWithText(SplitType, "SplitTypeDescription", "?");
                    createTagWithText(SplitType, "SplitTypeCOP", QString::number(cop.get_value_or(0.0f)));
                    createTagWithText(SplitType, "SplitTypekWth", "0");

                    if(coil.isRatedTotalCoolingCapacityAutosized()){
                        createTagWithText(SplitType, "AUTOSIZE", "true");
                    }else{
                        createTagWithText(SplitType, "AUTOSIZE", "false");
                    }
                    break;
                }
                case openstudio::IddObjectType::OS_Coil_Cooling_DX_TwoSpeed:
                {
                    model::CoilCoolingDXTwoSpeed coil = hvac.cast<model::CoilCoolingDXTwoSpeed>();
                    QDomElement SplitType = createTagWithText(SplitTypeSystem, "SplitType");
                    //////////////////////////////////////////////////
                    std::string name = coil.name().get();
                    boost::optional<double> cop = coil.ratedHighSpeedCOP();
                    boost::optional<double> coolingcap = coil.ratedHighSpeedTotalCoolingCapacity();
                    boost::optional<double> power = coil.highSpeedEvaporativeCondenserPumpRatedPowerConsumption();

                    createTagWithText(SplitType, "SplitTypeName", name.c_str());
                    createTagWithText(SplitType, "SplitTypeCoolingCapacity"
                                      , QString::number(coolingcap.get_value_or(0.0f)));
                    createTagWithText(SplitType, "SplitTypeCoolingCapacityUnit"
                                      , "TR");
                    createTagWithText(SplitType, "SplitTypePower"
                                      , QString::number(power.get_value_or(0.0f)/1000));
                    createTagWithText(SplitType, "SplitTypePowerUnit", "kW");
                    createTagWithText(SplitType, "SplitTypeDescription", "?");
                    createTagWithText(SplitType, "SplitTypeCOP", QString::number(cop.get_value_or(0.0f)));
                    createTagWithText(SplitType, "SplitTypekWth", "0");
                    break;
                }
                }
            }
            continue;
        }
        else if(listName.startsWith("Thai Central System Loop"))
        {
            QDomElement CentralACL = createTagWithText(CentralACList, "CentralACL");
            createTagWithText(CentralACL, "CentralACListName", listName);
            createTagWithText(CentralACL, "CentralACListType", airLoop.iddObject().name().c_str());

            int size = 0;
            for (const model::ModelObject & hvac : hvacs){
                if(isSkip(hvac)){
                    continue;
                }
                switch(hvac.iddObject().type().value())
                {
                case openstudio::IddObjectType::OS_Coil_Cooling_DX_SingleSpeed :
                {
                    size++;
                    model::CoilCoolingDXSingleSpeed coil = hvac.cast<model::CoilCoolingDXSingleSpeed>();

                    std::string name = coil.name().get();
                    boost::optional<double> cop = coil.ratedCOP();
                    boost::optional<double> coolingcap = coil.ratedTotalCoolingCapacity();
                    //boost::optional<double> power2 = coil.evaporativeCondenserPumpRatedPowerConsumption();
                    boost::optional<double> power = coil.ratedEvaporatorFanPowerPerVolumeFlowRate();

                    QDomElement CentralACD = createTagWithText(CentralACDetail, "CentralACD");
                    createTagWithText(CentralACD, "CentralACDetailListName", listName);
                    createTagWithText(CentralACD, "CentralACDetailName", name.c_str());
                    createTagWithText(CentralACD, "CentralACDetailEQType", "Air Cooled Water Chiller");
                    createTagWithText(CentralACD, "CentralACDetailChillerType", "All");
                    createTagWithText(CentralACD, "CentralACDetailQuantity", "1");
                    createTagWithText(CentralACD, "CentralACDetailCoolingCapacity", QString::number(coolingcap.get_value_or(0.0f)));
                    createTagWithText(CentralACD, "CentralACDetailCoolingCapacityUnit", "kW");
                    createTagWithText(CentralACD, "CentralACDetailPower", QString::number(power.get_value_or(0.0f)));
                    createTagWithText(CentralACD, "CentralACDetailPowerUnit", "kW");
                    createTagWithText(CentralACD, "CentralACDetailCOP", QString::number(cop.get_value_or(0.0f)));
                    createTagWithText(CentralACD, "CentralACDetailkWth", QString::number(power.get_value_or(0.0f)));

                    createTagWithText(CentralACD, "CentralACD_idd_name", hvac.name().get().c_str());
                    createTagWithText(CentralACD, "CentralACD_idd_iddname", hvac.iddObject().name().c_str());

                    if(coil.isRatedTotalCoolingCapacityAutosized()){
                        createTagWithText(CentralACD, "AUTOSIZE", "true");
                    }else{
                        createTagWithText(CentralACD, "AUTOSIZE", "false");
                    }
                    break;
                }
                case openstudio::IddObjectType::OS_Coil_Cooling_DX_TwoSpeed:
                {
                    size++;
                    model::CoilCoolingDXTwoSpeed coil = hvac.cast<model::CoilCoolingDXTwoSpeed>();
                    //////////////////////////////////////////////////
                    std::string name = coil.name().get();
                    boost::optional<double> cop = coil.ratedHighSpeedCOP();
                    boost::optional<double> coolingcap = coil.ratedHighSpeedTotalCoolingCapacity();
                    boost::optional<double> power = coil.highSpeedEvaporativeCondenserPumpRatedPowerConsumption();

                    QDomElement CentralACD = createTagWithText(CentralACDetail, "CentralACD");
                    createTagWithText(CentralACD, "CentralACDetailListName", listName);
                    createTagWithText(CentralACD, "CentralACDetailName", name.c_str());
                    createTagWithText(CentralACD, "CentralACDetailEQType", "Air Cooled Water Chiller");
                    createTagWithText(CentralACD, "CentralACDetailChillerType", "All");
                    createTagWithText(CentralACD, "CentralACDetailQuantity", "1");
                    createTagWithText(CentralACD, "CentralACDetailCoolingCapacity", QString::number(coolingcap.get_value_or(0.0f)));
                    createTagWithText(CentralACD, "CentralACDetailCoolingCapacityUnit", "kW");
                    createTagWithText(CentralACD, "CentralACDetailPower", QString::number(power.get_value_or(0.0f)));
                    createTagWithText(CentralACD, "CentralACDetailPowerUnit", "kW");
                    createTagWithText(CentralACD, "CentralACDetailCOP", QString::number(cop.get_value_or(0.0f)));
                    createTagWithText(CentralACD, "CentralACDetailkWth", QString::number(power.get_value_or(0.0f)));
                    createTagWithText(CentralACD, "CentralACD_idd_name", hvac.name().get().c_str());
                    createTagWithText(CentralACD, "CentralACD_idd_iddname", hvac.iddObject().name().c_str());
                    break;
                }
                default:
                {
                    QDomElement CentralACD = createTagWithText(CentralACDetail, "CentralACD_UNKNOW");
                    createTagWithText(CentralACD, "CentralACDetailListName", listName);
                    createTagWithText(CentralACD, "CentralACD_idd_name", hvac.name().get().c_str());
                    createTagWithText(CentralACD, "CentralACD_idd_iddname", hvac.iddObject().name().c_str());
                    break;
                }
                }
            }
            createTagWithText(CentralACL, "CentralACListEQ", QString::number(size));
            continue;
        }

        for (const model::ModelObject & hvac : hvacs){

            if(isSkip(hvac)){
                continue;
            }
            QDomElement CentralACD = createTagWithText(CentralACDetail, "airLoop_UNKNOW");
            createTagWithText(CentralACD, "airLoop_ListName", listName);
            createTagWithText(CentralACD, "airLoop_name", hvac.name().get().c_str());
            createTagWithText(CentralACD, "airLoop_iddname", hvac.iddObject().name().c_str());
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    for (model::PlantLoop &plantLoop : plantLoops) {
        std::vector<model::ModelObject> hvacs = plantLoop.supplyComponents();
        QString listName = plantLoop.name().get().c_str();
        QDomElement CentralACL = createTagWithText(CentralACList, "CentralACL");
        createTagWithText(CentralACL, "CentralACListName", listName);
        createTagWithText(CentralACL, "CentralACListType", plantLoop.iddObject().name().c_str());

        int size = 0;
        for (const model::ModelObject & hvac : hvacs){
            if(isSkip(hvac)){
                continue;
            }

            if(hvac.iddObject().type().value() == IddObjectType::OS_CoolingTower_VariableSpeed){
                size++;
                QDomElement CentralACD = createTagWithText(CentralACDetail, "CentralACD");
                model::CoolingTowerVariableSpeed tower = hvac.cast<model::CoolingTowerVariableSpeed>();
                createTagWithText(CentralACD, "CentralACDetailListName", listName);
                createTagWithText(CentralACD, "CentralACDetailName", hvac.name().get().c_str());
                createTagWithText(CentralACD, "CentralACDetailEQType", "Air Handling Unit");
                createTagWithText(CentralACD, "CentralACDetailChillerType", "All");
                createTagWithText(CentralACD, "CentralACDetailQuantity", "1");
                createTagWithText(CentralACD, "CentralACDetailCoolingCapacity"
                                  , QString::number(tower.designFanPower().get_value_or(0.0f)/0.0105));
                createTagWithText(CentralACD, "CentralACDetailCoolingCapacityUnit", "kW");
                createTagWithText(CentralACD, "CentralACDetailPower"
                                  , QString::number(tower.designAirFlowRate().get_value_or(0.0f)));
                createTagWithText(CentralACD, "CentralACDetailPowerUnit", "kW");
                createTagWithText(CentralACD, "CentralACDetailCOP", QString::number(0));
                createTagWithText(CentralACD, "CentralACDetailkWth", "0");
                createTagWithText(CentralACD, "plantLoop_name_success", hvac.name().get().c_str());
                createTagWithText(CentralACD, "iddname", hvac.iddObject().name().c_str());
            }
            else if(hvac.iddObject().type().value() == IddObjectType::OS_Pump_VariableSpeed){
                size++;
                QDomElement CentralACD = createTagWithText(CentralACDetail, "CentralACD");
                model::PumpVariableSpeed pump = hvac.cast<model::PumpVariableSpeed>();
                createTagWithText(CentralACD, "CentralACDetailListName", listName);
                createTagWithText(CentralACD, "CentralACDetailName", hvac.name().get().c_str());
                createTagWithText(CentralACD, "CentralACDetailEQType", "Chilled Water Pump");
                createTagWithText(CentralACD, "CentralACDetailChillerType", "None");
                createTagWithText(CentralACD, "CentralACDetailQuantity", "1");
                createTagWithText(CentralACD, "CentralACDetailCoolingCapacity"
                                  , QString::number(pump.ratedPowerConsumption().get_value_or(0.0f)));
                createTagWithText(CentralACD, "CentralACDetailCoolingCapacityUnit", "kW");
                createTagWithText(CentralACD, "CentralACDetailPower"
                                  , QString::number(pump.ratedPowerConsumption().get_value_or(0.0f)));
                createTagWithText(CentralACD, "CentralACDetailPowerUnit", "kW");
                createTagWithText(CentralACD, "CentralACDetailCOP", "0");
                createTagWithText(CentralACD, "CentralACDetailkWth", "0");
                createTagWithText(CentralACD, "plantLoop_name_success", hvac.name().get().c_str());
                createTagWithText(CentralACD, "iddname", hvac.iddObject().name().c_str());
            }
            else if(hvac.iddObject().type().value() == IddObjectType::OS_Chiller_Electric_EIR){
                size++;
                QDomElement CentralACD = createTagWithText(CentralACDetail, "CentralACD");
                model::ChillerElectricEIR chiller = hvac.cast<model::ChillerElectricEIR>();
                double cop = chiller.referenceCOP();
                double refcap = chiller.referenceCapacity().get_value_or(0.0f);

                if(cop==0.0)
                    cop = 0.0000001;

                createTagWithText(CentralACD, "CentralACDetailListName", listName);
                createTagWithText(CentralACD, "CentralACDetailName", hvac.name().get().c_str());
                createTagWithText(CentralACD, "CentralACDetailEQType", "Air Cooled Water Chiller");
                createTagWithText(CentralACD, "CentralACDetailChillerType", "None");
                createTagWithText(CentralACD, "CentralACDetailQuantity", "1");
                createTagWithText(CentralACD, "CentralACDetailCoolingCapacity", QString::number(chiller.referenceCapacity().get_value_or(0.0f)));
                createTagWithText(CentralACD, "CentralACDetailCoolingCapacityUnit", "kW");
                createTagWithText(CentralACD, "CentralACDetailPower"
                                  , QString::number(refcap/cop));
                createTagWithText(CentralACD, "CentralACDetailPowerUnit", "kW");
                createTagWithText(CentralACD, "CentralACDetailCOP", QString::number(0));
                createTagWithText(CentralACD, "CentralACDetailkWth", "0");
                createTagWithText(CentralACD, "plantLoop_name_success", hvac.name().get().c_str());
                createTagWithText(CentralACD, "iddname", hvac.iddObject().name().c_str());
            }
            //              else if(hvac.iddObject().type().value() == IddObjectType::OS_CoolingTower_SingleSpeed){
            //                  model::CoolingTowerSingleSpeed tower = hvac.cast<model::CoolingTowerSingleSpeed>();
            //              }
            //              else if(hvac.iddObject().type().value() == IddObjectType::OS_CoolingTower_TwoSpeed){
            //                  model::CoolingTowerTwoSpeed tower = hvac.cast<model::CoolingTowerTwoSpeed>();
            //              }
            else{
                QDomElement CentralACD = createTagWithText(CentralACDetail, "CentralACD_UNKNOW_plantLoop");
                createTagWithText(CentralACD, "CentralACDetailListName", listName);
                createTagWithText(CentralACD, "CentralACDetailName", hvac.name().get().c_str());
                createTagWithText(CentralACD, "plantLoop_name_unsuccess", hvac.name().get().c_str());
                createTagWithText(CentralACD, "iddname", hvac.iddObject().name().c_str());
            }
        }
        createTagWithText(CentralACL, "CentralACListEQ", QString::number(size));
    }
}

void ForwardTranslator::doBuildingType(QDomElement &becInput, const QString& typeName)
{
    QDomElement BT = createTagWithText(becInput, "BT");
    QDomElement BuildType = createTagWithText(BT, "BuildType");
    createTagWithText(BuildType, "BuildingType", typeName);
}

void ForwardTranslator::doPV(const model::Model &model, QDomElement &becInput)
{
    QDomElement PVSystem = createTagWithText(becInput, "PVSystem");

    std::vector<model::Photovoltaic> pvs = model.getModelObjects<model::Photovoltaic>();
    for (model::Photovoltaic& pv : pvs){
        QDomElement PV = createTagWithText(PVSystem, "PV");
        createTagWithText(PV, "PVName", pv.name().get().c_str());
        createTagWithText(PV, "PVSystemEfficiency", QString::number(pv.cellEfficiency()));
        createTagWithText(PV, "PVSystemEfficiencyUnit", "%");
        createTagWithText(PV, "PVModuleArea", QString::number(pv.surfaceArea()));
        createTagWithText(PV, "PVModuleAreaUnit", "m^2");
        createTagWithText(PV, "PVAzimuth", QString::number(pv.azimuthAngle()));
        createTagWithText(PV, "PVAzimuthUnit", "degrees");
        createTagWithText(PV, "PVInclination", QString::number(pv.inclinationAngle()));
        createTagWithText(PV, "PVInclinationUnit", "degrees");
    }

    std::vector<model::PhotovoltaicThermal> pvst = model.getModelObjects<model::PhotovoltaicThermal>();
    for (model::PhotovoltaicThermal& pv : pvst){
        QDomElement PV = createTagWithText(PVSystem, "PV");
        createTagWithText(PV, "PVName", pv.name().get().c_str());
        createTagWithText(PV, "PVSystemEfficiency", QString::number(pv.cellEfficiency()));
        createTagWithText(PV, "PVSystemEfficiencyUnit", "%");
        createTagWithText(PV, "PVModuleArea", QString::number(pv.surfaceArea()));
        createTagWithText(PV, "PVModuleAreaUnit", "m^2");
        createTagWithText(PV, "PVAzimuth", QString::number(pv.azimuthAngle()));
        createTagWithText(PV, "PVAzimuthUnit", "degrees");
        createTagWithText(PV, "PVInclination", QString::number(pv.inclinationAngle()));
        createTagWithText(PV, "PVInclinationUnit", "degrees");
    }
}

void ForwardTranslator::doHotWaterSystem(const model::Model &model, QDomElement &hotWaterSystem)
{
    std::vector<openstudio::model::HVACComponent> hvacs = model.getModelObjects<model::HVACComponent>();
    for (const model::HVACComponent & hvac : hvacs) {

        boost::optional<openstudio::model::BoilerHotWater> isHW
                = hvac.optionalCast<openstudio::model::BoilerHotWater>();

        if(isHW){
            openstudio::model::BoilerHotWater boiler = isHW.get();
            QDomElement hw = createTagWithText(hotWaterSystem, "HW");
            createTagWithText(hw, "HWName"
                              , boiler.name().get().c_str());
            createTagWithText(hw, "HWBoilerType", "Gas Fired Hot Water Boiler");
            createTagWithText(hw, "HWBoilerEfficiency", QString::number(boiler.nominalThermalEfficiency()*100));
            createTagWithText(hw, "HWBoilerEfficiencyUnit", "%");
            createTagWithText(hw, "HWHeatPumpType", "None");
            //boiler.children.r.briefDescription.boilerFlowMode.ratedHeatingCOP()
            //WARNING: NOTING
            createTagWithText(hw, "HWHeatPumpCOP", "0");
            createTagWithText(hw, "HWDescription", "");
            continue;
        }
    }
}

//  void ForwardTranslator::doLightingSystem(const model::Model &model, QDomElement &root)
//  {
//        LightingSystem
//  }

//  void ForwardTranslator::doACSystem(const model::Model &model, QDomElement &root)
//  {

//  }

//  void ForwardTranslator::doPVSystem(const model::Model &model, QDomElement &root)
//  {

//  }

//  void ForwardTranslator::doHotWaterSystem(const model::Model &model, QDomElement &root)
//  {

//  }

//  void ForwardTranslator::doOtherEquipment(const model::Model &model, QDomElement &root)
//  {

//  }

void ForwardTranslator::doBuildingEnvelope(const model::Model &model, QDomElement &becInput)
{
    QDomElement buildingEnvelope = _doc->createElement("BuildingEnvelope");
    becInput.appendChild(buildingEnvelope);

    QDomElement buildingZoneList = _doc->createElement("BuildingZoneList");
    buildingEnvelope.appendChild(buildingZoneList);

    QDomElement buildingZoneDetail = _doc->createElement("BuildingZoneDetail");
    buildingEnvelope.appendChild(buildingZoneDetail);

    QDomElement buildingZoneExteriorWall = createTagWithText(buildingZoneDetail
                                                             , "BuildingZoneExteriorWall");

    QDomElement BuildingZoneLightingEQ = createTagWithText(buildingZoneDetail
                                                           , "BuildingZoneLightingEQ");
    //TODO: THIS ELEMENT
    QDomElement BuildingZoneDXACUnit
            = createTagWithText(buildingZoneDetail, "BuildingZoneDXACUnit");
    //TODO: THIS ELEMENT
    QDomElement BuildingZoneCentralACEQ
            = createTagWithText(buildingZoneDetail, "BuildingZoneCentralACEQ");
    //TODO: THIS ELEMENT
    QDomElement BuildingZoneOtherEquipment
            = createTagWithText(buildingZoneDetail, "BuildingZoneOtherEquipment");

    // Get stories
    std::vector<openstudio::model::BuildingStory> stories = model.getConcreteModelObjects<openstudio::model::BuildingStory>();
    // We will need each story to have an elevation and a floor-to-floor height.
    int floor = 1;
    for (const openstudio::model::BuildingStory& buildingStory : stories) {

        std::vector<openstudio::model::Space> spaces = buildingStory.spaces();
        for (model::Space& space : spaces){

            QDomElement buildingZoneL = _doc->createElement("BuildingZoneL");
            buildingZoneList.appendChild(buildingZoneL);

            //createTagWithText(buildingZoneL, "BuildingZoneListTYPE", surface.surfaceType().c_str());
            //createTagWithText(buildingZoneL, "BuildingZoneListIsGround", QString::number(surface.isGroundSurface()));
            std::string zonelistName = space.name().get();

            std::vector<model::Lights> lights = space.spaceType().get().lights();

            if(lights.size()>0){
                for (const model::Lights& light : lights){
                    if(light.lightingLevel()){
                        QDomElement BuildingZoneLighting = createTagWithText(BuildingZoneLightingEQ, "BuildingZoneLighting");
                        createTagWithText(BuildingZoneLighting, "BuildingZoneLightingListName"
                                          , zonelistName.c_str());
                        createTagWithText(BuildingZoneLighting, "BuildingZoneLightingName"
                                          , light.lightsDefinition().name().get().c_str());
                        createTagWithText(BuildingZoneLighting, "BuildingZoneLightingQuantity"
                                          , QString::number(light.quantity()));
                        createTagWithText(BuildingZoneLighting, "BuildingZoneLightingDaylighted", "0");
                    }
                }
            }

            if(boost::optional<model::ThermalZone> thermal = space.thermalZone()){

                QString zoneName;
                boost::optional<model::AirLoopHVAC> airloop =  thermal.get().airLoopHVAC();
                if(airloop)
                    zoneName = airloop.get().name().get().c_str();

                boost::optional<model::PlantLoop> planloop = thermal.get().plantLoop();
                if(planloop)
                    zoneName = planloop.get().name().get().c_str();

                createTagWithText(BuildingZoneDXACUnit, "ZONE_NAME", zoneName);

                if(zoneName.startsWith("Thai Split Type Air")){
                    QDomElement BuildingZoneDXAC
                            = createTagWithText(BuildingZoneDXACUnit, "BuildingZoneDXAC");
                    createTagWithText(BuildingZoneDXAC, "BuildingZoneDXACListName", space.name().get().c_str());
                    createTagWithText(BuildingZoneDXAC, "BuildingZoneDXACName", zoneName);
                    createTagWithText(BuildingZoneDXAC, "BuildingZoneDXACQuantity", QString::number(thermal.get().multiplier()));
                }
                else if(zoneName.startsWith("Thai Central System Loop")){
                    boost::optional<model::AirLoopHVAC> airLoop = thermal.get().airLoopHVAC();
                    if (airLoop){
                        std::vector<model::ModelObject> hvacs = airLoop.get().supplyComponents();
                        createTagWithText(BuildingZoneCentralACEQ, "BuildingZoneCentralACSize", QString::number(hvacs.size()));
                        for (const model::ModelObject & hvac : hvacs)
                        {
                            if(hvac.iddObject().type().value() == openstudio::IddObjectType::OS_Coil_Cooling_DX_SingleSpeed)
                            {
                                QDomElement BuildingZoneCentralAC
                                        = createTagWithText(BuildingZoneCentralACEQ, "BuildingZoneCentralAC");
                                createTagWithText(BuildingZoneCentralAC, "BuildingZoneCentralACListName", space.name().get().c_str());
                                createTagWithText(BuildingZoneCentralAC, "BuildingZoneCentralACSystemName", zoneName);
                                createTagWithText(BuildingZoneCentralAC, "BuildingZoneCentralACEQName", hvac.name().get().c_str());
                                createTagWithText(BuildingZoneCentralAC, "BuildingZoneDXACQuantity", QString::number(thermal.get().multiplier()));
                            }
                            else if(hvac.iddObject().type().value() == openstudio::IddObjectType::OS_Coil_Cooling_DX_TwoSpeed)
                            {
                                QDomElement BuildingZoneCentralAC
                                        = createTagWithText(BuildingZoneCentralACEQ, "BuildingZoneCentralAC");
                                createTagWithText(BuildingZoneCentralAC, "BuildingZoneCentralACListName", space.name().get().c_str());
                                createTagWithText(BuildingZoneCentralAC, "BuildingZoneCentralACSystemName", zoneName);
                                createTagWithText(BuildingZoneCentralAC, "BuildingZoneCentralACEQName", hvac.name().get().c_str());
                                createTagWithText(BuildingZoneCentralAC, "BuildingZoneDXACQuantity", QString::number(thermal.get().multiplier()));
                            }
                            else
                            {
                                QDomElement BuildingZoneCentralAC
                                        = createTagWithText(BuildingZoneCentralACEQ, "BuildingZoneCentralAC_UNUSE");
                                createTagWithText(BuildingZoneCentralAC, "BuildingZoneCentralACListName", space.name().get().c_str());
                                createTagWithText(BuildingZoneCentralAC, "BuildingZoneCentralACSystemName", zoneName);
                                createTagWithText(BuildingZoneCentralAC, "BuildingZoneCentralACEQName", hvac.name().get().c_str());
                                createTagWithText(BuildingZoneCentralAC, "BuildingZoneDXACQuantity", QString::number(thermal.get().multiplier()));
                                createTagWithText(BuildingZoneCentralAC, "iddObject", hvac.iddObject().name().c_str());
                            }
                        }
                    }
                }
                else if(zoneName.startsWith("Thai Packaged Air Conditioner"))
                {
                    QDomElement BuildingZoneDXAC
                            = createTagWithText(BuildingZoneDXACUnit, "BuildingZoneDXAC");
                    createTagWithText(BuildingZoneDXAC, "BuildingZoneDXACListName", space.name().get().c_str());
                    createTagWithText(BuildingZoneDXAC, "BuildingZoneDXACName", zoneName);
                    createTagWithText(BuildingZoneDXAC, "BuildingZoneDXACQuantity", QString::number(thermal.get().multiplier()));
                }
            }

            if(zonelistName.empty())
                zonelistName = std::string("ZONE_") + QString::number(floor).toStdString();

            createTagWithText(buildingZoneL, "BuildingZoneListName", zonelistName.c_str());
            createTagWithText(buildingZoneL, "BuildingZoneListFloor", QString::number(floor));
            createTagWithText(buildingZoneL, "BuildingZoneListDescription", "???");

            double floorArea = 0.0;
            openstudio::model::SurfaceVector surfaces = space.surfaces();
            for (openstudio::model::Surface& surface : surfaces){
                if(surface.surfaceType() == "Floor"){
                    floorArea+= surface.netArea();
                }
                else if(surface.surfaceType() == "Wall" || surface.surfaceType() == "RoofCeiling"){
                    //buildingZoneExteriorWall
                    QDomElement buildingZoneWall = createTagWithText(buildingZoneExteriorWall, "BuildingZoneWall");
                    createTagWithText(buildingZoneWall, "BuildingZoneWallListName", zonelistName.c_str());
                    //createTagWithText(buildingZoneWall, "BuildingZoneWallWallName", surface.surfaceType().c_str());
                    createTagWithText(buildingZoneWall, "BuildingZoneWallWallName", surface.name().get().c_str());
                    //TODO:LINK TO MATERIAL.
                    createTagWithText(buildingZoneWall, "BuildingZoneWallSectionName", surface.name().get().c_str());

                    openstudio::model::SubSurfaceVector subs = surface.subSurfaces();

                    double area = surface.netArea();
                    for (openstudio::model::SubSurface& sub : subs){
                        area+=sub.netArea();
                    }
                    createTagWithText(buildingZoneWall, "BuildingZoneWallArea", QString::number(area));
                    createTagWithText(buildingZoneWall, "BuildingZoneWallAreaUnit", "m^2");
                }
                else{ //RoofCeiling

                }
            }
            createTagWithText(buildingZoneL, "BuildingZoneListArea", QString::number(floorArea));
            createTagWithText(buildingZoneL, "BuildingZoneListAreaUnit", "m^2");
        }
        floor++;
    }
}

boost::optional<QDomDocument> ForwardTranslator::translateModel(const openstudio::model::Model& model, const QString& type)
{
    QDomDocument doc;

    QDomProcessingInstruction instr = doc.createProcessingInstruction(
                "xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instr);

    QDomElement BECInput = doc.createElement("BECInput");
    _doc = &doc;
    doc.appendChild(BECInput);

    doBuildingType(BECInput, type);
    doEnvelope(model, BECInput);
    doModelLoop(model, BECInput);
    //doLightingSystem(model, BECInput);
    //doACSystem(model, BECInput);
    //doPVSystem(model, BECInput);
    //doHotWaterSystem(model, BECInput);
    //doOtherEquipment(model, BECInput);
    doBuildingEnvelope(model, BECInput);

    //boost::optional<QDomElement> myModel = translateMyModel(model,doc);
    //BECElement.appendChild(*myModel);

    /*
    boost::optional<model::Facility> facility = model.getOptionalUniqueModelObject<model::Facility>();
    if (facility){
      boost::optional<QDomElement> campusElement = translateFacility(*facility, doc);
      if (campusElement){
        BECElement.appendChild(*campusElement);
      }
    }
    */
    /*
    // do constructions
    for (const model::ConstructionBase& constructionBase : model.getModelObjects<model::ConstructionBase>()){
      boost::optional<QDomElement> constructionElement = translateConstructionBase(constructionBase, doc);
      if (constructionElement){
        projectElement.appendChild(*constructionElement);
      }
    }

    // do materials
    for (const model::Material& material : model.getModelObjects<model::Material>()){
      boost::optional<QDomElement> materialElement = translateMaterial(material, doc);
      if (materialElement){
        projectElement.appendChild(*materialElement);
      }
    }
*/
    /*
    // do thermal zones
    std::vector<model::ThermalZone> thermalZones = model.getConcreteModelObjects<model::ThermalZone>();
    if (m_progressBar){
      m_progressBar->setWindowTitle(toString("Translating Thermal Zones"));
      m_progressBar->setMinimum(0);
      m_progressBar->setMaximum((int)thermalZones.size());
      m_progressBar->setValue(0);
    }

    for (const model::ThermalZone& thermalZone : thermalZones){
      boost::optional<QDomElement> zoneElement = translateThermalZone(thermalZone, doc);
      if (zoneElement){
        BECElement.appendChild(*zoneElement);
      }

      if (m_progressBar){
        m_progressBar->setValue(m_progressBar->value() + 1);
      }
    }
    */

    return doc;
}

boost::optional<QDomElement> ForwardTranslator::translateMyModel(const openstudio::model::Model& model, QDomDocument& doc)
{
    std::string name;
    //double value;
    QDomElement result = doc.createElement("MyModel");

    // translate building
    boost::optional<model::Building> building = model.getOptionalUniqueModelObject<model::Building>();

    if (building){
        QDomElement buildingElement = doc.createElement("Building");
        name = *building.get().name();
        buildingElement.setAttribute("Name", escapeName(name));
        QDomElement buildingStoryElement = doc.createElement("buildingStory");

        // Space
        boost::optional<QDomElement> spaceElement = translateMySpace(*building,doc);
        /*
          QDomElement spaceElement = doc.createElement("Spaces");
          std::vector<model::Space> spaces = building.get().spaces();

          for (const model::Space& space : spaces){

              QDomElement m_spaceElement = doc.createElement("Space");
              name = *space.name();
              m_spaceElement.setAttribute("id", escapeName(name));

              // Space Type name
              std::string space_name = *space.spaceType().get().name();
              m_spaceElement.setAttribute("name", escapeName(space_name));
              spaceElement.appendChild(m_spaceElement);

              QDomElement m_area = doc.createElement("Area");
              double area = space.floorArea();
              m_area.appendChild(doc.createTextNode(QString::number(area)));
              m_spaceElement.appendChild(m_area);

              QDomElement m_valume = doc.createElement("Valume");
              double valume = space.volume();
              m_valume.appendChild(doc.createTextNode(QString::number(valume)));
              m_spaceElement.appendChild(m_valume);


              QDomElement loadElement = doc.createElement("Load");
              */
        /*
              std::vector<model::Lights> v_light = space.spaceType().get().lights();
              std::sort(v_light.begin(), v_light.end(), WorkspaceObjectNameLess());

              QDomElement nameElement;
              QDomElement lightElement;

              for (const model::Lights& i_light : v_light){

                  lightElement = doc.createElement("Light");

                  //!- Name
                  nameElement = doc.createElement("Name");
                  boost::optional<std::string> lightsDefinition = i_light.lightsDefinition().name().get();
                  nameElement.appendChild(doc.createTextNode(escapeName(*lightsDefinition)));
                  lightElement.appendChild(nameElement);

                  //!- Calculaion Method
                  std::string calculationMethod = i_light.lightsDefinition().designLevelCalculationMethod();
                  nameElement = doc.createElement("CalculationMethod");
                  nameElement.appendChild(doc.createTextNode(escapeName(calculationMethod)));
                  lightElement.appendChild(nameElement);

                  //!- Light Power
                  double lightPower = i_light.lightingLevel().get();
                  nameElement = doc.createElement("LightingLevel");
                  nameElement.appendChild(doc.createTextNode(QString::number(lightPower)));
                  lightElement.appendChild(nameElement);

                  //!- EnergyPerSpace
                  lightPower = i_light.powerPerFloorArea().get();
                  nameElement = doc.createElement("PowerPerArea");
                  nameElement.appendChild(doc.createTextNode(QString::number(lightPower)));
                  lightElement.appendChild(nameElement);

                  //!- EnergyPerPeople
                  lightPower = i_light.powerPerPerson().get();
                  nameElement = doc.createElement("PowerPerPerson");
                  nameElement.appendChild(doc.createTextNode(QString::number(lightPower)));
                  lightElement.appendChild(nameElement);

                  //!- Multiplier
                  double multipiler = i_light.multiplier();
                  nameElement = doc.createElement("Multiplier");
                  nameElement.appendChild(doc.createTextNode(QString::number(multipiler)));
                  lightElement.appendChild(nameElement);

                  loadElement.appendChild(lightElement);
              }

              */

        /*
              std::vector<model::OtherEquipment> v_otherEquipment = space.spaceType().get().otherEquipment();
              std::sort(v_otherEquipment.begin(), v_otherEquipment.end(), WorkspaceObjectNameLess());


              for (const model::OtherEquipment& i_otherEquipment : v_otherEquipment){
                  QDomElement otherEquipment = doc.createElement("OtherEquipment");
                  loadElement.appendChild(otherEquipment);

                  //!- Name
                  name = i_otherEquipment.otherEquipmentDefinition().name().get();
                  QDomElement nameElement = doc.createElement("Name");
                  nameElement.appendChild(doc.createTextNode(escapeName(name)));
                  otherEquipment.appendChild(nameElement);

                  //!- Calculation Method
                  std::string calculationMethod = i_otherEquipment.otherEquipmentDefinition().designLevelCalculationMethod();
                  nameElement = doc.createElement("CalculationMethod");
                  nameElement.appendChild(doc.createTextNode(escapeName(calculationMethod)));
                  otherEquipment.appendChild(nameElement);

                  //!- Design Level
                  double designLevel = i_otherEquipment.otherEquipmentDefinition().designLevel().get();
                  nameElement = doc.createElement("DesignLevel");
                  nameElement.appendChild(doc.createTextNode(QString::number(designLevel)));
                  otherEquipment.appendChild(nameElement);

                  //!- Watt Per Space
                  double wattPerSpace = i_otherEquipment.otherEquipmentDefinition().wattsperSpaceFloorArea().get();
                  nameElement = doc.createElement("WattPerSpace");
                  nameElement.appendChild(doc.createTextNode(QString::number(wattPerSpace)));
                  otherEquipment.appendChild(nameElement);

                  //!- Watt Per Space
                  double wattPerPerson = i_otherEquipment.otherEquipmentDefinition().wattsperPerson().get();
                  nameElement = doc.createElement("WattPerPerson");
                  nameElement.appendChild(doc.createTextNode(QString::number(wattPerPerson)));
                  otherEquipment.appendChild(nameElement);

                  //!- Multipiler
                  double muliplier = i_otherEquipment.multiplier();
                  nameElement = doc.createElement("Multipiler");
                  nameElement.appendChild(doc.createTextNode(QString::number(muliplier)));
                  otherEquipment.appendChild(nameElement);

              }
              */
        /*
              std::vector<model::ElectricEquipment> v_electricEquipment = space.spaceType().get().electricEquipment();
              std::sort(v_electricEquipment.begin(), v_electricEquipment.end(), WorkspaceObjectNameLess());

              for (const model::ElectricEquipment& i_electricEquipment : v_electricEquipment){

                  QDomElement electricEquipment = doc.createElement("ElectricEquipment");
                  loadElement.appendChild(electricEquipment);

                  //!- Name
                  name = i_electricEquipment.name().get();
                  QDomElement nameElement = doc.createElement("Name");
                  nameElement.appendChild(doc.createTextNode(escapeName(name)));
                  electricEquipment.appendChild(nameElement);

                  //!- Calculation Method
                  std::string calculationMethod = i_electricEquipment.electricEquipmentDefinition().designLevelCalculationMethod();
                  nameElement = doc.createElement("CalculationMethod");
                  nameElement.appendChild(doc.createTextNode(escapeName(calculationMethod)));
                  electricEquipment.appendChild(nameElement);

                  //!- Design Level
                  double designLevel = i_electricEquipment.designLevel().get();
                  nameElement = doc.createElement("DesignLevel");
                  nameElement.appendChild(doc.createTextNode(QString::number(designLevel)));
                  electricEquipment.appendChild(nameElement);

                  //!- Watt Per Space
                  double wattPerSpace = i_electricEquipment.powerPerFloorArea().get();
                  nameElement = doc.createElement("WattPerSpace");
                  nameElement.appendChild(doc.createTextNode(QString::number(wattPerSpace)));
                  electricEquipment.appendChild(nameElement);

                  //!- Watt Per Person
                  double wattPerPerson = i_electricEquipment.powerPerPerson().get();
                  nameElement = doc.createElement("WattPerPerson");
                  nameElement.appendChild(doc.createTextNode(QString::number(wattPerPerson)));
                  electricEquipment.appendChild(nameElement);

                  //!- Multipiler
                  double muliplier = i_electricEquipment.multiplier();
                  nameElement = doc.createElement("Multipiler");
                  nameElement.appendChild(doc.createTextNode(QString::number(muliplier)));
                  electricEquipment.appendChild(nameElement);
              }
              */
        /*
              m_spaceElement.appendChild(loadElement);

          }
          */

        QDomElement hvacElement = doc.createElement("HVAC");

        QDomElement materialsElement = doc.createElement("Materails");
        std::vector<model::Material> materials = model.getModelObjects<model::Material>();
        std::sort(materials.begin(), materials.end(), WorkspaceObjectNameLess());
        for (const model::Material& material : materials){

            QDomElement nameElement;
            QDomElement materialElement = doc.createElement("Materail");
            materialsElement.appendChild(materialElement);

            //!- Name
            nameElement = doc.createElement("Name");
            name = material.name().get();
            materialElement.appendChild(doc.createTextNode(escapeName(name)));
            materialElement.appendChild(nameElement);

            if (material.optionalCast<model::StandardOpaqueMaterial>()){

                //!- Conductivity{ W / m - K }
                model::StandardOpaqueMaterial m = material.optionalCast<model::StandardOpaqueMaterial>().get();

                std::double_t conductivity = m.conductivity();
                nameElement = doc.createElement("Conductivity");
                nameElement.appendChild(doc.createTextNode(QString::number(conductivity)));
                materialElement.appendChild(nameElement);

                //!- Density{ kg / m3 }
                std::double_t density = m.density();
                nameElement = doc.createElement("Density");
                nameElement.appendChild(doc.createTextNode(QString::number(density)));
                materialElement.appendChild(nameElement);

                //!- Specific Heat{ J / kg - K }
                std::double_t specificHeat = m.specificHeat();
                nameElement = doc.createElement("Specific_Heat");
                nameElement.appendChild(doc.createTextNode(QString::number(specificHeat)));
                materialElement.appendChild(nameElement);

                //!- Thermal Absorptance
                std::double_t thermalAbsorptance = m.thermalAbsorptance();
                nameElement = doc.createElement("ThermalAbsorptance");
                nameElement.appendChild(doc.createTextNode(QString::number(thermalAbsorptance)));
                materialElement.appendChild(nameElement);

                //!- Solar Absorptance
                std::double_t solarAbsorptance = m.solarAbsorptance();
                nameElement = doc.createElement("SolarAbsorptance");
                nameElement.appendChild(doc.createTextNode(QString::number(solarAbsorptance)));
                materialElement.appendChild(nameElement);

                //!- Visible Absorptance
                std::double_t visibleAbsorptance = m.visibleAbsorptance();
                nameElement = doc.createElement("VisibleAbsorptance");
                nameElement.appendChild(doc.createTextNode(QString::number(visibleAbsorptance)));
                materialElement.appendChild(nameElement);
            }
        }

        //		  spaceElement.appendChild(loadElement);
        //spaceElement.appendChild(hvacElement);
        buildingStoryElement.appendChild(*spaceElement);
        buildingElement.appendChild(buildingStoryElement);

        result.appendChild(materialsElement);
        result.appendChild(buildingElement);

    }

    // Load
    // Hvac

    return result;
}

boost::optional<QDomElement> ForwardTranslator::translateMySpace(const openstudio::model::Building& building, QDomDocument& doc){

    // Translate Space
    QDomElement spaceElement = doc.createElement("Spaces");
    std::vector<model::Space> spaces = building.spaces();

    for (const model::Space& space : spaces){

        QDomElement m_spaceElement = doc.createElement("Space");

        // Space Type name
        std::string space_name = *space.spaceType().get().name();
        m_spaceElement.setAttribute("SpaceType", toQString(space_name));
        spaceElement.appendChild(m_spaceElement);

        // Space name
        std::string name = *space.name();
        m_spaceElement.setAttribute("name", toQString(name));

        QDomElement m_area = doc.createElement("Area");
        double area = space.floorArea();
        m_area.appendChild(doc.createTextNode(QString::number(area)));
        m_spaceElement.appendChild(m_area);

        QDomElement m_valume = doc.createElement("Valume");
        double valume = space.volume();
        m_valume.appendChild(doc.createTextNode(QString::number(valume)));
        m_spaceElement.appendChild(m_valume);


        QDomElement loadElement = doc.createElement("Load");

        boost::optional<QDomElement> lightsElement = translateMyLights(space, doc);
        loadElement.appendChild(*lightsElement);

        boost::optional<QDomElement> OtherEquipmentElement = translateMyOtherEquipment(space, doc);
        loadElement.appendChild(*lightsElement);

        boost::optional<QDomElement> ElectronicEquipmentElement = translateMyElectronicEquipment(space, doc);
        loadElement.appendChild(*ElectronicEquipmentElement);

        m_spaceElement.appendChild(loadElement);

        // Translate Construction
        QDomElement defaultConstruction = doc.createElement("Constructions");
        boost::optional<QDomElement> constructionElement = translateMyConstruction(space, doc);
        defaultConstruction.appendChild(*constructionElement);

        m_spaceElement.appendChild(defaultConstruction);
    }

    return spaceElement;
}

boost::optional<QDomElement> ForwardTranslator::translateMyLights(const openstudio::model::Space& space, QDomDocument& doc){

    std::vector<model::Lights> v_light = space.spaceType().get().lights();
    std::sort(v_light.begin(), v_light.end(), WorkspaceObjectNameLess());

    QDomElement Lights = doc.createElement("Lights");

    QDomElement nameElement;
    QDomElement lightElement;


    for (const model::Lights& i_light : v_light){

        lightElement = doc.createElement("Light");

        //!- Name
        nameElement = doc.createElement("Name");
        boost::optional<std::string> lightsDefinition = i_light.lightsDefinition().name().get();
        nameElement.appendChild(doc.createTextNode(toQString(*lightsDefinition)));
        lightElement.appendChild(nameElement);

        //!- Calculaion Method
        std::string calculationMethod = i_light.lightsDefinition().designLevelCalculationMethod();
        nameElement = doc.createElement("CalculationMethod");
        nameElement.appendChild(doc.createTextNode(toQString(calculationMethod)));
        lightElement.appendChild(nameElement);

        //!- Light Power
        double lightPower = i_light.lightingLevel().get();
        nameElement = doc.createElement("LightingLevel");
        nameElement.appendChild(doc.createTextNode(QString::number(lightPower)));
        lightElement.appendChild(nameElement);

        //!- EnergyPerSpace
        lightPower = i_light.powerPerFloorArea().get();
        nameElement = doc.createElement("PowerPerArea");
        nameElement.appendChild(doc.createTextNode(QString::number(lightPower)));
        lightElement.appendChild(nameElement);

        //!- EnergyPerPeople
        lightPower = i_light.powerPerPerson().get();
        nameElement = doc.createElement("PowerPerPerson");
        nameElement.appendChild(doc.createTextNode(QString::number(lightPower)));
        lightElement.appendChild(nameElement);

        //!- Multiplier
        double multipiler = i_light.multiplier();
        nameElement = doc.createElement("Multiplier");
        nameElement.appendChild(doc.createTextNode(QString::number(multipiler)));
        lightElement.appendChild(nameElement);

        Lights.appendChild(lightElement);
    }
    return Lights;
}

boost::optional<QDomElement> ForwardTranslator::translateMyOtherEquipment(const openstudio::model::Space& space, QDomDocument& doc){

    QDomElement otherEquipments = doc.createElement("OtherEquipments");

    std::vector<model::OtherEquipment> v_otherEquipment = space.spaceType().get().otherEquipment();
    std::sort(v_otherEquipment.begin(), v_otherEquipment.end(), WorkspaceObjectNameLess());


    for (const model::OtherEquipment& i_otherEquipment : v_otherEquipment){

        QDomElement otherEquipment = doc.createElement("OtherEquipment");

        //!- Name
        std::string name = i_otherEquipment.otherEquipmentDefinition().name().get();
        QDomElement nameElement = doc.createElement("Name");
        nameElement.appendChild(doc.createTextNode(toQString(name)));
        otherEquipment.appendChild(nameElement);

        //!- Calculation Method
        std::string calculationMethod = i_otherEquipment.otherEquipmentDefinition().designLevelCalculationMethod();
        nameElement = doc.createElement("CalculationMethod");
        nameElement.appendChild(doc.createTextNode(toQString(calculationMethod)));
        otherEquipment.appendChild(nameElement);

        //!- Design Level
        double designLevel = i_otherEquipment.otherEquipmentDefinition().designLevel().get();
        nameElement = doc.createElement("DesignLevel");
        nameElement.appendChild(doc.createTextNode(QString::number(designLevel)));
        otherEquipment.appendChild(nameElement);

        //!- Watt Per Space
        double wattPerSpace = i_otherEquipment.otherEquipmentDefinition().wattsperSpaceFloorArea().get();
        nameElement = doc.createElement("WattPerSpace");
        nameElement.appendChild(doc.createTextNode(QString::number(wattPerSpace)));
        otherEquipment.appendChild(nameElement);

        //!- Watt Per Space
        double wattPerPerson = i_otherEquipment.otherEquipmentDefinition().wattsperPerson().get();
        nameElement = doc.createElement("WattPerPerson");
        nameElement.appendChild(doc.createTextNode(QString::number(wattPerPerson)));
        otherEquipment.appendChild(nameElement);

        //!- Multipiler
        double muliplier = i_otherEquipment.multiplier();
        nameElement = doc.createElement("Multipiler");
        nameElement.appendChild(doc.createTextNode(QString::number(muliplier)));
        otherEquipment.appendChild(nameElement);

        otherEquipments.appendChild(otherEquipment);
    }
    return otherEquipments;
}

boost::optional<QDomElement> ForwardTranslator::translateMyElectronicEquipment(const openstudio::model::Space& space, QDomDocument& doc){

    QDomElement electricEquipments = doc.createElement("ElectricEquipments");

    std::vector<model::ElectricEquipment> v_electricEquipment = space.spaceType().get().electricEquipment();
    std::sort(v_electricEquipment.begin(), v_electricEquipment.end(), WorkspaceObjectNameLess());

    for (const model::ElectricEquipment& i_electricEquipment : v_electricEquipment){

        QDomElement electricEquipment = doc.createElement("ElectricEquipment");

        //!- Name
        std::string name = i_electricEquipment.name().get();
        QDomElement nameElement = doc.createElement("Name");
        nameElement.appendChild(doc.createTextNode(toQString(name)));
        electricEquipment.appendChild(nameElement);

        //!- Calculation Method
        std::string calculationMethod = i_electricEquipment.electricEquipmentDefinition().designLevelCalculationMethod();
        nameElement = doc.createElement("CalculationMethod");
        nameElement.appendChild(doc.createTextNode(toQString(calculationMethod)));
        electricEquipment.appendChild(nameElement);

        //!- Design Level
        double designLevel = i_electricEquipment.designLevel().get();
        nameElement = doc.createElement("DesignLevel");
        nameElement.appendChild(doc.createTextNode(QString::number(designLevel)));
        electricEquipment.appendChild(nameElement);

        //!- Watt Per Space
        double wattPerSpace = i_electricEquipment.powerPerFloorArea().get();
        nameElement = doc.createElement("WattPerSpace");
        nameElement.appendChild(doc.createTextNode(QString::number(wattPerSpace)));
        electricEquipment.appendChild(nameElement);

        //!- Watt Per Person
        double wattPerPerson = i_electricEquipment.powerPerPerson().get();
        nameElement = doc.createElement("WattPerPerson");
        nameElement.appendChild(doc.createTextNode(QString::number(wattPerPerson)));
        electricEquipment.appendChild(nameElement);

        //!- Multipiler
        double muliplier = i_electricEquipment.multiplier();
        nameElement = doc.createElement("Multipiler");
        nameElement.appendChild(doc.createTextNode(QString::number(muliplier)));
        electricEquipment.appendChild(nameElement);

        electricEquipments.appendChild(electricEquipment);
    }
    return electricEquipments;
}

boost::optional<QDomElement> ForwardTranslator::translateFacility(const openstudio::model::Facility& facility, QDomDocument& doc)
{
    QDomElement result = doc.createElement("Campus");
    m_translatedObjects[facility.handle()] = result;

    boost::optional<std::string> name = facility.name();

    // id
    result.setAttribute("id", "Facility");

    // name
    QDomElement nameElement = doc.createElement("Name");
    result.appendChild(nameElement);
    if (name){
        nameElement.appendChild(doc.createTextNode(QString::fromStdString(name.get())));
    }else{
        nameElement.appendChild(doc.createTextNode("Facility"));
    }

    model::Model model = facility.model();

    // todo: translate location

    // translate building
    boost::optional<model::Building> building = model.getOptionalUniqueModelObject<model::Building>();
    if (building){
        boost::optional<QDomElement> buildingElement = translateBuilding(*building, doc);
        if (buildingElement){
            result.appendChild(*buildingElement);
        }
    }

    // translate surfaces
    std::vector<model::Surface> surfaces = model.getConcreteModelObjects<model::Surface>();
    if (m_progressBar){
        m_progressBar->setWindowTitle(toString("Translating Surfaces"));
        m_progressBar->setMinimum(0);
        m_progressBar->setMaximum((int)surfaces.size());
        m_progressBar->setValue(0);
    }

    for (const model::Surface& surface : surfaces){
        boost::optional<QDomElement> surfaceElement = translateSurface(surface, doc);
        if (surfaceElement){
            result.appendChild(*surfaceElement);
        }

        if (m_progressBar){
            m_progressBar->setValue(m_progressBar->value() + 1);
        }
    }


    return result;
}

boost::optional<QDomElement> ForwardTranslator::translateMyPeople(const openstudio::model::Space& space, QDomDocument& doc){

    QDomElement peopleElement = doc.createElement("Peoples");
    std::vector<model::People> v_people = space.people();
    for (const model::People& i_people : v_people){
        //TODO: translate people
    }

    return peopleElement;
}

boost::optional<QDomElement> ForwardTranslator::translateMyGasEquipment(const openstudio::model::Space& space, QDomDocument& doc){

    QDomElement gasEquipmentElement = doc.createElement("GasEquipments");
    std::vector<model::GasEquipment> v_gasEquipment = space.gasEquipment();
    for (const model::GasEquipment& i_gasEquipment : v_gasEquipment){
        //TODO: translate Gas Equipment
    }

    return gasEquipmentElement;
}

boost::optional<QDomElement> ForwardTranslator::translateMyWaterUse(const openstudio::model::Space space, QDomDocument& doc){

    QDomElement waterUseElement = doc.createElement("WaterUses");
    std::vector<model::WaterUseEquipment> v_waterUse = space.waterUseEquipment();
    for (const model::WaterUseEquipment& i_waterUse : v_waterUse){
        //TODO: translate Water Use
    }

    return waterUseElement;
}

boost::optional<QDomElement> ForwardTranslator::translateMyLuminaire(const openstudio::model::Space& space, QDomDocument& doc){
    QDomElement luminaireElement = doc.createElement("Luminaires");
    std::vector<model::Luminaire> v_luminaire = space.luminaires();
    for (const model::Luminaire& i_luminaire : v_luminaire){
        //TODO: translate luminaire
    }

    return luminaireElement;
}

boost::optional<QDomElement> ForwardTranslator::translateMyConstruction(const openstudio::model::Space& space, QDomDocument& doc){

    QDomElement surfacesElement = doc.createElement("Surfaces");
    std::vector<model::Surface> v_serface = space.surfaces();

    for (const model::Surface& i_surface : v_serface){
        //TODO: translate construction
        QDomElement surfaceElement = doc.createElement("Surface");

        QDomElement nameElement = doc.createElement("Name");
        nameElement.appendChild(doc.createTextNode(toQString(i_surface.name().get())));
        surfaceElement.appendChild(nameElement);


        QDomElement constructElement = doc.createElement("Construction");
        std::string mat = i_surface.construction().get().name().get();
        constructElement.appendChild(doc.createTextNode(toQString(mat)));
        surfaceElement.appendChild(constructElement);

        surfacesElement.appendChild(surfaceElement);
    }

    return surfacesElement;
}

boost::optional<QDomElement> ForwardTranslator::translateBuilding(const openstudio::model::Building& building, QDomDocument& doc)
{
    QDomElement result = doc.createElement("Building");
    m_translatedObjects[building.handle()] = result;

    // id
    std::string name = building.name().get();
    result.setAttribute("id", escapeName(name));

    // building type
    //result.setAttribute("buildingType", "Office");
    result.setAttribute("buildingType", "Unknown");

    // space type
    boost::optional<model::SpaceType> spaceType = building.spaceType();
    if (spaceType){
        //std::string spaceTypeName = spaceType->name().get();
        // todo: map to BEC types
        //result.setAttribute("buildingType", escapeName(spaceTypeName));
    }

    // name
    QDomElement nameElement = doc.createElement("Name");
    result.appendChild(nameElement);
    nameElement.appendChild(doc.createTextNode(QString::fromStdString(name)));

    // area
    QDomElement areaElement = doc.createElement("Area");
    result.appendChild(areaElement);
    areaElement.appendChild(doc.createTextNode(QString::number(building.floorArea())));

    // translate spaces
    std::vector<model::Space> spaces = building.spaces();
    if (m_progressBar){
        m_progressBar->setWindowTitle(toString("Translating Spaces"));
        m_progressBar->setMinimum(0);
        m_progressBar->setMaximum((int)spaces.size());
        m_progressBar->setValue(0);
    }

    for (const model::Space& space : spaces){
        boost::optional<QDomElement> spaceElement = translateSpace(space, doc);
        if (spaceElement){
            result.appendChild(*spaceElement);
        }

        if (m_progressBar){
            m_progressBar->setValue(m_progressBar->value() + 1);
        }
    }

    return result;
}

boost::optional<QDomElement> ForwardTranslator::translateSpace(const openstudio::model::Space& space, QDomDocument& doc)
{
    QDomElement result = doc.createElement("Space");
    m_translatedObjects[space.handle()] = result;

    // id
    std::string name = space.name().get();
    result.setAttribute("id", escapeName(name));

    // space type
    boost::optional<model::SpaceType> spaceType = space.spaceType();
    if (spaceType){
        //std::string spaceTypeName = spaceType->name().get();
        // todo: map to BEC types
        //result.setAttribute("spaceType", escapeName(spaceTypeName));
    }

    // thermal zone
    boost::optional<model::ThermalZone> thermalZone = space.thermalZone();
    if (thermalZone){
        std::string thermalZoneName = thermalZone->name().get();
        result.setAttribute("zoneIdRef", escapeName(thermalZoneName));
    }

    // name
    QDomElement nameElement = doc.createElement("Name");
    result.appendChild(nameElement);
    nameElement.appendChild(doc.createTextNode(QString::fromStdString(name)));

    // append floor area
    double area = space.floorArea();
    QDomElement areaElement = doc.createElement("Area");
    areaElement.appendChild(doc.createTextNode(QString::number(area)));
    result.appendChild(areaElement);
    
    // append volume
    double volume = space.volume();
    QDomElement volumeElement = doc.createElement("Volume");
    volumeElement.appendChild(doc.createTextNode(QString::number(volume)));
    result.appendChild(volumeElement);

    // Load
    QDomElement loadElement = doc.createElement("Loads");

    // lighting
    double lightPower = space.lightingPower();
    QDomElement lightElement = doc.createElement("light");
    lightElement.appendChild(doc.createTextNode(QString::number(lightPower)));
    loadElement.appendChild(lightElement);

    // Electronic
    double electPower = space.electricEquipmentPower();
    QDomElement electElement = doc.createElement("Electonic");
    electElement.appendChild(doc.createTextNode(QString::number(electPower)));
    loadElement.appendChild(electElement);

    //	std::vector<model::Lights,std::allocator<model::Lights>> v_lights = space.lights();

    result.appendChild(loadElement);

    return result;
}

boost::optional<QDomElement> ForwardTranslator::translateSurface(const openstudio::model::Surface& surface, QDomDocument& doc)
{
    // return if already translated
    if (m_translatedObjects.find(surface.handle()) != m_translatedObjects.end()){
        return boost::none;
    }

    QDomElement result = doc.createElement("Surface");
    m_translatedObjects[surface.handle()] = result;

    // id
    std::string name = surface.name().get();
    result.setAttribute("id", escapeName(name));

    // DLM: currently unhandled
    //Shade
    //FreestandingColumn
    //EmbeddedColumn

    if (surface.isAirWall()){
        result.setAttribute("surfaceType", "Air");
    }else{
        std::string surfaceType = surface.surfaceType();
        std::string outsideBoundaryCondition = surface.outsideBoundaryCondition();
        if (istringEqual("Wall", surfaceType)){
            if (istringEqual("Outdoors", outsideBoundaryCondition)){
                result.setAttribute("surfaceType", "ExteriorWall");
            }else if (istringEqual("Surface", outsideBoundaryCondition)){
                result.setAttribute("surfaceType", "InteriorWall");
            }else if (surface.isGroundSurface()){
                result.setAttribute("surfaceType", "UndergroundWall");
            }
        }else if (istringEqual("RoofCeiling", surfaceType)){
            if (istringEqual("Outdoors", outsideBoundaryCondition)){
                result.setAttribute("surfaceType", "Roof");
            }else if (istringEqual("Surface", outsideBoundaryCondition)){
                result.setAttribute("surfaceType", "Ceiling");
            }else if (surface.isGroundSurface()){
                result.setAttribute("surfaceType", "UndergroundCeiling");
            }
        }else if (istringEqual("Floor", surfaceType)){
            if (istringEqual("Outdoors", outsideBoundaryCondition)){
                result.setAttribute("surfaceType", "RaisedFloor");
            }else if (surface.isGroundSurface()){
                result.setAttribute("surfaceType", "UndergroundSlab"); // or SlabOnGrade?
            }else if (istringEqual("Surface", outsideBoundaryCondition)){
                result.setAttribute("surfaceType", "InteriorFloor");
            }
        }
    }

    // construction
    boost::optional<model::ConstructionBase> construction = surface.construction();
    if (construction){
        //std::string constructionName = construction->name().get();
        // todo:: translate construction
        //result.setAttribute("constructionIdRef", "constructionName");
    }

    // this space
    Transformation transformation;
    boost::optional<model::Space> space = surface.space();
    if (space){
        transformation = space->siteTransformation();

        std::string spaceName = space->name().get();
        QDomElement adjacentSpaceIdElement = doc.createElement("AdjacentSpaceId");
        result.appendChild(adjacentSpaceIdElement);
        adjacentSpaceIdElement.setAttribute("spaceIdRef", escapeName(spaceName));
    }

    // adjacent surface
    boost::optional<model::Surface> adjacentSurface = surface.adjacentSurface();
    if (adjacentSurface){
        boost::optional<model::Space> adjacentSpace = adjacentSurface->space();
        if (adjacentSpace){
            std::string adjacentSpaceName = adjacentSpace->name().get();
            QDomElement adjacentSpaceIdElement = doc.createElement("AdjacentSpaceId");
            result.appendChild(adjacentSpaceIdElement);
            adjacentSpaceIdElement.setAttribute("spaceIdRef", escapeName(adjacentSpaceName));

            // count adjacent surface as translated
            m_translatedObjects[adjacentSurface->handle()] = result;
        }
    }

    // transform vertices to world coordinates
    Point3dVector vertices = transformation*surface.vertices();

    // check if we can make rectangular geometry
    OptionalVector3d outwardNormal = getOutwardNormal(vertices);
    double area = surface.grossArea();
    if (outwardNormal && area > 0){

        // get tilt, duplicate code in planar surface
        Vector3d up(0.0,0.0,1.0);
        double tiltRadians = getAngle(*outwardNormal, up);

        // get azimuth, duplicate code in planar surface
        Vector3d north(0.0,1.0,0.0);
        double azimuthRadians = getAngle(*outwardNormal, north);
        if (outwardNormal->x() < 0.0) {
            azimuthRadians = -azimuthRadians + 2.0*boost::math::constants::pi<double>();
        }

        // transform vertices to face coordinates
        Transformation faceTransformation = Transformation::alignFace(vertices);
        Point3dVector faceVertices = faceTransformation.inverse()*vertices;
        BoundingBox faceBoundingBox;
        faceBoundingBox.addPoints(faceVertices);
        double width = faceBoundingBox.maxX().get() - faceBoundingBox.minX().get();
        double height = faceBoundingBox.maxY().get() - faceBoundingBox.minY().get();
        double areaCorrection = 1.0;
        if (width > 0 && height > 0){
            areaCorrection = sqrt(area/(width*height));
        }

        // pick lower left corner vertex in face coordinates
        double minY = std::numeric_limits<double>::max();
        double minX = std::numeric_limits<double>::max();
        size_t llcIndex = 0;
        size_t N = vertices.size();
        for (size_t i = 0; i < N; ++i){
            OS_ASSERT(std::abs(faceVertices[i].z()) < 0.001);
            if ((minY > faceVertices[i].y()) || ((minY > faceVertices[i].y() - 0.00001) && (minX > faceVertices[i].x()))){
                llcIndex = i;
                minY = faceVertices[i].y();
                minX = faceVertices[i].x();
            }
        }
        Point3d vertex = vertices[llcIndex];

        // rectangular geometry
        QDomElement rectangularGeometryElement = doc.createElement("RectangularGeometry");
        result.appendChild(rectangularGeometryElement);

        QDomElement azimuthElement = doc.createElement("Azimuth");
        rectangularGeometryElement.appendChild(azimuthElement);
        azimuthElement.appendChild(doc.createTextNode(QString::number(radToDeg(azimuthRadians))));

        QDomElement cartesianPointElement = doc.createElement("CartesianPoint");
        rectangularGeometryElement.appendChild(cartesianPointElement);

        QDomElement coordinateXElement = doc.createElement("Coordinate");
        cartesianPointElement.appendChild(coordinateXElement);
        coordinateXElement.appendChild(doc.createTextNode(QString::number(vertex.x())));

        QDomElement coordinateYElement = doc.createElement("Coordinate");
        cartesianPointElement.appendChild(coordinateYElement);
        coordinateYElement.appendChild(doc.createTextNode(QString::number(vertex.y())));

        QDomElement coordinateZElement = doc.createElement("Coordinate");
        cartesianPointElement.appendChild(coordinateZElement);
        coordinateZElement.appendChild(doc.createTextNode(QString::number(vertex.z())));

        QDomElement tiltElement = doc.createElement("Tilt");
        rectangularGeometryElement.appendChild(tiltElement);
        tiltElement.appendChild(doc.createTextNode(QString::number(radToDeg(tiltRadians))));

        QDomElement widthElement = doc.createElement("Width");
        rectangularGeometryElement.appendChild(widthElement);
        widthElement.appendChild(doc.createTextNode(QString::number(areaCorrection*width)));

        QDomElement heightElement = doc.createElement("Height");
        rectangularGeometryElement.appendChild(heightElement);
        heightElement.appendChild(doc.createTextNode(QString::number(areaCorrection*height)));
    }

    // planar geometry
    QDomElement planarGeometryElement = doc.createElement("PlanarGeometry");
    result.appendChild(planarGeometryElement);

    QDomElement polyLoopElement = doc.createElement("PolyLoop");
    planarGeometryElement.appendChild(polyLoopElement);
    for (const Point3d& vertex : vertices){
        QDomElement cartesianPointElement = doc.createElement("CartesianPoint");
        polyLoopElement.appendChild(cartesianPointElement);

        QDomElement coordinateXElement = doc.createElement("Coordinate");
        cartesianPointElement.appendChild(coordinateXElement);
        coordinateXElement.appendChild(doc.createTextNode(QString::number(vertex.x())));

        QDomElement coordinateYElement = doc.createElement("Coordinate");
        cartesianPointElement.appendChild(coordinateYElement);
        coordinateYElement.appendChild(doc.createTextNode(QString::number(vertex.y())));

        QDomElement coordinateZElement = doc.createElement("Coordinate");
        cartesianPointElement.appendChild(coordinateZElement);
        coordinateZElement.appendChild(doc.createTextNode(QString::number(vertex.z())));
    }

    // translate sub surfaces
    for (const model::SubSurface& subSurface : surface.subSurfaces()){
        boost::optional<QDomElement> openingElement = translateSubSurface(subSurface, transformation, doc);
        if (openingElement){
            result.appendChild(*openingElement);
        }
    }

    return result;
}

boost::optional<QDomElement> ForwardTranslator::translateSubSurface(const openstudio::model::SubSurface& subSurface, const openstudio::Transformation& transformation, QDomDocument& doc)
{
    // return if already translated
    if (m_translatedObjects.find(subSurface.handle()) != m_translatedObjects.end()){
        return boost::none;
    }

    QDomElement result = doc.createElement("Opening");
    m_translatedObjects[subSurface.handle()] = result;

    // id
    std::string name = subSurface.name().get();
    result.setAttribute("id", escapeName(name));

    // construction
    boost::optional<model::ConstructionBase> construction = subSurface.construction();
    if (construction){
        //std::string constructionName = construction->name().get();
        // todo: translate construction
        // result.setAttribute("constructionIdRef", "constructionName");
    }

    // DLM: currently unhandled
    // OperableSkylight
    // SlidingDoor

    if (subSurface.isAirWall()){
        result.setAttribute("openingType", "Air");
    }else{
        std::string subSurfaceType = subSurface.subSurfaceType();
        if (istringEqual("FixedWindow", subSurfaceType)){
            result.setAttribute("openingType", "FixedWindow");
        }else if(istringEqual("OperableWindow", subSurfaceType)){
            result.setAttribute("openingType", "OperableWindow");
        }else if (istringEqual("Skylight", subSurfaceType)){
            result.setAttribute("openingType", "FixedSkylight");
        }else if (istringEqual("Door", subSurfaceType)){
            result.setAttribute("openingType", "NonSlidingDoor");
        }else if (istringEqual("OverheadDoor", subSurfaceType)){
            result.setAttribute("openingType", "NonSlidingDoor");
        }
    }

    // transform vertices to world coordinates
    Point3dVector vertices = transformation*subSurface.vertices();

    // check if we can make rectangular geometry
    OptionalVector3d outwardNormal = getOutwardNormal(vertices);
    double area = subSurface.grossArea();
    if (outwardNormal && area > 0){

        // get tilt, duplicate code in planar surface
        Vector3d up(0.0,0.0,1.0);
        double tiltRadians = getAngle(*outwardNormal, up);

        // get azimuth, duplicate code in planar surface
        Vector3d north(0.0,1.0,0.0);
        double azimuthRadians = getAngle(*outwardNormal, north);
        if (outwardNormal->x() < 0.0) {
            azimuthRadians = -azimuthRadians + 2.0*boost::math::constants::pi<double>();
        }

        // transform vertices to face coordinates
        Transformation faceTransformation = Transformation::alignFace(vertices);
        Point3dVector faceVertices = faceTransformation.inverse()*vertices;
        BoundingBox faceBoundingBox;
        faceBoundingBox.addPoints(faceVertices);
        double width = faceBoundingBox.maxX().get() - faceBoundingBox.minX().get();
        double height = faceBoundingBox.maxY().get() - faceBoundingBox.minY().get();
        double areaCorrection = 1.0;
        if (width > 0 && height > 0){
            areaCorrection = sqrt(area/(width*height));
        }

        // pick lower left corner vertex in face coordinates
        double minY = std::numeric_limits<double>::max();
        double minX = std::numeric_limits<double>::max();
        size_t llcIndex = 0;
        size_t N = vertices.size();
        for (size_t i = 0; i < N; ++i){
            OS_ASSERT(std::abs(faceVertices[i].z()) < 0.001);
            if ((minY > faceVertices[i].y()) || ((minY > faceVertices[i].y() - 0.00001) && (minX > faceVertices[i].x()))){
                llcIndex = i;
                minY = faceVertices[i].y();
                minX = faceVertices[i].x();
            }
        }
        Point3d vertex = vertices[llcIndex];

        // rectangular geometry
        QDomElement rectangularGeometryElement = doc.createElement("RectangularGeometry");
        result.appendChild(rectangularGeometryElement);

        QDomElement azimuthElement = doc.createElement("Azimuth");
        rectangularGeometryElement.appendChild(azimuthElement);
        azimuthElement.appendChild(doc.createTextNode(QString::number(radToDeg(azimuthRadians))));

        QDomElement cartesianPointElement = doc.createElement("CartesianPoint");
        rectangularGeometryElement.appendChild(cartesianPointElement);

        QDomElement coordinateXElement = doc.createElement("Coordinate");
        cartesianPointElement.appendChild(coordinateXElement);
        coordinateXElement.appendChild(doc.createTextNode(QString::number(vertex.x())));

        QDomElement coordinateYElement = doc.createElement("Coordinate");
        cartesianPointElement.appendChild(coordinateYElement);
        coordinateYElement.appendChild(doc.createTextNode(QString::number(vertex.y())));

        QDomElement coordinateZElement = doc.createElement("Coordinate");
        cartesianPointElement.appendChild(coordinateZElement);
        coordinateZElement.appendChild(doc.createTextNode(QString::number(vertex.z())));

        QDomElement tiltElement = doc.createElement("Tilt");
        rectangularGeometryElement.appendChild(tiltElement);
        tiltElement.appendChild(doc.createTextNode(QString::number(radToDeg(tiltRadians))));

        QDomElement widthElement = doc.createElement("Width");
        rectangularGeometryElement.appendChild(widthElement);
        widthElement.appendChild(doc.createTextNode(QString::number(areaCorrection*width)));

        QDomElement heightElement = doc.createElement("Height");
        rectangularGeometryElement.appendChild(heightElement);
        heightElement.appendChild(doc.createTextNode(QString::number(areaCorrection*height)));
    }

    // planar geometry
    QDomElement planarGeometryElement = doc.createElement("PlanarGeometry");
    result.appendChild(planarGeometryElement);

    // translate vertices
    QDomElement polyLoopElement = doc.createElement("PolyLoop");
    planarGeometryElement.appendChild(polyLoopElement);
    for (const Point3d& vertex : vertices){
        QDomElement cartesianPointElement = doc.createElement("CartesianPoint");
        polyLoopElement.appendChild(cartesianPointElement);

        QDomElement coordinateXElement = doc.createElement("Coordinate");
        cartesianPointElement.appendChild(coordinateXElement);
        coordinateXElement.appendChild(doc.createTextNode(QString::number(vertex.x())));

        QDomElement coordinateYElement = doc.createElement("Coordinate");
        cartesianPointElement.appendChild(coordinateYElement);
        coordinateYElement.appendChild(doc.createTextNode(QString::number(vertex.y())));

        QDomElement coordinateZElement = doc.createElement("Coordinate");
        cartesianPointElement.appendChild(coordinateZElement);
        coordinateZElement.appendChild(doc.createTextNode(QString::number(vertex.z())));
    }

    return result;
}

boost::optional<QDomElement> ForwardTranslator::translateThermalZone(const openstudio::model::ThermalZone& thermalZone, QDomDocument& doc)
{
    QDomElement result = doc.createElement("Zone");
    m_translatedObjects[thermalZone.handle()] = result;

    // id
    std::string name = thermalZone.name().get();
    result.setAttribute("id", escapeName(name));

    // name
    QDomElement nameElement = doc.createElement("Name");
    result.appendChild(nameElement);
    nameElement.appendChild(doc.createTextNode(QString::fromStdString(name)));

    return result;
}

} // bec
} // openstudio


