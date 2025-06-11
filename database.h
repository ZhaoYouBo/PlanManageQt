#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QDate>

struct Task {
    int id; // Primary key
    QString name; // Task name
    QDate createdDate; // Created date
    QDate dueDate; // Due date
    QDate completedDate; // Completed date
    int status; // Task status
};
class Database
{
public:
    Database(const QString& dbName);

private:
    QSqlDatabase m_db;

    /**
     * @brief createTables Creates core database tables if they don't exist
     */
    void createTables();

    QList<Task> getTaskAll();
};

#endif // DATABASE_H
