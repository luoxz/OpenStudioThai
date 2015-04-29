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

#include "MeasureManager.hpp"

#include "BaseApp.hpp"
#include "BCLMeasureDialog.hpp"
#include "LocalLibraryController.hpp"
#include "ProcessEventsProgressBar.hpp"

#include "BuildingComponentDialog.hpp"
#include "OSDialog.hpp"

#include "../analysisdriver/CurrentAnalysis.hpp"

#include "../analysis/Measure.hpp"
#include "../analysis/MeasureGroup.hpp"
#include "../analysis/MeasureGroup_Impl.hpp"
#include "../analysis/InputVariable.hpp"
#include "../analysis/Problem.hpp"
#include "../analysis/Analysis.hpp"
#include "../analysis/AnalysisObject.hpp"
#include "../analysis/AnalysisObject_Impl.hpp"

#include "../runmanager/lib/RunManager.hpp"

#include "../ruleset/OSArgument.hpp"

#include "../model/Model.hpp"

#include "../utilities/core/ApplicationPathHelpers.hpp"
#include "../utilities/core/Assert.hpp"
#include "../utilities/core/PathHelpers.hpp"
#include "../utilities/core/RubyException.hpp"
#include "../utilities/bcl/BCLMeasure.hpp"
#include "../utilities/bcl/RemoteBCL.hpp"

#include <QAbstractButton>
#include <QBoxLayout>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QUrl>
#include <QRadioButton>
#include <QProgressDialog>

namespace openstudio {

MeasureManager::MeasureManager(const QSharedPointer<ruleset::RubyUserScriptInfoGetter> &t_infoGetter, BaseApp *t_app)
  : m_app(t_app), m_infoGetter(t_infoGetter)
{
}

std::pair<bool,std::string> MeasureManager::updateMeasure(analysisdriver::SimpleProject &t_project, const BCLMeasure &t_measure)
{
  std::pair<bool,std::string> result(true,"");
  try {
    ruleset::OSArgumentVector args = getArguments(t_project, t_measure);
    bool differentVersions = t_project.updateMeasure(t_measure, args);
    
    if (!differentVersions) {
      OptionalBCLMeasure existingMeasure = t_project.getMeasureByUUID(t_measure.uuid());
      if (existingMeasure) {
        LOG(Debug, "Measure hasn't changed, but args were reloaded, forcing argument resetting");
        t_project.registerArguments(*existingMeasure,args);
        t_project.analysis().problem().updateMeasure(*existingMeasure, args, false);
      }
    } 
  } catch ( const RubyException &e ) {
    std::stringstream ss;
    ss << "An error occurred while updating measure '" << t_measure.displayName() << "':" << std::endl;
    ss << "  " << e.what();
    LOG(Error, ss.str());
    result = std::pair<bool,std::string>(false,ss.str());
  }

  return result;
}

BCLMeasure MeasureManager::insertReplaceMeasure(analysisdriver::SimpleProject &t_project, const UUID &t_id)
{
  boost::optional<BCLMeasure> measure = getMeasure(t_id);
  OS_ASSERT(measure);

  // if this is a user measure check one last time if there are updates
  // DLM: not sure if this is necessary
  bool isMyMeasure = (m_myMeasures.find(t_id) != m_myMeasures.end());
  if (isMyMeasure) {
    bool updated = checkForUpdates(*measure);
    if (updated) {
      measure->save(); 
      m_myMeasures.erase(t_id);
      m_myMeasures.insert(std::map<UUID,BCLMeasure>::value_type(t_id,*measure));
    }
  }

  boost::optional<BCLMeasure> existingMeasure = t_project.getMeasureByUUID(t_id);

  if (existingMeasure && (existingMeasure->versionUUID() != measure->versionUUID()))
  {
    QDialog dialog(m_app->mainWidget(), Qt::WindowFlags(Qt::Dialog | Qt::WindowTitleHint));
    auto mainContentVLayout = new QVBoxLayout();
    dialog.setWindowTitle(QCoreApplication::applicationName());

    dialog.setLayout(mainContentVLayout);
    QLabel *label = new QLabel("A modified copy of this measure is already being used in this project.");
    QRadioButton *replace = new QRadioButton("Replace all instances of this measure in this project with this one.");
    replace->setChecked(true);
    QRadioButton *copy = new QRadioButton("Create a new instance using this project's copy of this measure.");
    mainContentVLayout->addWidget(label);
    mainContentVLayout->addWidget(replace);
    mainContentVLayout->addWidget(copy);

    auto buttons = new QHBoxLayout();

    QPushButton *cancel = new QPushButton("Cancel");
    QPushButton *apply = new QPushButton("Apply");

    buttons->addStretch();
    buttons->addWidget(cancel);
    buttons->addWidget(apply);

    connect(cancel, &QPushButton::pressed, &dialog, &QDialog::reject);

    connect(apply, &QPushButton::pressed, &dialog, &QDialog::accept);
    
    mainContentVLayout->addLayout(buttons);

    if (dialog.exec() == QDialog::Accepted)
    {
      if (replace->isChecked())
      {
        LOG(Info, "User chose to replace existing instances with new version of measure");
        std::pair<bool,std::string> updateResult = updateMeasure(t_project, *measure);
        if (updateResult.first)
        {
          boost::optional<BCLMeasure> updatedMeasure = getMeasure(t_id);
          OS_ASSERT(updatedMeasure);
          return *updatedMeasure;
        } else {
          QMessageBox::critical(m_app->mainWidget(), QString("Error Updating Measure"), QString::fromStdString(updateResult.second));
          throw std::runtime_error("Unknown error occurred when calling project.updateMeasure, false was returned");
        }
      } else {
        LOG(Info, "User chose to use existing copy of measure for new instance");
        return *existingMeasure;
      }
    } else {
      LOG(Info, "User canceled drop event for mismatched measure version");
      throw std::runtime_error("User canceled update / insert of measure");
    }
  } else if (existingMeasure) {
    return *existingMeasure;
  } else {
    BCLMeasure projectmeasure = t_project.insertMeasure(*measure);
    return projectmeasure;
  }
}

std::vector<ruleset::OSArgument> MeasureManager::getArguments(analysisdriver::SimpleProject &t_project, const BCLMeasure &t_measure)
{
  auto projectMeasure = t_project.getMeasureByUUID(t_measure.uuid());
  // If currentModel and currentWorkspace are present then we are running from the OS Application
  // If they are not there then the model and workspace are in the project managed by PAT
  auto t_model = m_app->currentModel();
  auto t_idf = m_app->currentWorkspace();

  if (projectMeasure
      && projectMeasure->versionUUID() == t_measure.versionUUID()
      && t_project.hasStoredArguments(*projectMeasure)
      && ! t_model)
  {
    LOG(Info, "returning stored arguments for measure " << t_measure.displayName() << "(" << toString(t_measure.uuid()) << " version: " << toString(t_measure.versionUUID()) << ")");
    return t_project.getStoredArguments(*projectMeasure);
  } else {
    auto model = t_project.seedModel();
    auto idf = t_project.seedIdf();

    if( t_model ) {
      model = t_model;
    }

    if( t_idf ) {
      idf = t_idf;
    }

    ruleset::RubyUserScriptInfo info = m_infoGetter->getInfo(t_measure, model, idf);
    std::vector<ruleset::OSArgument> args = info.arguments();
    LOG(Info, "Loaded " << args.size() << " arguments for measure " << t_measure.displayName() << "(" << toString(t_measure.uuid()) << " version: " << toString(t_measure.versionUUID()) << ")");
    t_project.registerArguments(t_measure, args);
    return args;
  }
}

std::string MeasureManager::suggestMeasureGroupName(const BCLMeasure &t_measure)
{
  std::string baseName = t_measure.displayName();

  baseName.append(" Group");

  std::set<std::string> allNames;
  if( boost::optional<analysisdriver::SimpleProject> project = m_app->project() ){
    analysis::Analysis analysis = project->analysis();
    analysis::Problem problem = analysis.problem();
    for (const analysis::InputVariable& variable : problem.variables()) {
      allNames.insert(variable.name());
      allNames.insert(variable.displayName());
    }
  }

  std::string result = baseName;
  int i = 1;
  while (allNames.find(result) != allNames.end()){
    result = baseName + " " + QString::number(i).toStdString();
    i++;
  }

  return result;
}

std::string MeasureManager::suggestMeasureName(const BCLMeasure &t_measure, bool t_fixed)
{
  std::string baseName = t_measure.displayName();

  if (!t_fixed)
  {
    baseName.append(" Alternative");
  }

  std::set<std::string> allNames;
  if( boost::optional<analysisdriver::SimpleProject> project = m_app->project() ){
    analysis::Analysis analysis = project->analysis();
    analysis::Problem problem = analysis.problem();
    for (const analysis::InputVariable& variable : problem.variables()) {
      boost::optional<analysis::MeasureGroup> discreteVariable = variable.optionalCast<analysis::MeasureGroup>();
      if (discreteVariable){
        for (const analysis::Measure& measure : discreteVariable->measures(false)) {
          allNames.insert(measure.name());
          allNames.insert(measure.displayName());
        }
      }
    }
  }

  std::string result = baseName;
  int i = 1;
  while (allNames.find(result) != allNames.end()){
    result = baseName + " " + QString::number(i).toStdString();
    i++;
  }

  return result;
}

void MeasureManager::updateMeasures(analysisdriver::SimpleProject &t_project, 
                                    const std::vector<BCLMeasure> &t_newMeasures, 
                                    bool t_showMessage)
{
  auto progress = new ProcessEventsProgressBar();
  progress->setMaximum(std::numeric_limits<double>::max());

  size_t loc = 0;
  std::vector<std::string> failMessages;
  for (const auto & newMeasure : t_newMeasures)
  {
    progress->setValue(loc);
    std::pair<bool,std::string> updateResult = updateMeasure(t_project, newMeasure);
    if (!updateResult.first) {
      failMessages.push_back(updateResult.second);
    }
    ++loc;
  }
  progress->setValue(t_newMeasures.size());

  delete progress;

  if (t_showMessage)
  {
    size_t numUpdated = loc - failMessages.size();
    size_t numFailed = failMessages.size();

    std::stringstream ss;

    ss << numUpdated << " measure";
    if (numUpdated == 0 || numUpdated > 1) {
      ss << "s";
    }
    ss << " updated";

    if (numFailed > 0) {
      ss << std::endl << numFailed << " measure update";
      if (numFailed > 1) {
        ss << "s";
      }
      ss << " failed";
      QString errors;
      for (const std::string& failMessage : failMessages) {
        errors.append(QString::fromStdString(failMessage));
        errors.append("\n\n");
      }

      auto messageBox = new QMessageBox(m_app->mainWidget());
      messageBox->setWindowTitle(QString("Measures Updated"));
      messageBox->setText(toQString(ss.str()));
      messageBox->setDetailedText(errors);
      //messageBox->setMinimumWidth(330);
      //messageBox->setSizeGripEnabled(true);

      // DLM: there is a bug in QMessageBox where setMinimumWidth is not used
      // http://www.qtcentre.org/threads/22298-QMessageBox-Controlling-the-width?p=113348#post113348
      auto horizontalSpacer = new QSpacerItem(330, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
      QGridLayout* layout = static_cast<QGridLayout*>(messageBox->layout());
      layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
      
      messageBox->exec();
      delete messageBox;
    }
    else {
      QMessageBox::information(m_app->mainWidget(), QString("Measures Updated"), toQString(ss.str()));
    }
  }
}

void MeasureManager::updatePatApplicationMeasures(analysisdriver::SimpleProject &t_project)
{
  updateMeasuresLists();

  std::vector<BCLMeasure> toUpdate;

  std::vector<BCLMeasure> measures = patApplicationMeasures();

  for (std::vector<BCLMeasure>::iterator itr = measures.begin();
      itr != measures.end();
      ++itr)
  {
    boost::optional<BCLMeasure> projectmeasure = t_project.getMeasureByUUID(itr->uuid());
    if (projectmeasure)
    {
      if (projectmeasure->versionUUID() != itr->versionUUID())
      {
        toUpdate.push_back(*itr);
      }
    }
  }

  updateMeasures(t_project, toUpdate, false);
}

void MeasureManager::updateBCLMeasures(analysisdriver::SimpleProject &t_project)
{
  updateMeasuresLists();

  std::vector<BCLMeasure> toUpdate;

  std::vector<BCLMeasure> measures = bclMeasures();

  for (auto & measure : measures)
  {
    if (m_patApplicationMeasures.find(measure.uuid()) != m_patApplicationMeasures.end()){
      // do not attempt to update built in measures with different version in bcl
      LOG(Warn, "Skipping update of built in measure");
      continue;
    }

    if (m_myMeasures.find(measure.uuid()) != m_myMeasures.end()){
      // do not attempt to update user measures with different version in bcl
      LOG(Warn, "Skipping update of user measure");
      continue;
    }

    boost::optional<BCLMeasure> projectmeasure = t_project.getMeasureByUUID(measure.uuid());
    if (projectmeasure)
    {
      if (projectmeasure->versionUUID() != measure.versionUUID())
      {
        toUpdate.push_back(measure);
      }
    }
  }

  updateMeasures(t_project, toUpdate);
}

bool MeasureManager::checkForUpdates(BCLMeasure& measure, bool force)
{
  // first check files for updates
  bool result = measure.checkForUpdatesFiles();
  if (result || force){
    // if files updated or being forced to, try to load the ruby measure
    try{
      ruleset::RubyUserScriptInfo info = m_infoGetter->getInfo(measure);
      info.update(measure);
    } catch(const std::exception& e) {
      // failed to get info, put error into the measure's xml
      ruleset::RubyUserScriptInfo info(e.what());
      info.update(measure);
    }
  }
  // last, check for xml updates
  result = (result || measure.checkForUpdatesXML());

  return result;
}

void MeasureManager::downloadBCLMeasures()
{
  auto remoteBCL = new RemoteBCL();
  int numUpdates = remoteBCL->checkForMeasureUpdates();
  if (numUpdates == 0){
    QMessageBox::information(m_app->mainWidget(), "Measures Updated", "All measures are up-to-date.");
  }else{
    std::vector<BCLSearchResult> updates = remoteBCL->measuresWithUpdates();
    QStringList measureNames;
    for (const BCLSearchResult& update : updates) {
      measureNames.push_back(QString(0x2022) + " " + toQString(update.name()));
    }
    remoteBCL->updateMeasures();
    QMessageBox msg(m_app->mainWidget());
    msg.setIcon(QMessageBox::Information);
    msg.setWindowTitle("Measures Updated");
    if (numUpdates == 1){
      msg.setText("1 measure has been updated.                   ");
    }else{
      msg.setText(QString::number(numUpdates) + " measures have been updated.                   ");
    }
    msg.setDetailedText(measureNames.join("\n"));
    msg.exec();
  }
  delete remoteBCL;
}

void MeasureManager::updateMyMeasures(analysisdriver::SimpleProject &t_project)
{
  updateMeasuresLists();

  std::vector<BCLMeasure> toUpdate;

  std::vector<BCLMeasure> measures = myMeasures();

  for (auto & measure : measures)
  {

    if (m_patApplicationMeasures.find(measure.uuid()) != m_patApplicationMeasures.end()){
      // do not attempt to update built in measures
      LOG(Warn, "Skipping update of built in measure");
      continue;
    }

    boost::optional<BCLMeasure> projectmeasure = t_project.getMeasureByUUID(measure.uuid());
    if (projectmeasure)
    {
      if (projectmeasure->versionUUID() != measure.versionUUID())
      {
        toUpdate.push_back(measure);
      }
    }
  }

  updateMeasures(t_project, toUpdate);
}

bool MeasureManager::isPatApplicationMeasure(const UUID & id) const
{
  if (m_patApplicationMeasures.find(id) != m_patApplicationMeasures.end()){
    return true;
  }
  return false;
}

std::vector<BCLMeasure> MeasureManager::patApplicationMeasures() const
{
  std::vector<BCLMeasure> result;

  for(const auto & measure : m_patApplicationMeasures)
  {
    result.push_back(measure.second);
  }

  return result;
}

std::vector<BCLMeasure> MeasureManager::bclMeasures() const
{
  std::vector<BCLMeasure> result;

  for(const auto & bclMeasure : m_bclMeasures)
  {
    result.push_back(bclMeasure.second);
  }

  return result;
}

std::vector<BCLMeasure> MeasureManager::myMeasures() const
{
  std::vector<BCLMeasure> result;

  for(const auto & measure : m_myMeasures )
  {
    result.push_back(measure.second);
  }

  return result;
}

void MeasureManager::updateMeasuresLists()
{
  openstudio::path userMeasuresDir = BCLMeasure::userMeasuresDir();

  auto updateUserMeasures = true;
  if (isNetworkPath(userMeasuresDir) && !isNetworkPathAvailable(userMeasuresDir)) {
    updateUserMeasures = false;
  }

  updateMeasuresLists(updateUserMeasures);
}

void MeasureManager::updateMeasuresLists(bool updateUserMeasures)
{
  m_patApplicationMeasures.clear();
  m_myMeasures.clear();
  m_bclMeasures.clear();
  
  std::vector<BCLMeasure> patApplicationMeasures = BCLMeasure::patApplicationMeasures();
  for( auto & measure : patApplicationMeasures )
  {
    // installed measures should not change
    // DLM: TODO!
    //OS_ASSERT(!checkForUpdates(measure));

    std::map<UUID,BCLMeasure>::iterator it = m_patApplicationMeasures.find(measure.uuid());
    if (it != m_patApplicationMeasures.end()){
      // duplicate measure detected, user copy and paste
      LOG(Error, "UUID of built in measure at '" << measure.directory() << "' conflicts with other built in measure, measure at '" << it->second.directory() << "' will be used instead");
    }else{
      m_patApplicationMeasures.insert(std::pair<UUID,BCLMeasure>(measure.uuid(),measure));
    }
  }

  if (updateUserMeasures) {
   std::vector<BCLMeasure> userMeasures = BCLMeasure::userMeasures();
    for( auto & measure : userMeasures )
    {
      if (checkForUpdates(measure)){
        measure.save();
      }

      bool updateUUID = false;
      if (m_myMeasures.find(measure.uuid()) != m_myMeasures.end()){
        updateUUID = true;
      }
      if (m_bclMeasures.find(measure.uuid()) != m_bclMeasures.end()){
        updateUUID = true;
      }
      if (m_patApplicationMeasures.find(measure.uuid()) != m_patApplicationMeasures.end()){
        updateUUID = true;
      }

      if (updateUUID){
        // duplicate measure detected, manual copy and paste likely cause
        // assign measure a new UUID here and save
        measure.changeUID();
        measure.incrementVersionId();
        measure.save();
      }

      m_myMeasures.insert(std::pair<UUID,BCLMeasure>(measure.uuid(),measure));
    }
  }
 
  std::vector<BCLMeasure> localBCLMeasures = BCLMeasure::localBCLMeasures();
  for( auto & measure : localBCLMeasures )
  {
    // downloaded measure might have been updated on version translation
    if (checkForUpdates(measure)){
      measure.save();
    }

    std::map<UUID,BCLMeasure>::iterator it = m_bclMeasures.find(measure.uuid());
    if (it != m_bclMeasures.end()){
      // duplicate measure detected
      LOG(Error, "UUID of bcl measure at '" << measure.directory() << "' conflicts with other bcl measure, measure at '" << it->second.directory() << "' will be used instead");
    }else{
      m_bclMeasures.insert(std::pair<UUID,BCLMeasure>(measure.uuid(),measure));
    }
  }

  if (m_libraryController)
  {
    m_libraryController->reset();
  }
}

void MeasureManager::addMeasure()
{
  // open modal dialog
  //QSharedPointer<BCLMeasureDialog> dialog(new BCLMeasureDialog(this->mainWindow));
  QSharedPointer<BCLMeasureDialog> dialog(new BCLMeasureDialog());

  if (dialog->exec()){

    // not canceled, create measure
    boost::optional<BCLMeasure> measure = dialog->createMeasure();
    if (measure){

      // open directory for editing
      QString path = QDir::toNativeSeparators(toQString(measure->directory()));
      QDesktopServices::openUrl(QUrl("file:///" + path));

      updateMeasuresLists();

      // emit signal
      emit newMeasure(*measure);
    }else{
      // error, this should not happen
    }

  }else{
    // user canceled, do nothing
  }
}

void MeasureManager::duplicateSelectedMeasure()
{
  OS_ASSERT(m_libraryController);
  QPointer<LibraryItem> item = m_libraryController->selectedItem();

  if( !item.isNull() )
  {
    boost::optional<BCLMeasure> bclMeasure = this->getMeasure(item->uuid());
    if (bclMeasure){

      // check for updates in case measure being copied has changed
      // do not save bclMeasure 
      checkForUpdates(*bclMeasure, true);

      // open modal dialog
      //QSharedPointer<BCLMeasureDialog> dialog(new BCLMeasureDialog(*bclMeasure, this->mainWindow));
      QSharedPointer<BCLMeasureDialog> dialog(new BCLMeasureDialog(*bclMeasure));

      if (dialog->exec()){

        // not canceled, create measure
        boost::optional<BCLMeasure> measure = dialog->createMeasure();
        if (measure){

          // open directory for editing
          QString path = QDir::toNativeSeparators(toQString(measure->directory()));
          QDesktopServices::openUrl(QUrl("file:///" + path));

          updateMeasuresLists();

          // emit signal
          emit newMeasure(*measure);
        }else{
          // error, this should not happen
        }

      }else{
        // user canceled, do nothing
      }
    }
  }
}

bool MeasureManager::isMeasureSelected()
{
  if (m_libraryController){
    QPointer<LibraryItem> item = m_libraryController->selectedItem();
    return !item.isNull();
  }
  return false;
}

QSharedPointer<ruleset::RubyUserScriptInfoGetter> MeasureManager::infoGetter() const
{
  return m_infoGetter;
}

std::vector<BCLMeasure> MeasureManager::combinedMeasures(bool includePatApplicationMeasures) const
{
  std::vector<BCLMeasure> result;
  std::set<UUID> resultUUIDs;
  
  if (includePatApplicationMeasures){
    // insert pat application measures
    for (std::map<UUID,BCLMeasure>::const_iterator it = m_patApplicationMeasures.begin(), itend = m_patApplicationMeasures.end(); it != itend; ++it){
      if (resultUUIDs.find(it->first) == resultUUIDs.end()){
        resultUUIDs.insert(it->first);
        result.push_back(it->second);
      }else{
        LOG(Error, "UUID of built in measure at '" << it->second.directory() << "' conflicts with other measure, other measure will be used instead");
      }
    }
  }

  // insert my measures
  for (std::map<UUID,BCLMeasure>::const_iterator it = m_myMeasures.begin(), itend = m_myMeasures.end(); it != itend; ++it){
    if (resultUUIDs.find(it->first) == resultUUIDs.end()){
      resultUUIDs.insert(it->first);
      result.push_back(it->second);
    }else{
      LOG(Error, "UUID of user measure at '" << it->second.directory() << "' conflicts with other measure, other measure will be used instead");
    }
  }

   // insert bcl measures
  for (std::map<UUID,BCLMeasure>::const_iterator it = m_bclMeasures.begin(), itend = m_bclMeasures.end(); it != itend; ++it){
    if (resultUUIDs.find(it->first) == resultUUIDs.end()){
      resultUUIDs.insert(it->first);
      result.push_back(it->second);
    }else{
      LOG(Error, "UUID of user measure at '" << it->second.directory() << "' conflicts with other measure, other measure will be used instead");
    }
  }

  return result;
}

boost::optional<BCLMeasure> MeasureManager::getMeasure(const UUID & id)
{
  boost::optional<BCLMeasure> result;

  std::map<UUID,BCLMeasure>::iterator it;
  
  // search pat application measures
  it = m_patApplicationMeasures.find(id);
  if( it != m_patApplicationMeasures.end() ) {
    if (result){
      LOG(Error, "UUID of built in measure at '" << it->second.directory() << "' conflicts with other measure, measure at '" << result->directory() << "' will be used instead");
    }else{
      result = it->second; 
    }
  }

  // search my measures
  it = m_myMeasures.find(id);
  if( it != m_myMeasures.end() ) { 
    if (result){
      LOG(Error, "UUID of user measure at '" << it->second.directory() << "' conflicts with other measure, measure at '" << result->directory() << "' will be used instead");
    }else{
      result = it->second; 
    }
  }

  // search bcl measures
  it = m_bclMeasures.find(id);
  if( it != m_bclMeasures.end() ) {     
    if (result){
      LOG(Error, "UUID of bcl measure at '" << it->second.directory() << "' conflicts with other measure, measure at '" << result->directory() << "' will be used instead");
    }else{
      result = it->second; 
    }
  }

  return result;
}

void MeasureManager::setLibraryController(const QSharedPointer<LocalLibraryController> &t_controller)
{
  m_libraryController = t_controller;
}

} // openstudio


