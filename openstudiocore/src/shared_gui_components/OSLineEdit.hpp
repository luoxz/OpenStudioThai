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

#ifndef SHAREDGUICOMPONENTS_OSLINEEDIT_HPP
#define SHAREDGUICOMPONENTS_OSLINEEDIT_HPP

#include "FieldMethodTypedefs.hpp"

#include "../model/Model.hpp"

#include <QLineEdit>

#include <QTimer>

class QFocusEvent;
class QMouseEvent;

namespace openstudio {

class OSItem;

class OSLineEdit2 : public QLineEdit {
  Q_OBJECT

public:

  OSLineEdit2(QWidget * parent = nullptr);

  virtual ~OSLineEdit2() {}

  void enableClickFocus() { this->m_hasClickFocus = true; }
  void setDeleteObject(bool deleteObject) { m_deleteObject = deleteObject; }
  bool hasData() { return !this->text().isEmpty(); }
  bool deleteObject() { return m_deleteObject; }

  void bind(model::ModelObject& modelObject,
            StringGetter get,
            boost::optional<StringSetter> set=boost::none,
            boost::optional<NoFailAction> reset=boost::none,
            boost::optional<BasicQuery> isDefaulted=boost::none);

  void bind(model::ModelObject& modelObject, 
            OptionalStringGetter get,
            boost::optional<StringSetter> set=boost::none,
            boost::optional<NoFailAction> reset=boost::none,
            boost::optional<BasicQuery> isDefaulted=boost::none);
  
  void bind(model::ModelObject& modelObject,
            OptionalStringGetterBoolArg get,
            boost::optional<StringSetterOptionalStringReturn> set,
            boost::optional<NoFailAction> reset=boost::none,
            boost::optional<BasicQuery> isDefaulted=boost::none);

  void unbind();

protected:

  void mouseReleaseEvent(QMouseEvent* event);

  virtual void focusInEvent(QFocusEvent * e);

  virtual void focusOutEvent(QFocusEvent * e);

signals:

  void itemClicked(OSItem* item);

  void objectRemoved(boost::optional<model::ParentObject> parent);

  void inFocus(bool inFocus, bool hasData);

  public slots:

  void onItemRemoveClicked();

  private slots :

  void onEditingFinished();

  void onModelObjectChange();

  void onModelObjectRemove(Handle handle);

  void emitItemClicked();

 private:

  void onModelObjectChangeInternal(bool startingup);
  void completeBind();
  void adjustWidth();

  boost::optional<model::ModelObject> m_modelObject;
  boost::optional<StringGetter> m_get;
  boost::optional<OptionalStringGetter> m_getOptional;
  boost::optional<OptionalStringGetterBoolArg> m_getOptionalBoolArg;
  boost::optional<StringSetter> m_set;
  boost::optional<StringSetterOptionalStringReturn> m_setOptionalStringReturn;
  boost::optional<NoFailAction> m_reset;
  boost::optional<BasicQuery> m_isDefaulted;

  OSItem * m_item = nullptr;

  std::string m_text = "";

  QTimer m_timer;

  bool m_hasClickFocus = false;

  bool m_deleteObject = false;
};

class OSLineEdit : public QLineEdit
{
  Q_OBJECT

  public:

  OSLineEdit(QWidget * parent = nullptr);

  virtual ~OSLineEdit() {}

  void bind(model::ModelObject & modelObject, const char * property);

  void unbind();

  private slots:

  void onEditingFinished();

  void onModelObjectChange();

  void onModelObjectRemove(Handle handle);

  private:

  boost::optional<model::ModelObject> m_modelObject;

  std::string m_property;
};

} // openstudio

#endif // SHAREDGUICOMPONENTS_OSLINEEDIT_HPP

