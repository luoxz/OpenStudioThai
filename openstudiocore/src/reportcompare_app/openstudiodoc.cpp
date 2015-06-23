#include <QDebug>
#include <QCoreApplication>
#include <QMessageBox>
#include <QProgressDialog>

#include "openstudiodoc.h"
#include "opetableelement.h"
#include "htmltutil.h"
#include <QWebView>
#include <QWebFrame>

OpenStudioDoc::OpenStudioDoc(const QString &projectName, QWebView *webView)
    :IDoc(projectName, webView)
{
    QWebElement body = getBody(webView);
    QWebElement elmit = body.firstChild();

    QString js = "var table = document.getElementsByTagName(\"table\");";
    js += "var i;";
    js += "for (i = 0; i < table.length; i++) {";
    js += "    table[i].style.maxWidth = \"10000px\";";
    js += "}";

    webView->page()->mainFrame()->evaluateJavaScript(js);

    //body.setStyleProperty("width", "5000px");
    //QWebFrame *frame = webView->page()->mainFrame();
    //QWebElement doc = frame->documentElement();

    QString title;
    bool isnext1 = nextTableWithTitle(title, elmit);
    while(isnext1){
        qDebug()  << "Create At title : " << title;
        QSharedPointer<OPETableElement> telm = QSharedPointer<OPETableElement>
                (new OPETableElement(this, title, elmit));
        elements.append(telm);
        elmit = elmit.nextSibling();
        isnext1 = nextTableWithTitle(title, elmit);
        //TODO:REMOVE IF IS STABLE.
        //break;
    }
}

OpenStudioDoc::~OpenStudioDoc()
{

}

void OpenStudioDoc::doCmp(const QString &projectName, QWebView *webView2)
{
    QString error;
    resetTables();

    QSharedPointer<OpenStudioDoc> other = QSharedPointer<OpenStudioDoc>(new OpenStudioDoc(projectName, webView2));
    QProgressDialog progress("Doing compare Open Studio.", "Abort", 0, (int)other->elementCount(), NULL);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    QCoreApplication::processEvents();

    int i=0;
    QList<QSharedPointer<ICompareElement> >::iterator eit = other->getElements().begin();
    while(eit!=other->getElements().end()){
        ICompareElement* ce = (*eit).data();
        qDebug()  << "Create At title : " << ce->Id();
        ICompareElement* tbelm = find(ce->Id());
        if(tbelm){
            if(tbelm->Compare(ce)){
                tbelm->Update();
                tbelm->setUse(true);
            }
            else{
                error += tbelm->getError()+"\n";
                tbelm->clearError();
            }
        }
        progress.setValue(i++);
        eit++;
        QCoreApplication::processEvents();
    }
    progress.setValue((int)other->elementCount());
    QCoreApplication::processEvents();
    if(!error.isEmpty()){
        QMessageBox msgBox;
        msgBox.setText(error);
        msgBox.exec();
    }
}

