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

#ifndef BEC_FORWARDTRANSLATOR_HPP
#define BEC_FORWARDTRANSLATOR_HPP

#include "BECAPI.hpp"

#include "../utilities/core/Path.hpp"
#include "../utilities/core/Optional.hpp"
#include "../utilities/core/Logger.hpp"
#include "../utilities/core/StringStreamLogSink.hpp"
#include "../model/OtherEquipment.hpp"
#include "../model/Lights.hpp"
#include "../model/ElectricEquipment.hpp"
#include "../model/ModelObject.hpp"

#include <map>
#include <QDomElement>

class QDomDocument;
class QDomElement;
class QDomNodeList;

namespace openstudio {

  class ProgressBar;
  class Transformation;

namespace model {
  class Model;
  class ModelObject;
  class Material;
  class ConstructionBase;
  class Facility;
  class Building;
  class BuildingStory;
  class ThermalZone;
  class Space;
  class Surface;
  class SubSurface;
  class PlantLoop;
  class Loop;
}

namespace bec {

  class BEC_API ForwardTranslator {
  public:
    
    ForwardTranslator();

    virtual ~ForwardTranslator();

    bool modelTobec(const openstudio::model::Model& model
                    , const openstudio::path& path
                    , ProgressBar* progressBar = nullptr
                    , std::string *bvName=nullptr, QHash<QString, QList<double> > *sunlits=nullptr);

      /** Get warning messages generated by the last translation. */
    std::vector<LogMessage> warnings() const;

    /** Get error messages generated by the last translation. */
    std::vector<LogMessage> errors() const;

    QString GenParantLoopName(const QString &parantLoopName);

  private:

    QString escapeName(const std::string& name);

    //Util function.
    QDomElement createTagWithText(QDomElement& parent
                                  , const QString &tag
                                  , const QString &text=QString());

    //Create BEC XML
    void doEnvelope(const openstudio::model::Model& model, QDomElement& parent);
        void doMaterial(const openstudio::model::Model& model, QDomElement& parent);
        void doTransparentMaterial(const openstudio::model::Model& model, QDomElement& parent);
        void doAirGapMaterial(const openstudio::model::Model& model, QDomElement& parent);
        void doComponentLoop(const openstudio::model::Model& model, QDomElement& parent);

        void doComponentOfSection(model::Surface &surface
                                  , QDomElement &OpaqueComponentList
                                  , QDomElement &OpaqueComponentDetail
                                  , QDomElement &transparentComponentList
                                  , QHash<QString, int> &componentCheck);

            void doOpaqueComponentList(const model::Model &model, QDomElement &parent);
                void doOpaqueList(const model::Space &model, QDomElement &parent);
            void doOpaqueComponentDetail(const model::Model &model, QDomElement &parent);
            void doTransparentComponentList(const model::Model &model, QDomElement &parent);

        void doSectionOfWall(const openstudio::model::Model& model, QDomElement& Envelope);

    void doModelLoop(const openstudio::model::Model& model, QDomElement& becInput);

    void doLightingSystem(const openstudio::model::Space& space, QDomElement& LightingSystem, QHash<QString, bool> &checkDup);
    void doHotWaterSystem(const model::Model &model, QDomElement& hotWaterSystem);
    void doOtherEquipment(const openstudio::model::Space& space, QDomElement& OtherEquipment, QHash<QString, bool>& checkDup);
    void doACSystem(const model::Model &model, QDomElement& ACSystem);
    void doAirLoop(QDomElement &CentralACList, QDomElement CentralACDetail, model::Loop* loop, const QString parantLoopName, QHash<QString, bool> &duplicate);
    void doPV(const model::Model &model, QDomElement& becInput);

    void DoOtherEquipment(std::vector<model::OtherEquipment>& electris, QDomElement &OtherEquipment, QHash<QString, bool>& checkDup);
    void DoElectricEquipment(std::vector<model::ElectricEquipment>& others, QDomElement &OtherEquipment, QHash<QString, bool>& checkDup);

    void doBuildingType(QDomElement& becInput, const QString &typeName);

//    void doACSystem(const openstudio::model::Model& model, QDomElement& root);
//    void doPVSystem(const openstudio::model::Model& model, QDomElement& root);
//    void doHotWaterSystem(const openstudio::model::Model& model, QDomElement& root);
//    void doOtherEquipment(const openstudio::model::Model& model, QDomElement& root);

    void DoLighting(QDomElement &LightingSystem
                            , const std::vector<model::Lights>& lights
                            , QHash<QString, bool>& checkDup);
    void DoLightBuilidingZone(const std::string & zonelistName
                                      , const std::vector<model::Lights>& lights
                                      , const std::vector<model::OtherEquipment>& others
                                      , const std::vector<model::ElectricEquipment>& elects);
    void doBuildingEnvelope(const openstudio::model::Model& model, QDomElement& becInput);

    // listed in translation order
    boost::optional<QDomDocument> translateModel(const openstudio::model::Model& model, const QString &type);
    boost::optional<QDomElement> translateFacility(const openstudio::model::Facility& facility, QDomDocument& doc);
    boost::optional<QDomElement> translateBuilding(const openstudio::model::Building& building, QDomDocument& doc);
    boost::optional<QDomElement> translateSpace(const openstudio::model::Space& space, QDomDocument& doc);
    boost::optional<QDomElement> translateSurface(const openstudio::model::Surface& surface, QDomDocument& doc);
    boost::optional<QDomElement> translateSubSurface(const openstudio::model::SubSurface& subSurface, const openstudio::Transformation& transformation, QDomDocument& doc);
    boost::optional<QDomElement> translateThermalZone(const openstudio::model::ThermalZone& thermalZone, QDomDocument& doc);
    boost::optional<QDomElement> translateMaterial(const openstudio::model::Material& material, QDomDocument& doc);
    boost::optional<QDomElement> translateConstructionBase(const openstudio::model::ConstructionBase& constructionBase, QDomDocument& doc);
	boost::optional<QDomElement> translateMyModel(const openstudio::model::Model& model, QDomDocument& doc);

	/* Custom translate */
	boost::optional<QDomElement> translateMySpace(const openstudio::model::Building& space, QDomDocument& doc);
	boost::optional<QDomElement> translateMyElectronicEquipment(const openstudio::model::Space& space, QDomDocument& doc);
	boost::optional<QDomElement> translateMyLights(const openstudio::model::Space& space, QDomDocument& doc);
	boost::optional<QDomElement> translateMyOtherEquipment(const openstudio::model::Space& space, QDomDocument& doc);
	boost::optional<QDomElement> translateMyPeople(const openstudio::model::Space& space, QDomDocument& doc);
	boost::optional<QDomElement> translateMyGasEquipment(const openstudio::model::Space& space, QDomDocument& doc);
	boost::optional<QDomElement> translateMyWaterUse(const openstudio::model::Space space, QDomDocument& doc);
	boost::optional<QDomElement> translateMyLuminaire(const openstudio::model::Space& space, QDomDocument& doc);
	boost::optional<QDomElement> translateMyConstruction(const openstudio::model::Space& space, QDomDocument& doc);

    std::map<openstudio::Handle, QDomElement> m_translatedObjects;

    QDomElement buildingEnvelope;
    QDomElement buildingZoneList;
    QDomElement buildingZoneDetail;
    QDomElement buildingZoneExteriorWall;
    QDomElement BuildingZoneLightingEQ;
    QDomElement BuildingZoneDXACUnit;
    QDomElement BuildingZoneCentralACEQ;
    QDomElement BuildingZoneOtherEquipment;

    QHash<QString, QList<double>>* _sunlits;
    QHash<QString, QList<QString>>  chilledEq;
    StringStreamLogSink m_logSink;
    ProgressBar* m_progressBar;
    QDomDocument* _doc;
    REGISTER_LOGGER("openstudio.bec.ForwardTranslator");
  };

} // bec
} // openstudio

#endif // BEC_FORWARDTRANSLATOR_HPP
