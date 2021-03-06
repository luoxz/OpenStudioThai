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

#ifndef RULESET_EMBEDDEDRUBYUSERSCRIPTINFOGETTER_HPP
#define RULESET_EMBEDDEDRUBYUSERSCRIPTINFOGETTER_HPP

#include "RubyUserScriptInfoGetter.hpp"

#include "../model/Model.hpp"
#include "../model/Model_Impl.hpp"

namespace openstudio {
namespace ruleset {

  /** Should only be instantiated in C++ Applications. For an example of how to use this, see
   *  src/ruleset/test/EmbeddedRuby_GTest.cpp. */
  template<typename RubyInterpreterType>
  class EmbeddedRubyUserScriptInfoGetter : public RubyUserScriptInfoGetter {
   public:
    EmbeddedRubyUserScriptInfoGetter(const std::shared_ptr<RubyInterpreterType>& rubyInterpreter)
      : m_rubyInterpreter(rubyInterpreter)
    {
      m_rubyInterpreter->template registerType<openstudio::BCLMeasure>("openstudio::BCLMeasure");
      m_rubyInterpreter->template registerType<openstudio::Workspace>("openstudio::Workspace");
      m_rubyInterpreter->template registerType<openstudio::model::Model>("openstudio::model::Model");
      //m_rubyInterpreter->template registerType<openstudio::ruleset::OSArgument>("openstudio::ruleset::OSArgument");
      m_rubyInterpreter->template registerType<openstudio::ruleset::RubyUserScriptInfo>("openstudio::ruleset::RubyUserScriptInfo");

      m_rubyInterpreter->evalString(infoExtractorRubyFunction());
    }

    virtual ~EmbeddedRubyUserScriptInfoGetter() {}

    RubyUserScriptInfo getInfo(const BCLMeasure& measure) {
      return m_rubyInterpreter->template execWithReturn<RubyUserScriptInfo>(
          "infoExtractor", measure, model::OptionalModel(), OptionalWorkspace());
    }

    RubyUserScriptInfo getInfo(const BCLMeasure& measure, const Workspace& workspace) {
      if (model::OptionalModel model = workspace.optionalCast<model::Model>()) {
        return m_rubyInterpreter->template execWithReturn<RubyUserScriptInfo>(
              "infoExtractor", measure, model::OptionalModel(model), OptionalWorkspace());
      }
      else {
        return m_rubyInterpreter->template execWithReturn<RubyUserScriptInfo>(
            "infoExtractor", measure, model::OptionalModel(), OptionalWorkspace(workspace));
      }
    }

    RubyUserScriptInfo getInfo(const BCLMeasure& measure,
                               const model::Model& model,
                               const Workspace& workspace)
    {
      return m_rubyInterpreter->template execWithReturn<RubyUserScriptInfo>(
          "infoExtractor", measure, model::OptionalModel(model), OptionalWorkspace(workspace));
    }

   private:
    std::shared_ptr<RubyInterpreterType> m_rubyInterpreter;
  };

} // ruleset
} // openstudio

#endif // RULESET_EMBEDDEDRUBYUSERSCRIPTINFOGETTER_HPP
