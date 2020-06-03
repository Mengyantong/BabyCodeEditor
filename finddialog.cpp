#include "finddialog.h"
#include <QtWidgets>
FindDialog::FindDialog(QWidget *parent):QDialog(parent)
{
     label = new QLabel(tr("Find &what")); //tr()函数是把他们翻译成其他语言的标记
     lineEdit = new QLineEdit;
     label->setBuddy(lineEdit);  //设置了行编辑器作为标签的伙伴（buddy）
                                 //所谓“buddy”是指一个窗口部件，它可以在按下标签的快捷键时接受焦点（focus）
     caseCheckBox = new QCheckBox(tr("Match &case"));
     backwardCheckBox = new QCheckBox(tr("Search &backward"));

     labelr = new QLabel(tr("replace &what"));//替换按钮 文本框
     lineEditr = new QLineEdit;
     labelr->setBuddy(lineEditr);

     findButton = new QPushButton(tr("&Find"));
     findButton->setDefault(true);  //setDefault（）让Find按钮成为对话框的默认按钮（default button）。
                                    //default button 就是指当用户按下Enter健时能够按下对应的按钮
     findButton->setEnabled(false); //默认禁止，显示灰色

     replaceButton = new QPushButton(tr("&replace"));
     replaceButton->setEnabled(false);

     closeButton = new QPushButton(tr("Close"));
     //让两个按钮变成可点击状态
     connect(lineEdit,SIGNAL(textChanged(const QString &)),this,SLOT(enableFindButton(const QString &)));
     connect(lineEditr,SIGNAL(textChanged(const QString &)),this,SLOT(enableReplaceButton(const QString &)));

     connect(findButton,SIGNAL(clicked()),this,SLOT(findClicked()));
     connect(replaceButton,SIGNAL(clicked()),this,SLOT(replaceClicked()));//tihuan
     connect(closeButton,SIGNAL(clicked()),this,SLOT(close()));

     QHBoxLayout *topLeftLayout = new QHBoxLayout;
     topLeftLayout->addWidget(label);
     topLeftLayout->addWidget(lineEdit);
     topLeftLayout->addWidget(labelr);//xin
     topLeftLayout->addWidget(lineEditr);
     QVBoxLayout *leftLayout = new QVBoxLayout;
     leftLayout->addLayout(topLeftLayout);
     leftLayout->addWidget(caseCheckBox);
     leftLayout->addWidget(backwardCheckBox);
     QVBoxLayout *rightLayout = new QVBoxLayout;
     rightLayout->addWidget(findButton);
     rightLayout->addWidget(replaceButton);//xin
     rightLayout->addWidget(closeButton);
     rightLayout->addStretch();  //添加分隔符（Stretch）

     QHBoxLayout *mainLayout = new QHBoxLayout;
     mainLayout->addLayout(leftLayout);
     mainLayout->addLayout(rightLayout);
     this->setLayout(mainLayout);

     setWindowTitle(tr("Find"));
     setFixedHeight(sizeHint().height()); //让窗口具有一个固定的高度，对话框

 };

void FindDialog::findClicked()
{
     QString text = lineEdit->text();  //取编辑框中的文本保存在一个String中
     Qt::CaseSensitivity cs = caseCheckBox->isChecked()?Qt::CaseSensitive:Qt::CaseInsensitive;//Qt::CaseSensitivity 是一个枚举类型
     if(backwardCheckBox->isChecked())
         emit findPrevious(text,cs);
     else
         emit findNext(text,cs);
 }

 void FindDialog::enableFindButton(const QString &text) {
    findButton->setEnabled(!text.isEmpty());
}

void FindDialog::replaceClicked()
{

     QString text = lineEditr->text();  //取编辑框中的文本保存在一个String中
     Qt::CaseSensitivity cs = caseCheckBox->isChecked()?Qt::CaseSensitive:Qt::CaseInsensitive;
         emit emitReplaceSignal(text,cs);
 }

  void FindDialog::enableReplaceButton(const QString &text) {
    replaceButton->setEnabled(!text.isEmpty());
}

