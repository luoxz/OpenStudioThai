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

#include "MaterialInfraredTransparentInspectorView.hpp"

#include "StandardsInformationMaterialWidget.hpp"

#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"

#include "../model/InfraredTransparentMaterial.hpp"
#include "../model/InfraredTransparentMaterial_Impl.hpp"

#include "../utilities/core/Assert.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>

namespace openstudio {

// MaterialInfraredTransparentInspectorView

MaterialInfraredTransparentInspectorView::MaterialInfraredTransparentInspectorView(bool isIP, const openstudio::model::Model& model, QWidget * parent)
  : ModelObjectInspectorView(model, true, parent),
  m_isIP(isIP)
{
  createLayout();
}

void MaterialInfraredTransparentInspectorView::createLayout()
{
  QWidget* hiddenWidget = new QWidget();
  this->stackedWidget()->addWidget(hiddenWidget);

  QWidget* visibleWidget = new QWidget();
  this->stackedWidget()->addWidget(visibleWidget);

  QGridLayout* mainGridLayout = new QGridLayout();
  mainGridLayout->setContentsMargins(7, 7, 7, 7);
  mainGridLayout->setSpacing(14);
  visibleWidget->setLayout(mainGridLayout);

  int row = mainGridLayout->rowCount();

  QLabel * label = nullptr;

  // Name

  label = new QLabel("Name: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, row, 0);

  ++row;

  m_nameEdit = new OSLineEdit();
  mainGridLayout->addWidget(m_nameEdit, row, 0, 1, 3);

  ++row;

  // Standards Information

  m_standardsInformationWidget = new StandardsInformationMaterialWidget(m_isIP, mainGridLayout, row);

  ++row;

  // Stretch

  mainGridLayout->setRowStretch(100,100);

  mainGridLayout->setColumnStretch(100,100);
}

void MaterialInfraredTransparentInspectorView::onClearSelection()
{
  ModelObjectInspectorView::onClearSelection(); // call parent implementation
  detach();
}

void MaterialInfraredTransparentInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject)
{
  detach();
  model::InfraredTransparentMaterial infraredTransparentMaterial = modelObject.cast<model::InfraredTransparentMaterial>();
  attach(infraredTransparentMaterial);
  refresh();
}

void MaterialInfraredTransparentInspectorView::onUpdate()
{
  refresh();
}

void MaterialInfraredTransparentInspectorView::attach(openstudio::model::InfraredTransparentMaterial & infraredTransparentMaterial)
{
  m_nameEdit->bind(infraredTransparentMaterial,"name");

  m_standardsInformationWidget->attach(infraredTransparentMaterial);

  this->stackedWidget()->setCurrentIndex(1);
}

void MaterialInfraredTransparentInspectorView::detach()
{
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();

  m_standardsInformationWidget->detach();
}

void MaterialInfraredTransparentInspectorView::refresh()
{
}

} // openstudio
