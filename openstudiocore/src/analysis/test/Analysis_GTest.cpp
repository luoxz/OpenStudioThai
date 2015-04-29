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
#include "AnalysisFixture.hpp"

#include "../Analysis.hpp"
#include "../Analysis_Impl.hpp"
#include "../Problem.hpp"
#include "../Variable.hpp"
#include "../DataPoint.hpp"
#include "../DataPoint_Impl.hpp"
#include "../Measure.hpp"
#include "../MeasureGroup.hpp"
#include "../MeasureGroup_Impl.hpp"
#include "../NullMeasure.hpp"
#include "../RubyMeasure.hpp"
#include "../RubyMeasure_Impl.hpp"
#include "../RubyContinuousVariable.hpp"
#include "../ParameterStudyAlgorithm.hpp"
#include "../ParameterStudyAlgorithm_Impl.hpp"
#include "../ParameterStudyAlgorithmOptions.hpp"
#include "../WorkflowStep.hpp"

#include "../../runmanager/lib/Workflow.hpp"
#include "../../runmanager/lib/RunManager.hpp"

#include "../../ruleset/OSArgument.hpp"

#include "../../utilities/core/Containers.hpp"
#include "../../utilities/core/PathHelpers.hpp"
#include "../../utilities/bcl/BCLMeasure.hpp"
#include "../../utilities/data/Tag.hpp"

#include <resources.hxx>
#include <OpenStudio.hxx>
#include <runmanager/Test/ToolBin.hxx>

using namespace openstudio;
using namespace openstudio::analysis;
using namespace openstudio::ruleset;

TEST_F(AnalysisFixture, Analysis_Constructors) {
  // minimal information
  Analysis analysis("Analysis",
                    Problem("Problem",VariableVector(),runmanager::Workflow()),
                    FileReference(toPath("./in.osm")));
  EXPECT_TRUE(analysis.isDirty());

  analysis = Analysis("Analysis",
                      Problem("Problem",VariableVector(),runmanager::Workflow()),
                      FileReferenceType::OSM);
  EXPECT_TRUE(analysis.isDirty());
  EXPECT_EQ("*.osm",toString(analysis.seed().path()));
}

TEST_F(AnalysisFixture, Analysis_SetSeed) {
  Analysis analysis("Analysis",
                    Problem("Problem",VariableVector(),runmanager::Workflow()),
                    FileReferenceType::OSM);
  EXPECT_TRUE(analysis.isDirty());
  bool ok = analysis.clearDirtyFlag();
  EXPECT_TRUE(ok);
  EXPECT_FALSE(analysis.isDirty());
  EXPECT_FALSE(analysis.problem().isDirty());

  ok = analysis.setSeed(FileReference(toPath("MyModel.osm")));
  EXPECT_TRUE(ok);
  EXPECT_TRUE(analysis.isDirty());
  EXPECT_FALSE(analysis.problem().isDirty());
  EXPECT_FALSE(analysis.resultsAreInvalid()); // no results in analysis to be invalidated
  EXPECT_FALSE(analysis.dataPointsAreInvalid());
  ok = analysis.clearDirtyFlag();
  EXPECT_TRUE(ok);

  // if have data point with no results, results are not invalid

  OptionalDataPoint dataPoint = analysis.problem().createDataPoint(std::vector<QVariant>());
  ASSERT_TRUE(dataPoint);
  ok = analysis.addDataPoint(*dataPoint);
  EXPECT_TRUE(ok);
  dataPoint = analysis.problem().createDataPoint(std::vector<QVariant>());
  ASSERT_TRUE(dataPoint);
  ok = analysis.addDataPoint(*dataPoint);
  EXPECT_FALSE(ok); // cannot add second data point with same variable values
  ASSERT_FALSE(analysis.dataPoints().empty());
  EXPECT_TRUE(analysis.dataPoints()[0].isDirty());
  analysis.clearDirtyFlag();
  EXPECT_FALSE(analysis.dataPoints()[0].isDirty());
  ok = analysis.setSeed(FileReference(toPath("YourModel.osm")));
  EXPECT_TRUE(ok);
  EXPECT_TRUE(analysis.isDirty());
  EXPECT_FALSE(analysis.problem().isDirty());
  ASSERT_FALSE(analysis.dataPoints().empty());
  EXPECT_FALSE(analysis.dataPoints()[0].isDirty());
  EXPECT_FALSE(analysis.resultsAreInvalid());
  EXPECT_FALSE(analysis.dataPointsAreInvalid());
  ok = analysis.clearDirtyFlag();
  EXPECT_TRUE(ok);

  // if data point has results, results are invalidated by changing seed

  analysis.removeAllDataPoints();
  dataPoint = DataPoint(createUUID(),
                        createUUID(),
                        "Fake Data Point",
                        "",
                        "",
                        analysis.problem(),
                        true,
                        false,
                        true,
                        DataPointRunType::Local,
                        std::vector<QVariant>(),
                        DoubleVector(),
                        toPath("dataPoint1"),
                        FileReference(toPath("out.osm")),
                        FileReference(toPath("in.idf")),
                        OptionalFileReference(),
                        boost::none,
                        std::vector<openstudio::path>(),
                        TagVector(),
                        AttributeVector()); //DLM: Elaine is this ok?
  ok = analysis.addDataPoint(*dataPoint);
  EXPECT_TRUE(ok);
  analysis.clearDirtyFlag();
  ok = analysis.setSeed(FileReference(toPath("HerModel.osm")));
  EXPECT_TRUE(ok);
  EXPECT_TRUE(analysis.isDirty());
  EXPECT_FALSE(analysis.problem().isDirty());
  ASSERT_FALSE(analysis.dataPoints().empty());
  EXPECT_FALSE(analysis.dataPoints()[0].isDirty());
  EXPECT_TRUE(analysis.resultsAreInvalid());
  EXPECT_FALSE(analysis.dataPointsAreInvalid());
  ok = analysis.clearDirtyFlag();
  EXPECT_FALSE(ok); // have to clear results first, before declaring clean
  analysis.clearAllResults();
  EXPECT_FALSE(analysis.resultsAreInvalid());
  ASSERT_FALSE(analysis.dataPoints().empty());
  EXPECT_TRUE(analysis.dataPoints()[0].isDirty()); // results were deleted
  ok = analysis.clearDirtyFlag();
  EXPECT_TRUE(ok);
}

TEST_F(AnalysisFixture, Analysis_DataPointsAreInvalid) {
  Analysis analysis("Analysis",
                    Problem("Problem",VariableVector(),runmanager::Workflow()),
                    FileReferenceType::OSM);
  EXPECT_FALSE(analysis.dataPointsAreInvalid());

  // add a single, null-only variable
  // ETH@20130206 - Alternate code for this test.
  // Problem problem = analysis.problem();
  // MeasureGroup dv("South Facade WWR",
  //                     MeasureVector(1u,NullMeasure()));
  bool test = analysis.problem().push(MeasureGroup("South Facade WWR",
                                                   MeasureVector(1u,NullMeasure())));
  // ETH@20130206
  // bool test = problem.push(dv);
  EXPECT_TRUE(test);
  ASSERT_FALSE(analysis.problem().variables().empty());
  EXPECT_FALSE(analysis.dataPointsAreInvalid());

  // create a data point
  OptionalDataPoint dataPoint = analysis.problem().createDataPoint(std::vector<QVariant>(1u,0));
  ASSERT_TRUE(dataPoint);
  test = analysis.addDataPoint(*dataPoint);
  EXPECT_TRUE(test);

  // pushing measures does not invalidate data point
  RubyMeasure measure1(toPath("myMeasure.rb"),
                       FileReferenceType::OSM,
                       FileReferenceType::OSM,
                       true);
  // ETH@20130206
  // test = dv.push(measure1);
  // EXPECT_EQ(1u,dv.numMeasures(false));
  // EXPECT_EQ(2u,analysis.problem().variables()[0].cast<MeasureGroup>().numMeasures(false));
  test = analysis.problem().variables()[0].cast<MeasureGroup>().push(measure1);
  EXPECT_TRUE(test);
  EXPECT_FALSE(analysis.dataPointsAreInvalid());
  // should be able to add another data point
  dataPoint = analysis.problem().createDataPoint(std::vector<QVariant>(1u,1));
  ASSERT_TRUE(dataPoint);
  test = analysis.addDataPoint(*dataPoint);
  EXPECT_TRUE(test);

  RubyMeasure measure2 = measure1.clone().cast<RubyMeasure>();
  test = analysis.problem().variables()[0].cast<MeasureGroup>().push(measure2);
  EXPECT_TRUE(test);
  EXPECT_FALSE(analysis.dataPointsAreInvalid());

  // swapping measures invalidates data points
  test = analysis.problem().variables()[0].cast<MeasureGroup>().swap(measure1,measure2);
  EXPECT_TRUE(test);
  EXPECT_TRUE(analysis.dataPointsAreInvalid());
  // and should not be able to add data points now
  dataPoint = analysis.problem().createDataPoint(std::vector<QVariant>(1u,2));
  ASSERT_TRUE(dataPoint);
  test = analysis.addDataPoint(*dataPoint);
  EXPECT_FALSE(test);

  // removing all data points clears state
  analysis.removeAllDataPoints();
  EXPECT_FALSE(analysis.dataPointsAreInvalid());
  // and can now add points again
  dataPoint = analysis.problem().createDataPoint(std::vector<QVariant>(1u,1));
  ASSERT_TRUE(dataPoint);
  test = analysis.addDataPoint(*dataPoint);
  EXPECT_TRUE(test);

  // adding a new variable re-invalidates them
  test = analysis.problem().push(MeasureGroup("West Facade WWR",
                                              MeasureVector(1u,NullMeasure())));
  EXPECT_TRUE(test);
  EXPECT_TRUE(analysis.dataPointsAreInvalid());
  std::vector<QVariant> values;
  values.push_back(QVariant(2));
  values.push_back(QVariant(0));
  dataPoint = analysis.problem().createDataPoint(values);
  ASSERT_TRUE(dataPoint);
  test = analysis.addDataPoint(*dataPoint);
  EXPECT_FALSE(test);

  // removing all data points clears state
  analysis.removeAllDataPoints();
  EXPECT_FALSE(analysis.dataPointsAreInvalid());
  // and can now add points again
  test = analysis.addDataPoint(*dataPoint);
  EXPECT_TRUE(test);

  // erasing a variable re-invalidates ...
  test = analysis.problem().erase(analysis.problem().workflow()[0]);
  EXPECT_TRUE(test);
  EXPECT_TRUE(analysis.dataPointsAreInvalid());
}

TEST_F(AnalysisFixture, Analysis_ClearAllResults) {
  // create dummy problem
  BCLMeasure bclMeasure(resourcesPath() / toPath("utilities/BCL/Measures/v2/SetWindowToWallRatioByFacade"));
  RubyMeasure measure(bclMeasure);
  OSArgument arg = OSArgument::makeDoubleArgument("wwr");
  RubyContinuousVariable var("Window to Wall Ratio",
                             arg,
                             measure);
  var.setMinimum(0.0);
  var.setMaximum(0.6);
  Problem problem("Problem",VariableVector(1u,var),runmanager::Workflow());

  // create dummy algorithm (already initialized to be complete)
  ParameterStudyAlgorithmOptions options(ParameterStudyAlgorithmType::list_parameter_study);
  DoubleVector points;
  for (double val = 0.0; val < 0.6; val += 0.1) {
    points.push_back(val);
  }
  options.setListOfPoints(points);
  ParameterStudyAlgorithm algorithm(createUUID(),
                                    createUUID(),
                                    std::string(),
                                    std::string(),
                                    true,
                                    false,
                                    0,
                                    options,
                                    FileReference(toPath("dakota.rst")),
                                    FileReference(toPath("dakota.out")),
                                    boost::none); //DLM: Elaine is this ok?

  // create analysis
  Analysis analysis("Analysis",problem,algorithm,FileReference(toPath("./in.osm")));

  // add data points
  std::stringstream ss;
  int i = 1;
  for (double point : points) {
    ss << "dataPoint" << i;
    DataPoint dataPoint(createUUID(),
                        createUUID(),
                        "",
                        "",
                        "",
                        problem,
                        true,
                        false,
                        true,
                        DataPointRunType::Local,
                        std::vector<QVariant>(1u,point),
                        DoubleVector(),
                        toPath(ss.str()),
                        FileReference(toPath(ss.str() + "/out.osm")),
                        FileReference(toPath(ss.str() + "/in.idf")),
                        OptionalFileReference(),
                        boost::none,
                        std::vector<openstudio::path>(),
                        TagVector(),
                        AttributeVector()); //DLM: Elaine is this ok?
    ss.str("");
    bool test = analysis.addDataPoint(dataPoint);
    EXPECT_TRUE(test);
  }

  // check current status
  EXPECT_EQ(points.size(),analysis.dataPoints().size());
  EXPECT_EQ(points.size(),analysis.completeDataPoints().size());
  ASSERT_TRUE(analysis.algorithm());
  EXPECT_TRUE(analysis.algorithm()->isComplete());
  EXPECT_FALSE(analysis.algorithm()->failed());
  ASSERT_TRUE(analysis.algorithm()->optionalCast<DakotaAlgorithm>());
  EXPECT_TRUE(analysis.algorithm()->cast<DakotaAlgorithm>().restartFileReference());
  EXPECT_TRUE(analysis.algorithm()->cast<DakotaAlgorithm>().outFileReference());

  // clear results and check status
  analysis.clearAllResults();
  EXPECT_EQ(points.size(),analysis.dataPoints().size());
  EXPECT_TRUE(analysis.completeDataPoints().empty());
  ASSERT_TRUE(analysis.algorithm());
  EXPECT_FALSE(analysis.algorithm()->isComplete());
  EXPECT_FALSE(analysis.algorithm()->failed());
  ASSERT_TRUE(analysis.algorithm()->optionalCast<DakotaAlgorithm>());
  EXPECT_FALSE(analysis.algorithm()->cast<DakotaAlgorithm>().restartFileReference());
  EXPECT_FALSE(analysis.algorithm()->cast<DakotaAlgorithm>().outFileReference());
}

TEST_F(AnalysisFixture,Analysis_JSONSerialization_PreRun_Roundtrip) {
  // Create example analysis
  Analysis analysis = analysis1(PreRun);

  // Serialize Analysis with no data points
  std::string json = analysis.toJSON(AnalysisSerializationOptions());
  EXPECT_FALSE(json.empty());

  // Deserialize and check results
  AnalysisJSONLoadResult loadResult = loadJSON(json);
  ASSERT_TRUE(loadResult.analysisObject);
  ASSERT_TRUE(loadResult.analysisObject->optionalCast<Analysis>());
  Analysis formulationCopy = loadResult.analysisObject->cast<Analysis>();
  std::string jsonCopy = formulationCopy.toJSON(AnalysisSerializationOptions());
  bool test = (jsonCopy == json);
  EXPECT_TRUE(test);
  if (!test) {
    LOG(Debug,"Original JSON: " << std::endl << json);
    LOG(Debug,"Copy JSON: " << std::endl << jsonCopy);
  }
  EXPECT_EQ(0u,formulationCopy.dataPoints().size());

  // Save analysis with no data points
  openstudio::path p = toPath("AnalysisFixtureData/formulation_pre_run.json");
  EXPECT_TRUE(analysis.saveJSON(p,AnalysisSerializationOptions(),true));

  // Load and check results
  loadResult = loadJSON(p);
  ASSERT_TRUE(loadResult.analysisObject);
  ASSERT_TRUE(loadResult.analysisObject->optionalCast<Analysis>());
  formulationCopy = loadResult.analysisObject->cast<Analysis>();
  jsonCopy = formulationCopy.toJSON(AnalysisSerializationOptions());
  test = (jsonCopy == json);
  EXPECT_TRUE(test);
  if (!test) {
    LOG(Debug,"Original JSON: " << std::endl << json);
    LOG(Debug,"Copy JSON: " << std::endl << jsonCopy);
  }
  EXPECT_EQ(0u,formulationCopy.dataPoints().size());

  // Serialize Analysis with data points
  AnalysisSerializationOptions options(openstudio::path(),AnalysisSerializationScope::Full);
  json = analysis.toJSON(options);
  EXPECT_FALSE(json.empty());

  // Deserialize and check results
  loadResult = loadJSON(json);
  ASSERT_TRUE(loadResult.analysisObject);
  ASSERT_TRUE(loadResult.analysisObject->optionalCast<Analysis>());
  Analysis copy = loadResult.analysisObject->cast<Analysis>();
  jsonCopy = copy.toJSON(options);
  test = (jsonCopy == json);
  EXPECT_TRUE(test);
  if (!test) {
    LOG(Debug,"Original JSON: " << std::endl << json);
    LOG(Debug,"Copy JSON: " << std::endl << jsonCopy);
  }
  EXPECT_FALSE(copy.dataPoints().empty());

  // Save analysis with data points
  p = toPath("AnalysisFixtureData/analysis_pre_run.json");
  EXPECT_TRUE(analysis.saveJSON(p,options,true));

  // Load and check results
  loadResult = loadJSON(p);
  ASSERT_TRUE(loadResult.analysisObject);
  ASSERT_TRUE(loadResult.analysisObject->optionalCast<Analysis>());
  copy = loadResult.analysisObject->cast<Analysis>();
  jsonCopy = copy.toJSON(options);
  test = (jsonCopy == json);
  EXPECT_TRUE(test);
  if (!test) {
    LOG(Debug,"Original JSON: " << std::endl << json);
    LOG(Debug,"Copy JSON: " << std::endl << jsonCopy);
  }
  EXPECT_FALSE(copy.dataPoints().empty());
}

TEST_F(AnalysisFixture,Analysis_JSONSerialization_Versioning) {
  openstudio::path dir = resourcesPath() / toPath("analysis/version");

  // create the formulation json file
  Analysis analysis = analysis1(PreRun);
  std::string dashVersionString = boost::regex_replace(openStudioVersion(),boost::regex("\\."),"-");
  openstudio::path p = dir / toPath("analysis_" + dashVersionString + ".json");
  bool ok = analysis.saveJSON(p,AnalysisSerializationOptions(),true);
  EXPECT_TRUE(ok);

  // loop through all versions' json files
  for (boost::filesystem::directory_iterator it(dir); it != boost::filesystem::directory_iterator(); ++it) {
    if (boost::regex_match(toString(it->path().stem()),boost::regex("analysis_.*"))) {
      LOG(Debug,"Loading " << toString(it->path().filename()) << ".");

      // open and check results
      AnalysisJSONLoadResult loadResult = loadJSON(it->path());
      ASSERT_TRUE(loadResult.analysisObject);
      ASSERT_TRUE(loadResult.analysisObject->optionalCast<Analysis>());
      Analysis loaded = loadResult.analysisObject->cast<Analysis>();
      
      Problem problem = loaded.problem();
      EXPECT_EQ(3u,problem.numVariables());
      EXPECT_EQ(7u,problem.numWorkflowSteps());
      EXPECT_EQ(3u,problem.numResponses());
    }
  }
}

TEST_F(AnalysisFixture,Analysis_JSONSerialization_Analysis2_Simulate_Roundtrip) {
  // Create example analysis
  Analysis analysis = analysis2(true);

  // Serialize Analysis with no data points
  std::string json = analysis.toJSON(AnalysisSerializationOptions());
  EXPECT_FALSE(json.empty());

  // Deserialize and check results
  AnalysisJSONLoadResult loadResult = loadJSON(json);
  ASSERT_TRUE(loadResult.analysisObject);
  ASSERT_TRUE(loadResult.analysisObject->optionalCast<Analysis>());
  Analysis formulationCopy = loadResult.analysisObject->cast<Analysis>();
  std::string jsonCopy = formulationCopy.toJSON(AnalysisSerializationOptions());
  bool test = (jsonCopy == json);
  EXPECT_TRUE(test);
  if (!test) {
    LOG(Debug,"Original JSON: " << std::endl << json);
    LOG(Debug,"Copy JSON: " << std::endl << jsonCopy);
  }
  EXPECT_EQ(0u,formulationCopy.dataPoints().size());

  // Save analysis with no data points
  openstudio::path p = toPath("AnalysisFixtureData/analysis2_simulate.json");
  EXPECT_TRUE(analysis.saveJSON(p,AnalysisSerializationOptions(),true));

  // Load and check results
  loadResult = loadJSON(p);
  ASSERT_TRUE(loadResult.analysisObject);
  ASSERT_TRUE(loadResult.analysisObject->optionalCast<Analysis>());
  formulationCopy = loadResult.analysisObject->cast<Analysis>();
  jsonCopy = formulationCopy.toJSON(AnalysisSerializationOptions());
  test = (jsonCopy == json);
  EXPECT_TRUE(test);
  if (!test) {
    LOG(Debug,"Original JSON: " << std::endl << json);
    LOG(Debug,"Copy JSON: " << std::endl << jsonCopy);
  }
  EXPECT_EQ(0u,formulationCopy.dataPoints().size());
}

TEST_F(AnalysisFixture,Analysis_JSONSerialization_Analysis2_NoSimulate_Roundtrip) {
  // Create example analysis
  Analysis analysis = analysis2(false);

  // Serialize Analysis with no data points
  std::string json = analysis.toJSON(AnalysisSerializationOptions());
  EXPECT_FALSE(json.empty());

  // Deserialize and check results
  AnalysisJSONLoadResult loadResult = loadJSON(json);
  ASSERT_TRUE(loadResult.analysisObject);
  ASSERT_TRUE(loadResult.analysisObject->optionalCast<Analysis>());
  Analysis formulationCopy = loadResult.analysisObject->cast<Analysis>();
  std::string jsonCopy = formulationCopy.toJSON(AnalysisSerializationOptions());
  bool test = (jsonCopy == json);
  EXPECT_TRUE(test);
  if (!test) {
    LOG(Debug,"Original JSON: " << std::endl << json);
    LOG(Debug,"Copy JSON: " << std::endl << jsonCopy);
  }
  EXPECT_EQ(0u,formulationCopy.dataPoints().size());

  // Save analysis with no data points
  openstudio::path p = toPath("AnalysisFixtureData/analysis2_no_simulate.json");
  EXPECT_TRUE(analysis.saveJSON(p,AnalysisSerializationOptions(),true));

  // Load and check results
  loadResult = loadJSON(p);
  ASSERT_TRUE(loadResult.analysisObject);
  ASSERT_TRUE(loadResult.analysisObject->optionalCast<Analysis>());
  formulationCopy = loadResult.analysisObject->cast<Analysis>();
  jsonCopy = formulationCopy.toJSON(AnalysisSerializationOptions());
  test = (jsonCopy == json);
  EXPECT_TRUE(test);
  if (!test) {
    LOG(Debug,"Original JSON: " << std::endl << json);
    LOG(Debug,"Copy JSON: " << std::endl << jsonCopy);
  }
  EXPECT_EQ(0u,formulationCopy.dataPoints().size());
}

TEST_F(AnalysisFixture, PatExportRun)
{
  openstudio::path patdir = resourcesPath() / openstudio::toPath("analysis") / openstudio::toPath("pat_export");
  openstudio::path forumlation = patdir / openstudio::toPath("formulation.json");
  openstudio::path data_point = patdir / openstudio::toPath("data_point_469b52c3-4aae-4cdd-b580-5c9494eefa11") / openstudio::toPath("data_point.json");;

  openstudio::path outdir = openstudio::tempDir() / openstudio::toPath("PatExportRun");
  if (exists(outdir)){
    removeDirectory(outdir);
  }
  create_directory(outdir);
  ASSERT_TRUE(exists(outdir));

  FileLogSink logFile(outdir / toPath("openstudio.log"));
  logFile.setLogLevel(Debug);

  // load the analysis formulation
  openstudio::analysis::AnalysisJSONLoadResult loadResult = openstudio::analysis::loadJSON(forumlation);
  ASSERT_TRUE(loadResult.analysisObject);
  ASSERT_TRUE(loadResult.analysisObject.get().optionalCast<openstudio::analysis::Analysis>());

  openstudio::analysis::Analysis analysis = loadResult.analysisObject.get().optionalCast<openstudio::analysis::Analysis>().get();

  analysis.updateInputPathData(loadResult.projectDir, patdir);

  // save updated formulation for reference only
  openstudio::path forumlation_final = outdir / openstudio::toPath("formulation_final.json");
  openstudio::analysis::AnalysisSerializationOptions options(patdir);
  analysis.saveJSON(forumlation_final, options, true);

  // load data point to run
  loadResult = openstudio::analysis::loadJSON(data_point);
  ASSERT_TRUE(loadResult.analysisObject);
  ASSERT_TRUE(loadResult.analysisObject.get().optionalCast<openstudio::analysis::DataPoint>());

  openstudio::analysis::DataPoint dataPoint = loadResult.analysisObject.get().optionalCast<openstudio::analysis::DataPoint>().get();


  openstudio::path db = outdir / openstudio::toPath("run.db");
  openstudio::runmanager::RunManager kit(db, true, false, false);

  openstudio::runmanager::Workflow workflow = analysis.problem().createWorkflow(dataPoint, rubyOpenStudioDir());
  openstudio::runmanager::JobParams params;
  params.append("cleanoutfiles", "standard");
  workflow.add(params);


  // Build list of tools
  openstudio::runmanager::Tools tools
    = openstudio::runmanager::ConfigOptions::makeTools(energyPlusExePath().parent_path(), openstudio::path(), openstudio::path(),
    rubyExePath().parent_path(), openstudio::path());
  workflow.add(tools);

  ASSERT_TRUE(analysis.weatherFile());

  openstudio::runmanager::Job job = workflow.create(outdir, analysis.seed().path(), analysis.weatherFile().get().path(), std::vector<openstudio::URLSearchPath>());
  openstudio::runmanager::JobFactory::optimizeJobTree(job);
  analysis.setDataPointRunInformation(dataPoint, job, std::vector<openstudio::path>());

  kit.enqueue(job, false);

  kit.waitForFinished();

  analysis.problem().updateDataPoint(dataPoint, job);

  EXPECT_FALSE(dataPoint.failed());
}
