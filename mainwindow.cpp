#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addtaskdialog.h"
#include "addhabitdialog.h"
#include "datedelegate.h"
#include "taskstatusdelegate.h"
#include "habitstatusdelegate.h"
#include "habitfrequencydelegate.h"
#include "utils.h"
#include <QDate>

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
    // Create a model for the table view
    m_modelTask = new TaskModel(this);
    m_modelHabit = new HabitModel(this);
    m_modelPlan = new PlanModel(this);

    m_modelTask->setHorizontalHeaderLabels({"ID", "任务名称", "创建日期", "截止日期", "完成日期", "完成状态"});
    m_modelHabit->setHorizontalHeaderLabels({"ID", "习惯名称", "创建日期", "习惯频率", "完成状态"});
    m_modelPlan->setHorizontalHeaderLabels({"计划名称", "完成状态"});

    // Set the model to the table view
    ui->tableView_task->setModel(m_modelTask);
    ui->tableView_habit->setModel(m_modelHabit);
    ui->tableView_plan->setModel(m_modelPlan);

    ui->tableView_task->verticalHeader()->setVisible(false);
    ui->tableView_habit->verticalHeader()->setVisible(false);

    // Enable word wrap
    ui->tableView_task->setWordWrap(true);
    ui->tableView_habit->setWordWrap(true);
    ui->tableView_plan->setWordWrap(true);

    ui->tableView_task->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView_habit->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView_plan->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Adjust row height based on content
    ui->tableView_task->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView_habit->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView_plan->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->tableView_task->setItemDelegateForColumn(3, new DateDelegate(ui->tableView_task));
    ui->tableView_task->setItemDelegateForColumn(5, new TaskStatusDelegate(ui->tableView_task));
    ui->tableView_habit->setItemDelegateForColumn(2, new DateDelegate(ui->tableView_habit));
    ui->tableView_habit->setItemDelegateForColumn(3, new HabitFrequencyDelegate(ui->tableView_habit));
    ui->tableView_habit->setItemDelegateForColumn(4, new HabitStatusDelegate(ui->tableView_habit));


    connect(m_modelTask, &TaskModel::dataChanged, this, &MainWindow::onTableViewTaskDataChanged);
    connect(m_modelHabit, &HabitModel::dataChanged, this, &MainWindow::onTableViewHabitDataChanged);

    ui->comboBox_task->addItems(Utils::taskStatusList());
    ui->comboBox_habit->addItems(Utils::habitStatusList());
    ui->comboBox_task->setCurrentText("进行中");
    ui->comboBox_habit->setCurrentText("进行中");
}


void MainWindow::on_pushButton_add_task_clicked()
{
    AddTaskDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted)
    {
        TaskData taskData;
        taskData.name = dialog.getTaskName();
        taskData.dueDate = dialog.getDueDate();

        m_dbManager.addTask(taskData);

        on_comboBox_task_currentIndexChanged(ui->comboBox_task->currentIndex());
    }
}

void MainWindow::onTableViewTaskDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if (!roles.contains(Qt::EditRole)) return;

    int row = topLeft.row();
    int col = topLeft.column();

    const QAbstractItemModel *model = topLeft.model();

    QModelIndex idIndex = model->index(row, 0);
    QString taskIdStr = model->data(idIndex, Qt::DisplayRole).toString();
    bool ok;
    int taskId = taskIdStr.toInt(&ok);

    if (!ok) {
        qDebug() << "Invalid task ID at row" << row;
        return;
    }

    QVariant newValue = model->data(topLeft, Qt::EditRole); // Get the new value from the model

    switch (col) {
    case 1:
        m_dbManager.updateTaskName(taskId, newValue.toString());
        break;
    case 3:
        m_dbManager.updateTaskDueDate(taskId, QDate::fromString(newValue.toString(), "yyyy年MM月dd日"));
        break;
    case 5:
        m_dbManager.updateTaskStatus(taskId, Utils::taskStatusFromString(newValue.toString()));
        break;
    default:
        qDebug() << "Uneditable column modified.";
        break;
    }
    on_comboBox_task_currentIndexChanged(ui->comboBox_task->currentIndex());
}

void MainWindow::onTableViewHabitDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if (!roles.contains(Qt::EditRole)) return;

    int row = topLeft.row();
    int col = topLeft.column();

    const QAbstractItemModel *model = topLeft.model();

    QModelIndex idIndex = model->index(row, 0);
    QString habitIdStr = model->data(idIndex, Qt::DisplayRole).toString();
    bool ok;
    int habitId = habitIdStr.toInt(&ok);

    if (!ok) {
        qDebug() << "Invalid habit ID at row" << row;
        return;
    }

    QVariant newValue = model->data(topLeft, Qt::EditRole); // Get the new value from the model

    switch (col) {
    case 1:
        m_dbManager.updateHabitName(habitId, newValue.toString());
        break;
    case 2:
        m_dbManager.updateHabitCreatedDate(habitId, QDate::fromString(newValue.toString(), "yyyy年MM月dd日"));
        break;
    case 3:
        m_dbManager.updateHabitFrequency(habitId, newValue.toString());
        break;
    case 4:
        m_dbManager.updateHabitStatus(habitId, Utils::habitStatusFromString(newValue.toString()));
        break;
    default:
        qDebug() << "Uneditable column modified.";
        break;
    }
    on_comboBox_habit_currentIndexChanged(ui->comboBox_task->currentIndex());
}


TaskModel::TaskModel(QObject *parent)
    : QStandardItemModel(parent)
{

}

Qt::ItemFlags TaskModel::flags(const QModelIndex &index) const
{
    switch (index.column()) {
    case 0:
    case 2:
    case 4:
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    default:
        return QStandardItemModel::flags(index);
    }
}

void MainWindow::on_comboBox_task_currentIndexChanged(int index)
{
    m_modelTask->removeRows(0, m_modelTask->rowCount());

    QList<TaskData> taskDataList;
    taskDataList = m_dbManager.getTaskByStatus(index);

    for (const TaskData &taskData : std::as_const(taskDataList)) {
        QList<QStandardItem*> items;
        items.append(new QStandardItem(QString::number(taskData.id)));
        items.append(new QStandardItem(taskData.name));
        items.append(new QStandardItem(taskData.createdDate.toString("yyyy年MM月dd日")));
        items.append(new QStandardItem(taskData.dueDate.toString("yyyy年MM月dd日")));
        items.append(new QStandardItem(taskData.completedDate.toString("yyyy年MM月dd日")));
        items.append(new QStandardItem(Utils::taskStatusToString(taskData.status)));

        for (int i = 0; i < items.size(); ++i) {
            if (i == 1) continue;
            items[i]->setTextAlignment(Qt::AlignCenter);
        }

        m_modelTask->appendRow(items);
    }

    ui->tableView_task->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}


void MainWindow::on_pushButton_add_habit_clicked()
{
    AddHabitDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted)
    {
        HabitData habitData;
        habitData.name = dialog.getHabitName();
        habitData.target_frequency = dialog.getHabitFrequency();

        m_dbManager.addHabit(habitData);

        on_comboBox_habit_currentIndexChanged(ui->comboBox_habit->currentIndex());
    }
}


HabitModel::HabitModel(QObject *parent)
    : QStandardItemModel(parent)
{

}

Qt::ItemFlags HabitModel::flags(const QModelIndex &index) const
{
    switch (index.column()) {
    case 0:
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    default:
        return QStandardItemModel::flags(index);
    }
}

void MainWindow::on_comboBox_habit_currentIndexChanged(int index)
{
    m_modelHabit->removeRows(0, m_modelHabit->rowCount());

    QList<HabitData> habitDataList;
    habitDataList = m_dbManager.getHabitByStatus(index);

    for (const HabitData &habitData : std::as_const(habitDataList)) {
        QList<QStandardItem*> items;
        items.append(new QStandardItem(QString::number(habitData.id)));
        items.append(new QStandardItem(habitData.name));
        items.append(new QStandardItem(habitData.createdDate.toString("yyyy年MM月dd日")));
        items.append(new QStandardItem(habitData.target_frequency));
        items.append(new QStandardItem(Utils::habitStatusToString(habitData.status)));

        for (int i = 0; i < items.size(); ++i) {
            if (i == 1) continue;
            items[i]->setTextAlignment(Qt::AlignCenter);
        }

        m_modelHabit->appendRow(items);
    }

    ui->tableView_habit->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}


void MainWindow::on_calendarWidget_clicked(const QDate &date)
{
    m_modelPlan->removeRows(0, m_modelPlan->rowCount());

    QList<HabitData> habitDataList;
    habitDataList = m_dbManager.getHabitByStatus(index);
}


PlanModel::PlanModel(QObject *parent)
    : QStandardItemModel(parent)
{

}

Qt::ItemFlags PlanModel::flags(const QModelIndex &index) const
{
    switch (index.column()) {
    case 0:
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    default:
        return QStandardItemModel::flags(index);
    }
}
