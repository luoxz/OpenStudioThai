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

#include "CollapsibleComponentList.hpp"
#include "CollapsibleComponent.hpp"
#include "CollapsibleComponentHeader.hpp"
#include "Component.hpp"
#include "ComponentList.hpp"

#include "../utilities/core/Assert.hpp"

#include <QBoxLayout>
#include <QButtonGroup>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>

namespace openstudio {

CollapsibleComponentList::CollapsibleComponentList(QWidget * parent)
  : QWidget(parent),
  m_mainLayout(nullptr),
  m_contentLayout(nullptr),
  m_collapsibleComponentGroup(nullptr)
{
  createLayout();
}

CollapsibleComponentList::CollapsibleComponentList(const std::vector<CollapsibleComponent *> & collapsibleComponents,
                                                   QWidget * parent)
  : QWidget(parent),
  m_mainLayout(nullptr),
  m_contentLayout(nullptr),
  m_collapsibleComponentGroup(nullptr)

{
  createLayout();
  setCollapsibleComponents(collapsibleComponents);
}

void CollapsibleComponentList::createLayout()
{
  this->setObjectName("GrayWidget"); 

  auto outerVLayout = new QVBoxLayout();
  outerVLayout->setContentsMargins(0,0,0,0);
  this->setLayout(outerVLayout);

  auto outerWidget = new QWidget();

  outerVLayout->addWidget(outerWidget);

  m_mainLayout = new QVBoxLayout();
  outerWidget->setLayout(m_mainLayout);
  m_mainLayout->setContentsMargins(0,0,0,0);
  m_mainLayout->setSpacing(0);
  m_mainLayout->addStretch(10);

  ///! QButtonGroup is exclusive, by default
  m_collapsibleComponentGroup = new QButtonGroup(this);
}

Component * CollapsibleComponentList::checkedComponent() const
{
  Component * result = nullptr;
  if (checkedCollapsibleComponent()){
    result = checkedCollapsibleComponent()->componentList()->checkedComponent();
  }
  return result;
}

CollapsibleComponent * CollapsibleComponentList::checkedCollapsibleComponent() const
{
  return qobject_cast<CollapsibleComponent *>(m_collapsibleComponentGroup->button(0)); // TODO fix this hack
}

CollapsibleComponent * CollapsibleComponentList::firstCollapsibleComponent()
{
  return qobject_cast<CollapsibleComponent *>(m_collapsibleComponentGroup->button(0));
}

CollapsibleComponent * CollapsibleComponentList::lastCollapsibleComponent()
{
  return qobject_cast<CollapsibleComponent *>(m_collapsibleComponentGroup->button(m_collapsibleComponentGroup->buttons().size() - 1));
}

std::vector<CollapsibleComponent *> CollapsibleComponentList::collapsibleComponents()
{
  std::vector<CollapsibleComponent *> result;

  for (QAbstractButton * button : m_collapsibleComponentGroup->buttons().toVector().toStdVector()) {
    result.push_back(qobject_cast<CollapsibleComponent *>(button));
  }

  return result;
}

std::vector<Component *> CollapsibleComponentList::components()
{
  std::vector<Component *> result;
  std::vector<Component *> components;

  for (QAbstractButton * button : m_collapsibleComponentGroup->buttons().toVector().toStdVector()) {
    components = qobject_cast<CollapsibleComponent *>(button)->componentList()->components();
    for (Component * component : components) {
      result.push_back(component);
    }
    components.clear();
  }

  return result;
}

void CollapsibleComponentList::addCollapsibleComponent(CollapsibleComponent * collapsibleComponent)
{
  OS_ASSERT(collapsibleComponent);

  m_mainLayout->addWidget(collapsibleComponent);
  m_collapsibleComponentGroup->addButton(collapsibleComponent,m_collapsibleComponentGroup->buttons().size());

  connect(collapsibleComponent, &CollapsibleComponent::headerClicked, this, &CollapsibleComponentList::headerClicked);

  connect(collapsibleComponent, &CollapsibleComponent::headerClicked, this, &CollapsibleComponentList::on_headerClicked);

  connect(collapsibleComponent, &CollapsibleComponent::componentClicked, this, &CollapsibleComponentList::componentClicked);

  connect(collapsibleComponent, &CollapsibleComponent::componentClicked, this, &CollapsibleComponentList::on_componentClicked);

  connect(collapsibleComponent, &CollapsibleComponent::clicked, this, &CollapsibleComponentList::collapsibleComponentClicked);

  connect(collapsibleComponent, &CollapsibleComponent::clicked, this, &CollapsibleComponentList::on_collapsibleComponentClicked);

  connect(collapsibleComponent, &CollapsibleComponent::getComponentsByPage, this, &CollapsibleComponentList::getComponentsByPage);

  connect(collapsibleComponent, &CollapsibleComponent::getComponentsByPage, this, &CollapsibleComponentList::on_getComponentsByPage);

  collapsibleComponent->setChecked(true);

  QString style;
  style.append( "QWidget#CollapsibleComponentList {" );
  style.append( "background: #F2F2F2; ");
  style.append( "border-bottom: 1px solid black; " );
  style.append( "}" );
  style.append( "QWidget#SideBar {background: #EEDEDE;}" );

  collapsibleComponent->setStyleSheet(style);
}

void CollapsibleComponentList::paintEvent(QPaintEvent * event)
{
  QStyleOption opt;
  opt.init(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CollapsibleComponentList::setCollapsibleComponents(const std::vector<CollapsibleComponent *> & collapsibleComponents)
{
  for (CollapsibleComponent * collapsibleComponent : collapsibleComponents)
  {
    addCollapsibleComponent(collapsibleComponent);
  }
}

void CollapsibleComponentList::setText(const QString& text)
{
  checkedCollapsibleComponent()->setText(text);
}

void CollapsibleComponentList::setNumResults(int numResults)
{
  checkedCollapsibleComponent()->setNumResults(numResults);
}

void CollapsibleComponentList::setNumPages(int numPages)
{
  checkedCollapsibleComponent()->setNumPages(numPages);
}

void CollapsibleComponentList::firstPage()
{
  checkedCollapsibleComponent()->firstPage();
}

///! SLOTS

void CollapsibleComponentList::on_headerClicked(bool checked)
{
}

void CollapsibleComponentList::on_componentClicked(bool checked)
{
}

void CollapsibleComponentList::on_collapsibleComponentClicked(bool checked)
{
}

void CollapsibleComponentList::on_getComponentsByPage(int pageIdx)
{
}

} // openstudio
