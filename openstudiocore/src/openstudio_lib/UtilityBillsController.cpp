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

#include "UtilityBillsController.hpp"

#include "UtilityBillAllFuelTypesListView.hpp"
#include "UtilityBillsView.hpp"

#include "../model/Model.hpp"
#include "../model/Model_Impl.hpp"
#include "../model/UtilityBill.hpp"
#include "../model/UtilityBill_Impl.hpp"

#include "OSInspectorView.hpp"
#include "OSItemSelectorButtons.hpp"

#include "../utilities/core/Assert.hpp"
#include "../utilities/data/DataEnums.hpp"

namespace openstudio {

UtilityBillsController::UtilityBillsController(const model::Model& model)
  : ModelSubTabController(new UtilityBillsView(model), model)
{
  subTabView()->itemSelectorButtons()->disableCopyButton();
  subTabView()->itemSelectorButtons()->disablePurgeButton();
  subTabView()->itemSelectorButtons()->hideDropZone();

  UtilityBillsInspectorView * utilityBillsInspectorView = static_cast<UtilityBillsInspectorView *>(subTabView()->inspectorView());

  connect(this, &UtilityBillsController::toggleUnitsClicked, utilityBillsInspectorView, &UtilityBillsInspectorView::toggleUnitsClicked);

  connect(utilityBillsInspectorView, &UtilityBillsInspectorView::enableAddNewObjectButton, this, &UtilityBillsController::enableAddNewObjectButton);
}

void UtilityBillsController::onAddObject(const openstudio::IddObjectType& iddObjectType)
{
  OSItemSelector* itemSelector = this->subTabView()->itemSelector(); 
  UtilityBillAllFuelTypesListView * utilityBillAllFuelTypesListView = qobject_cast<UtilityBillAllFuelTypesListView *>(itemSelector);
  OS_ASSERT(utilityBillAllFuelTypesListView);
  FuelType fuelType = utilityBillAllFuelTypesListView->currentFuelType();
  model::Model model = this->model();
  openstudio::model::UtilityBill(fuelType,model);
}

void UtilityBillsController::onCopyObject(const openstudio::model::ModelObject& modelObject)
{
  // not desired
}

void UtilityBillsController::onRemoveObject(openstudio::model::ModelObject modelObject)
{
  modelObject.remove();
}

void UtilityBillsController::onReplaceObject(openstudio::model::ModelObject modelObject, const OSItemId& replacementItemId)
{
  // not yet implemented
}

void UtilityBillsController::onPurgeObjects(const openstudio::IddObjectType& iddObjectType)
{
  // DLM: we don't want the purge button enabled
}

void UtilityBillsController::onDrop(const OSItemId& itemId)
{
  // DLM: we don't want the drop zone enabled
}

void UtilityBillsController::onInspectItem(OSItem* item)
{
  subTabView()->inspectorView()->selectItem(item);
}

void UtilityBillsController::onSelectItem(OSItem* item)
{
  m_subTabView->inspectorView()->selectItem(item);
  m_subTabView->itemSelectorButtons()->disableAddButton();
  m_subTabView->itemSelectorButtons()->enableRemoveButton();
}

void UtilityBillsController::onClearSelection()
{
  m_subTabView->inspectorView()->clearSelection();
  m_subTabView->itemSelectorButtons()->disableRemoveButton();

  openstudio::OSInspectorView * inspectorView = subTabView()->inspectorView();
  UtilityBillsInspectorView * utilityBillsInspectorView = qobject_cast<UtilityBillsInspectorView *>(inspectorView);
  OS_ASSERT(utilityBillsInspectorView);

  enableAddNewObjectButton(utilityBillsInspectorView->runPeriodDates());
}

///// SLOTS

void UtilityBillsController::enableAddNewObjectButton(bool enable)
{
  if(enable){
    m_subTabView->itemSelectorButtons()->enableAddButton();
  } else {
    m_subTabView->itemSelectorButtons()->disableAddButton();
  };
}

} // openstudio
