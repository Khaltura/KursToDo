// CalendarWidget.h
#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QCalendarWidget>

class CalendarWidget : public QWidget {
    Q_OBJECT
public:
    CalendarWidget(QWidget *parent = nullptr) {
        QVBoxLayout *layout = new QVBoxLayout;
        QLabel *title = new QLabel("📅 Календарь");
        title->setAlignment(Qt::AlignCenter);

        QCalendarWidget *calendar = new QCalendarWidget;

        layout->addWidget(title);
        layout->addWidget(calendar);
        layout->addStretch();
        setLayout(layout);
    }
};

#endif // CALENDARWIDGET_H
