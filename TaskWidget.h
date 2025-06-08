// TaskWidget.h
#ifndef TASKWIDGET_H
#define TASKWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class TaskWidget : public QWidget {
    Q_OBJECT
public:
    TaskWidget(QWidget *parent = nullptr) {
        QVBoxLayout *layout = new QVBoxLayout;
        QLabel *title = new QLabel("📋 Задачи");
        title->setAlignment(Qt::AlignCenter);

        QLineEdit *input = new QLineEdit;
        input->setPlaceholderText("Введите новую задачу...");
        QPushButton *addBtn = new QPushButton("➕ Добавить задачу");

        layout->addWidget(title);
        layout->addWidget(input);
        layout->addWidget(addBtn);
        layout->addStretch();

        setLayout(layout);
    }
};

#endif // TASKWIDGET_H
