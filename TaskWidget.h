#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
#include <QAction>
#include <QSqlQuery>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDateEdit>

#include "DatabaseManager.h"

class TaskWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TaskWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        dbManager.openDatabase("tasks_notes.db");
        dbManager.createTables();

        auto *mainLayout = new QVBoxLayout(this);

        input = new QLineEdit(this);
        input->setPlaceholderText("Введите новую задачу");

        dateEdit = new QDateEdit(QDate::currentDate(), this);
        dateEdit->setCalendarPopup(true);

        tagBox = new QComboBox(this);
        tagBox->addItems({"Без тега", "Учеба", "Работа", "Личное"});

        addBtn = new QPushButton("Добавить", this);

        auto *inputLayout = new QHBoxLayout();
        inputLayout->addWidget(input);
        inputLayout->addWidget(dateEdit);
        inputLayout->addWidget(tagBox);
        inputLayout->addWidget(addBtn);

        list = new QListWidget(this);
        list->setContextMenuPolicy(Qt::CustomContextMenu);

        mainLayout->addLayout(inputLayout);
        mainLayout->addWidget(list);

        connect(addBtn, &QPushButton::clicked, this, &TaskWidget::addTask);
        connect(list, &QListWidget::customContextMenuRequested, this, &TaskWidget::showContextMenu);

        loadTasks();
    }

private slots:
    void addTask()
    {
        QString taskText = input->text().trimmed();
        if (taskText.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Введите текст задачи");
            return;
        }

        QString dateStr = dateEdit->date().toString("yyyy-MM-dd");
        QString tag = tagBox->currentText();

        if (!dbManager.addTask(taskText, dateStr, tag)) {
            QMessageBox::warning(this, "Ошибка", "Не удалось сохранить задачу в базу данных");
            return;
        }

        loadTasks();

        input->clear();
        dateEdit->setDate(QDate::currentDate());
        tagBox->setCurrentIndex(0);
    }

    void loadTasks()
    {
        list->clear();
        QSqlQuery query = dbManager.getAllTasks();
        while (query.next()) {
            int id = query.value("id").toInt();
            QString text = query.value("text").toString();
            QString date = query.value("date").toString();
            QString tag = query.value("tag").toString();

            QString itemText = QString("%1 [%2] {%3}").arg(text, date, tag);
            QListWidgetItem *item = new QListWidgetItem(itemText);
            item->setData(Qt::UserRole, id);
            list->addItem(item);
        }
    }

    void showContextMenu(const QPoint &pos)
    {
        QListWidgetItem *item = list->itemAt(pos);
        if (!item)
            return;

        QMenu contextMenu;
        QAction *editAction = contextMenu.addAction("Редактировать");
        QAction *deleteAction = contextMenu.addAction("Удалить");

        QAction *selectedAction = contextMenu.exec(list->viewport()->mapToGlobal(pos));
        if (selectedAction == editAction) {
            editTask(item);
        } else if (selectedAction == deleteAction) {
            deleteTask(item);
        }
    }

    void editTask(QListWidgetItem *item)
    {
        int id = item->data(Qt::UserRole).toInt();

        QSqlQuery query = dbManager.getTaskById(id);
        if (!query.next()) {
            QMessageBox::warning(this, "Ошибка", "Задача не найдена в базе");
            return;
        }

        QString currentText = query.value("text").toString();
        QString currentDateStr = query.value("date").toString();
        QString currentTag = query.value("tag").toString();

        bool ok;
        QString newText = QInputDialog::getText(this, "Редактировать задачу",
                                                "Текст задачи:", QLineEdit::Normal,
                                                currentText, &ok);
        if (!ok || newText.trimmed().isEmpty())
            return;

        QDate currentDate = QDate::fromString(currentDateStr, "yyyy-MM-dd");
        QDate newDate = getDateFromDialog(this, "Редактировать дату задачи", currentDate);
        if (!newDate.isValid())
            return; // Отмена

        QStringList tags = {"Без тега", "Учеба", "Работа", "Личное"};
        QString newTag = QInputDialog::getItem(this, "Редактировать задачу",
                                               "Тег:", tags, tags.indexOf(currentTag), false, &ok);
        if (!ok)
            return;

        if (!dbManager.updateTask(id, newText.trimmed(), newDate.toString("yyyy-MM-dd"), newTag)) {
            QMessageBox::warning(this, "Ошибка", "Не удалось обновить задачу");
            return;
        }

        loadTasks();
    }

    void deleteTask(QListWidgetItem *item)
    {
        int id = item->data(Qt::UserRole).toInt();

        if (QMessageBox::question(this, "Удаление", "Удалить задачу?") == QMessageBox::Yes) {
            if (!dbManager.deleteTask(id)) {
                QMessageBox::warning(this, "Ошибка", "Не удалось удалить задачу");
                return;
            }
            loadTasks();
        }
    }

private:
    // Функция для выбора даты с диалогом
    QDate getDateFromDialog(QWidget *parent, const QString &title, const QDate &currentDate)
    {
        QDialog dialog(parent);
        dialog.setWindowTitle(title);

        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        QDateEdit *dateEditDialog = new QDateEdit(currentDate, &dialog);
        dateEditDialog->setCalendarPopup(true);
        layout->addWidget(dateEditDialog);

        QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                         Qt::Horizontal, &dialog);
        layout->addWidget(buttons);

        QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted) {
            return dateEditDialog->date();
        } else {
            return QDate(); // invalid date — отмена
        }
    }

private:
    QLineEdit *input;
    QDateEdit *dateEdit;
    QComboBox *tagBox;
    QPushButton *addBtn;
    QListWidget *list;

    DatabaseManager dbManager;
};
