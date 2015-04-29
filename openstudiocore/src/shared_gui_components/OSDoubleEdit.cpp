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

#include "OSDoubleEdit.hpp"

#include "../model/ModelObject_Impl.hpp"

#include "../utilities/core/Assert.hpp"
#include "../utilities/core/Containers.hpp"
#include "../utilities/data/Attribute.hpp"

#include <QDoubleValidator>
#include <QFocusEvent>

#include <iomanip>

using openstudio::model::ModelObject;

namespace openstudio {

OSDoubleEdit2::OSDoubleEdit2( QWidget * parent )
  : m_isScientific(false)
{
  this->setFixedWidth(90);
  this->setAcceptDrops(false);
  setEnabled(false);

  m_doubleValidator = new QDoubleValidator();
  this->setValidator(m_doubleValidator);
}

void OSDoubleEdit2::bind(model::ModelObject& modelObject,
                         DoubleGetter get,
                         boost::optional<DoubleSetter> set,
                         boost::optional<NoFailAction> reset,
                         boost::optional<NoFailAction> autosize,
                         boost::optional<NoFailAction> autocalculate,
                         boost::optional<BasicQuery> isDefaulted,
                         boost::optional<BasicQuery> isAutosized,
                         boost::optional<BasicQuery> isAutocalculated)
{
  m_modelObject = modelObject;
  m_get = get;
  m_set = set;
  m_reset = reset;
  m_autosize = autosize;
  m_autocalculate = autocalculate;
  m_isDefaulted = isDefaulted;
  m_isAutosized = isAutosized;
  m_isAutocalculated = isAutocalculated;

  completeBind();
}

void OSDoubleEdit2::bind(model::ModelObject& modelObject,
                         OptionalDoubleGetter get,
                         boost::optional<DoubleSetter> set,
                         boost::optional<NoFailAction> reset,
                         boost::optional<NoFailAction> autosize,
                         boost::optional<NoFailAction> autocalculate,
                         boost::optional<BasicQuery> isDefaulted,
                         boost::optional<BasicQuery> isAutosized,
                         boost::optional<BasicQuery> isAutocalculated)
{
  m_modelObject = modelObject;
  m_getOptional = get;
  m_set = set;
  m_reset = reset;
  m_autosize = autosize;
  m_autocalculate = autocalculate;
  m_isDefaulted = isDefaulted;
  m_isAutosized = isAutosized;
  m_isAutocalculated = isAutocalculated;

  completeBind();
}

void OSDoubleEdit2::bind(model::ModelObject& modelObject,
                         DoubleGetter get,
                         DoubleSetterVoidReturn set,
                         boost::optional<NoFailAction> reset,
                         boost::optional<NoFailAction> autosize,
                         boost::optional<NoFailAction> autocalculate,
                         boost::optional<BasicQuery> isDefaulted,
                         boost::optional<BasicQuery> isAutosized,
                         boost::optional<BasicQuery> isAutocalculated)
{
  m_modelObject = modelObject;
  m_get = get;
  m_setVoidReturn = set;
  m_reset = reset;
  m_autosize = autosize;
  m_autocalculate = autocalculate;
  m_isDefaulted = isDefaulted;
  m_isAutosized = isAutosized;
  m_isAutocalculated = isAutocalculated;

  completeBind();
}

void OSDoubleEdit2::bind(model::ModelObject& modelObject,
                         OptionalDoubleGetter get,
                         DoubleSetterVoidReturn set,
                         boost::optional<NoFailAction> reset,
                         boost::optional<NoFailAction> autosize,
                         boost::optional<NoFailAction> autocalculate,
                         boost::optional<BasicQuery> isDefaulted,
                         boost::optional<BasicQuery> isAutosized,
                         boost::optional<BasicQuery> isAutocalculated)
{
  m_modelObject = modelObject;
  m_getOptional = get;
  m_setVoidReturn = set;
  m_reset = reset;
  m_autosize = autosize;
  m_autocalculate = autocalculate;
  m_isDefaulted = isDefaulted;
  m_isAutosized = isAutosized;
  m_isAutocalculated = isAutocalculated;

  completeBind();
}

void OSDoubleEdit2::bind(model::ModelExtensibleGroup& modelExtensibleGroup,
                         DoubleGetter get,
                         boost::optional<DoubleSetter> set,
                         boost::optional<NoFailAction> reset,
                         boost::optional<NoFailAction> autosize,
                         boost::optional<NoFailAction> autocalculate,
                         boost::optional<BasicQuery> isDefaulted,
                         boost::optional<BasicQuery> isAutosized,
                         boost::optional<BasicQuery> isAutocalculated)
{
  m_modelExtensibleGroup = modelExtensibleGroup;
  m_modelObject = modelExtensibleGroup.getOptionalObject<model::ModelObject>();
  m_get = get;
  m_set = set;
  m_reset = reset;
  m_autosize = autosize;
  m_autocalculate = autocalculate;
  m_isDefaulted = isDefaulted;
  m_isAutosized = isAutosized;
  m_isAutocalculated = isAutocalculated;

  completeBind();
}

void OSDoubleEdit2::bind(model::ModelExtensibleGroup& modelExtensibleGroup,
                         OptionalDoubleGetter get,
                         boost::optional<DoubleSetter> set,
                         boost::optional<NoFailAction> reset,
                         boost::optional<NoFailAction> autosize,
                         boost::optional<NoFailAction> autocalculate,
                         boost::optional<BasicQuery> isDefaulted,
                         boost::optional<BasicQuery> isAutosized,
                         boost::optional<BasicQuery> isAutocalculated)
{
  m_modelExtensibleGroup = modelExtensibleGroup;
  m_modelObject = modelExtensibleGroup.getOptionalObject<model::ModelObject>();
  m_getOptional = get;
  m_set = set;
  m_reset = reset;
  m_autosize = autosize;
  m_autocalculate = autocalculate;
  m_isDefaulted = isDefaulted;
  m_isAutosized = isAutosized;
  m_isAutocalculated = isAutocalculated;

  completeBind();
}

void OSDoubleEdit2::completeBind() {

  // only let one of autosize/autocalculate
  if ((m_isAutosized && m_isAutocalculated) || 
      (m_isAutosized && m_autocalculate) || 
      (m_isAutocalculated && m_autosize)) 
  {
    LOG_AND_THROW("A field can only be autosized or autocalculated, it cannot be both.");
  }

  setEnabled(true);

  connect(this, &OSDoubleEdit2::editingFinished, this, &OSDoubleEdit2::onEditingFinished);

  connect(m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get(), &openstudio::model::detail::ModelObject_Impl::onChange, this, &OSDoubleEdit2::onModelObjectChange);

  connect(m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get(), &openstudio::model::detail::ModelObject_Impl::onRemoveFromWorkspace, this, &OSDoubleEdit2::onModelObjectRemove);

  refreshTextAndLabel();
}

void OSDoubleEdit2::unbind() {
  if (m_modelObject){
    this->disconnect(m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get());
    m_modelObject.reset();
    m_modelExtensibleGroup.reset();
    m_get.reset();
    m_getOptional.reset();
    m_set.reset();
    m_setVoidReturn.reset();
    m_reset.reset();
    m_autosize.reset();
    m_autocalculate.reset();
    m_isDefaulted.reset();
    m_isAutosized.reset();
    m_isAutocalculated.reset();
    setEnabled(false);
  }
}

void OSDoubleEdit2::onEditingFinished() {

  emit inFocus(true, hasData());

  QString text = this->text();
  if (text.isEmpty() || m_text == text) return;

  if (m_modelObject) {
    std::string str = this->text().toStdString();
    boost::regex autore("[aA][uU][tT][oO]");
    ModelObject modelObject = m_modelObject.get();

    if (str.empty()) {
      if (m_reset) {
        (*m_reset)();
      }
    }
    else if (boost::regex_search(str,autore)) {
      if (m_isAutosized) {
        if (m_autosize) {
          (*m_autosize)();
        }
        else if (m_reset) {
          (*m_reset)();
        }
      }
      if (m_isAutocalculated) {
        if (m_autocalculate) {
          (*m_autocalculate)();
        }
        else if (m_reset) {
          (*m_reset)();
        }
      }
    }
    else {
      try {
        double value = boost::lexical_cast<double>(str);
        setPrecision(str);
        if (m_set) {
          QString text = this->text();
          if (text.isEmpty() || m_text == text) return;
          bool result = (*m_set)(value);
          if (!result){
            //restore
            refreshTextAndLabel();
          }
        }else if (m_setVoidReturn){
          QString text = this->text();
          if (text.isEmpty() || m_text == text) return;
          (*m_setVoidReturn)(value);
        }
      }
      catch (...) 
      {
        //restore
        refreshTextAndLabel();
      }
    }
  }
}

void OSDoubleEdit2::onModelObjectChange() {
  if (m_modelExtensibleGroup){
    if (m_modelExtensibleGroup->empty()){
      // this is equivalent to onModelObjectRemove for the extensible group
      unbind();
      return;
    }
  }
  refreshTextAndLabel();
}

void OSDoubleEdit2::onModelObjectRemove(Handle handle) {
  unbind();
}

void OSDoubleEdit2::refreshTextAndLabel() {

  QString text = this->text();

  if (m_text == text) return;

  if (m_modelObject) {
    QString textValue;
    ModelObject modelObject = m_modelObject.get();
    std::stringstream ss;

    if (m_isAutosized && (*m_isAutosized)()) {
      textValue = QString("autosize");
    }

    if (m_isAutocalculated && (*m_isAutocalculated)()) {
      textValue = QString("autocalculate");
    }

    OptionalDouble od;
    if (m_get) {
      od = (*m_get)();
    }
    else {
      OS_ASSERT(m_getOptional);
      od = (*m_getOptional)();
    }
    if (od) {
      double value = *od;
      if (m_isScientific) {
        ss << std::scientific;
      }
      else {
        ss << std::fixed;
      }
      if (m_precision) {

        // check if precision is too small to display value
        int precision = *m_precision;
        double minValue = std::pow(10.0, -precision);
        if (value < minValue){
          m_precision.reset();
        }

        if (m_precision){
          ss << std::setprecision(*m_precision);
        }
      }
      ss << value;
      textValue = toQString(ss.str());
      ss.str("");
    }

    if (!textValue.isEmpty() && m_text != textValue){
      m_text = textValue;
      this->blockSignals(true);
      this->setText(m_text);
      this->blockSignals(false);
    }
    else {
      return;
    }

    if (m_isDefaulted) {
      if ((*m_isDefaulted)()) {
        this->setStyleSheet("color:green");
      }
      else {
        this->setStyleSheet("color:black");
      }
    }
  }
}

void OSDoubleEdit2::setPrecision(const std::string& str) {
  boost::regex rgx("-?([[:digit:]]*)(\\.)?([[:digit:]]+)([EDed][-\\+]?[[:digit:]]+)?");
  boost::smatch m;
  if (boost::regex_match(str,m,rgx)) {
    std::string sci, prefix, postfix;
    if (m[1].matched) {
      prefix = std::string(m[1].first,m[1].second);
    }
    if (m[3].matched) {
      postfix = std::string(m[3].first,m[3].second);
    }
    if (m[4].matched) {
      sci = std::string(m[4].first,m[4].second);
    }
    m_isScientific = !sci.empty();

    if (m_isScientific) {
      m_precision = prefix.size() + postfix.size() - 1;
    }
    else {
      if (m[2].matched) {
        m_precision = postfix.size();
      }
      else {
        m_precision = 0;
      }
    }
  }
  else {
    m_isScientific = false;
    m_precision.reset();
  }
}

void OSDoubleEdit2::focusInEvent(QFocusEvent * e)
{
  if (e->reason() == Qt::MouseFocusReason && m_hasClickFocus)
  {
    QString style("QLineEdit { background: #ffc627; }");
    setStyleSheet(style);

    emit inFocus(true, hasData());
  }

  QLineEdit::focusInEvent(e);
}

void OSDoubleEdit2::focusOutEvent(QFocusEvent * e)
{
  if (e->reason() == Qt::MouseFocusReason && m_hasClickFocus)
  {
    QString style("QLineEdit { background: white; }");
    setStyleSheet(style);

    emit inFocus(false, false);
  }

  QLineEdit::focusOutEvent(e);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OSDoubleEdit::OSDoubleEdit( QWidget * parent )
  : m_isScientific(false)
{
  this->setFixedWidth(90);
  this->setAcceptDrops(false);
  setEnabled(false);

  m_doubleValidator = new QDoubleValidator();
  this->setValidator(m_doubleValidator);
}

void OSDoubleEdit::bind(model::ModelObject& modelObject,
                          const char* property,
                          const boost::optional<std::string>& isDefaultedProperty,
                          const boost::optional<std::string>& isAutosizedProperty,
                          const boost::optional<std::string>& isAutocalculatedProperty)
{
  m_modelObject = modelObject;
  m_property = property;
  m_isDefaultedProperty = isDefaultedProperty;
  m_isAutosizedProperty = isAutosizedProperty;
  m_isAutocalculatedProperty = isAutocalculatedProperty;

  // only let one of autosize/autocalculate
  if (isAutosizedProperty && isAutocalculatedProperty) {
    LOG_AND_THROW("A field can only be autosized or autocalculated, it cannot be both.");
  }

  // check for attribute existence
  StringVector attributeNames = modelObject.attributeNames();
  StringVector::const_iterator anb(attributeNames.begin()),ane(attributeNames.end());
  OS_ASSERT(std::find(anb,ane,m_property) != ane);
  if (m_isDefaultedProperty) {
    OS_ASSERT(std::find(anb,ane,*m_isDefaultedProperty) != ane);
  }
  if (m_isAutosizedProperty) {
    OS_ASSERT(std::find(anb,ane,*m_isAutosizedProperty) != ane);
  }
  if (m_isAutocalculatedProperty) {
    OS_ASSERT(std::find(anb,ane,*m_isAutocalculatedProperty) != ane);
  }

  setEnabled(true);

  connect(this, &OSDoubleEdit::editingFinished, this, &OSDoubleEdit::onEditingFinished);

  connect(m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get(), &openstudio::model::detail::ModelObject_Impl::onChange, this, &OSDoubleEdit::onModelObjectChange);

  connect(m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get(), &openstudio::model::detail::ModelObject_Impl::onRemoveFromWorkspace, this, &OSDoubleEdit::onModelObjectRemove);

  refreshTextAndLabel();
}

void OSDoubleEdit::unbind() {
  if (m_modelObject){
    this->disconnect(m_modelObject->getImpl<openstudio::model::detail::ModelObject_Impl>().get());
    m_modelObject.reset();
    m_property = "";
    setEnabled(false);
  }
}

void OSDoubleEdit::onEditingFinished() {
  if (m_modelObject) {
    std::string str = this->text().toStdString();
    boost::regex autore("[aA][uU][tT][oO]");
    ModelObject modelObject = m_modelObject.get();

    if (str.empty()) {
      modelObject.resetAttribute(m_property);
    }
    else if (boost::regex_search(str,autore)) {
      if (m_isAutosizedProperty) {
        if (modelObject.isSettableAttribute(*m_isAutosizedProperty)) {
          modelObject.setAttribute(*m_isAutosizedProperty,true);
        }
        else {
          modelObject.resetAttribute(m_property);
        }
      }
      if (m_isAutocalculatedProperty) {
        if (modelObject.isSettableAttribute(*m_isAutocalculatedProperty)) {
          modelObject.setAttribute(*m_isAutocalculatedProperty,true);
        }
        else {
          modelObject.resetAttribute(m_property);
        }
      }
    }
    else {
      try {
        double value = boost::lexical_cast<double>(str);
        setPrecision(str);
        modelObject.setAttribute(m_property,value);
      }
      catch (...) {}
    }
  }
}

void OSDoubleEdit::onModelObjectChange() {
  refreshTextAndLabel();
}

void OSDoubleEdit::onModelObjectRemove(Handle handle) {
  m_modelObject.reset();
  m_property = "";
  setEnabled(false);
}

void OSDoubleEdit::refreshTextAndLabel() {
  if (m_modelObject) {
    QString textValue;
    ModelObject modelObject = m_modelObject.get();
    std::stringstream ss;

    if (m_isAutosizedProperty) {
      Attribute autosized = modelObject.getAttribute(*m_isAutosizedProperty).get();
      if (autosized.valueAsBoolean()) {
        textValue = QString("autosize");
      }
    }

    if (m_isAutocalculatedProperty) {
      Attribute autocalculated = modelObject.getAttribute(*m_isAutocalculatedProperty).get();
      if (autocalculated.valueAsBoolean()) {
        textValue = QString("autocalculate");
      }
    }

    OptionalAttribute attribute = modelObject.getAttribute(m_property);
    if (attribute) {
      double value = attribute->valueAsDouble();
      if (m_isScientific) {
        ss << std::scientific;
      }
      else {
        ss << std::fixed;
      }
      if (m_precision) {
        
        // check if precision is too small to display value
        int precision = *m_precision;
        double minValue = std::pow(10.0, -precision);
        if (value < minValue){
          m_precision.reset();
        }  

        if (m_precision){
          ss << std::setprecision(*m_precision);
        }
      }
      ss << value;
      textValue = toQString(ss.str());
      ss.str("");
    }

    this->setText(textValue);

    if (m_isDefaultedProperty) {
      Attribute defaulted = modelObject.getAttribute(*m_isDefaultedProperty).get();
      if (defaulted.valueAsBoolean()) {
        this->setStyleSheet("color:green");
      }
      else {
        this->setStyleSheet("color:black");
      }
    }
  }
}

void OSDoubleEdit::setPrecision(const std::string& str) {
  boost::regex rgx("-?([[:digit:]]*)(\\.)?([[:digit:]]+)([EDed][-\\+]?[[:digit:]]+)?");
  boost::smatch m;
  if (boost::regex_match(str,m,rgx)) {
    std::string sci, prefix, postfix;
    if (m[1].matched) {
      prefix = std::string(m[1].first,m[1].second);
    }
    if (m[3].matched) {
      postfix = std::string(m[3].first,m[3].second);
    }
    if (m[4].matched) {
      sci = std::string(m[4].first,m[4].second);
    }
    m_isScientific = !sci.empty();

    if (m_isScientific) {
      m_precision = prefix.size() + postfix.size() - 1;
    }
    else {
      if (m[2].matched) {
        m_precision = postfix.size();
      }
      else {
        m_precision = 0;
      }
    }
  }
  else {
    m_isScientific = false;
    m_precision.reset();
  }
}

} // openstudio

