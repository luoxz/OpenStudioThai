#ifndef COMPAREDIALOG_H
#define COMPAREDIALOG_H

#include <QDialog>
#include <memory>
#include <QtWebKit/QWebElement>

namespace Ui {
class CompareDialog;
}

class QWebElementCollection;
class QWebView;

class TableCompare
{
    QVector <QStringList> datas;
    int _row, _col;
public:
    TableCompare();

    size_t rowCount(){return _row;}
    size_t colCount(){return _col;}
    void resize(size_t row, size_t col);
    void pushValue(size_t row, size_t col, QString value);
    QStringList& at(size_t row, size_t col);
};

class TableElement
{
public:
    TableElement(QString& header, QWebElement elm);
    void AddTable(QWebElement elm, const QString& header);
    QString toInsideTBODY();
    void updateTableElement();
    QString getHeader(){return header;}
    bool isUse(){ return _isUse; }
    void setUse(bool used){ _isUse = used;}
    void reset(){_isUse = false;}
private:
    QString header;
    bool _isUse;
    QWebElement elm;
    TableCompare table;
    int rowCount, columnCount;
    void doTable(QWebElement elm, const QString& projectName);
};


//TODO:MAKE ENEGY PLUS DOC TO INTERFACE DOC
// ::nextTable || nextMergElement
// ::find return QSharedPointer<IMergElement>
class EnegyPlusDoc
{
public:
    QList<QSharedPointer<TableElement > > tables;
    QSharedPointer<TableElement > find(const QString& key);
    void resetTables();
};

class CompareDialog : public QDialog
{
    Q_OBJECT

public:
    enum CMPTYPE { CMPTYPE_UNKNOW, CMPTYPE_OPENSTUDIO, CMPTYPE_ENYGYPLUS, CMPTYPE_BEC };
    enum FINDTARGETRES { FINDTARGETRES_NONE=0, FINDTARGETRES_DUP, FINDTARGETRES_CANTFIND};
    explicit CompareDialog(QWidget *parent = 0);
    bool SetParam(int argc, char *argv[]);
    bool SetParam(const QString &file1, const QString &file2, const QString &type);
    ~CompareDialog();
    QString loadHtml(const QString &path);
    QWebElement getBody(QWebView *webView);
    QString scanFolderPathWithRegex(const QString& path, const QRegExp& reg, int recursive);
    void SetCmpType(CompareDialog::CMPTYPE type);
    bool isDuplicatePath(QString filePath);
    void doCmp(QString filePath);
    QString findTargetPath(const QString& filePath, CompareDialog::FINDTARGETRES &res);
    void msgRes(FINDTARGETRES res, QString filePath);
    QString loadFileToWebView(const QString& fn, QWebView *webView);
    void LoadCompareFile(const QString &filePath);

    CMPTYPE getDocTypeFromTitle(const QString& title);

private slots:
    void on_chkShowTree_clicked(bool checked);
    void on_btAddCmpPath_clicked();
    void eventDropFilePath(const QString& filePath);
    void on_webView_loadFinished(bool arg1);
    void on_webView2_loadFinished(bool arg1);

private:
    Ui::CompareDialog *ui;
    QString seplustbl0, seplustbl1, sost0, sost1;
    QString file1, file2, reportName2, type;
    QStringList fileCmps;
    CMPTYPE cmpType;
    bool mergingTable(QWebElement& table, const QWebElement& table2, const QString &title);
    bool nextTable(QString &title, QWebElement &elm);
    void doRowDic(const QWebElementCollection *tr, QHash<QString, QWebElement> &dic);
    void makeEnegyPlusCmp();
    void makeOpenStudioPlusCmp();
    void makeBecPlusCmp();
    EnegyPlusDoc *createEnegyPlusDoc();
    EnegyPlusDoc* enegyPlusDoc;
    QString getReportName(const QString& filePath);

};

#endif // COMPAREDIALOG_H
