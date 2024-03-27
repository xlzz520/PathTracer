#ifndef RENDER_WIDGET_H
#define RENDER_WIDGET_H

#include <vector>

#include <QVector3D>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QLineEdit>
#include <QIntValidator>
#include <QButtonGroup>
#include <QPushButton>
#include <QString>
#include <QImage>
#include <QCoreApplication>
#include <QPixmap>
#include <QFileDialog>

#include <tinyxml2/tinyxml2.h>

#include "ConfigHelper.h"
#include "Scene.h"

class Displayer : public QWidget {
    Q_OBJECT
private:
    QVBoxLayout vertical;
    QHBoxLayout horizontal;
    
    QGridLayout grid;
    //标签
    QLabel sceneLabel, threshmethodLabel, parameterLabel, sppLabel, timeLabel, iterationLabel, imageLabel;
    //勾选框
    QButtonGroup sceneGroup,threshmethodGroup;
    QRadioButton sceneButton0, sceneButton1, sceneButton2, threshmethodButton0, threshmethodButton1;
    //编辑框
    QLineEdit sppEdit, iprEdit;
    //监听器
    QIntValidator validator;
    QPushButton calculateButton, saveButton;
    Scene scene;
    QImage image;
    void refresh(const std::vector<std::vector<QVector3D>> &sum, const int sample);


public:
    Displayer();
    ~Displayer() override;

private slots:
    void calculate();
    void save();
};

#endif