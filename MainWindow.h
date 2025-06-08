// MainWindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QWidget>
#include "TaskWidget.h"
#include "CalendarWidget.h"
#include "NotesWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:
    QStackedWidget *stackedWidget;
    QPushButton *taskButton;
    QPushButton *calendarButton;
    QPushButton *notesButton;
};

#endif // MAINWINDOW_H
