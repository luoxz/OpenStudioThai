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

#include "WindowMaterialShadeInspectorView.hpp"

#include "StandardsInformationMaterialWidget.hpp"

#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"

#include "../model/Shade.hpp"
#include "../model/Shade_Impl.hpp"

#include "../utilities/core/Assert.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>

namespace openstudio {

// WindowMaterialShadeInspectorView

WindowMaterialShadeInspectorView::WindowMaterialShadeInspectorView(bool isIP, const openstudio::model::Model& model, QWidget * parent)
  : ModelObjectInspectorView(model, true, parent),
    m_isIP(isIP)
{
  createLayout();
}

void WindowMaterialShadeInspectorView::createLayout()
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

  // Solar Transmittance

  label = new QLabel("Solar Transmittance: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label,row++,0);

  m_solarTransmittance = new OSQuantityEdit(m_isIP);
  connect(this, &WindowMaterialShadeInspectorView::toggleUnitsClicked, m_solarTransmittance, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_solarTransmittance,row++,0,1,3);

  // Solar Reflectance

  label = new QLabel("Solar Reflectance: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label,row++,0);

  m_solarReflectance = new OSQuantityEdit(m_isIP);
  connect(this, &WindowMaterialShadeInspectorView::toggleUnitsClicked, m_solarReflectance, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_solarReflectance,row++,0,1,3);

  // Visible Transmittance

  label = new QLabel("Visible Transmittance: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label,row++,0);

  m_visibleTransmittance = new OSQuantityEdit(m_isIP);
  connect(this, &WindowMaterialShadeInspectorView::toggleUnitsClicked, m_visibleTransmittance, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_visibleTransmittance,row++,0,1,3);

  // Visible Reflectance

  label = new QLabel("Visible Reflectance: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label,row++,0);

  m_visibleReflectance = new OSQuantityEdit(m_isIP);
  connect(this, &WindowMaterialShadeInspectorView::toggleUnitsClicked, m_visibleReflectance, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_visibleReflectance,row++,0,1,3);

  // Thermal Hemispherical Emissivity

  label = new QLabel("Thermal Hemispherical Emissivity: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label,row++,0);

  m_thermalHemisphericalEmissivity = new OSQuantityEdit(m_isIP);
  connect(this, &WindowMaterialShadeInspectorView::toggleUnitsClicked, m_thermalHemisphericalEmissivity, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_thermalHemisphericalEmissivity,row++,0,1,3);

  // Thermal Transmittance

  label = new QLabel("Thermal Transmittance: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label,row++,0);

  m_thermalTransmittance = new OSQuantityEdit(m_isIP);
  connect(this, &WindowMaterialShadeInspectorView::toggleUnitsClicked, m_thermalTransmittance, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_thermalTransmittance,row++,0,1,3);

  // Thickness

  label = new QLabel("Thickness: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label,row++,0);

  m_thickness = new OSQuantityEdit(m_isIP);
  connect(this, &WindowMaterialShadeInspectorView::toggleUnitsClicked, m_thickness, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_thickness,row++,0,1,3);

  // Conductivity

  label = new QLabel("Conductivity: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label,row++,0);

  m_conductivity = new OSQuantityEdit(m_isIP);
  connect(this, &WindowMaterialShadeInspectorView::toggleUnitsClicked, m_conductivity, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_conductivity,row++,0,1,3);

  // Shade To Glass Distance

  label = new QLabel("Shade To Glass Distance: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label,row++,0);

  m_shadeToGlassDistance = new OSQuantityEdit(m_isIP);
  connect(this, &WindowMaterialShadeInspectorView::toggleUnitsClicked, m_shadeToGlassDistance, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_shadeToGlassDistance,row++,0,1,3);

  // Top Opening Multiplier

  label = new QLabel("Top Opening Multiplier: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label,row++,0);

  m_topOpeningMultiplier = new OSQuantityEdit(m_isIP);
  connect(this, &WindowMaterialShadeInspectorView::toggleUnitsClicked, m_topOpeningMultiplier, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_topOpeningMultiplier,row++,0,1,3);

  // Bottom Opening Multiplier

  label = new QLabel("Bottom Opening Multiplier: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label,row++,0);

  m_bottomOpeningMultiplier = new OSQuantityEdit(m_isIP);
  connect(this, &WindowMaterialShadeInspectorView::toggleUnitsClicked, m_bottomOpeningMultiplier, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_bottomOpeningMultiplier,row++,0,1,3);

  // Left-Side Opening Multiplier

  label = new QLabel("Left-Side Opening Multiplier: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label,row++,0);

  m_leftSideOpeningMultiplier = new OSQuantityEdit(m_isIP);
  connect(this, &WindowMaterialShadeInspectorView::toggleUnitsClicked, m_leftSideOpeningMultiplier, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_leftSideOpeningMultiplier,row++,0,1,3);

  // Right-Side Opening Multiplier

  label = new QLabel("Right-Side Opening Multiplier: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label,row++,0);

  m_rightSideOpeningMultiplier = new OSQuantityEdit(m_isIP);
  connect(this, &WindowMaterialShadeInspectorView::toggleUnitsClicked, m_rightSideOpeningMultiplier, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_rightSideOpeningMultiplier,row++,0,1,3);

  // Airflow Permeability

  label = new QLabel("Airflow Permeability: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label,row++,0);

  m_airflowPermeability = new OSQuantityEdit(m_isIP);
  connect(this, &WindowMaterialShadeInspectorView::toggleUnitsClicked, m_airflowPermeability, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_airflowPermeability,row++,0,1,3);

  // Stretch

  mainGridLayout->setRowStretch(100,100);

  mainGridLayout->setColumnStretch(100,100);
}

void WindowMaterialShadeInspectorView::onClearSelection()
{
  ModelObjectInspectorView::onClearSelection(); // call parent implementation
  detach();
}

void WindowMaterialShadeInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject)
{
  detach();
  model::Shade shade = modelObject.cast<model::Shade>();
  attach(shade);
  refresh();
}

void WindowMaterialShadeInspectorView::onUpdate()
{
  refresh();
}

void WindowMaterialShadeInspectorView::attach(openstudio::model::Shade & shade)
{
  m_nameEdit->bind(shade,"name");
  m_solarTransmittance->bind(shade,"solarTransmittance",m_isIP);
  m_solarReflectance->bind(shade,"solarReflectance",m_isIP);
  m_visibleTransmittance->bind(shade,"visibleTransmittance",m_isIP);
  m_visibleReflectance->bind(shade,"visibleReflectance",m_isIP);
  m_thermalHemisphericalEmissivity->bind(shade,"thermalHemisphericalEmissivity",m_isIP);
  m_thermalTransmittance->bind(shade,"thermalTransmittance",m_isIP);
  m_thickness->bind(shade,"thickness",m_isIP);
  m_conductivity->bind(shade,"conductivity",m_isIP);
  m_shadeToGlassDistance->bind(shade,"shadetoGlassDistance",m_isIP);
  m_topOpeningMultiplier->bind(shade,"topOpeningMultiplier",m_isIP);
  m_bottomOpeningMultiplier->bind(shade,"bottomOpeningMultiplier",m_isIP);
  m_leftSideOpeningMultiplier->bind(shade,"leftSideOpeningMultiplier",m_isIP);
  m_rightSideOpeningMultiplier->bind(shade,"rightSideOpeningMultiplier",m_isIP);
  m_airflowPermeability->bind(shade,"airflowPermeability",m_isIP);

  m_standardsInformationWidget->attach(shade);

  this->stackedWidget()->setCurrentIndex(1);
}

void WindowMaterialShadeInspectorView::detach()
{
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();
  m_solarTransmittance->unbind();
  m_solarReflectance->unbind();
  m_visibleTransmittance->unbind();
  m_visibleReflectance->unbind();
  m_thermalHemisphericalEmissivity->unbind();
  m_thermalTransmittance->unbind();
  m_thickness->unbind();
  m_conductivity->unbind();
  m_shadeToGlassDistance->unbind();
  m_topOpeningMultiplier->unbind();
  m_bottomOpeningMultiplier->unbind();
  m_leftSideOpeningMultiplier->unbind();
  m_rightSideOpeningMultiplier->unbind();
  m_airflowPermeability->unbind();

  m_standardsInformationWidget->detach();
}

void WindowMaterialShadeInspectorView::refresh()
{
}

} // openstudio
