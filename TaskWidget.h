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

        // Ввод задачи
        QHBoxLayout *inputLayout = new QHBoxLayout;
        taskInput = new QLineEdit;
        taskInput->setPlaceholderText("Введите новую задачу...");
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

        QWidget *container = new QWidget;
        taskLayout = new QVBoxLayout(container);
        taskLayout->setAlignment(Qt::AlignTop);
        container->setLayout(taskLayout);

        scrollArea->setWidget(container);
        mainLayout->addWidget(scrollArea);

        // Подключение кнопок
        connect(dateBtn, &QPushButton::clicked, this, &TaskWidget::openDatePopup);
        connect(tagBtn, &QPushButton::clicked, this, &TaskWidget::openTagPopup);
        connect(addBtn, &QPushButton::clicked, this, &TaskWidget::addTask);
    }

private:
    QLineEdit *taskInput;
    QScrollArea *scrollArea;
    QVBoxLayout *taskLayout;
    QString selectedDate;
    QString selectedTag;

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

        QFrame *taskFrame = new QFrame;
        taskFrame->setFrameShape(QFrame::Box);
        taskFrame->setStyleSheet("background-color: #2e2e2e; border-radius: 10px; padding: 10px;");
        QHBoxLayout *taskRow = new QHBoxLayout(taskFrame);

        QLabel *taskLabel = new QLabel(fullText);
        taskLabel->setStyleSheet("color: white; font-size: 16px;");
        taskRow->addWidget(taskLabel);

        QPushButton *removeBtn = new QPushButton("❌");
        removeBtn->setFixedSize(30, 30);
        taskRow->addWidget(removeBtn);

        taskLayout->addWidget(taskFrame);

        connect(removeBtn, &QPushButton::clicked, taskFrame, [=]() {
            taskLayout->removeWidget(taskFrame);
            taskFrame->deleteLater();
        });

        // Очистка
        taskInput->clear();
        selectedDate.clear();
        selectedTag.clear();
    }
};

#endif // TASKWIDGET_H
