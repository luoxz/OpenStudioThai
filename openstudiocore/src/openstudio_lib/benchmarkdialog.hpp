#ifndef BENCHMARKDIALOG_H
#define BENCHMARKDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QSettings>

class QHBoxLayout;
class QLineEdit;
class QLineEdit;
class QVBoxLayout;
class QDoubleSpinBox;

class BenchmarkValue : public QWidget
{
    Q_OBJECT
public:
    explicit BenchmarkValue(const QString& key, const QString &name, double value, QWidget *parent = 0);
    ~BenchmarkValue();
    void setName(const QString& name);
    void setValue(double value);
    void setEditable(bool isEdit);
    QString toString();
    QString name();
    QString key() const;
    double value() const;
signals:

public slots:

private:
    QString _key;
    QHBoxLayout *hl;
    QLineEdit   *lnName;
    QDoubleSpinBox   *dValue;
};

class BenchmarkDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BenchmarkDialog(const QString &defaultConfigPath, QWidget *parent = 0);
    ~BenchmarkDialog();
    void setEditable(bool isEdit);
    void addBenchmark(const QString &key, const QString& name, double value);
    QString hashOfUsernamePassword(const QString& username, const QString& password);
    bool isCorrectPass(const QString& pass);
    void setNewPassword(const QString& pass);
    void setupBenchmarkValues(QSettings &settingINI, QSettings *settingsReg);
    size_t valuesCount() const;
    BenchmarkValue* valueAt(size_t idx) const;
    double getValueByName(const QString &name);
signals:

public slots:
    void eventExport();
    void eventImport();
    void eventEdit();
    void eventAccept();
    void eventClose();
    void eventChangePassword();
private:
    QList<BenchmarkValue*> values;
    QPushButton *btExport;
    QPushButton *btImport;
    QPushButton *btChangPassword;
    QPushButton *btEdit;
    QPushButton *btAccept;
    QPushButton *btClose;
    QVBoxLayout *vlayout;
};

#endif // BENCHMARKDIALOG_H
