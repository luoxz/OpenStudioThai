#ifndef OPENSTUDIODOC_H
#define OPENSTUDIODOC_H

#include "idoc.h"

class OpenStudioDoc : public IDoc
{
public:
    OpenStudioDoc(const QString &projectName, QWebView *webView);
    ~OpenStudioDoc();

    // IDoc interface
public:
    void doCmp(const QString &projectName, QWebView *webView2);
};

#endif // OPENSTUDIODOC_H
