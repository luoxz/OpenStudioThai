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

#include <gtest/gtest.h>

#include "SqlFileFixture.hpp"

#include "../../time/Date.hpp"
#include "../../time/Calendar.hpp"
#include "../../core/Optional.hpp"
#include "../../data/DataEnums.hpp"
#include "../../data/TimeSeries.hpp"
#include "../../filetypes/EpwFile.hpp"
#include "../../units/UnitFactory.hpp"
#include "../../core/Application.hpp"

#include <resources.hxx>

#include <iostream>

using namespace std;
using namespace boost;
using namespace openstudio;

TEST_F(SqlFileFixture, SummaryValues)
{
  // check values
  ASSERT_TRUE(sqlFile.netSiteEnergy());
  ASSERT_TRUE(sqlFile.totalSiteEnergy());
  ASSERT_TRUE(sqlFile.netSourceEnergy());
  ASSERT_TRUE(sqlFile.totalSourceEnergy());

  EXPECT_NEAR(224.91, *(sqlFile.netSiteEnergy()), 2);     // for 6.0, was 222.69
  EXPECT_NEAR(224.91, *(sqlFile.totalSiteEnergy()), 2);   // for 6.0, was 222.69
  EXPECT_NEAR(570.38, *(sqlFile.netSourceEnergy()), 2);   // for 6.0, was 564.41
  EXPECT_NEAR(570.38, *(sqlFile.totalSourceEnergy()), 2); // for 6.0, was 564.41
}


TEST_F(SqlFileFixture, EnvPeriods)
{
  std::vector<std::string> availableEnvPeriods = sqlFile.availableEnvPeriods();
  ASSERT_FALSE(availableEnvPeriods.empty());
  EXPECT_EQ(static_cast<unsigned>(1), availableEnvPeriods.size());
  //EXPECT_EQ("Chicago Ohare Intl Ap IL USA TMY3 WMO#=725300", availableEnvPeriods[0]);
  EXPECT_EQ("CHICAGO OHARE INTL AP IL USA TMY3 WMO#=725300", availableEnvPeriods[0]);
}

TEST_F(SqlFileFixture, TimeSeriesValues)
{
  std::vector<std::string> availableEnvPeriods = sqlFile.availableEnvPeriods();
  ASSERT_FALSE(availableEnvPeriods.empty());
  EXPECT_EQ(static_cast<unsigned>(1), availableEnvPeriods.size());
  //EXPECT_EQ("Chicago Ohare Intl Ap IL USA TMY3 WMO#=725300", availableEnvPeriods[0]);
  EXPECT_EQ("CHICAGO OHARE INTL AP IL USA TMY3 WMO#=725300", availableEnvPeriods[0]);

  openstudio::OptionalTimeSeries ts = sqlFile.timeSeries(availableEnvPeriods[0], "Hourly", "Site Outdoor Air Drybulb Temperature",  "Environment");
  ASSERT_TRUE(ts);
//  ASSERT_EQ(static_cast<unsigned>(8760), ts->dateTimes().size());
  ASSERT_EQ(static_cast<unsigned>(8760), ts->daysFromFirstReport().size());
  ASSERT_EQ(static_cast<unsigned>(8760), ts->values().size());
//  EXPECT_EQ(DateTime(Date(MonthOfYear::Jan, 1), Time(0,1,0,0)), ts->dateTimes().front());
  EXPECT_EQ(DateTime(Date(MonthOfYear::Jan, 1), Time(0,1,0,0)), ts->firstReportDateTime());
  EXPECT_DOUBLE_EQ(-8.2625, ts->values()[0]);
  EXPECT_DOUBLE_EQ(ts->outOfRangeValue(), ts->value(DateTime(Date(MonthOfYear::Jan, 1), Time(0,0,0,0))));
  // DLM: there is commented out code in openstudio::OptionalTime SqlFile_Impl::timeSeriesInterval(const DataDictionaryItem& dataDictionary)
  // that does not recognize hourly as an interval type, so the following line is not expected to work
  //EXPECT_DOUBLE_EQ(-8.2625, ts->value(DateTime(Date(MonthOfYear::Jan, 1), Time(0,0,0,1))));
  EXPECT_DOUBLE_EQ(-8.2625, ts->value(DateTime(Date(MonthOfYear::Jan, 1), Time(0,1,0,0))));
  EXPECT_DOUBLE_EQ(-11.8875, ts->value(DateTime(Date(MonthOfYear::Jan, 1), Time(0,1,0,1))));
//  EXPECT_EQ(DateTime(Date(MonthOfYear::Dec, 31), Time(0,24,0,0)), ts->dateTimes().back());
  EXPECT_EQ(DateTime(Date(MonthOfYear::Dec, 31), Time(0,24,0,0)), ts->firstReportDateTime()+Time(ts->daysFromFirstReport(ts->daysFromFirstReport().size()-1)));
  EXPECT_DOUBLE_EQ(-5.6875, ts->values()[8759]);
  EXPECT_DOUBLE_EQ(-4.775, ts->value(DateTime(Date(MonthOfYear::Dec, 31), Time(0,22,59,59))));
  EXPECT_DOUBLE_EQ(-4.775, ts->value(DateTime(Date(MonthOfYear::Dec, 31), Time(0,23,0,0))));
  EXPECT_DOUBLE_EQ(-5.6875, ts->value(DateTime(Date(MonthOfYear::Dec, 31), Time(0,23,0,1))));
  EXPECT_DOUBLE_EQ(-5.6875, ts->value(DateTime(Date(MonthOfYear::Dec, 31), Time(0,24,0,0))));
  EXPECT_DOUBLE_EQ(ts->outOfRangeValue(), ts->value(DateTime(Date(MonthOfYear::Dec, 31), Time(0,24,0,1))));

  ts = sqlFile.timeSeries(availableEnvPeriods[0], "HVAC System Timestep", "Site Outdoor Air Drybulb Temperature",  "Environment");
  ASSERT_TRUE(ts);
//  EXPECT_EQ(DateTime(Date(MonthOfYear::Jan, 1), Time(0,0,15,0)), ts->dateTimes().front());
  EXPECT_EQ(DateTime(Date(MonthOfYear::Jan, 1), Time(0,0,15,0)), ts->firstReportDateTime());
  EXPECT_DOUBLE_EQ(ts->outOfRangeValue(), ts->value(DateTime(Date(MonthOfYear::Jan, 1), Time(0,0,0,0))));
  EXPECT_DOUBLE_EQ(ts->outOfRangeValue(), ts->value(DateTime(Date(MonthOfYear::Jan, 1), Time(0,0,0,1))));
  EXPECT_DOUBLE_EQ(ts->outOfRangeValue(), ts->value(DateTime(Date(MonthOfYear::Jan, 1), Time(0,0,0,14))));
  EXPECT_DOUBLE_EQ(-4.325, ts->value(DateTime(Date(MonthOfYear::Jan, 1), Time(0,0,15,0))));
  EXPECT_DOUBLE_EQ(-6.95, ts->value(DateTime(Date(MonthOfYear::Jan, 1), Time(0,0,15,1))));
  EXPECT_DOUBLE_EQ(-6.95, ts->value(DateTime(Date(MonthOfYear::Jan, 1), Time(0,0,30,0))));
  EXPECT_DOUBLE_EQ(-12.2 , ts->value(DateTime(Date(MonthOfYear::Jan, 1), Time(0,1,0,0))));
//  EXPECT_EQ(DateTime(Date(MonthOfYear::Dec, 31), Time(0,24,0,0)), ts->dateTimes().back());
  EXPECT_EQ(DateTime(Date(MonthOfYear::Dec, 31), Time(0,24,0,0)), ts->firstReportDateTime()+Time(ts->daysFromFirstReport(ts->daysFromFirstReport().size()-1)));
  EXPECT_DOUBLE_EQ(-5.0  , ts->value(DateTime(Date(MonthOfYear::Dec, 31), Time(0,22,59,59))));
  EXPECT_DOUBLE_EQ(-5.0  , ts->value(DateTime(Date(MonthOfYear::Dec, 31), Time(0,23,0,0))));
  EXPECT_DOUBLE_EQ(-5.275, ts->value(DateTime(Date(MonthOfYear::Dec, 31), Time(0,23,0,1))));
  EXPECT_DOUBLE_EQ(-5.275, ts->value(DateTime(Date(MonthOfYear::Dec, 31), Time(0,23,15,0))));
  EXPECT_DOUBLE_EQ(-6.1  , ts->value(DateTime(Date(MonthOfYear::Dec, 31), Time(0,24,0,0))));
  EXPECT_DOUBLE_EQ(ts->outOfRangeValue(), ts->value(DateTime(Date(MonthOfYear::Dec, 31), Time(0,24,0,1))));
}

TEST_F(SqlFileFixture, TimeSeriesCount)
{
  std::vector<std::string> availableTimeSeries = sqlFile.availableTimeSeries();
  ASSERT_FALSE(availableTimeSeries.empty());

  std::vector<std::string> availableEnvPeriods = sqlFile.availableEnvPeriods();
  ASSERT_FALSE(availableEnvPeriods.empty());

  // check that "Electricity:Facility" is available
  ASSERT_EQ(1, std::count(availableTimeSeries.begin(), availableTimeSeries.end(), "Electricity:Facility"));
  openstudio::OptionalTimeSeries ts = sqlFile.timeSeries(availableEnvPeriods[0], "Hourly", "Electricity:Facility",  "");
  ASSERT_TRUE(ts);

  // check that "Gas:Facility" is available
  ASSERT_EQ(1, std::count(availableTimeSeries.begin(), availableTimeSeries.end(), "Gas:Facility"));
  ts = sqlFile.timeSeries(availableEnvPeriods[0], "Hourly", "Gas:Facility",  "");
  ASSERT_TRUE(ts);

  // check that "NotAVariable:Facility" is not available
  EXPECT_TRUE(availableTimeSeries.end() == std::find(availableTimeSeries.begin(), availableTimeSeries.end(), "NotAVariable:Facility"));
  ts = sqlFile.timeSeries(availableEnvPeriods[0], "Hourly", "NotAVariable:Facility",  "");
  EXPECT_FALSE(ts);

  // check that "Electricity:Facility" is available
  ASSERT_EQ(1, std::count(availableTimeSeries.begin(), availableTimeSeries.end(), "Electricity:Facility"));
  ts = sqlFile.timeSeries(availableEnvPeriods[0], "Run Period", "Electricity:Facility",  "");
  ASSERT_TRUE(ts);

  // check that "Gas:Facility" is available
  ASSERT_EQ(1, std::count(availableTimeSeries.begin(), availableTimeSeries.end(), "Gas:Facility"));
  ts = sqlFile.timeSeries(availableEnvPeriods[0], "Run Period", "Gas:Facility",  "");
  ASSERT_TRUE(ts);

  // check that "NotAVariable:Facility" is not available
  EXPECT_TRUE(availableTimeSeries.end() == std::find(availableTimeSeries.begin(), availableTimeSeries.end(), "NotAVariable:Facility"));
  ts = sqlFile.timeSeries(availableEnvPeriods[0], "Run Period", "NotAVariable:Facility",  "");
  EXPECT_FALSE(ts);
}

TEST_F(SqlFileFixture, TimeSeries)
{
  std::vector<std::string> availableEnvPeriods = sqlFile.availableEnvPeriods();
  ASSERT_FALSE(availableEnvPeriods.empty());

  // make a timeseries
  openstudio::OptionalTimeSeries ts = sqlFile.timeSeries(availableEnvPeriods[0], "Hourly", "Electricity:Facility",  "");
  ASSERT_TRUE(ts);

  // should have 365 days minus 1 hour
//  Time duration = ts->dateTimes().back() - ts->dateTimes().front();
  Time duration = ts->firstReportDateTime() + Time(ts->daysFromFirstReport(ts->daysFromFirstReport().size()-1)) - ts->firstReportDateTime();
  EXPECT_DOUBLE_EQ(365-1.0/24.0, duration.totalDays());
}

TEST_F(SqlFileFixture, BadStatement)
{
  OptionalDouble result = sqlFile.execAndReturnFirstDouble("SELECT * FROM NonExistantTable");
  EXPECT_FALSE(result);
}

TEST_F(SqlFileFixture, CreateSqlFile)
{
  openstudio::path outfile = openstudio::tempDir() / openstudio::toPath("OpenStudioSqlFileTest.sql");
  if (boost::filesystem::exists(outfile))
  {
    boost::filesystem::remove(outfile);
  }

  openstudio::Calendar c(2012);
  c.standardHolidays();


  std::vector<double> values;
  values.push_back(100);
  values.push_back(10);
  values.push_back(1);
  values.push_back(100.5);

  TimeSeries timeSeries(c.startDate(), openstudio::Time(0,1), openstudio::createVector(values), "lux");

  {
    openstudio::SqlFile sql(outfile,
        openstudio::EpwFile(resourcesPath() / toPath("runmanager/USA_CO_Golden-NREL.724666_TMY3.epw")),
        openstudio::DateTime::now(),
        c);

    EXPECT_TRUE(sql.connectionOpen());

    sql.insertTimeSeriesData("Sum", "Zone", "Zone", "DAYLIGHTING WINDOW", "Daylight Luminance", openstudio::ReportingFrequency::Hourly,
        boost::optional<std::string>(), "lux", timeSeries);

    sql.insertZone("CLASSROOM",
        0,
        0,0,0,
        1,1,1,
        3,
        1,
        1,
        0, 2,
        0, 2,
        0, 2,
        2,
        8,
        3,
        3,
        4,
        4,
        2,
        2,
        true);

    std::vector<double> xs;
    std::vector<double> ys;
    xs.push_back(1.1);
    xs.push_back(2.2);
    xs.push_back(3.3);

    ys.push_back(2.8);
    ys.push_back(3.9);

    std::vector<DateTime> dates;
    dates.push_back(DateTime("2012-09-01 1:00"));
    dates.push_back(DateTime("2012-09-01 2:00"));

    std::vector<Matrix> maps;

    Matrix m1(3, 2);
    m1(0,0) = 0;
    m1(1,0) = 0.1;
    m1(2,0) = 0.2;
    m1(0,1) = 0.3;
    m1(1,1) = 0.4;
    m1(2,1) = 0.5;

    maps.push_back(m1);
    Matrix m2(3, 2);
    m1(0,0) = 2;
    m1(1,0) = 2.1;
    m1(2,0) = 2.2;
    m1(0,1) = 2.3;
    m1(1,1) = 2.4;
    m1(2,1) = 2.5;
    maps.push_back(m2);


    sql.insertIlluminanceMap("CLASSROOM", "CLASSROOM DAYLIGHT MAP", "GOLDEN",
      dates, xs, ys, 3.8, maps);
  }


  {
    openstudio::SqlFile sql(outfile);
    EXPECT_TRUE(sql.connectionOpen());
    std::vector<std::string> envPeriods = sql.availableEnvPeriods();
    ASSERT_EQ(envPeriods.size(), 1u);
    std::vector<std::string> reportingFrequencies = sql.availableReportingFrequencies(envPeriods[0]);
    ASSERT_EQ(reportingFrequencies.size(), 1u);
    std::vector<std::string> timeSeriesNames = sql.availableTimeSeries();
    ASSERT_EQ(reportingFrequencies.size(), 1u);
    std::vector<std::string> keyValues = sql.availableKeyValues(envPeriods[0], reportingFrequencies[0], timeSeriesNames[0]);
    ASSERT_EQ(keyValues.size(), 1u);

    boost::optional<TimeSeries> ts = sql.timeSeries(envPeriods[0], reportingFrequencies[0], timeSeriesNames[0], keyValues[0]);
    ASSERT_TRUE(ts);

    EXPECT_EQ(openstudio::toStandardVector(ts->values()), openstudio::toStandardVector(timeSeries.values()));
    EXPECT_EQ(openstudio::toStandardVector(ts->daysFromFirstReport()), openstudio::toStandardVector(timeSeries.daysFromFirstReport()));
  }

}

TEST_F(SqlFileFixture, AnnualTotalCosts) {
  
  // Total annual costs for all fuel types
  EXPECT_NEAR(205810981.2, *(sqlFile2.annualTotalUtilityCost()), 0.1);

  // Costs by fuel type
  EXPECT_NEAR(28388.36, *(sqlFile2.annualTotalCost(FuelType::Electricity)), 0.1);
  EXPECT_NEAR(427.78, *(sqlFile2.annualTotalCost(FuelType::Gas)), 0.1);
  EXPECT_NEAR(330.76, *(sqlFile2.annualTotalCost(FuelType::DistrictCooling)), 0.1);
  EXPECT_NEAR(833.49, *(sqlFile2.annualTotalCost(FuelType::DistrictHeating)), 0.1);
  EXPECT_NEAR(0.85, *(sqlFile2.annualTotalCost(FuelType::Water)), 0.1);
  EXPECT_NEAR(205781000, *(sqlFile2.annualTotalCost(FuelType::FuelOil_1)), 100);

  // Costs by total building area by fuel type
  EXPECT_NEAR(11.83, *(sqlFile2.annualTotalCostPerBldgArea(FuelType::Electricity)), 0.1);
  EXPECT_NEAR(0.18, *(sqlFile2.annualTotalCostPerBldgArea(FuelType::Gas)), 0.1);

  // Costs by conditioned building area by fuel type
  EXPECT_NEAR(11.83, *(sqlFile2.annualTotalCostPerNetConditionedBldgArea(FuelType::Electricity)), 0.1);
  EXPECT_NEAR(0.18, *(sqlFile2.annualTotalCostPerNetConditionedBldgArea(FuelType::Gas)), 0.1);

}
