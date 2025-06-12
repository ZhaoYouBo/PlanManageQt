#include "database.h"
#include <QSqlError>
#include <QSqlQuery>

Database::Database(const QString &dbName) {
    m_db = QSqlDatabase::addDatabase("QSQLITE"); // Add database
    m_db.setDatabaseName(dbName); // Set database name
    if (!m_db.open()) {
        qDebug() << "Error: " << m_db.lastError().text();
    } else {
        qDebug() << "Database opened";
    }

    createTables();
}

void Database::createTables()
{
    QSqlQuery query;

    // Task table
    query.exec(
        "CREATE TABLE IF NOT EXISTS task ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "created_date DATE DEFAULT CURRENT_DATE, "
        "due_date DATE, "
        "completed_date DATE, "
        "status INTEGER DEFAULT 0"
        ")"
    );

    // Habit table
    query.exec(
        "CREATE TABLE IF NOT EXISTS habits ("
        "id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "created_date DATE DEFAULT CURRENT_DATE, "
        "target_frequency TEXT, "
        "status INTEGER DEFAULT 0); "
    );

    // Daily plan table
    query.exec(
        "CREATE TABLE IF NOT EXISTS daily_plan ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "task_id INTEGER, "
        "habit_id INTEGER, "
        "plan_date DATE NOT NULL, "
        "plan_name TEXT, "
        "index_id INTEGER, "
        "status INTEGER DEFAULT 0, "
        "FOREIGN KEY (task_id) REFERENCES task(id) ON DELETE CASCADE, "
        "FOREIGN KEY (habit_id) REFERENCES habits(id) ON DELETE CASCADE, "
        "CHECK ( (task_id IS NOT NULL AND habit_id IS NULL) OR (task_id IS NULL AND habit_id IS NOT NULL) )"
        ")"
    );

    //  Daily review table
    query.exec(
        "CREATE TABLE IF NOT EXISTS daily_review ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "review_date DATE NOT NULL UNIQUE, "
        "reflection TEXT, "
        "summary TEXT"
        ")"
    );
}

QList<TaskData> Database::getTaskByStatus(int status)
{
    QList<TaskData> taskDataList;

    QSqlQuery query("SELECT id, name, created_date, due_date, completed_date, status FROM task WHERE status = ?;");
    query.addBindValue(status);

    if (!query.exec()) {
        qDebug() << "Failed to get task";
        return taskDataList;
    }

    while(query.next()) {
        TaskData taskData;
        taskData.id = query.value(0).toInt();
        taskData.name = query.value(1).toString();
        taskData.createdDate = query.value(2).toDate();
        taskData.dueDate = query.value(3).toDate();
        taskData.completedDate = query.value(4).toDate();
        taskData.status = query.value(5).toInt();

        taskDataList.append(taskData);
    }

    return taskDataList;
}

QList<HabitData> Database::getHabitByStatus(int status)
{
    QList<HabitData> habitDataList;

    QSqlQuery query("SELECT id, name, created_date, target_frequency, status FROM habits WHERE status = ?;");
    query.addBindValue(status);

    if (!query.exec()) {
        qDebug() << "Failed to get habit";
        return habitDataList;
    }

    while(query.next()) {
        HabitData habitData;
        habitData.id = query.value(0).toInt();
        habitData.name = query.value(1).toString();
        habitData.createdDate = query.value(2).toDate();
        habitData.target_frequency = query.value(3).toString();
        habitData.status = query.value(4).toInt();

        habitDataList.append(habitData);
    }

    return habitDataList;
}

QList<PlanData> Database::getPlanByDate(const QDate &date)
{
    QList<PlanData> planDataList;

    QSqlQuery query("SELECT task_id, habit_id, plan_name, status FROM daily_plan WHERE plan_date = ?;");
    query.addBindValue(date);

    if (!query.exec()) {
        qDebug() << "Failed to get plan";
        return planDataList;
    }

    while(query.next()) {
        PlanData planData;
        planData.name = query.value(2).toString();
        planData.status = query.value(3).toInt();

        if (!query.value(0).isNull()) {
            planData.type = "任务";
        } else if (!query.value(1).isNull()) {
            planData.type = "习惯";
        } else {
            continue;
        }

        planDataList.append(planData);
    }

    return planDataList;
}

ReviewData Database::getReviewByDate(const QDate &date)
{
    ReviewData reviewData;

    QSqlQuery query("SELECT reflection, summary FROM daiiy_review WHERE review_date = ?;");
    query.addBindValue(date);

    if (!query.exec() || !query.next()) {
        qDebug() << "Failed to get habit";
        return reviewData;
    }

    reviewData.reflection = query.value(0).toString();
    reviewData.summary = query.value(1).toString();

    return reviewData;
}

void Database::addTask(TaskData data)
{
    QSqlQuery query;
    query.prepare("INSERT INTO task (name, due_date) VALUES (?, ?);");
    query.addBindValue(data.name);
    query.addBindValue(data.dueDate);
    if (query.exec()) {
        qDebug() << "Task added successfully!";
    }
    else {
        qDebug() << "Failed to add Task: " << query.lastError().text();
    }
}

void Database::addHabit(HabitData data)
{
    QSqlQuery query;
    query.prepare("INSERT INTO habits (name, target_frequency) VALUES (?, ?);");
    query.addBindValue(data.name);
    query.addBindValue(data.target_frequency);
    if (query.exec()) {
        qDebug() << "Habit added successfully!";
    }
    else {
        qDebug() << "Failed to add Habit: " << query.lastError().text();
    }
}

void Database::updateTaskName(int id, const QString &name)
{
    QSqlQuery query;
    query.prepare("UPDATE task SET name = ? WHERE id = ?");
    query.addBindValue(name);
    query.addBindValue(id);
    if (!query.exec()) {
        qDebug() << "Failed to update task name:" << query.lastError().text();
    }
}

void Database::updateTaskDueDate(int id, const QDate &date)
{
    QSqlQuery query;
    query.prepare("UPDATE task SET due_date = ? WHERE id = ?");
    query.addBindValue(date);
    query.addBindValue(id);
    if (!query.exec()) {
        qDebug() << "Failed to update due date:" << query.lastError().text();
    }
}

void Database::updateTaskStatus(int id, int status)
{
    QSqlQuery query;
    switch (status) {
    case 0:
    case 2:
    case 4:
        query.prepare("UPDATE task SET status = ?, completed_date = '' WHERE id = ?");
        query.addBindValue(status);
        break;
    default:
        query.prepare("UPDATE task SET status = ?, completed_date = ? WHERE id = ?");
        query.addBindValue(status);
        query.addBindValue(QDate::currentDate());
        break;
    }
    query.addBindValue(id);
    if (!query.exec()) {
        qDebug() << "Failed to update status:" << query.lastError().text();
    }
}

void Database::updateHabitName(int id, const QString &name)
{
    QSqlQuery query;
    query.prepare("UPDATE habits SET name = ? WHERE id = ?");
    query.addBindValue(name);
    query.addBindValue(id);
    if (!query.exec()) {
        qDebug() << "Failed to update habit name:" << query.lastError().text();
    }
}

void Database::updateHabitCreatedDate(int id, const QDate &date)
{
    QSqlQuery query;
    query.prepare("UPDATE habits SET created_date = ? WHERE id = ?");
    query.addBindValue(date);
    query.addBindValue(id);
    if (!query.exec()) {
        qDebug() << "Failed to update created date:" << query.lastError().text();
    }
}

void Database::updateHabitFrequency(int id, QString frequency)
{
    QSqlQuery query;
    query.prepare("UPDATE habits SET target_frequency = ? WHERE id = ?");
    query.addBindValue(frequency);
    query.addBindValue(id);
    if (!query.exec()) {
        qDebug() << "Failed to update habit frequency:" << query.lastError().text();
    }
}

void Database::updateHabitStatus(int id, int status)
{
    QSqlQuery query;
    query.prepare("UPDATE habits SET status = ? WHERE id = ?");
    query.addBindValue(status);
    query.addBindValue(id);
    if (!query.exec()) {
        qDebug() << "Failed to update status:" << query.lastError().text();
    }
}
