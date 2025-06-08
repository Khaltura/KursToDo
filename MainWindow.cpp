// MainWindow.cpp
#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QWidget *sidePanel = new QWidget;
    sidePanel->setObjectName("SidePanel");
    QVBoxLayout *sideLayout = new QVBoxLayout;
    sideLayout->setContentsMargins(10, 10, 10, 10);
    sideLayout->setSpacing(10);

    taskButton = new QPushButton("📋 Tasks");
    calendarButton = new QPushButton("📅 Calendar");
    notesButton = new QPushButton("📝 Notes");

    sideLayout->addWidget(taskButton);
    sideLayout->addWidget(calendarButton);
    sideLayout->addWidget(notesButton);
    sideLayout->addStretch();
    sidePanel->setLayout(sideLayout);
    sidePanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    stackedWidget = new QStackedWidget;
    stackedWidget->addWidget(new TaskWidget);
    stackedWidget->addWidget(new CalendarWidget);
    stackedWidget->addWidget(new NotesWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(sidePanel);
    mainLayout->addWidget(stackedWidget);
    centralWidget->setLayout(mainLayout);

    connect(taskButton, &QPushButton::clicked, [=](){ stackedWidget->setCurrentIndex(0); });
    connect(calendarButton, &QPushButton::clicked, [=](){ stackedWidget->setCurrentIndex(1); });
    connect(notesButton, &QPushButton::clicked, [=](){ stackedWidget->setCurrentIndex(2); });
}
