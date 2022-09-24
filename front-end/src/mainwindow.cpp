#include <QtCore/QtDebug>
#include <QtCore/QTimer>
#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtGui/QPen>
#include <QtGui/QColor>
#include <QPainter>

#include "mainwindow.h"
#include "newButton.h"


static const QPen PEN_RED(QColor(0xFF, 0x55, 0x55));
static const QPen PEN_GREEN(QColor(0x55, 0xAA, 0x55));
static const QPen PEN_WHITE(QColor(0xCC, 0xCC, 0xCC));
static const QPen PEN_YELLOW(QColor(0xFF, 0xAA, 0x11));


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

    QWidget *centralWidget;

    if (this->objectName().isEmpty()) {
        this->setObjectName(QString::fromUtf8("MainWindow"));
    }
    centralWidget = new QWidget(this);
    centralWidget->setObjectName(QString::fromUtf8("CentralWidget"));
    this->setCentralWidget(centralWidget);

    /* Main windows property */
    setFixedSize(1280,720);
    setWindowIcon(QIcon("UNORes/icon_128x128.ico"));
    setWindowTitle("UNO");

    // setting button
    newButton *btnSetting = new newButton("SETTING", 100, 50);
    btnSetting->setParent(this);
    btnSetting->move(20, 650);
    btnSetting->setFont(QFont("Arial", 16, QFont::Bold));

    m_SettingWinPtr = new SettingWindow();
    m_GameWinPtr = new GameWindow();

    connect(m_SettingWinPtr, &SettingWindow::SIG_setting_win_back, this, [=]()
    {
        m_SettingWinPtr->hide();
        this->show();
    });

}


MainWindow::~MainWindow() {
    ; // Empty
}


void MainWindow::paintEvent(QPaintEvent *) {
    static QRect roi(0, 0, 0, 0);
    QImage pic;
    pic.load(":/UNORes/bg_welcome.png");
    roi.setWidth(this->width());
    roi.setHeight(this->height());
    QPainter(this).drawImage(roi, pic);
}


void MainWindow::mousePressEvent(QMouseEvent* event) {

}
