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

#include "ModelObjectListView.hpp"
#include "ModelObjectItem.hpp"
#include "BCLComponentItem.hpp"

#include "../model/Model_Impl.hpp"
#include "../model/ModelObject_Impl.hpp"
#include "../model/ZoneHVACComponent.hpp"
#include "../model/ZoneHVACComponent_Impl.hpp"
#include "../model/HVACComponent.hpp"
#include "../model/HVACComponent_Impl.hpp"
#include "../model/UtilityBill.hpp"
#include "../model/UtilityBill_Impl.hpp"

#include "../utilities/core/Assert.hpp"
#include "../utilities/bcl/LocalBCL.hpp"

#include <iostream>

namespace openstudio {

ModelObjectListController::ModelObjectListController(const openstudio::IddObjectType& iddObjectType, 
                                                     const model::Model& model,
                                                     bool showLocalBCL)
  : m_iddObjectType(iddObjectType), m_model(model), m_showLocalBCL(showLocalBCL)
{
  connect(model.getImpl<model::detail::Model_Impl>().get(), 
    static_cast<void (model::detail::Model_Impl::*)(std::shared_ptr<detail::WorkspaceObject_Impl>, const IddObjectType &, const UUID &) const>(&model::detail::Model_Impl::addWorkspaceObject),
    this,
    &ModelObjectListController::objectAdded,
    Qt::QueuedConnection);
  
  connect(model.getImpl<model::detail::Model_Impl>().get(), 
    static_cast<void (model::detail::Model_Impl::*)(std::shared_ptr<detail::WorkspaceObject_Impl>, const IddObjectType &, const UUID &) const>(&model::detail::Model_Impl::removeWorkspaceObject),
    this,
    &ModelObjectListController::objectRemoved,
    Qt::QueuedConnection);

}

IddObjectType ModelObjectListController::iddObjectType() const
{
  return m_iddObjectType;
}

void ModelObjectListController::objectAdded(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> impl, const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle)
{
  if (iddObjectType == m_iddObjectType){
    std::vector<OSItemId> ids = this->makeVector();
    emit itemIds(ids);

    for (const OSItemId& id : ids){
      if (id.itemId() == impl->handle().toString()){
        emit selectedItemId(id);
        break;
      }
    }
  }
}

void ModelObjectListController::objectRemoved(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> impl, const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle)
{
  if (iddObjectType == m_iddObjectType){
    emit itemIds(makeVector());
  }
}

std::vector<OSItemId> ModelObjectListController::makeVector()
{
  std::vector<OSItemId> result;

  if( m_showLocalBCL )
  {
    std::vector<std::pair<std::string, std::string> > pairs;
    pairs.push_back(std::make_pair<std::string,std::string>("OpenStudio Type",m_iddObjectType.valueDescription()));

    // get BCL results
    std::vector<BCLComponent> bclresults = LocalBCL::instance().componentAttributeSearch(pairs); 

    // sort by name
    std::sort(bclresults.begin(), bclresults.end(), BCLComponentNameGreater());

    for( std::vector<BCLComponent>::iterator it = bclresults.begin(); 
         it != bclresults.end();
         ++it )
    {
      result.push_back(bclComponentToItemId(*it));
    }
  }

  // get objects by type
  std::vector<WorkspaceObject> workspaceObjects = m_model.getObjectsByType(m_iddObjectType);

  // sort by name
  std::sort(workspaceObjects.begin(), workspaceObjects.end(), WorkspaceObjectNameGreater());

  for (WorkspaceObject workspaceObject : workspaceObjects){
    if (!workspaceObject.handle().isNull()){
      openstudio::model::ModelObject modelObject = workspaceObject.cast<openstudio::model::ModelObject>();
      if(boost::optional<model::HVACComponent> hvacComponent = modelObject.optionalCast<model::HVACComponent>()) {
        if( (! hvacComponent->containingHVACComponent()) && (! hvacComponent->containingZoneHVACComponent()) ) {
          result.push_back(modelObjectToItemId(hvacComponent.get(), false));
        }
      } else {
        result.push_back(modelObjectToItemId(modelObject, false));
      }
    }
  }

  return result;
}

ModelObjectListView::ModelObjectListView(const openstudio::IddObjectType& iddObjectType, 
                                         const model::Model& model, 
                                         bool addScrollArea,
                                         bool showLocalBCL,
                                         QWidget * parent )
  : OSItemList(new ModelObjectListController(iddObjectType, model,showLocalBCL), addScrollArea, parent)
{
  bool isConnected = false;
  isConnected = connect(this, SIGNAL(gridRowSelected(OSItem*)), this, SLOT(selectItem(OSItem*)));
  OS_ASSERT(isConnected);
}

boost::optional<openstudio::model::ModelObject> ModelObjectListView::selectedModelObject() const
{
  OSItem* selectedItem = this->selectedItem();
  ModelObjectItem* modelObjectItem = qobject_cast<ModelObjectItem*>(selectedItem);
  if (modelObjectItem){
    return modelObjectItem->modelObject();
  }
  return boost::none;
}

IddObjectType ModelObjectListView::iddObjectType() const
{
  OSVectorController* vectorController = this->vectorController();
  ModelObjectListController* modelObjectListController = qobject_cast<ModelObjectListController*>(vectorController);
  OS_ASSERT(modelObjectListController);
  return modelObjectListController->iddObjectType();
}

} // openstudio

