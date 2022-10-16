#include <QtCore/QtDebug>
#include <QtCore/QTimer>
#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QPen>
#include <QtGui/QColor>
#include <QPainter>

#include "mainwindow.h"
#include "newbutton.h"


static const QPen PEN_RED(QColor(0xFF, 0x55, 0x55));
static const QPen PEN_GREEN(QColor(0x55, 0xAA, 0x55));
static const QPen PEN_WHITE(QColor(0xCC, 0xCC, 0xCC));
static const QPen PEN_YELLOW(QColor(0xFF, 0xAA, 0x11));


MainWindow::MainWindow(int argc, char* argv[], QWidget *parent)
    : QMainWindow(parent) {
    
    QString message;
    int font_width;

    if (this->objectName().isEmpty()) {
        this->setObjectName(QString::fromUtf8("MainWindow"));
    }

    /* Main windows property */
    setFixedSize(1280,720);
    setWindowIcon(QIcon(":/UNORes/icon_128x128.ico"));
    setWindowTitle("UNO Welcome");

    m_screen = QImage(1280, 720, QImage::Format_RGB888);
    m_painterPtr = new QPainter(&m_screen);

    m_bg_welcome.load(":/UNORes/bg_welcome.png");
    m_bg_back.load(":/UNORes/back.png");
    m_painterPtr->drawImage(0, 0, m_bg_welcome);
    m_painterPtr->drawImage(580, 270, m_bg_back);

    /* draw message */
    QPen PEN_WHITE(QColor(0xCC, 0xCC, 0xCC));
    m_painterPtr->setFont(QFont("Arial", 18, QFont::Bold));
    m_painterPtr->setPen(PEN_WHITE);
    message = m_infoPtr->info_welcome();
    font_width = m_painterPtr->fontMetrics().horizontalAdvance(message);
    m_painterPtr->drawText(650 - font_width / 2, 487, message);
    
    /* SETTING button */
    NewButton *btnSetting = new NewButton("SETTING", 140, 60);
    btnSetting->setParent(this);
    btnSetting->move(20, 650);
    btnSetting->setFont(QFont("Arial", 18, QFont::ExtraBold));

    /* EXIT button */


    m_settingWinPtr = new SettingWindow(argc, argv, m_infoPtr);
    m_gameWinPtr = new GameWindow(argc, argv, m_infoPtr);
    m_infoPtr = Info::getInstance();

    connect(m_settingWinPtr, &SettingWindow::SIG_setting_win_back, this, [=]()
    {
        qDebug() << "return from setting window";
        m_settingWinPtr->hide();
        this->show();
    });


    connect(m_gameWinPtr, &GameWindow::SIG_enter_setting, this, [=]()
    {
        qDebug() << "enter setting window";
        btnSetting->btnPressed();
        btnSetting->btnReleased();
        QTimer::singleShot(100,this,[=](){
            m_gameWinPtr->hide();
            m_settingWinPtr->show();
            m_settingWinPtr->start();
        });
    });


    connect(btnSetting, &NewButton::clicked, [=]()
    {
        qDebug() << "enter setting window";
        btnSetting->btnPressed();
        btnSetting->btnReleased();
        QTimer::singleShot(100,this,[=](){
            this->hide();
            m_settingWinPtr->show();
            m_settingWinPtr->start();
        });
    });

    connect(this, &MainWindow::SIG_game_win_open, [=]()
    {
        qDebug() << "enter game window";
        QTimer::singleShot(100,this,[=](){
            this->hide();
            m_gameWinPtr->show();
            m_gameWinPtr->start();
        });
    });
    

}


MainWindow::~MainWindow() {
    ; // Empty
}


void MainWindow::refreshScreen() {
    QImage image;

    // Show screen
    update();
}


void MainWindow::paintEvent(QPaintEvent *) {
    static QRect roi(0, 0, 0, 0);

    roi.setWidth(this->width());
    roi.setHeight(this->height());
    QPainter(this).drawImage(roi, m_screen);
}


void MainWindow::mousePressEvent(QMouseEvent* event) {
    // Only response to left-click events, and ignore the others
    if (event->button() == Qt::LeftButton) {
        int x = 1280 * event->x() / this->width();
        int y = 720 * event->y() / this->height();
        if (270 <= y && y <= 450) {
            if (580 <= x && x <= 700) {
            // UNO button, start a new game
                emit SIG_game_win_open();
            }
        }
    }
}

