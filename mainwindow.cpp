#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addtaskdialog.h"
#include "addhabitdialog.h"
#include "datedelegate.h"
#include "taskstatusdelegate.h"
#include "habitstatusdelegate.h"
#include "planstatusdelegate.h"
#include "plannamedelegate.h"
#include "habitfrequencydelegate.h"
#include "utils.h"
#include <QDate>
#include <QFile>
#include <QLineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_dbManager("D:/Collection/Sqlite/PlanManage.db") // Initialize the database manager
{
    ui->setupUi(this);

    this->showMaximized(); // Show the window maximized

    this->setWindowTitle("计划管理软件"); // Set window title

    QFile file(QCoreApplication::applicationDirPath() + "/resource/Ubuntu.qss");
    file.open(QFile::ReadOnly);
    this->setStyleSheet(file.readAll());

    initChart();
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
    m_modelPlan->setHorizontalHeaderLabels({"类型", "计划名称", "完成状态"});

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

    // Adjust row height based on content
    ui->tableView_task->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView_habit->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView_plan->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->tableView_task->setItemDelegateForColumn(3, new DateDelegate(ui->tableView_task));
    ui->tableView_task->setItemDelegateForColumn(5, new TaskStatusDelegate(ui->tableView_task));
    ui->tableView_habit->setItemDelegateForColumn(2, new DateDelegate(ui->tableView_habit));
    ui->tableView_habit->setItemDelegateForColumn(3, new HabitFrequencyDelegate(ui->tableView_habit));
    ui->tableView_habit->setItemDelegateForColumn(4, new HabitStatusDelegate(ui->tableView_habit));
    PlanNameDelegate *planNameDelegate = new PlanNameDelegate(&m_dbManager, ui->tableView_plan, this);
    ui->tableView_plan->setItemDelegateForColumn(1, planNameDelegate);
    ui->tableView_plan->setItemDelegateForColumn(2, new PlanStatusDelegate(ui->tableView_plan));


    connect(m_modelTask, &TaskModel::dataChanged, this, &MainWindow::onTableViewTaskDataChanged);
    connect(m_modelHabit, &HabitModel::dataChanged, this, &MainWindow::onTableViewHabitDataChanged);

    QStringList taskStatuses = Utils::taskStatusList();
    taskStatuses.insert(0, "全部");
    ui->comboBox_task->addItems(taskStatuses);
    QStringList habitStatuses = Utils::habitStatusList();
    habitStatuses.insert(0, "全部");
    ui->comboBox_habit->addItems(habitStatuses);
    ui->comboBox_task->setCurrentText("进行中");
    ui->comboBox_habit->setCurrentText("进行中");

    ui->calendarWidget->clicked(QDate::currentDate());

    // Antomatically adjust table width
    adjustTableWidth(ui->tableView_plan);
    adjustTableWidth(ui->tableView_task);
    adjustTableWidth(ui->tableView_habit);

    ui->tableView_plan->viewport()->installEventFilter(this);
    ui->tableView_task->viewport()->installEventFilter(this);
    ui->tableView_habit->viewport()->installEventFilter(this);
}

void MainWindow::initChart()
{
    m_chartViewPlan = new QChartView(this);
    m_chartViewPlan->setRenderHint(QPainter::Antialiasing);

    QChart *chart = new QChart();
    chart->setTitle("任务完成情况");

    QDate startDate = QDate::currentDate().addDays(-15);
    QDate endDate = QDate::currentDate();
    QMap<QDate,double> resultDate = m_dbManager.getPlanNumberByDate(startDate, endDate);

    if (!resultDate.empty())
    {
        QLineSeries * series = new QLineSeries();

        for (auto it = resultDate.begin(); it != resultDate.end(); ++it) {
            QDateTime dateTime(it.key(), QTime(0, 0, 0));
            series->append(dateTime.toMSecsSinceEpoch(), it.value());
        }

        QDateTimeAxis *axisX = new QDateTimeAxis();
        axisX->setFormat("MM-dd");
        axisX->setTitleText("日期");
        axisX->setTickCount(16);

        QDateTime startDateTime(startDate, QTime(0, 0, 0));
        QDateTime endDateTime(endDate, QTime(0, 0, 0));
        // axisX->setLabelsAngle(-90);
        axisX->setRange(startDateTime, endDateTime);

        QValueAxis *axisY = new QValueAxis();
        axisY->setLabelFormat("%.2f");
        axisY->setTitleText("完成率");
        axisY->setTickCount(6);
        axisY->setRange(0.0, 1.0);

        chart->addSeries(series);
        chart->addAxis(axisX, Qt::AlignBottom);
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }
    else
    {
        chart->addSeries(new QLineSeries());
        chart->setTitle("暂无数据");
    }
    chart->legend()->hide();

    m_chartViewPlan->setChart(chart);

    ui->horizontalLayout->insertWidget(1, m_chartViewPlan);
}


bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Resize)
    {
        if (obj == ui->tableView_plan->viewport())
        {
            adjustTableWidth(ui->tableView_plan);
        }
        else if (obj == ui->tableView_task->viewport())
        {
            adjustTableWidth(ui->tableView_task);
        }
        else if (obj == ui->tableView_habit->viewport())
        {
            adjustTableWidth(ui->tableView_habit);
        }
    }
    return QObject::eventFilter(obj, event);
}

void MainWindow::saveData()
{
    QAbstractItemModel* model = ui->tableView_plan->model();
    if (!model) {
        qDebug() << "Model is not set!";
        return;
    }

    QDate selectedDate = ui->calendarWidget->selectedDate();

    int rowCount = model->rowCount();

    for (int row = 1; row <= rowCount; ++row) {
        QString type = model->index(row - 1, 0).data(Qt::DisplayRole).toString();
        int indexId = row;
        QString name = model->index(row - 1, 1).data(Qt::DisplayRole).toString();
        int status = Utils::planStatusFromString(model->index(row - 1, 2).data(Qt::DisplayRole).toString());
        if (type == "习惯")
        {
            int habitId = m_dbManager.getHabitIdByName(name);
            m_dbManager.updateHabitPlan(indexId, name, status, habitId, selectedDate);
        }
        else if (type == "任务")
        {
            int taskId = m_dbManager.getTaskIdByName(name);
            m_dbManager.updateTaskPlan(indexId, name, status, taskId, selectedDate);
        }
        else
        {
            continue;
        }
    }

    QString reflection = ui->textEdit_reflection->toPlainText();
    QString summary = ui->textEdit_summary->toPlainText();

    m_dbManager.updateReview(reflection, summary, selectedDate);
    on_calendarWidget_clicked(ui->calendarWidget->selectedDate());
}

void MainWindow::adjustTableWidth(QTableView *tableView)
{
    if (!tableView || !tableView->model())
        return;

    tableView->resizeColumnsToContents();

    int totalWidth = 0;
    int columnCount = tableView->horizontalHeader()->count();

    for (int i = 0; i < columnCount; ++i)
    {
        totalWidth += tableView->horizontalHeader()->sectionSize(i);
    }

    int viewportWidth = tableView->viewport()->width();

    if (totalWidth > 0 && totalWidth != viewportWidth)
    {
        double factor = static_cast<double>(viewportWidth) / totalWidth;

        for (int i = 0; i < columnCount; ++i)
        {
            int originalWidth = tableView->horizontalHeader()->sectionSize(i);
            int newWidth = static_cast<int>(originalWidth * factor);
            tableView->setColumnWidth(i, newWidth);
        }
    }
    else
    {
        return;
    }

    int adjustedTotalWidth = 0;
    for (int i = 0; i < columnCount; ++i)
    {
        adjustedTotalWidth += tableView->columnWidth(i);
    }

    int delta = viewportWidth - adjustedTotalWidth;
    if (delta != 0 && columnCount > 0)
    {
        int lastColumnIndex = columnCount - 1;
        tableView->setColumnWidth(lastColumnIndex, tableView->columnWidth(lastColumnIndex) + delta);
    }
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
    on_calendarWidget_clicked(ui->calendarWidget->selectedDate());
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

    QList<TaskData> taskDataList = m_dbManager.getTaskByStatus(index);

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

    PlanNameDelegate* planNameDelegate = qobject_cast<PlanNameDelegate*>(ui->tableView_plan->itemDelegateForColumn(1));
    if (planNameDelegate) {
        planNameDelegate->refreshTaskNames();
    }
    adjustTableWidth(ui->tableView_task);
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

    adjustTableWidth(ui->tableView_habit);
}


void MainWindow::on_calendarWidget_clicked(const QDate &date)
{
    // Update the chart
    QChart *chart = m_chartViewPlan->chart();
    chart->removeAllSeries(); // Clear existing series

    QDate startDate = date.addDays(-15);
    QMap<QDate, double> resultDate = m_dbManager.getPlanNumberByDate(startDate, date);

    QDateTimeAxis *axisX = nullptr;
    QValueAxis *axisY = nullptr;

    foreach (QAbstractAxis* ax, chart->axes(Qt::Horizontal)) {
        axisX = qobject_cast<QDateTimeAxis*>(ax);
    }
    foreach (QAbstractAxis* ay, chart->axes(Qt::Vertical)) {
        axisY = qobject_cast<QValueAxis*>(ay);
    }

    if (!axisX) {
        axisX = new QDateTimeAxis();
        chart->addAxis(axisX, Qt::AlignBottom);
    }
    if (!axisY) {
        axisY = new QValueAxis();
        chart->addAxis(axisY, Qt::AlignLeft);
    }

    axisX->setFormat("MM-dd");
    axisX->setTitleText("日期");
    axisX->setTickCount(16);
    axisX->setRange(QDateTime(date.addDays(-15), QTime(0,0,0)), QDateTime(date, QTime(0,0,0)));

    axisY->setLabelFormat("%.2f");
    axisY->setTitleText("完成率");
    axisY->setTickCount(6);
    axisY->setRange(0.0, 1.0);

    chart->removeAllSeries();

    if (!resultDate.empty()) {
        QLineSeries *series = new QLineSeries();
        for (auto it = resultDate.begin(); it != resultDate.end(); ++it) {
            series->append(QDateTime(it.key(), QTime(0,0,0)).toMSecsSinceEpoch(), it.value());
        }
        chart->addSeries(series);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
        chart->setTitle("任务完成情况");
    } else {
        chart->addSeries(new QLineSeries());
        chart->setTitle("暂无数据");
    }

    chart->legend()->hide();

    m_modelPlan->removeRows(0, m_modelPlan->rowCount());

    QList<PlanData> planDataList;
    planDataList = m_dbManager.getPlanByDate(date);
    bool needAdd = true;

    for (const PlanData &plan : planDataList)
    {
        if (plan.type == "习惯") needAdd = false;
        QList<QStandardItem*> items;
        items.append(new QStandardItem(plan.type));
        items.append(new QStandardItem(plan.name));
        items.append(new QStandardItem(Utils::planStatusToString(plan.status)));

        items[0]->setTextAlignment(Qt::AlignCenter);

        m_modelPlan->appendRow(items);
    }

    ReviewData reviewData;
    reviewData = m_dbManager.getReviewByDate(date);
    ui->textEdit_reflection->setText(reviewData.reflection);
    ui->textEdit_summary->setText(reviewData.summary);

    if (!needAdd)
    {
        return;
    }

    QList<HabitData> habitDataList;
    habitDataList = m_dbManager.getHabitByStatus(1);

    for (const HabitData &habit : habitDataList)
    {
        // Skip habits created after the selected date
        if (habit.createdDate > date)
            continue;

        QString frequency = habit.target_frequency;
        bool shouldAdd = false;

        if (frequency == "每日一次")
        {
            shouldAdd = true;
        }
        else if (frequency.startsWith("每二日一次"))
        {
            int daysDiff = habit.createdDate.daysTo(date);
            if (daysDiff % 2 == 0)
                shouldAdd = true;
        }
        else if (frequency.startsWith("每三日一次"))
        {
            int daysDiff = habit.createdDate.daysTo(date);
            if (daysDiff % 3 == 0)
                shouldAdd = true;
        }
        else if (frequency.startsWith("每周周五"))
        {
            if (date.dayOfWeek() == 5)
                shouldAdd = true;
        }
        else if (frequency.startsWith("每周周六"))
        {
            if (date.dayOfWeek() == 6)
                shouldAdd = true;
        }

        if (shouldAdd)
        {
            QList<QStandardItem*> items;
            items.append(new QStandardItem("习惯"));
            items.append(new QStandardItem(habit.name));
            items.append(new QStandardItem(Utils::planStatusToString(habit.status)));
            items[0]->setTextAlignment(Qt::AlignCenter);

            m_modelPlan->appendRow(items);
        }
    }
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

void MainWindow::on_pushButton_savePlan_clicked()
{
    saveData();
}


void MainWindow::on_pushButton_saveReflection_clicked()
{
    saveData();
}


void MainWindow::on_pushButton_saveSummary_clicked()
{
    saveData();
}


void MainWindow::on_pushButton_delete_clicked()
{
    QTableView *tableView = ui->tableView_plan;
    QAbstractItemModel *model = tableView->model();
    QItemSelectionModel *selectionModel = tableView->selectionModel();

    if (!model || !selectionModel) {
        qDebug() << "Model or selection model is null!";
        return;
    }

    QModelIndexList selectedRows = selectionModel->selectedRows();
    if (selectedRows.isEmpty()) {
        qDebug() << "No row selected!";
        return;
    }

    for (int i = selectedRows.size() - 1; i >= 0; --i) {
        int row = selectedRows[i].row();
        model->removeRow(row);
    }
}


void MainWindow::on_pushButton_insert_clicked()
{
    QTableView *tableView = ui->tableView_plan;
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(tableView->model());

    if (!model) {
        qDebug() << "Model is not a QStandardItemModel!";
        return;
    }

    QList<QStandardItem*> items;
    items.append(new QStandardItem("任务"));
    items.append(new QStandardItem(""));
    items.append(new QStandardItem("进行中"));

    model->appendRow(items);
}

