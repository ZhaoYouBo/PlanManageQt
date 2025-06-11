#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_dbManager("D:/Collection/Sqlite/PlanManage.db") // Initialize the database manager
{
    ui->setupUi(this);

    this->showMaximized(); // Show the window maximized

    this->setWindowTitle("计划管理软件"); // Set window title

    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    ui->treeWidget_task->setColumnCount(4);
    ui->treeWidget_task->setRootIsDecorated(false); //  Don't show the root item

    QTreeWidgetItem* headerItem = new QTreeWidgetItem();
    headerItem->setText(0, "任务名称");
    headerItem->setText(1, "创建日期");
    headerItem->setText(2, "截止日期");
    headerItem->setText(3, "完成日期");
    headerItem->setText(4, "完成状态");

    QFont boldFont;
    boldFont.setBold(true);

    headerItem->setFont(0, boldFont);
    headerItem->setFont(1, boldFont);
    headerItem->setFont(2, boldFont);
    headerItem->setFont(3, boldFont);
    headerItem->setFont(4, boldFont);

    ui->treeWidget_task->setHeaderItem(headerItem);
    ui->treeWidget_task->header()->setSectionResizeMode(QHeaderView::Stretch); // Set the header to stretch
}
