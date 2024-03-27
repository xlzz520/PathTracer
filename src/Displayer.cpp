#include "Displayer.h"

Displayer::Displayer() : QWidget()
{

    // 标签
    sceneLabel.setParent(this);
    sceneLabel.setText("选择你要渲染的场景:");

    sceneGroup.setParent(this);
    sceneButton0.setParent(this);
    sceneButton0.setText("Cornell Box");
    sceneButton0.setChecked(true);
    sceneButton1.setParent(this);
    sceneButton1.setText("staircase");
    sceneButton2.setParent(this);
    sceneButton2.setText("veach-mis");
    sceneGroup.addButton(&sceneButton0,0);
    sceneGroup.addButton(&sceneButton1,1);
    sceneGroup.addButton(&sceneButton2,2);
    
    threshmethodLabel.setParent(this);
    threshmethodLabel.setText("阈值生成方法:");

    threshmethodGroup.setParent(this);
    threshmethodButton0.setParent(this);
    threshmethodButton0.setText("平等法");
    threshmethodButton0.setChecked(true);
    threshmethodButton1.setParent(this);
    threshmethodButton1.setText("高光抑制法");
    threshmethodGroup.addButton(&threshmethodButton0,0);
    threshmethodGroup.addButton(&threshmethodButton1,1);

    parameterLabel.setParent(this);
    parameterLabel.setText("参数设置:");

    validator.setParent(this);

    sppLabel.setParent(this);
    sppLabel.setText("Sample per Pixel:");
    sppEdit.setParent(this);
    sppEdit.setValidator(&validator);
    sppEdit.setText(QString::number(SAMPLE_PER_PIXEL));

    grid.addWidget(&sppLabel, 0, 0);
    grid.addWidget(&sppEdit, 0, 1);

    iterationLabel.setParent(this);
    iterationLabel.setText(QString("Iteration: 0"));

    timeLabel.setParent(this);
    timeLabel.setText(QString("Time(s):"));

    calculateButton.setParent(this);
    calculateButton.setText("Calculate");
    connect(&calculateButton, SIGNAL(pressed()), this, SLOT(calculate()));
    saveButton.setParent(this);
    saveButton.setText("Save");
    connect(&saveButton, SIGNAL(pressed()), this, SLOT(save()));

    vertical.addWidget(&sceneLabel);
    vertical.addWidget(&sceneButton0);
    vertical.addWidget(&sceneButton1);
    vertical.addWidget(&sceneButton2);

    vertical.addWidget(&threshmethodLabel);
    vertical.addWidget(&threshmethodButton0);
    vertical.addWidget(&threshmethodButton1);
    // vertical.addWidget(&threshmethodGroup);

    // vertical.addWidget(&sceneGroup);
    vertical.addWidget(&parameterLabel);
    vertical.addLayout(&grid);
    vertical.addWidget(&iterationLabel);
    vertical.addWidget(&timeLabel);
    vertical.addWidget(&calculateButton);
    vertical.addWidget(&saveButton);
    vertical.addStretch();

    imageLabel.setParent(this);
    horizontal.addWidget(&imageLabel);
    horizontal.addLayout(&vertical);
    
    resize(400,300);
    setLayout(&horizontal);
    setWindowTitle(QString("蒙特卡洛路径追踪器(陈宏亮，22221156)"));
        
}

Displayer::~Displayer() {}

void Displayer::refresh(const std::vector<std::vector<QVector3D>> &sum, const int sample)
{
    for (int i = 0; i < sum.size(); i++)
        for (int j = 0; j < sum[i].size(); j++)
            image.setPixel(i, j, vectorToColor(sum[i][j] / (float)sample).rgb());

    imageLabel.setPixmap(QPixmap::fromImage(image));
}

void Displayer::calculate()
{

    SAMPLE_PER_PIXEL = sppEdit.text().toInt();

    std::string objpath;
    if (sceneGroup.checkedButton() == &sceneButton0)
    {
        objpath = "../example-scenes-cg22/cornell-box/cornell-box.obj";
    }
    else if (sceneGroup.checkedButton() == &sceneButton1)
    {
        objpath = "../example-scenes-cg22/staircase/staircase.obj";
    }
    else if (sceneGroup.checkedButton() == &sceneButton2)
    {
        objpath = "../example-scenes-cg22/veach-mis/veach-mis.obj";
    }

    // 采样阈值生成方法
    bool threshold_method;
    if (threshmethodGroup.checkedButton() == &threshmethodButton0)
    {
        threshold_method = false;
    }
    else
    {
        threshold_method = true;
    }

    Scene scene(objpath, threshold_method);
    // 构建相机
    std::string xmlpath = objpath.substr(0, objpath.find_last_of('.')) + ".xml";
    tinyxml2::XMLDocument doc;
    doc.LoadFile(xmlpath.c_str());
    Camera cam(doc);

    int width = cam.getWidth();
    int height = cam.getHeight();

    std::vector<std::vector<QVector3D>> sum(width, std::vector<QVector3D>(height, QVector3D(0.0f, 0.0f, 0.0f)));
    image = QImage(width, height, QImage::Format_RGB32);
    spdlog::set_level(spdlog::level::trace);
    spdlog::info("开始采样生成图像");
    double start = cpuSecond();
    for (int i = 0; i < SAMPLE_PER_PIXEL; i++)
    {
        scene.sample(cam, sum);
        iterationLabel.setText(QString("Iteration: %1").arg(i + 1));
        double end = cpuSecond();
        timeLabel.setText(QString("Time: %1").arg(end - start));
        QCoreApplication::processEvents();
        refresh(sum, i + 1);
    }
    double end = cpuSecond();
    spdlog::info("采样生成图像完毕，共花费: {:.6f}s", end - start);
}

void Displayer::save()
{
    QString path = QFileDialog::getSaveFileName(this, "保存标题", ".", "PNG files (*.png)");
    image.save(path);
}
