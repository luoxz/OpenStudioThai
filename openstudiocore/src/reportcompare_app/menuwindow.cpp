#include "menuwindow.h"
#include "ui_menuwindow.h"
#include "comparedialog.h"

#include <QMessageBox>
#include "genbecreport.h"
#include <QDebug>

MenuWindow::MenuWindow(QWidget *parent)
    :QMainWindow(parent)
    , ui(new Ui::MenuWindow)
{
    ui->setupUi(this);
}

void MenuWindow::ShowCompare(const QString& file1, const QString &file2, const QString& type)
{
    if(!file1.isEmpty() && !file2.isEmpty() && !type.isEmpty()){
        CompareDialog* dlg = new CompareDialog();
        dlg->SetParam(file1, file2, type);
    }
}

MenuWindow::~MenuWindow()
{
    delete ui;
}

void MenuWindow::on_btGenBEC_clicked()
{
    QString err;
    if(!doBecReport("C:/Users/Gorn/Desktop/BEC XML Sample Output/bec.xml", err))
        qDebug() << err;
}

void MenuWindow::on_btTest_clicked()
{
    testInsertSpaceInTag();
}
