#include "comparedialog.h"
#include "ui_comparedialog.h"

#include <QtWebKitWidgets/QWebFrame>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>
#include "genbecreport.h"

CompareDialog::CompareDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CompareDialog)
    , enegyPlusDoc(NULL)
{
    ui->setupUi(this);
    QString err;
    cmpType = CMPTYPE_UNKNOW;

    QObject::connect(ui->btAddCmpPath
                     , SIGNAL(eventDropFilePath(const QString &))
                     , this
                     , SLOT(eventDropFilePath(const QString &)));

    ui->webView->setAcceptDrops(false);
    Qt::WindowFlags flags = 0;
    flags |= Qt::WindowMinMaxButtonsHint|Qt::WindowCloseButtonHint;
    setWindowFlags( flags );
}

void CompareDialog::SetParam(int argc, char *argv[])
{
    if(argc>=4){
        SetParam(argv[1], argv[2], argv[3]);
    }
    else {
        QMessageBox::critical(this, "Enexpected to launch ReportCompare", "ReportCompare was unexpected parameter size.\n");
    }
}

CompareDialog::~CompareDialog()
{
    if(enegyPlusDoc!=NULL)
        delete enegyPlusDoc;
    delete ui;
    qDebug() << "CompareDialog::~CompareDialog()";
}

QString CompareDialog::loadFileToWebView(const QString& fn, QWebView* webView)
{
    FINDTARGETRES res;
    qDebug() << "File : " << fn;
    QString target = findTargetPath(fn, res);
    qDebug() << "Target : " << target;
    if(!target.isEmpty()){
        webView->load("file:///"+target);
        fileCmps.append(target);
    }
    msgRes(res, fn);
    return target;
}

void CompareDialog::SetParam(const QString& file1, const QString &file2, const QString& type)
{
    this->type = type;
    this->file1 = file1;
    this->file2 = file2;

    QString modeStr;
    if(type.toLower() == "enegyplus" || type.toLower() == "e"){
        SetCmpType(CMPTYPE_ENYGYPLUS);
        modeStr = "Enegy plus";
    }
    else if(type.toLower() == "bec" || type == "b"){
        SetCmpType(CMPTYPE_BEC);
        modeStr = "BEC";
    }
    else if(type.toLower() == "openstudio" || type.toLower() == "o"){
        SetCmpType(CMPTYPE_OPENSTUDIO);
        modeStr = "Open studio";
    }
    else{
        SetCmpType(CMPTYPE_UNKNOW);
        modeStr = "??????";
        QMessageBox::critical(this, "Enexpected to launch ReportCompare", "ReportCompare was not support argument is "+type);
    }

    ui->lbInfo->setText(QString("Compare report source file is %1 in %2 mode")
                        .arg(file1)
                        .arg(modeStr));

    this->file1 = loadFileToWebView(file1, ui->webView);
    ui->chkShowTree->setVisible(false);
    ui->chkShowTree->setChecked(false);
    on_chkShowTree_clicked(false);
}

QString CompareDialog::loadHtml(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Error: Cannot read file " << qPrintable(path)
                  << ": " << qPrintable(file.errorString());

        return "";
    }
    return file.readAll();
}

QWebElement CompareDialog::getBody(QWebView* webView)
{
    QWebFrame *frame = webView->page()->mainFrame();
    QWebElement doc = frame->documentElement();
    QWebElement body = doc.findFirst("body");
    return body;
}

bool CompareDialog::mergingTable(QWebElement &table, const QWebElement &table2, const QString& title)
{
    QWebElementCollection tr1 = table.findAll("TR");
    QWebElementCollection tr2 = table2.findAll("TR");

    QWebElement r1it = tr1.first();
    QWebElement r2it = tr2.first();
    while (!r1it.isNull()) {
        //qDebug() << "||| " << xit.toOuterXml();
        QWebElement td1it = r1it.firstChild();
        QWebElement td2it = r2it.firstChild();
        qDebug() << "############################";
        QString tdResult;
        bool isFirstCol = true;
        while (!td1it.isNull()) {
            //qDebug() << "...." << td1it.toOuterXml();
            tdResult += td1it.toOuterXml();
            if(!isFirstCol){
                for(int c=0;c<fileCmps.size();c++)
                    td1it = td1it.nextSibling();
                tdResult += td2it.toOuterXml();
                qDebug() << "XXX:" <<td2it.toOuterXml();
            }
            else{
                td1it = td1it.nextSibling();
            }
            td2it = td2it.nextSibling();
            isFirstCol = false;
        }
        qDebug() << "<<<< " << "<tr>"+tdResult+"</tr>";
        QWebElement r1tmp = r1it;
        r1it = r1it.nextSibling();
        r2it = r2it.nextSibling();
        r1tmp.setOuterXml(tdResult);
        qDebug() << "!!!!!!!!!!" << r1it.toOuterXml();
    }
    return true;
}

bool CompareDialog::nextTable(QString &title, QWebElement &elm)
{
    QString lastTitle;
    bool isnext=false;
    while (!elm.isNull()) {
        if(elm.tagName() == "B"){
            lastTitle = elm.toInnerXml();
        }else if(elm.tagName() == "TABLE"){
            isnext = true;
            break;
        }
        elm = elm.nextSibling();
    }
    title = lastTitle;
    return isnext;
}

void CompareDialog::doRowDic(const QWebElementCollection *tr, QHash<QString, QWebElement>& dic)
{
    QWebElementCollection::const_iterator it = tr->begin();
    while (it != tr->end()) {
        QWebElement td = (*it).findFirst("TD");
        dic[td.toInnerXml()] = *it;
        it++;
    }
}

void CompareDialog::makeEnegyPlusCmp()
{
    QWebElement body = getBody(ui->webView2);
    QWebElement elmit = body.firstChild();
    enegyPlusDoc->resetTables();

    QProgressDialog progress("Doing compare Enegy plus.", "Abort", 0, enegyPlusDoc->tables.count(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    QCoreApplication::processEvents();

    QString title;
    bool isnext = nextTable(title, elmit);
    int i=0;
    while(isnext){
        qDebug()  << "Create At title : " << title;
        QSharedPointer<TableElement> tbelm = enegyPlusDoc->find(title);
        if(!tbelm.isNull()){
            tbelm->AddTable(elmit, reportName2);
            tbelm->updateTableElement();
            tbelm->setUse(true);
        }
        elmit = elmit.nextSibling();
        isnext = nextTable(title, elmit);
        progress.setValue(i++);
        QCoreApplication::processEvents();
    }
    progress.setValue(enegyPlusDoc->tables.count());
    QCoreApplication::processEvents();
}

void CompareDialog::makeOpenStudioPlusCmp()
{

}

void CompareDialog::makeBecPlusCmp()
{
    QWebElement body = getBody(ui->webView2);
    QWebElement elmit = body.firstChild();
    enegyPlusDoc->resetTables();

    QProgressDialog progress("Doing compare BEC plus.", "Abort", 0, enegyPlusDoc->tables.count(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    QCoreApplication::processEvents();

    QString title;
    bool isnext = nextTable(title, elmit);
    int i=0;
    while(isnext){
        qDebug()  << "Create At title : " << title;
        QSharedPointer<TableElement> tbelm = enegyPlusDoc->find(title);
        if(!tbelm.isNull()){
            tbelm->AddTable(elmit, reportName2);
            tbelm->updateTableElement();
            tbelm->setUse(true);
        }
        elmit = elmit.nextSibling();
        isnext = nextTable(title, elmit);
        progress.setValue(i++);
        QCoreApplication::processEvents();
    }
    progress.setValue(enegyPlusDoc->tables.count());
    QCoreApplication::processEvents();
}

EnegyPlusDoc *CompareDialog::createEnegyPlusDoc()
{
    if(enegyPlusDoc!=NULL)
        delete enegyPlusDoc;

    enegyPlusDoc = new EnegyPlusDoc();
    QWebElement body = getBody(ui->webView);
    QWebElement elmit = body.firstChild();
    QString title;
    bool isnext1 = nextTable(title, elmit);
    while(isnext1){
        qDebug()  << "Create At title : " << title;
        QSharedPointer<TableElement> telm = QSharedPointer<TableElement>(new TableElement(title, elmit));
        enegyPlusDoc->tables.append(telm);
        elmit = elmit.nextSibling();
        isnext1 = nextTable(title, elmit);
    }
    return enegyPlusDoc;
}

QString CompareDialog::getReportName(const QString &filePath)
{
    QString fp = filePath;
    fp.replace("\\", "/");
    QStringList fpls = fp.split("/");
    qDebug() << "fpls.count = " << fpls.count();
    if(fpls.count()>=4){
        return fpls[fpls.count()-4];
    }
    return "????";
}

void CompareDialog::on_chkShowTree_clicked(bool checked)
{
    ui->wCenter->setVisible(checked);
    ui->wRight->setVisible(checked);
}

QString CompareDialog::scanFolderPathWithRegex(const QString& path, const QRegExp& reg, int recursive)
{
    recursive--;
    if(recursive < 0)
        return QString();

    QFileInfo rootFileInfo(path);
    if(rootFileInfo.isDir()){
        QDir dir(path);
        dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
        QStringList dirList = dir.entryList();
        for (int i=0; i<dirList.size(); ++i)
        {
            QString newPath = QString("%1/%2").arg(dir.absolutePath()).arg(dirList.at(i));
            bool match = newPath.contains(reg);
            QFileInfo pathFileInfo(newPath);
            if(!match)
            {
                if(pathFileInfo.isDir()){
                    QString matchPath = scanFolderPathWithRegex(newPath, reg, recursive);
                    if(!matchPath.isEmpty()){
                        return matchPath;
                    }
                }
            }
            else{
                return newPath;
            }
        }
    }
    else{
        bool match = path.contains(reg);
        if(match)
            return path;
    }

    return QString();
}

void CompareDialog::SetCmpType(CompareDialog::CMPTYPE type)
{
    cmpType = type;
}

bool CompareDialog::isDuplicatePath(QString filePath)
{
    QStringList::const_iterator cit = fileCmps.begin();
    while (cit!=fileCmps.end()) {
       QString cf = (*cit);
       if(cf == filePath){
           return true;
       }
       cit++;
    }
    return false;
}

void CompareDialog::msgRes(FINDTARGETRES res, QString filePath)
{
    if(res == FINDTARGETRES_CANTFIND)
    {
        QMessageBox msgBox;
        msgBox.setText(QString("Can't find result at path %1").arg(filePath));
        msgBox.exec();
    }
    else if(res == FINDTARGETRES_DUP)
    {
        QMessageBox msgBox;
        msgBox.setText("Select path is duplicate.");
        msgBox.exec();
    }
}

void CompareDialog::doCmp(QString filePath)
{
    if(filePath.startsWith("file:///")){
        filePath.remove(0, 8);
    }
    qDebug() << "Do compare at path:" << filePath;
    FINDTARGETRES res;
    QString targetPath =  findTargetPath(filePath, res);
    if(!targetPath.isEmpty()){
        LoadCompareFile(targetPath);
    }
    msgRes(res, filePath);
}

QString CompareDialog::findTargetPath(const QString &filePath, CompareDialog::FINDTARGETRES& res)
{
    static QRegExp becRegEx("(run[\\\\/]\\d[-]BEC[-]\\d[\\\\/]report\\.html)$");
    static QRegExp energyPlusRegEx("(run[\\\\/]\\d[-]EnergyPlus[-]\\d[\\\\/]eplustbl\\.htm)$");
    static QRegExp openStudioRegEx("(run[\\\\/]\\d[-]UserScript[-]\\d[\\\\/]report\\.html)$");

    res = FINDTARGETRES_NONE;
    bool dup = false;
    if(!filePath.isEmpty()){
        QString matchPath;
        switch (cmpType) {
        case CMPTYPE_BEC:
            matchPath = scanFolderPathWithRegex(filePath
                                                , becRegEx, 3);
            break;
        case CMPTYPE_OPENSTUDIO:
            matchPath = scanFolderPathWithRegex(filePath
                                                , openStudioRegEx, 3);
            break;
        default:
            matchPath = scanFolderPathWithRegex(filePath
                                                , energyPlusRegEx, 3);
            break;
        }
        dup = isDuplicatePath(matchPath);
        if(dup){
            res = FINDTARGETRES_DUP;
            return QString();
        }
        else{
            return matchPath;
        }
    }
    res = FINDTARGETRES_CANTFIND;
    return QString();
}

void CompareDialog::on_btAddCmpPath_clicked()
{
    QString filePath = QFileDialog::getExistingDirectory(this,
        tr("Open report folder"));

    if(!filePath.isEmpty())
        doCmp(filePath);
}

void CompareDialog::eventDropFilePath(const QString &filePath)
{
    doCmp(filePath);
}

void CompareDialog::LoadCompareFile(const QString& filePath)
{
    qDebug() << "filePath=" << filePath;
    file2 = filePath;
    file2 = loadFileToWebView(file2, ui->webView2);
    reportName2 = getReportName(file2);
    qDebug() << "file2=" << file2;
    qDebug() << "reportName2=" << reportName2;
}

void CompareDialog::on_webView_loadFinished(bool arg1)
{
    if(cmpType == CMPTYPE_ENYGYPLUS){
        enegyPlusDoc = createEnegyPlusDoc();
        LoadCompareFile(this->file2);
    }
    else if(cmpType == CMPTYPE_BEC){
        enegyPlusDoc = createEnegyPlusDoc();
        LoadCompareFile(this->file2);
    }
    else if(cmpType == CMPTYPE_OPENSTUDIO){

    }
    else{//CMPTYPE_UNKNOW

    }
}

void CompareDialog::on_webView2_loadFinished(bool arg1)
{
    fileCmps.append(this->file2);
    if(cmpType == CMPTYPE_ENYGYPLUS){
        makeEnegyPlusCmp();
    }
    else if(cmpType == CMPTYPE_BEC){
        makeBecPlusCmp();
    }
    else if(cmpType == CMPTYPE_OPENSTUDIO){
        makeOpenStudioPlusCmp();
    }
    else{//CMPTYPE_UNKNOW
        //makeEnegyPlusCmp(targetPath);
    }
}

TableElement::TableElement(QString &header, QWebElement elm){
    this->header = header;
    _isUse = false;
    rowCount = -1;
    columnCount = -1;
    this->elm = elm;
    doTable(this->elm, QString());
}

void TableElement::AddTable(QWebElement elm, const QString &header){
    qDebug() << "AddTable : " << header;
    doTable(elm, header);
}

bool peakFloat(const QString& str, float& val){
    bool isFloat;
    val = str.toFloat(&isFloat);
    return isFloat;
}

QString doHtmlTextColor(const QString& str, const QColor& color){
    return QString("<font color=\"%1\">%2</font>")
            .arg(color.name()).arg(str);
}

QString TableElement::toInsideTBODY(){
    QString tbody;
    for(size_t r=0;r<table.rowCount();r++){
        tbody += "<tr>\n";

        bool isFirstFloat = false;
        float firstVal;

        for(size_t c=0;c<table.colCount();c++){

            QString td = "<td align=\"center\">";
            if ( c % 2== 0)
                td = "<td  align=\"center\" bgcolor=\"#F7F7F7\">";

            QStringList& sls = table.at(r,c);
            QStringList::iterator slsit = sls.begin();
            isFirstFloat = peakFloat((*slsit), firstVal);
            while (slsit!=sls.end()) {
                if(c==0){
                    tbody += (td +(*slsit)+ "</td>\n");
                    break;
                }else{
                    float secondVal;
                    QString sstr = (*slsit);
                    if(peakFloat((*slsit), secondVal) && isFirstFloat){
                        if(firstVal>secondVal){
                            tbody += (td +doHtmlTextColor(sstr, "#008A2E")+ "</td>\n");
                        }
                        else if(firstVal<secondVal){
                            tbody += (td +doHtmlTextColor(sstr, Qt::red)+ "</td>\n");
                        }
                        else{
                            tbody += (td +sstr+ "</td>\n");
                        }
                    }
                    else{
                        tbody += (td +sstr+ "</td>\n");
                    }
                }
                slsit++;
            }
        }
        tbody += "</tr>\n";
    }
    return tbody;
}

void TableElement::updateTableElement()
{
    //qDebug() << "#################\n" << toInsideTBODY();
    elm.setInnerXml(toInsideTBODY());
}

void TableElement::doTable(QWebElement elm, const QString &header){
    bool isFirst = true;
    QWebElementCollection trs = elm.findAll("TR");
    for(int r=0;r<trs.count();r++){
        QWebElementCollection tds = trs[r].findAll("TD");

        if(isFirst){
            if(rowCount < 0 || columnCount < 0){
                rowCount = trs.count();
                columnCount = tds.count();
                table.resize(trs.count(), tds.count());
            }

            qDebug() << "#" << header << rowCount << ":" << trs.count();
            qDebug() << "#" << header << columnCount << ":" << tds.count();

            if(rowCount != trs.count()||columnCount != tds.count()){
                QMessageBox msgBox;
                QString msg = QString("The table label %1 is mismatch.\n\
                                      Source row is %2 and compare row is %3.\n\
                                      Source column is %4 and compare column is %5."
                                      ).arg(header)
                                       .arg(rowCount).arg(trs.count())
                                       .arg(columnCount).arg(tds.count());
                msgBox.setText(msg);
                msgBox.exec();
            }
            //Q_ASSERT(rowCount == trs.count());
            //Q_ASSERT(columnCount == tds.count());
            isFirst = false;
        }

        for(int c=0;c<tds.count();c++){
            if(r==0&&c!=0&&(!header.isEmpty())){
                table.pushValue(r,c, tds[c].toPlainText()+QString("<br>(%1)").arg(header));
            }
            else{
                table.pushValue(r,c, tds[c].toPlainText());
            }
        }
    }
}

TableCompare::TableCompare() {
}

void TableCompare::resize(size_t row, size_t col) {
    _row = row;
    _col = col;
    qDebug() << "001 data.size = " << datas.size() << ", row*col=" << row*col;
    datas.resize(row*col);
    qDebug() << "002 data.size = " << datas.size();
}

void TableCompare::pushValue(size_t row, size_t col, QString value){
    int idx = _col*row+col;
    //qDebug() << "IDX:" << idx << ", value:" << value << ", data.size = " << datas.count();
    datas[idx].append(value);
}

QStringList &TableCompare::at(size_t row, size_t col){
    return datas[_col*row+col];
}

QSharedPointer<TableElement> EnegyPlusDoc::find(const QString &key)
{
    QList<QSharedPointer<TableElement > >::iterator it = tables.begin();
    while (it!=tables.end()) {
        if(!(*it)->isUse() && (*it)->getHeader()==key){
            return (*it);
        }
        it++;
    }
    QSharedPointer<TableElement > spNULL;;
    return spNULL;
}

void EnegyPlusDoc::resetTables()
{
    QList<QSharedPointer<TableElement > >::iterator it = tables.begin();
    while (it!=tables.end()) {
        (*it)->reset();
        it++;
    }
}

