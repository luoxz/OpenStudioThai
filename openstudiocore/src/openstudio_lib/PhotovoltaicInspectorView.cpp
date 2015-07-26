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

#include "PhotovoltaicInspectorView.hpp"
#include "../shared_gui_components/OSComboBox.hpp"
#include "../shared_gui_components/OSLineEdit.hpp"
#include "../shared_gui_components/OSQuantityEdit.hpp"
#include "OSDropZone.hpp"
#include "../model/Photovoltaic.hpp"
#include "../model/Photovoltaic_Impl.hpp"
#include "../utilities/core/Assert.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include <QStackedWidget>

namespace openstudio {

PhotovoltaicInspectorView::PhotovoltaicInspectorView(bool isIP, const openstudio::model::Model& model, QWidget * parent)
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
  connect(this, &PhotovoltaicInspectorView::toggleUnitsClicked, m_surfaceAreaEdit, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_surfaceAreaEdit, 3, 0);

  label = new QLabel("PV type: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 2, 1);

  m_PVTypeComboBox = new OSComboBox();
  m_PVTypeComboBox->setFixedWidth(OSItem::ITEM_WIDTH);
  m_PVTypeComboBox->addItem("Monocrystalline");
  m_PVTypeComboBox->addItem("Polycrystalline");
  m_PVTypeComboBox->addItem("Amorphous");
  mainGridLayout->addWidget(m_PVTypeComboBox, 3, 1);

  label = new QLabel("System Efficiecy: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 2, 2);

  m_systemEfficiencyEdit = new OSQuantityEdit(m_isIP);
  connect(this, &PhotovoltaicInspectorView::toggleUnitsClicked, m_systemEfficiencyEdit, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_systemEfficiencyEdit, 3, 2);

  label = new QLabel("Faction Acive: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 4, 0);

  m_factionActiveEdit = new OSQuantityEdit(m_isIP);
  connect(this, &PhotovoltaicInspectorView::toggleUnitsClicked, m_factionActiveEdit, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_factionActiveEdit, 5, 0);

  label = new QLabel("Invert coefficiency: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 6, 0);

  m_inverterEfficiencyEdit = new OSQuantityEdit(m_isIP);
  connect(this, &PhotovoltaicInspectorView::toggleUnitsClicked, m_inverterEfficiencyEdit, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_inverterEfficiencyEdit, 7, 0);

  label = new QLabel("Azimuth angle: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 8, 0);

  m_azimuthAngleEdit = new OSQuantityEdit(m_isIP);
  connect(this, &PhotovoltaicInspectorView::toggleUnitsClicked, m_azimuthAngleEdit, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_azimuthAngleEdit, 9, 0);

  label = new QLabel("Inclination angle: ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 8, 1);

  m_inclinationAngleEdit = new OSQuantityEdit(m_isIP);
  connect(this, &PhotovoltaicInspectorView::toggleUnitsClicked, m_inclinationAngleEdit, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_inclinationAngleEdit, 9, 1);

  label = new QLabel("GT : ");
  label->setObjectName("H2");
  mainGridLayout->addWidget(label, 10, 0);

  m_gtEfficiencyEdit = new OSQuantityEdit(m_isIP);
  connect(this, &PhotovoltaicInspectorView::toggleUnitsClicked, m_gtEfficiencyEdit, &OSQuantityEdit::onUnitSystemChange);
  mainGridLayout->addWidget(m_gtEfficiencyEdit, 11, 0);

    // Stretch

  mainGridLayout->setRowStretch(12,100);

  mainGridLayout->setColumnStretch(3,100);
}

void PhotovoltaicInspectorView::onClearSelection()
{
  ModelObjectInspectorView::onClearSelection(); // call parent implementation
  detach();
}

void PhotovoltaicInspectorView::onSelectModelObject(const openstudio::model::ModelObject& modelObject)
{
  detach();
  model::Photovoltaic photovoltaic = modelObject.cast<model::Photovoltaic>();
  attach(photovoltaic);
  refresh();
}

void PhotovoltaicInspectorView::onUpdate()
{
  refresh();
}

void PhotovoltaicInspectorView::attach(openstudio::model::Photovoltaic & Photovoltaic)
{
  m_nameEdit->bind(Photovoltaic,"name");
  m_PVTypeComboBox->bind(Photovoltaic,"PVType");
  m_surfaceAreaEdit->bind(Photovoltaic, "surfaceArea", m_isIP);
  m_factionActiveEdit->bind(Photovoltaic, "factionActive", m_isIP);
  m_inverterEfficiencyEdit->bind(Photovoltaic, "inverterEfficiency", m_isIP);
  m_azimuthAngleEdit->bind(Photovoltaic, "azimuthAngle",  m_isIP);
  m_inclinationAngleEdit->bind(Photovoltaic, "inclinationAngle", m_isIP);
  m_gtEfficiencyEdit->bind(Photovoltaic, "gtEfficiency", m_isIP);
  m_systemEfficiencyEdit->bind(Photovoltaic, "cellEfficiency", m_isIP);
  
  this->stackedWidget()->setCurrentIndex(1);
}

void PhotovoltaicInspectorView::detach()
{
  this->stackedWidget()->setCurrentIndex(0);

  m_nameEdit->unbind();
  m_PVTypeComboBox->unbind();
  m_surfaceAreaEdit->unbind();
  m_factionActiveEdit->unbind();
  m_inverterEfficiencyEdit->unbind();
  m_azimuthAngleEdit->unbind();
  m_inclinationAngleEdit->unbind();
  m_gtEfficiencyEdit->unbind();
  m_systemEfficiencyEdit->unbind();

}

void PhotovoltaicInspectorView::refresh()
{
}

void PhotovoltaicInspectorView::toggleUnits(bool displayIP)
{
  m_isIP = displayIP;
}

} // openstudio

