// gui_main.h
#pragma once
#include <QWidget>

class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow();
private:
    QString currentImagePath;
    void uploadImage();
    void encodeMessage();
    void decodeMessage();
};
