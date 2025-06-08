#ifndef TASKWIDGET_H
#define TASKWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QCalendarWidget>
#include <QInputDialog>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDate>
#include <QFrame>
#include <QHBoxLayout>
#include <QComboBox>
#include <QMap>

class TaskWidget : public QWidget {
    Q_OBJECT
public:
    TaskWidget(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        // Заголовок
        QLabel *title = new QLabel("📋 Задачи");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-size: 24px; font-weight: bold;");
        mainLayout->addWidget(title);

        // Фильтр по тегам (ComboBox)
        tagFilterCombo = new QComboBox;
        tagFilterCombo->addItem("Все теги");
        tagFilterCombo->setStyleSheet(
            "QComboBox {"
            "  font-size: 14px;"
            "  padding: 4px 8px;"
            "  background-color: #1e1e1e;"
            "  color: white;"
            "  border: 1px solid #555555;"
            "  border-radius: 6px;"
            "}"
            "QComboBox:hover { border-color: #0078d7; }"
            );
        mainLayout->addWidget(tagFilterCombo);
        connect(tagFilterCombo, &QComboBox::currentTextChanged, this, &TaskWidget::filterTasksByTag);

        // Ввод задачи
        QHBoxLayout *inputLayout = new QHBoxLayout;
        taskInput = new QLineEdit;
        taskInput->setPlaceholderText("Введите новую задачу...");
        taskInput->setStyleSheet(
            "QLineEdit {"
            "  background-color: #1e1e1e;"
            "  color: #ffffff;"
            "  font-size: 16px;"
            "  padding: 8px 12px;"
            "  border: 2px solid #3a3a3a;"
            "  border-radius: 8px;"
            "}"
            "QLineEdit:focus {"
            "  border-color: #0078d7;"
            "  background-color: #252526;"
            "}"
            "QLineEdit:hover {"
            "  border-color: #555555;"
            "}"
            );
        inputLayout->addWidget(taskInput);

        QPushButton *dateBtn = new QPushButton("📅");
        QPushButton *tagBtn = new QPushButton("🏷");
        QPushButton *addBtn = new QPushButton("➕");

        inputLayout->addWidget(dateBtn);
        inputLayout->addWidget(tagBtn);
        inputLayout->addWidget(addBtn);

        mainLayout->addLayout(inputLayout);

        // Область задач
        scrollArea = new QScrollArea;
        scrollArea->setWidgetResizable(true);

        containerWidget = new QWidget;
        taskLayout = new QVBoxLayout(containerWidget);
        taskLayout->setAlignment(Qt::AlignTop);
        containerWidget->setLayout(taskLayout);

        scrollArea->setWidget(containerWidget);
        mainLayout->addWidget(scrollArea);

        // Подключение кнопок
        connect(dateBtn, &QPushButton::clicked, this, &TaskWidget::openDatePopup);
        connect(tagBtn, &QPushButton::clicked, this, &TaskWidget::openTagPopup);
        connect(addBtn, &QPushButton::clicked, this, &TaskWidget::addTask);
    }

private:
    struct TaskItem {
        QString text;      // Текст задачи с датой и тегом
        QString date;
        QString tag;
        QFrame *frame;
        QLabel *label;
        QLineEdit *edit;
        QPushButton *editBtn;
        QPushButton *saveBtn;
        QPushButton *removeBtn;
    };

    QLineEdit *taskInput;
    QScrollArea *scrollArea;
    QWidget *containerWidget;
    QVBoxLayout *taskLayout;
    QString selectedDate;
    QString selectedTag;
    QComboBox *tagFilterCombo;

    QList<TaskItem> tasks;

    void openDatePopup() {
        QDialog dialog(this);
        dialog.setWindowTitle("Выберите дату");

        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        QCalendarWidget *calendar = new QCalendarWidget;
        layout->addWidget(calendar);

        QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        layout->addWidget(buttons);

        connect(buttons, &QDialogButtonBox::accepted, [&]() {
            selectedDate = calendar->selectedDate().toString("dd.MM.yyyy");
            dialog.accept();
        });
        connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        dialog.exec();
    }

    void openTagPopup() {
        bool ok;
        QString tag = QInputDialog::getText(this, "Введите тег", "Тег:", QLineEdit::Normal, "", &ok);
        if (ok && !tag.isEmpty()) {
            selectedTag = tag;
        }
    }

    void addTask() {
        QString text = taskInput->text().trimmed();
        if (text.isEmpty()) return;

        QString fullText = text;
        if (!selectedDate.isEmpty()) fullText += "  ⏰ " + selectedDate;
        if (!selectedTag.isEmpty()) fullText += "  🏷 " + selectedTag;

        addTaskItem(text, selectedDate, selectedTag, fullText);

        // Обновить список тегов для фильтра
        updateTagFilter();

        // Очистка
        taskInput->clear();
        selectedDate.clear();
        selectedTag.clear();

        // Обновить фильтр, чтобы сразу показать задачу, если фильтр не "Все теги"
        filterTasksByTag(tagFilterCombo->currentText());
    }

    void addTaskItem(const QString &text, const QString &date, const QString &tag, const QString &fullText) {
        QFrame *taskFrame = new QFrame;
        taskFrame->setFrameShape(QFrame::Box);
        taskFrame->setStyleSheet("background-color: #2e2e2e; border-radius: 10px; padding: 10px;");
        QHBoxLayout *taskRow = new QHBoxLayout(taskFrame);

        QLabel *taskLabel = new QLabel(fullText);
        taskLabel->setStyleSheet("color: white; font-size: 16px;");
        taskRow->addWidget(taskLabel);

        QLineEdit *taskEdit = new QLineEdit(fullText);
        taskEdit->setStyleSheet(
            "QLineEdit {"
            "  background-color: #1e1e1e;"
            "  color: #ffffff;"
            "  font-size: 16px;"
            "  padding: 6px 8px;"
            "  border: 1px solid #555555;"
            "  border-radius: 6px;"
            "}"
            );
        taskEdit->setVisible(false);
        taskRow->addWidget(taskEdit);

        QPushButton *editBtn = new QPushButton("✏️");
        QPushButton *saveBtn = new QPushButton("💾");
        QPushButton *removeBtn = new QPushButton("❌");

        saveBtn->setEnabled(false);
        saveBtn->setVisible(false);

        editBtn->setFixedSize(30,30);
        saveBtn->setFixedSize(30,30);
        removeBtn->setFixedSize(30,30);

        taskRow->addWidget(editBtn);
        taskRow->addWidget(saveBtn);
        taskRow->addWidget(removeBtn);

        taskLayout->addWidget(taskFrame);

        TaskItem item {text, date, tag, taskFrame, taskLabel, taskEdit, editBtn, saveBtn, removeBtn};
        tasks.append(item);

        // Редактирование
        connect(editBtn, &QPushButton::clicked, this, [=]() {
            taskLabel->setVisible(false);
            taskEdit->setVisible(true);
            taskEdit->setFocus();
            editBtn->setVisible(false);
            saveBtn->setVisible(true);
            saveBtn->setEnabled(true);
        });

        // Сохранение изменений
        connect(saveBtn, &QPushButton::clicked, this, [=]() {
            QString newText = taskEdit->text().trimmed();
            if (newText.isEmpty()) return;

            // Обновить label
            taskLabel->setText(newText);
            taskLabel->setVisible(true);
            taskEdit->setVisible(false);
            saveBtn->setVisible(false);
            saveBtn->setEnabled(false);
            editBtn->setVisible(true);

            // Парсим дату и тег из newText (упрощённо)
            // Например, ищем "🏷 " и "⏰ " и выделяем
            QString newDate, newTag;
            int tagPos = newText.indexOf("🏷");
            int datePos = newText.indexOf("⏰");

            if (tagPos != -1)
                newTag = newText.mid(tagPos + 2).trimmed();
            if (datePos != -1)
                newDate = newText.mid(datePos + 2, tagPos != -1 ? tagPos - datePos - 2 : newText.length()).trimmed();

            // Обновляем в списке
            for (TaskItem &t : tasks) {
                if (t.frame == taskFrame) {
                    t.text = newText;
                    t.date = newDate;
                    t.tag = newTag;
                    break;
                }
            }

            updateTagFilter();
            filterTasksByTag(tagFilterCombo->currentText());
        });

        // Удаление
        connect(removeBtn, &QPushButton::clicked, taskFrame, [=]() {
            taskLayout->removeWidget(taskFrame);
            taskFrame->deleteLater();
            // Удаляем из списка задач
            for (int i = 0; i < tasks.size(); ++i) {
                if (tasks[i].frame == taskFrame) {
                    tasks.removeAt(i);
                    break;
                }
            }
            updateTagFilter();
            filterTasksByTag(tagFilterCombo->currentText());
        });
    }

    void updateTagFilter() {
        QSet<QString> uniqueTags;
        for (const TaskItem &item : tasks) {
            if (!item.tag.isEmpty())
                uniqueTags.insert(item.tag);
        }

        QString currentSelection = tagFilterCombo->currentText();

        tagFilterCombo->blockSignals(true);
        tagFilterCombo->clear();
        tagFilterCombo->addItem("Все теги");
        for (const QString &tag : uniqueTags) {
            tagFilterCombo->addItem(tag);
        }
        tagFilterCombo->blockSignals(false);

        int index = tagFilterCombo->findText(currentSelection);
        if (index != -1)
            tagFilterCombo->setCurrentIndex(index);
        else
            tagFilterCombo->setCurrentIndex(0);
    }

    void filterTasksByTag(const QString &tag) {
        for (TaskItem &item : tasks) {
            bool visible = (tag == "Все теги") || (item.tag == tag);
            item.frame->setVisible(visible);
        }
    }
};

#endif // TASKWIDGET_H
