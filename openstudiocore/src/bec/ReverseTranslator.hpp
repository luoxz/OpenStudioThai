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

// krishnan adding a test comment for SVN check-in

#ifndef BEC_REVERSETRANSLATOR_HPP
#define BEC_REVERSETRANSLATOR_HPP

#include "BECAPI.hpp"

#include "../utilities/core/Path.hpp"
#include "../utilities/core/Optional.hpp"
#include "../utilities/core/Logger.hpp"
#include "../utilities/core/StringStreamLogSink.hpp"

#include "../utilities/units/Unit.hpp"

class QDomDocument;
class QDomElement;
class QDomNodeList;

namespace openstudio {

  class ProgressBar;

namespace model {
  class Model;
  class ModelObject;
  class Surface;
}

namespace bec {

  class BEC_API ReverseTranslator {
  public:

    ReverseTranslator();

    virtual ~ReverseTranslator();
    
    boost::optional<openstudio::model::Model> loadModel(const openstudio::path& path, ProgressBar* progressBar = nullptr);
    
    /** Get warning messages generated by the last translation. */
    std::vector<LogMessage> warnings() const;

    /** Get error messages generated by the last translation. */
    std::vector<LogMessage> errors() const;

  private:
    openstudio::Unit m_temperatureUnit;
    openstudio::Unit m_lengthUnit;
    double m_lengthMultiplier;
    openstudio::Unit m_areaUnit;
    openstudio::Unit m_volumeUnit;
    bool m_useSIUnitsForResults;
  
  private:

    std::string escapeName(QString name);

    boost::optional<openstudio::model::Model> convert(const QDomDocument& doc);
    boost::optional<openstudio::model::Model> translateBEC(const QDomElement& element, const QDomDocument& doc);
    boost::optional<openstudio::model::ModelObject> translateCampus(const QDomElement& element, const QDomDocument& doc, openstudio::model::Model& model);
    boost::optional<openstudio::model::ModelObject> translateBuilding(const QDomElement& element, const QDomDocument& doc, openstudio::model::Model& model);
    boost::optional<openstudio::model::ModelObject> translateBuildingStory(const QDomElement& element, const QDomDocument& doc, openstudio::model::Model& model);
    boost::optional<openstudio::model::ModelObject> translateThermalZone(const QDomElement& element, const QDomDocument& doc, openstudio::model::Model& model);
    boost::optional<openstudio::model::ModelObject> translateConstruction(const QDomElement& element, const QDomNodeList& layerElements, const QDomDocument& doc, openstudio::model::Model& model);
    boost::optional<openstudio::model::ModelObject> translateMaterial(const QDomElement& element, const QDomDocument& doc, openstudio::model::Model& model);
    boost::optional<openstudio::model::ModelObject> translateScheduleDay(const QDomElement& element, const QDomDocument& doc, openstudio::model::Model& model);
    boost::optional<openstudio::model::ModelObject> translateScheduleWeek(const QDomElement& element, const QDomDocument& doc, openstudio::model::Model& model);
    boost::optional<openstudio::model::ModelObject> translateSchedule(const QDomElement& element, const QDomDocument& doc, openstudio::model::Model& model);
    boost::optional<openstudio::model::ModelObject> translateSpace(const QDomElement& element, const QDomDocument& doc, openstudio::model::Model& model);
    boost::optional<openstudio::model::ModelObject> translateSurface(const QDomElement& element, const QDomDocument& doc, openstudio::model::Model& model);
    boost::optional<openstudio::model::ModelObject> translateSubSurface(const QDomElement& element, const QDomDocument& doc, openstudio::model::Surface& surface);
      
    StringStreamLogSink m_logSink;

    ProgressBar* m_progressBar;

    REGISTER_LOGGER("openstudio.bec.ReverseTranslator");
  };

} // bec
} // openstudio

#endif // BEC_REVERSETRANSLATOR_HPP
