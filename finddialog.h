#ifndef FINDDIALOG_H
#define FINDDIALOG_H
#include <QDialog>
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class FindDialog : public QDialog
{
    Q_OBJECT

public:
    FindDialog(QWidget *parent = 0);//构造函数
signals:
     void findNext(const QString &str,Qt::CaseSensitivity cs);
     void findPrevious(const QString &str,Qt::CaseSensitivity cs);
     void emitReplaceSignal(const QString &str,Qt::CaseSensitivity cs);
private slots:
     void findClicked();
     void replaceClicked();
     void enableFindButton(const QString &text);
     void enableReplaceButton(const QString &text);

private:
     QLabel *label;
     QLabel *labelr;
     QLineEdit *lineEdit;
     QLineEdit *lineEditr;
     QCheckBox *caseCheckBox;
     QCheckBox *backwardCheckBox;
     QPushButton *findButton;
     QPushButton *replaceButton;
     QPushButton *closeButton;
};

#endif // FINDDIALOG_H

