#include "database.h"
#include <QSqlError>
#include <QSqlQuery>

Database::Database(const QString &dbName) {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbName);
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

    query.exec(
        "CREATE TABLE IF NOT EXISTS habits ("
        "id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "created_date DATE DEFAULT CURRENT_DATE, "
        "target_frequency TEXT, "
        "status INTEGER DEFAULT 0); "
    );

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
    QSqlQuery query;

    if (status == 0) {
        if (!query.exec("SELECT id, name, created_date, due_date, completed_date, status "
                        "FROM task"))
        {
            return taskDataList;
        }
    } else {
        status = status - 1;
        query.prepare("SELECT id, name, created_date, due_date, completed_date, status "
                      "FROM task "
                      "WHERE status = ?");
        query.addBindValue(status);
        if (!query.exec())
        {
            return taskDataList;
        }
    }

    while (query.next()) {
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
    QSqlQuery query;

    if (status == 0) {
        if (!query.exec("SELECT id, name, created_date, target_frequency, status "
                        "FROM habits"))
        {
            return habitDataList;
        }
    } else {
        status = status - 1;
        query.prepare("SELECT id, name, created_date, target_frequency, status "
                      "FROM habits "
                      "WHERE status = ?");
        query.addBindValue(status);
        if (!query.exec()) {
            return habitDataList;
        }
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

    QSqlQuery query("SELECT task_id, habit_id, plan_name, status "
                    "FROM daily_plan "
                    "WHERE plan_date = ?;");
    query.addBindValue(date);

    if (!query.exec()) {
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

QMap<QDate, double> Database::getPlanNumberByDate(const QDate &startDate, const QDate &endDate)
{
    QMap<QDate, double> resultData;

    QSqlQuery query;
    query.prepare("SELECT plan_date, COUNT(*), SUM(CASE WHEN status = 1 THEN 1 ELSE 0 END) "
                  "FROM daily_plan "
                  "WHERE plan_date BETWEEN :start AND :end "
                  "GROUP BY plan_date");
    query.bindValue(":start", startDate);
    query.bindValue(":end", endDate);

    if (!query.exec()) {
        return resultData;
    }

    while (query.next())
    {
        QDate date = query.value(0).toDate();
        int total = query.value(1).toInt();
        int completed = query.value(2).toInt();

        double ratio = (total > 0) ? static_cast<double>(completed) / total : 0.0;
        resultData.insert(date, ratio);
    }

    return resultData;
}

ReviewData Database::getReviewByDate(const QDate &date)
{
    ReviewData reviewData;

    QSqlQuery query("SELECT reflection, summary "
                    "FROM daily_review "
                    "WHERE review_date = ?;");
    query.addBindValue(date);

    if (!query.exec() || !query.next()) {
        return reviewData;
    }

    reviewData.reflection = query.value(0).toString();
    reviewData.summary = query.value(1).toString();

    return reviewData;
}

void Database::addTask(TaskData data)
{
    QSqlQuery query;
    query.prepare("INSERT INTO task (name, due_date) "
                  "VALUES (?, ?);");
    query.addBindValue(data.name);
    query.addBindValue(data.dueDate);
    if (query.exec()) {
    }
    else {
    }
}

void Database::addHabit(HabitData data)
{
    QSqlQuery query;
    query.prepare("INSERT INTO habits (name, target_frequency) "
                  "VALUES (?, ?);");
    query.addBindValue(data.name);
    query.addBindValue(data.target_frequency);
    if (query.exec()) {
    }
    else {
    }
}

void Database::updateTaskName(int id, const QString &name)
{
    QSqlQuery query;
    query.prepare("UPDATE task "
                  "SET name = ? "
                  "WHERE id = ?");
    query.addBindValue(name);
    query.addBindValue(id);
    if (!query.exec()) {
    }
}

void Database::updateTaskDueDate(int id, const QDate &date)
{
    QSqlQuery query;
    query.prepare("UPDATE task "
                  "SET due_date = ? "
                  "WHERE id = ?");
    query.addBindValue(date);
    query.addBindValue(id);
    if (!query.exec()) {
    }
}

void Database::updateTaskStatus(int id, int status)
{
    QSqlQuery query;
    switch (status) {
    case 0:
    case 2:
    case 4:
        query.prepare("UPDATE task "
                      "SET status = ?, completed_date = '' "
                      "WHERE id = ?");
        query.addBindValue(status);
        break;
    default:
        query.prepare("UPDATE task "
                      "SET status = ?, completed_date = ? "
                      "WHERE id = ?");
        query.addBindValue(status);
        query.addBindValue(QDate::currentDate());
        break;
    }
    query.addBindValue(id);

    if (!query.exec()) {
        return;
    }

    QSqlQuery planQuery;
    if (status == 1 || status == 3) {
        planQuery.prepare("UPDATE daily_plan "
                          "SET status = 1 "
                          "WHERE plan_date = ? AND task_id = ?");
        planQuery.addBindValue(QDate::currentDate());
        planQuery.addBindValue(id);
    } else if (status == 2 || status == 4) {
        planQuery.prepare("UPDATE daily_plan "
                          "SET status = 2 "
                          "WHERE plan_date = ? AND task_id = ?");
        planQuery.addBindValue(QDate::currentDate());
        planQuery.addBindValue(id);
    }

    if (!planQuery.exec()) {
    }
}

void Database::updateHabitName(int id, const QString &name)
{
    QSqlQuery query;
    query.prepare("UPDATE habits "
                  "SET name = ? "
                  "WHERE id = ?");
    query.addBindValue(name);
    query.addBindValue(id);
    if (!query.exec()) {
    }
}

void Database::updateHabitCreatedDate(int id, const QDate &date)
{
    QSqlQuery query;
    query.prepare("UPDATE habits "
                  "SET created_date = ? "
                  "WHERE id = ?");
    query.addBindValue(date);
    query.addBindValue(id);
    if (!query.exec()) {
    }
}

void Database::updateHabitFrequency(int id, QString frequency)
{
    QSqlQuery query;
    query.prepare("UPDATE habits "
                  "SET target_frequency = ? "
                  "WHERE id = ?");
    query.addBindValue(frequency);
    query.addBindValue(id);
    if (!query.exec()) {
    }
}

void Database::updateHabitStatus(int id, int status)
{
    QSqlQuery query;
    query.prepare("UPDATE habits "
                  "SET status = ? "
                  "WHERE id = ?");
    query.addBindValue(status);
    query.addBindValue(id);
    if (!query.exec()) {
    }
}

int Database::getHabitIdByName(QString name)
{
    int habitId = 0;

    QSqlQuery query("SELECT id "
                    "FROM habits "
                    "WHERE name = ?;");
    query.addBindValue(name);

    if (!query.exec() || !query.next()) {
        return habitId;
    }

    habitId = query.value(0).toInt();

    return habitId;
}

int Database::getTaskIdByName(QString name)
{
    int taskId = 0;

    QSqlQuery query("SELECT id "
                    "FROM task "
                    "WHERE name = ?;");
    query.addBindValue(name);

    if (!query.exec() || !query.next())
    {
        return taskId;
    }

    taskId = query.value(0).toInt();

    return taskId;
}

void Database::updateHabitPlan(int index, QString name, int status, int habitId, QDate date)
{
    QSqlQuery query;
    query.prepare("UPDATE daily_plan "
                  "SET habit_id = ?, plan_name = ?, status = ? "
                  "WHERE plan_date = ? AND index_id = ?");
    query.addBindValue(habitId);
    query.addBindValue(name);
    query.addBindValue(status);
    query.addBindValue(date);
    query.addBindValue(index);

    if (!query.exec())
    {
        return;
    }

    if (query.numRowsAffected() == 0)
    {
        query.prepare("INSERT INTO daily_plan (habit_id, plan_date, plan_name, index_id, status) "
                      "VALUES (?, ?, ?, ?, ?)");
        query.addBindValue(habitId);
        query.addBindValue(date);
        query.addBindValue(name);
        query.addBindValue(index);
        query.addBindValue(status);

        if (!query.exec())
        {
        }
    }
}

void Database::updateTaskPlan(int index, QString name, int status, int taskId, QDate date)
{
    QSqlQuery query;
    query.prepare("UPDATE daily_plan "
                  "SET task_id = ?, plan_name = ?, status = ? "
                  "WHERE plan_date = ? AND index_id = ?");
    query.addBindValue(taskId);
    query.addBindValue(name);
    query.addBindValue(status);
    query.addBindValue(date);
    query.addBindValue(index);

    if (!query.exec())
    {
        return;
    }

    if (query.numRowsAffected() == 0)
    {
        query.prepare("INSERT INTO daily_plan (task_id, plan_date, plan_name, index_id, status) "
                      "VALUES (?, ?, ?, ?, ?)");
        query.addBindValue(taskId);
        query.addBindValue(date);
        query.addBindValue(name);
        query.addBindValue(index);
        query.addBindValue(status);

        if (!query.exec())
        {
        }
    }
}

void Database::updateReview(QString reflection, QString summary, QDate date)
{
    QSqlQuery query;
    query.prepare("UPDATE daily_review "
                  "SET reflection = ?, summary = ? "
                  "WHERE review_date = ?");
    query.addBindValue(reflection);
    query.addBindValue(summary);
    query.addBindValue(date);

    if (!query.exec())
    {
        return;
    }

    if (query.numRowsAffected() == 0)
    {
        query.prepare("INSERT INTO daily_review (review_date, reflection, summary) "
                      "VALUES (?, ?, ?)");
        query.addBindValue(date);
        query.addBindValue(reflection);
        query.addBindValue(summary);

        if (!query.exec())
        {
        }
    }
}
