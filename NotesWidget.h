// NotesWidget.h
#ifndef NOTESWIDGET_H
#define NOTESWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>  // 🔧 Добавляем это


class NotesWidget : public QWidget {
    Q_OBJECT
public:
    NotesWidget(QWidget *parent = nullptr) {
        QVBoxLayout *layout = new QVBoxLayout;
        QLabel *title = new QLabel("📝 Заметки");
        title->setAlignment(Qt::AlignCenter);

        QTextEdit *noteArea = new QTextEdit;
        noteArea->setPlaceholderText("Введите текст заметки...");

        QPushButton *addImageBtn = new QPushButton("📎 Добавить изображение (PNG/JPEG)");
        connect(addImageBtn, &QPushButton::clicked, [=]() {
            QString fileName = QFileDialog::getOpenFileName(this, "Выберите изображение", "", "Images (*.png *.jpg *.jpeg)");
            if (!fileName.isEmpty()) {
                QFile file(fileName);
                if (file.size() > 5 * 1024 * 1024) {
                    QMessageBox::warning(this, "Ошибка", "Файл превышает 5 МБ");
                    return;
                }
                // Пример вставки пути в текст заметки
                noteArea->append("<img src='" + fileName + "' width='200' />");
            }
        });

        layout->addWidget(title);
        layout->addWidget(noteArea);
        layout->addWidget(addImageBtn);
        layout->addStretch();
        setLayout(layout);
    }
};

#endif // NOTESWIDGET_H
