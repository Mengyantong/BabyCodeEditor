#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qmenubar.h>//菜单栏
#include <qmenu.h>
#include <QSplitter>
#include <QHBoxLayout>
#include <qaction.h>
#include <qaction.h>
#include <qstring.h>
#include <qtextedit.h>
#include <qplaintextedit.h>
#include <Qsci/qsciscintilla.h>
#include "Qsci/qscilexercpp.h"
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexer.h>
#include <Qsci/qsciapis.h>
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPoint>
#include <QSettings>
#include <QSize>
#include <QStatusBar>
#include <QTextStream>
#include <QToolBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDockWidget>
#include <QtWidgets/QMainWindow>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QTextEdit>
#include<QListWidget>
#include <QListWidgetItem>
#include <QDir>
#include <QProcess>
#include <QDebug>
#include <QtWidgets>
#include <QDialog>
#include "finddialog.h"
#include <windows.h>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class QAction;
class QMenu;
class QsciScintilla;
class QDockWidget;
class QTreeWidget;
class QWidget;
class FindDialog;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void CreatUi();
    QDockWidget *dock = new QDockWidget(tr("Programs"),this); //创建一个QDockWiget
     QTreeWidget *treeWidget = new QTreeWidget();           //创建一个QTreeWidget
    void setLexer(const QString &);//设置不同语言的词法分析器
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void documentWasModified();
    void openFile();
    void openProgram();
    void setUtf8();
    void ItemOpen(QTreeWidgetItem*,int);//树形目录双击打开文件
    void OpenCurerentFile(); //树形目录右键菜单打开文件
    void setAnsi();
    virtual void keyPressEvent(QKeyEvent *event);
    void showFindDialog();//显示查找框
    void searchText(const QString &text,Qt::CaseSensitivity cs);
    void replaceText(const QString &text,Qt::CaseSensitivity cs);
    void showProgramView(); //显示项目视图
    void hideProgramView(); //隐藏项目视图
    void popMenu(const QPoint&); //树形目录的右键菜单
    void deleteCurrentItem();//删除树形目录的节点
    void addFileItem();//为项目增加文件
    void compile();
    void run();



private:
    Ui::MainWindow *ui;
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void setDockWindows();
    void setTreeWidget(QTreeWidget *treeWidget);
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void LoadFileTree(QString,QTreeWidget*,QTreeWidgetItem*);
    QString strippedName(const QString &fullFileName);
    QsciScintilla *editor;
    QString curFile;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QMenu *setMenu;
    QMenu *viewMenu;//视图
    QMenu *programViewMenu;//树形目录的视图
    QMenu *encodeMenu;//编码转换
    QMenu *openMenu;//打开文件还是文件夹
    QMenu *buildMenu;//编译构建运行
    QToolBar *fileToolBar;
    QToolBar *editToolBar;

    QAction *unicodeAct;//不同编码
    QAction *ansiAct;
    QAction *utf8Act;

    QAction *undoAct; //撤销
    QAction *redoAct;  //重做
    QAction *newAct;
    //QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *aboutAct;
    QAction *searchAct;//查找
    QAction *openFileAct;
    QAction *openProAct;
    QAction *compileAct;//编译
    QAction *runAct;//运行
    QAction *showProgramViewAct;
    QAction *hideProgramViewAct;
    QAction *deleteCurrentItemAct;
    QAction *addfileAct;
    QAction *ItemOpenAct;
    FindDialog *findDialog;//前置声明一个查找框的实例化

};

#endif // MAINWINDOW_H
