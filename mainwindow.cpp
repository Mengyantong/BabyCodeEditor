#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QVBoxLayout"
#include <QEvent>
#include <QKeyEvent>
#include <QWindow>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    editor = new QsciScintilla;//编辑器
    setCentralWidget(editor);
    CreatUi();
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    setDockWindows();//设置DockWindow用来放置树形目录
    readSettings();

    connect(editor, SIGNAL(textChanged()),
            this, SLOT(documentWasModified()));
    connect(treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(ItemOpen(QTreeWidgetItem*,int)));
    connect(treeWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
            this,SLOT(popMenu(const QPoint&)));//检测鼠标右键（右键菜单）
    setCurrentFile("");
    //this->grabKeyboard();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::CreatUi()
{
    setWindowTitle(QString::fromLocal8Bit("**CODE EDITOR**"));
    this->resize(QSize(600,500));   //设置初始窗口大小
    QsciLexerCPP *textLexer=new QsciLexerCPP;
   // QsciLexerPython *textLexer = new QsciLexerPython;
    textLexer->setColor(QColor(0,100,0),QsciLexerCPP::CommentLine);    //设置自带的注释行为绿色
    textLexer->setFoldComments(true);//设置可以折叠多行注释
       editor->setLexer(textLexer);
        //行号提示
       editor->setMarginType(0,QsciScintilla::NumberMargin);//设置编号为0的页边显示行号。
       editor->setMarginLineNumbers(0,true);//对该页边启用行号
       editor->setMarginWidth(0,30);//设置页边宽度//页边宽度改为30，可以显示一位数以上的数
       editor->setMarginsForegroundColor(QColor(234, 234, 234));//边栏字体颜色
       editor->setMarginsBackgroundColor(QColor(72, 61, 139));//边栏字体背景

       editor->setFont(QFont("Courier 10 Pitch"));//设置显示字体
       //editor->SendScintilla(QsciScintilla::SCI_SETCODEPAGE,QsciScintilla::SC_CP_UTF8);//设置编码为UTF-8
       //光标
       editor->setCaretWidth(2);//光标宽度，0表示不显示光标
       editor->setCaretLineVisible(true); //是否高亮显示光标所在行
       editor->setCaretLineBackgroundColor(QColor(255, 240, 245));//光标所在行背景颜色
       //折叠
       editor->setFolding(QsciScintilla::BoxedTreeFoldStyle);//折叠样式
       editor->setFoldMarginColors(Qt::gray,Qt::lightGray);//折叠栏颜色

       //自动补全
       QsciAPIs *apis = new QsciAPIs(textLexer);
       if(!apis->load(QString(":/api_cpp.txt")))
       QMessageBox::warning(this,QString("warning"),QString("Failed to read code completion file"));
       else
           apis->prepare();
       editor->setAutoCompletionSource(QsciScintilla::AcsAll);//自动补全。对于所有Ascii字符
       editor->setAutoCompletionThreshold(1);//设置每输入一个字符就会出现自动补全的提示

       editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);//括号匹配
       editor->callTip();//调用提示

}
void MainWindow::setUtf8()
{
   editor->SendScintilla(QsciScintilla::SCI_SETCODEPAGE,QsciScintilla::SC_CP_UTF8);
   QMessageBox::about(this, tr("About Application"),
            tr("已经设为了utf8"));
}
void MainWindow::setAnsi()//待改
{
   editor->SendScintilla(QsciScintilla::SCI_SETCODEPAGE,QsciScintilla::SC_CHARSET_ANSI);
   QMessageBox::about(this, tr("About Application"),
            tr("已经设为了ANSI"));
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        editor->clear();
        setCurrentFile("");
    }
}

void MainWindow::open()
{
  int ret = QMessageBox::warning(this, tr("BabyCodeEditor"),
                        tr("Do you want to open program?"),
                        QMessageBox::Yes| QMessageBox::Default,
                        QMessageBox::No | QMessageBox::Escape);
           if (ret == QMessageBox::Yes)
           {   openProgram();
               return;
           }
           int ter = QMessageBox::warning(this, tr("BabyCodeEditor"),
                        tr("Do you want to open file?"),
                        QMessageBox::Yes| QMessageBox::Default,
                        QMessageBox::No | QMessageBox::Escape);
           if (ter == QMessageBox::Yes)
           {  openFile();
              return;
           }
}
void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadFile(fileName);
}

void MainWindow::openProgram()
{
    QFileInfo fileinfo(QFileDialog::getExistingDirectory(this));
    QTreeWidgetItem *item1 = new QTreeWidgetItem(QStringList() << fileinfo.fileName(), 0);
    QFont font ("Microsoft YaHei", 9, 75); //设置item1的字体、大小、加粗程度
    item1->setFont(0,font);
    treeWidget->addTopLevelItem(item1); //设置顶层节点
    LoadFileTree(fileinfo.filePath(),NULL,item1); //加载树形目录
    dock->setVisible(true);//显示出项目视图
}
bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> example demonstrates how to "
               "write modern GUI applications using Qt, with a menu bar, "
               "toolbars, and a status bar."));
}

void MainWindow::documentWasModified()
{
    setWindowModified( editor->isModified());
}
void MainWindow::showFindDialog(){
    findDialog = new FindDialog;
    connect(findDialog, SIGNAL(findPrevious(const QString &,Qt::CaseSensitivity)), this, SLOT(searchText(const QString &,Qt::CaseSensitivity)));
    connect(findDialog, SIGNAL(findNext(const QString &,Qt::CaseSensitivity)), this, SLOT(searchText(const QString &,Qt::CaseSensitivity)));
    connect(findDialog, SIGNAL(emitReplaceSignal(const QString &,Qt::CaseSensitivity)), this, SLOT(replaceText(const QString &,Qt::CaseSensitivity)));
    findDialog->show();
}
void MainWindow::searchText(const QString &text,Qt::CaseSensitivity cs){
    editor->findFirst(text,true,cs,true,true);
}

void MainWindow::replaceText(const QString &text,Qt::CaseSensitivity cs)
{  //   qDebug()<<"huan到了";
      editor->replace(text);
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAct->setShortcut(tr("Ctrl+N"));
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcut(tr("Ctrl+X"));
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), editor, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcut(tr("Ctrl+C"));
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()),  editor, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcut(tr("Ctrl+V"));
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()),  editor, SLOT(paste()));

    undoAct = new QAction( tr("&undo"), this);
    undoAct->setShortcut(tr("Ctrl+Z"));
    connect(undoAct, SIGNAL(triggered()),  editor, SLOT(undo()));

    redoAct = new QAction( tr("&Redo"), this);
    connect(redoAct, SIGNAL(triggered()),  editor, SLOT(redo()));
    undoAct->setShortcut(tr("Ctrl+Y"));

    aboutAct = new QAction(QIcon(":/images/about.png"),tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the code edtior's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    openFileAct = new QAction(tr("&OpenFiles"), this);
    connect(openFileAct, SIGNAL(triggered()), this, SLOT(openFile()));
    openProAct = new QAction(tr("&OpenPrograms"), this);
    connect(openProAct, SIGNAL(triggered()), this, SLOT(openProgram()));

    showProgramViewAct = new QAction(tr("&ShowProgramView"), this);
    connect(showProgramViewAct, SIGNAL(triggered()), this, SLOT(showProgramView()));
    hideProgramViewAct = new QAction(tr("&HideProgramView"), this);
    connect(hideProgramViewAct, SIGNAL(triggered()), this, SLOT(hideProgramView()));

    deleteCurrentItemAct = new QAction(tr("&Delete"), this);
    connect(deleteCurrentItemAct, SIGNAL(triggered()), this, SLOT(deleteCurrentItem()));
    ItemOpenAct = new QAction(tr("OpenCurrentFile"),this);
    connect(ItemOpenAct, SIGNAL(triggered()), this, SLOT(OpenCurerentFile()));
    addfileAct = new QAction(tr("&addFiles"), this);
    connect(addfileAct, SIGNAL(triggered()), this, SLOT(addFileItem()));

    searchAct = new QAction(QIcon(":/images/search.png"),tr("&search"), this);
    searchAct->setShortcut(tr("Ctrl+F"));
    connect(searchAct, SIGNAL(triggered()), this, SLOT(showFindDialog()));

    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect( editor, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect( editor, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
    ansiAct = new QAction( tr("&ANSI"), this);
    ansiAct->setStatusTip(tr("set ANSI"));
    connect(ansiAct, SIGNAL(triggered()), this, SLOT(setAnsi()));

    unicodeAct = new QAction( tr("&UNICODE"), this);

    utf8Act = new QAction( tr("&Utf8"), this);
    utf8Act->setStatusTip(tr("set utf8"));
    connect(utf8Act, SIGNAL(triggered()), this, SLOT(setUtf8()));

    compileAct = new QAction( tr("&Compile"), this);
    connect(compileAct, SIGNAL(triggered()), this, SLOT(compile()));

    runAct = new QAction( tr("&Run"), this);
    connect(runAct, SIGNAL(triggered()), this, SLOT(run()));

}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    //fileMenu->addAction(openAct);
    openMenu = fileMenu->addMenu(tr("&open"));
    openMenu->addAction(openFileAct);
    openMenu->addAction(openProAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addAction(searchAct);

    setMenu = menuBar()->addMenu(tr("&Setting"));
    encodeMenu = setMenu->addMenu(tr("&setcode"));
    encodeMenu->addAction(utf8Act);
    encodeMenu->addAction(ansiAct);

    buildMenu = menuBar()->addMenu(tr("&Build"));
    buildMenu->addAction(compileAct);
    buildMenu->addAction(runAct);

    menuBar()->addSeparator();

    viewMenu = menuBar()->addMenu(tr("&View")); //视图
    programViewMenu = viewMenu->addMenu(tr("&ProgramView"));
    programViewMenu->addAction(showProgramViewAct);
    programViewMenu->addAction(hideProgramViewAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    //fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    editToolBar->addAction(searchAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}
void MainWindow::setDockWindows()
{
    dock->setFeatures(QDockWidget::DockWidgetMovable);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    dock->setVisible(false);//初始设置为不可见
    addDockWidget(Qt::LeftDockWidgetArea,dock);//将QDockWidget加入到MainWindow中，初始位置为左侧
    dock->setMaximumSize(300,800);
    dock->setMinimumSize(300,600);

    treeWidget->setHeaderHidden(true); //隐藏表头
    QPalette p(treeWidget->palette());//设置背景色
    p.setColor(QPalette::Base, QColor("#e5ebf4"));
    treeWidget->setPalette(p);
    setTreeWidget(treeWidget); //初始化文件树
    dock->setWidget(treeWidget);  //设置锚接部件的内容 TreeWidget
}

void MainWindow::setTreeWidget(QTreeWidget *treeWidget)
{
   treeWidget->setColumnCount(1);  //设置一列
   treeWidget->setColumnWidth(0,50);  //列的宽度
   treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
}

void MainWindow::ItemOpen(QTreeWidgetItem *item,int column)//双击打开树形目录
{
    if(column==0)
    {
        QString filename=item->data(0,Qt::UserRole).toString();//获取节点data里存的文件名
        loadFile(filename);//在编辑器里加载文件内容
    }

}

void MainWindow::OpenCurerentFile()//右键菜单打开树形目录
{
    QTreeWidgetItem * curItem = treeWidget->currentItem();//获取当前节点
    QString filename=curItem->data(0,Qt::UserRole).toString();//获取节点data里存的文件名
    loadFile(filename);//在编辑器里加载文件内容
}
void MainWindow::popMenu(const QPoint&)
{

    QTreeWidgetItem* curItem=treeWidget->currentItem();  //获取当前被点击的节点
    if (curItem == NULL)
    {
        QMenu treeRightMenu(treeWidget);
        treeRightMenu.addAction(hideProgramViewAct);
        treeRightMenu.addAction(openProAct);
        treeRightMenu.exec();
        return;
    }//右键的位置不在treeWidget的Item的范围内，即在空白位置右击
    else
    {
        QMenu itemRightMenu(treeWidget);
        itemRightMenu.addAction(deleteCurrentItemAct);
        if (curItem->childCount()==0)
        {
            itemRightMenu.addAction(ItemOpenAct);
        }//没有子节点，是一个文件
        if (curItem->parent()== NULL)
        {
            itemRightMenu.addAction(addfileAct);
        }//没有父节点（即为一个项目）
        itemRightMenu.exec(QCursor::pos());
    }//右键的位置在treeWidget的Item的范围内
}

void MainWindow::deleteCurrentItem()
{
    QTreeWidgetItem * curItem = treeWidget->currentItem();//获取当前节点
    if(curItem==NULL)
    {
         return;
    }//右键不在节点上的情况

    int ret = QMessageBox::warning(this, tr("BabyCodeEditor"),
                          tr("Do you really want to delete this from editor?"),
                          QMessageBox::Yes| QMessageBox::Default,
                          QMessageBox::No | QMessageBox::Escape);
    if (ret == QMessageBox::Yes)
    {
        if(curItem->parent()==NULL)
        {
             delete treeWidget->takeTopLevelItem(treeWidget->currentIndex().row());
        }//没有父节点的情况，直接删除
        else
        {
            delete curItem->parent()->takeChild(treeWidget->currentIndex().row());
        }//有父节点的情况，用父节点的takeChild删除节点
        return;
    }
}

void MainWindow::addFileItem()//为项目添加文件
{
    QTreeWidgetItem * curItem = treeWidget->currentItem();//获取当前节点
    QFileInfo fileinfo = (QFileDialog::getOpenFileName(this));
    QTreeWidgetItem *fileItem = new QTreeWidgetItem(QStringList() << fileinfo.fileName(), 1);   //1表示是文件
    fileItem->setData(0,Qt::UserRole,QVariant(fileinfo.filePath())); //设置节点Qt::UserRole的Data,存储完整文件名称(包括路径），便于打开文件
    curItem->addChild(fileItem);
}

void MainWindow::showProgramView()
{
    dock->setVisible(true);
}

void MainWindow::hideProgramView()
{
    dock->setVisible(false);
}

void MainWindow::readSettings()
{
    QSettings settings("Trolltech", "BabyCodeEditor");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("Trolltech", "BabyCodeEditor");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

bool MainWindow::maybeSave()
{
    if (editor->isModified()) {
        int ret = QMessageBox::warning(this, tr("BabyCodeEditor"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Yes | QMessageBox::Default,
                     QMessageBox::No,
                     QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Yes)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("BabyCodeEditor"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    editor->setText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("BabyCodeEditor"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << editor->text();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}
void MainWindow::LoadFileTree(QString path, QTreeWidget *treewidget, QTreeWidgetItem *item)
{
    //加载树形目录
    QDir dir(path);
    if (!dir.exists())
    {
        return;
    }
    dir.setFilter(QDir::Dirs|QDir::Files| QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();

    int size = list.size();

    for (int i = 0; i < size; i++) {
        QFileInfo info = list.at(i);
        if (info.fileName() == "." || info.fileName() == "..")
            continue;

        if (info.isDir()) {
            QTreeWidgetItem *fileItem = new QTreeWidgetItem(QStringList() << info.fileName(), 0);  //0表示目录
            fileItem->setData(0,Qt::UserRole,QVariant(info.filePath())); //设置节点Qt::UserRole的Data,存储完整文件名称(包括路径），便于打开
            if (treewidget == NULL) item->addChild(fileItem);
            else treewidget->addTopLevelItem(fileItem);
            LoadFileTree(info.filePath(), NULL, fileItem);
        }
        else {
            QTreeWidgetItem *fileItem = new QTreeWidgetItem(QStringList() << info.fileName(), 1);   //1表示是文件
            fileItem->setData(0,Qt::UserRole,QVariant(info.filePath())); //设置节点Qt::UserRole的Data,存储完整文件名称(包括路径），便于打开文件
            if (treewidget == NULL) item->addChild(fileItem);
            else treewidget->addTopLevelItem(fileItem);
        }
    }
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
     editor->setModified(false);
    setWindowModified(false);

    QString shownName;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = strippedName(curFile);

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("BabyCodeEditor")));
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
//括号自动补全及自动缩进（没改好）
void MainWindow::keyPressEvent(QKeyEvent *event){
    setFocusPolicy(Qt::StrongFocus);
  qDebug()<<event->key()<<"lll";
  if(event->modifiers()==Qt::ShiftModifier&&event->key()=='('){
      editor->insert(tr("()"));
      //editor->setCursorPosition()
      //this->insertPlainText(tr("()"));
      int line, index;
      editor->getCursorPosition(&line,&index);
      qDebug()<<line<<index<<endl;
      editor->setCursorPosition(line,index-1);
      qDebug()<<line<<index<<endl;
      //this->moveCursor(QTextCursor::PreviousCharacter);
    }
}
void MainWindow::compile()
{
    //SetCurrentDirectory("E://");
    QString srcname = curFile;
    qDebug()<<srcname<<endl;
    QString destname = srcname.replace(".cpp",".exe");
    qDebug()<<destname<<endl;
    QString command = "g++ "+curFile+" -o "+destname;
    system(command.toStdString().data());
    qDebug()<<command<<endl;
    system(destname.toStdString().data());
    //system("pause");
}
void MainWindow::run()
{
     //  qDebug()<<"我在run";
       QString tname = curFile;
       QString exefile = curFile.replace(".cpp",".exe");
       system(exefile.toStdString().data());
       curFile = tname;
}
