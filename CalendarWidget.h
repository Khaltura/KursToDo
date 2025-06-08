#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QCalendarWidget>
#include <QDialog>
#include <QTextBrowser>
#include <QPushButton>
#include "DatabaseManager.h"

class CalendarWidget : public QWidget {
    Q_OBJECT
public:
    CalendarWidget(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *layout = new QVBoxLayout(this);
        QLabel *title = new QLabel("📅 Календарь");
        title->setAlignment(Qt::AlignCenter);

        calendar = new QCalendarWidget;
        layout->addWidget(title);
        layout->addWidget(calendar);
        layout->addStretch();

        setLayout(layout);

        connect(calendar, &QCalendarWidget::clicked, this, &CalendarWidget::showTasksForDate);

        dbManager = new DatabaseManager(this);
        dbManager->openDatabase("tasks.db"); // укажите путь к вашей БД
    }

private slots:
    void showTasksForDate(const QDate &date) {
        QString dateString = date.toString("yyyy-MM-dd");
        QSqlQuery query = dbManager->getAllTasks();

        QStringList tasks;
        while (query.next()) {
            QString taskDate = query.value("date").toString();
            if (taskDate == dateString) {
                QString text = query.value("text").toString();
                QString tag = query.value("tag").toString();
                tasks << QString("📌 %1 [%2]").arg(text, tag);
            }
        }

        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle("Задачи на " + date.toString("dd.MM.yyyy"));
        dialog->resize(400, 300);

        QVBoxLayout *dialogLayout = new QVBoxLayout(dialog);
        QTextBrowser *browser = new QTextBrowser;
        if (tasks.isEmpty()) {
            browser->setText("Нет задач на эту дату.");
        } else {
            browser->setText(tasks.join("\n\n"));
        }

        QPushButton *closeBtn = new QPushButton("Закрыть");
        connect(closeBtn, &QPushButton::clicked, dialog, &QDialog::accept);

        dialogLayout->addWidget(browser);
        dialogLayout->addWidget(closeBtn);
        dialog->exec();
    }

private:
    QCalendarWidget *calendar;
    DatabaseManager *dbManager;
};

#endif // CALENDARWIDGET_H
