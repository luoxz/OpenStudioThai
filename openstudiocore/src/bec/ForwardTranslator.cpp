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
#include "../model/Facility.hpp"
#include "../model/Facility_Impl.hpp"
#include "../model/Building.hpp"
#include "../model/Building_Impl.hpp"
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

#include "../model/ThermalZone.hpp"
#include "../model/ThermalZone_Impl.hpp"

#include "../utilities/geometry/Transformation.hpp"
#include "../utilities/geometry/EulerAngles.hpp"
#include "../utilities/geometry/BoundingBox.hpp"
#include "../utilities/geometry/Geometry.hpp"
#include "../utilities/plot/ProgressBar.hpp"
#include "../utilities/core/Assert.hpp"


#include <boost/math/constants/constants.hpp>

#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QThread>

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

  bool ForwardTranslator::modelTobec(const openstudio::model::Model& model, const openstudio::path& path, ProgressBar* progressBar)
  {
    m_progressBar = progressBar;

    m_logSink.setThreadId(QThread::currentThread());

    m_logSink.resetStringStream();

    boost::optional<QDomDocument> doc = this->translateModel(model);
    if (!doc){
      return false;
    }

    QFile file(toQString(path));
    if (file.open(QFile::WriteOnly)){
      QTextStream textStream(&file);
      textStream << doc->toString(2);
      file.close();
      return true;
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

  boost::optional<QDomDocument> ForwardTranslator::translateModel(const openstudio::model::Model& model)
  {
    QDomDocument doc;
    doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");

    QDomElement BECElement = doc.createElement("BEC");
    doc.appendChild(BECElement);
    BECElement.setAttribute("xmlns", "http://www.bec.org/schema");
    BECElement.setAttribute("xmlns:xhtml", "http://www.w3.org/1999/xhtml");
    BECElement.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    BECElement.setAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
    BECElement.setAttribute("xsi:schemaLocation", "http://www.bec.org/schema http://www.bec.org/schema/0-37/GreenBuildingXML.xsd");
    BECElement.setAttribute("temperatureUnit", "C");
    BECElement.setAttribute("lengthUnit", "Meters");
    BECElement.setAttribute("areaUnit", "SquareMeters");
    BECElement.setAttribute("volumeUnit", "CubicMeters");
    BECElement.setAttribute("useSIUnitsForResults", "true");
    BECElement.setAttribute("version", "0.37");

	boost::optional<QDomElement> myModel = translateMyModel(model,doc);
	BECElement.appendChild(*myModel);

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
