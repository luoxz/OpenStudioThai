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

#include "LocationTabController.hpp"

#include "LifeCycleCostsTabView.hpp"
#include "LocationTabView.hpp"
#include "UtilityBillsView.hpp"
#include "UtilityBillsController.hpp"

#include "../model/Model.hpp"
#include "../model/Model_Impl.hpp"
#include "../model/RunPeriod.hpp"
#include "../model/RunPeriod_Impl.hpp"
#include "../model/WeatherFile.hpp"
#include "../model/WeatherFile_Impl.hpp"
#include "../model/YearDescription.hpp"
#include "../model/YearDescription_Impl.hpp"

#include <QLabel>
#include <QStackedWidget>

namespace openstudio {

LocationTabController::LocationTabController(const model::Model & model,
                                             const QString& modelTempDir,
											 const QString& resourceDir)
  : MainTabController(new LocationTabView(model,modelTempDir,resourceDir))
{
	LocationView * locationView = new LocationView(model, modelTempDir, resourceDir);
  mainContentWidget()->addSubTab("Weather File && Design Days",locationView,WEATHER_FILE);

  LifeCycleCostsView * lifeCycleCostsView = new LifeCycleCostsView(model);
  mainContentWidget()->addSubTab("Life Cycle Costs",lifeCycleCostsView,LIFE_CYCLE_COSTS);

  QLabel * label;

  label = new QLabel();
  label->setPixmap(QPixmap(":/images/utility_calibration_warning.png"));
  label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

  m_utilityBillsController = std::shared_ptr<UtilityBillsController>(new UtilityBillsController(model));

  m_utilityBillsStackedWidget = new QStackedWidget();
  m_warningWidgetIndex = m_utilityBillsStackedWidget->addWidget(label);
  m_visibleWidgetIndex = m_utilityBillsStackedWidget->addWidget(m_utilityBillsController->subTabView()); 

  mainContentWidget()->addSubTab("Utility Bills",m_utilityBillsStackedWidget,UTILITY_BILLS);

  // Determine if the utility bill sub-tab is shown
  boost::optional<model::YearDescription> yearDescription = model.yearDescription();
  if (yearDescription){
    boost::optional<int> calendarYear = yearDescription.get().calendarYear();
    if (calendarYear){
      boost::optional<model::WeatherFile> weatherFile = model.weatherFile();
      if (weatherFile){
        boost::optional<model::RunPeriod> runPeriod = model.getOptionalUniqueModelObject<model::RunPeriod>();
        if (runPeriod.is_initialized()){
          m_utilityBillsStackedWidget->setCurrentIndex(m_visibleWidgetIndex);
        }
        else {
          m_utilityBillsStackedWidget->setCurrentIndex(m_warningWidgetIndex);
        }
      }
    }
  }

  // Hack code to remove when tab active
  label = new QLabel();
  label->setPixmap(QPixmap(":/images/coming_soon_utility_rates.png"));
  label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  mainContentWidget()->addSubTab("Utility Rates",label,UTILITY_RATES);

  // Hack code to remove when tab active
  //label = new QLabel();
  //label->setPixmap(QPixmap(":/images/coming_soon_ground_temperature.png"));
  //label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  //mainContentWidget()->addSubTab("Ground Temperature",label,GROUND_TEMPERATURE);

  // Hack code to remove when tab active
  //label = new QLabel();
  //label->setPixmap(QPixmap(":/images/coming_soon_water_mains_temperature.png"));
  //label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  //mainContentWidget()->addSubTab("Water Mains Temperature",label,WATER_MAINS_TEMPERATURE);
}

void LocationTabController::showSubTabView(bool showSubTabView)
{
  if(showSubTabView){
    m_utilityBillsStackedWidget->setCurrentIndex(m_visibleWidgetIndex);
  } else {
    m_utilityBillsStackedWidget->setCurrentIndex(m_warningWidgetIndex);
  }
}

} // openstudio
