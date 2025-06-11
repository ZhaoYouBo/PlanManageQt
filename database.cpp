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
        "created_date DATE, "
        "due_date DATE, "
        "completed_date DATE, "
        "status INTEGER DEFAULT 0"
        ")"
    );

    // Daily plan table
    query.exec(
        "CREATE TABLE IF NOT EXISTS daily_plan ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "task_id INTEGER NOT NULL, "
        "plan_date DATE NOT NULL, "
        "sort_order INTEGER, "
        "status INTEGER DEFAULT 0, "
        "FOREIGN KEY(task_id) REFERENCES task(id) ON DELETE CASCADE ON UPDATE CASCADE"
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

QList<Task> Database::getTaskAll()
{
    QList<Task> taskData;

    QSqlQuery query("SELECT name, created_date, due_date where status = 1;");

    return taskData;
}
