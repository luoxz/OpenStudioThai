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

#include "PhotovoltaicThermalInspectorView.hpp"
#include "../shared_gui_components/OSComboBox.hpp"
#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"
#include "OSDropZone.hpp"
#include "../model/PhotovoltaicThermal.hpp"
#include "../model/PhotovoltaicThermal_Impl.hpp"
#include "../utilities/core/Assert.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include <QStackedWidget>

namespace openstudio {

PhotovoltaicThermalInspectorView::PhotovoltaicThermalInspectorView(bool isIP, const openstudio::model::Model& model, QWidget * parent)
  : ModelObjectInspectorView(model, true, parent)
{
  m_isIP = isIP;

  QWidget* visibleWidget = new QWidget();
  this->stackedWidget()->addWidget(visibleWidget);

  QGridLayout* mainGridLayout = new QGridLayout();
  mainGridLayout->setContentsMargins(7,7,7,7);
  mainGridLayout->setSpacing(14);
  visibleWidget->setLayout(mainGridLayout);

  // Name

  QLabel * label = new QLabel("Name: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label,0,0);

  m_nameEdit = new OSLineEdit();
  mainGridLayout->addWidget(m_nameEdit,1,0,1,2);

  label = new QLabel("Surface Area: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 2, 0);

  m_surfaceAreaEdit = new OSQuantityEdit(m_isIP);
  connect(this, &PhotovoltaicThermalInspectorView::toggleUnitsClicked, m_surfaceAreaEdit, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_surfaceAreaEdit, 3, 0);

  label = new QLabel("System Efficiecy: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 2, 1);

  m_systemEfficiencyEdit = new OSQuantityEdit(m_isIP);
  connect(this, &PhotovoltaicThermalInspectorView::toggleUnitsClicked, m_systemEfficiencyEdit, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_systemEfficiencyEdit, 3, 1);

  label = new QLabel("Faction Acive: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 2, 2);

  m_factionActiveEdit = new OSQuantityEdit(m_isIP);
  connect(this, &PhotovoltaicThermalInspectorView::toggleUnitsClicked, m_factionActiveEdit, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_factionActiveEdit, 3, 2);

  label = new QLabel("Collector coefficiency: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 4, 0);

  m_collectorEfficiencyEdit  = new OSQuantityEdit(m_isIP);
  connect(this, &PhotovoltaicThermalInspectorView::toggleUnitsClicked, m_collectorEfficiencyEdit, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_collectorEfficiencyEdit, 5, 0);

  label = new QLabel("Boiler coefficiency: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 4, 1);

  m_boilerEfficiencyEdit = new OSQuantityEdit(m_isIP);
  connect(this, &PhotovoltaicThermalInspectorView::toggleUnitsClicked, m_boilerEfficiencyEdit, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_boilerEfficiencyEdit, 5, 1);

  label = new QLabel("Azimuth angle: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 6, 0);

  m_azimuthAngleEdit = new OSQuantityEdit(m_isIP);
  connect(this, &PhotovoltaicThermalInspectorView::toggleUnitsClicked, m_azimuthAngleEdit, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_azimuthAngleEdit, 7, 0);

  label = new QLabel("Inclination angle: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 6, 1);

  m_inclinationAngleEdit = new OSQuantityEdit(m_isIP);
  connect(this, &PhotovoltaicThermalInspectorView::toggleUnitsClicked, m_inclinationAngleEdit, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_inclinationAngleEdit, 7, 1);

  label = new QLabel("GT : ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 8, 0);

  m_gtEfficiencyEdit = new OSQuantityEdit(m_isIP);
  connect(this, &PhotovoltaicThermalInspectorView::toggleUnitsClicked, m_gtEfficiencyEdit, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_gtEfficiencyEdit, 9, 0);

    // Stretch

  mainGridLayout->setRowStretch(10,100);

  mainGridLayout->setColumnStretch(3,100);
}

void PhotovoltaicThermalInspectorView::onClearSelection()
{
  ModelObjectInspectorView::onClearSelection(); // call parent implementation
  detach();
}

void PhotovoltaicThermalInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject)
{
  detach();
  model::PhotovoltaicThermal photovoltaicThermal = modelObject.cast<model::PhotovoltaicThermal>();
  attach(photovoltaicThermal);
  refresh();
}

void PhotovoltaicThermalInspectorView::onUpdate()
{
  refresh();
}

void PhotovoltaicThermalInspectorView::attach(openstudio::model::PhotovoltaicThermal & PhotovoltaicThermal)
{
  m_nameEdit->bind(PhotovoltaicThermal,"name");
  m_surfaceAreaEdit->bind(PhotovoltaicThermal, "surfaceArea", m_isIP);
  m_factionActiveEdit->bind(PhotovoltaicThermal, "factionActive", m_isIP);
  m_boilerEfficiencyEdit->bind(PhotovoltaicThermal,"boilerEfficiency", m_isIP);
  m_collectorEfficiencyEdit->bind(PhotovoltaicThermal, "collectorEfficiency", m_isIP);
  m_azimuthAngleEdit->bind(PhotovoltaicThermal, "azimuthAngle", m_isIP);
  m_inclinationAngleEdit->bind(PhotovoltaicThermal, "inclinationAngle", m_isIP);
  m_gtEfficiencyEdit->bind(PhotovoltaicThermal, "gtEfficiency", m_isIP);
  m_systemEfficiencyEdit->bind(PhotovoltaicThermal, "cellEfficiency", m_isIP);
  
  this->stackedWidget()->setCurrentIndex(1);
}

void PhotovoltaicThermalInspectorView::detach()
{
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();
  m_surfaceAreaEdit->unbind();
  m_factionActiveEdit->unbind();
  m_boilerEfficiencyEdit->unbind();
  m_collectorEfficiencyEdit->unbind();
  m_azimuthAngleEdit->unbind();
  m_inclinationAngleEdit->unbind();
  m_gtEfficiencyEdit->unbind();
  m_systemEfficiencyEdit->unbind();

}

void PhotovoltaicThermalInspectorView::refresh()
{
}

void PhotovoltaicThermalInspectorView::toggleUnits(bool displayIP)
{
  m_isIP = displayIP;
}

} // openstudio

