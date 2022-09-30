#include "settingwindow.h"

#include <QtGui/QMouseEvent>
#include <QPainter>


static const QPen PEN_RED(QColor(0xFF, 0x55, 0x55));
static const QPen PEN_GREEN(QColor(0x55, 0xAA, 0x55));
static const QPen PEN_WHITE(QColor(0xCC, 0xCC, 0xCC));
static const QPen PEN_YELLOW(QColor(0xFF, 0xAA, 0x11));

SettingWindow::SettingWindow(int argc, char* argv[], Info* info, QWidget *parent) 
    : m_unoPtr(nullptr), m_infoPtr(info), m_painterPtr(nullptr) {

    if (argc > 1) {
        unsigned seed = unsigned(atoi(argv[1]));
        m_unoPtr = UNO::getInstance(seed);
    }
    else {
        m_unoPtr = UNO::getInstance();
    }

    m_sndPtr = Sound::getInstance();

    /* Main windows property */
    setFixedSize(1280,720);
    setWindowIcon(QIcon(":/UNORes/icon_128x128.ico"));
    setWindowTitle("UNO Setting");

    m_screen = QImage(1280, 720, QImage::Format_RGB888);
    m_painterPtr = new QPainter(&m_screen);

    m_painterPtr->drawImage(0, 0, m_unoPtr->getBackgroundImage());
    m_painterPtr->setFont(QFont("Arial", 18, QFont::Bold));
    m_painterPtr->setPen(PEN_WHITE);

    /* back button */
    NewButton *btnBack = new NewButton("BACK", 100, 50);
    btnBack->setParent(this);
    btnBack->move(20, 20);
    btnBack->setFont(QFont("Arial", 18, QFont::ExtraBold));

    connect(btnBack, &NewButton::clicked, this, [=]()
    {
        emit SIG_setting_win_back();
    });
}

SettingWindow::~SettingWindow() {
    ; // Empty
}


void SettingWindow::start() {
    this->refreshScreen(m_infoPtr->info_ruleSettings());
}


void SettingWindow::refreshScreen(const QString& message) {
    QImage image;

    // clear screen
    m_painterPtr->drawImage(0, 0, m_unoPtr->getBackgroundImage());
    m_painterPtr->setPen(PEN_WHITE);

    // [Players] option: 3 / 4
    m_painterPtr->drawText(640, 350, m_infoPtr->label_players());
    image = m_unoPtr->getPlayers() == 3 ?
        m_unoPtr->findCard(GREEN, NUM3)->normalImg :
        m_unoPtr->findCard(GREEN, NUM3)->darkImg;
    m_painterPtr->drawImage(790, 250, image);
    image = m_unoPtr->getPlayers() == 4 ?
        m_unoPtr->findCard(YELLOW, NUM4)->normalImg :
        m_unoPtr->findCard(YELLOW, NUM4)->darkImg;
    m_painterPtr->drawImage(970, 250, image);

    // Rule settings
    // Force play switch
    m_painterPtr->drawText(60, 540, m_infoPtr->label_forcePlay());
    m_painterPtr->setPen(m_unoPtr->isForcePlay() ? PEN_WHITE : PEN_RED);
    m_painterPtr->drawText(790, 540, m_infoPtr->btn_keep());
    m_painterPtr->setPen(m_unoPtr->isForcePlay() ? PEN_GREEN : PEN_WHITE);
    m_painterPtr->drawText(970, 540, m_infoPtr->btn_play());
    m_painterPtr->setPen(PEN_WHITE);

    // 7-0
    m_painterPtr->drawText(60, 590, m_infoPtr->label_7_0());
    m_painterPtr->setPen(m_unoPtr->isSevenZeroRule() ? PEN_WHITE : PEN_RED);
    m_painterPtr->drawText(790, 590, m_infoPtr->btn_off());
    m_painterPtr->setPen(m_unoPtr->isSevenZeroRule() ? PEN_GREEN : PEN_WHITE);
    m_painterPtr->drawText(970, 590, m_infoPtr->btn_on());
    m_painterPtr->setPen(PEN_WHITE);

    // +2 stack
    m_painterPtr->drawText(60, 640, m_infoPtr->label_draw2Stack());
    m_painterPtr->setPen(m_unoPtr->isDraw2StackRule() ? PEN_WHITE : PEN_RED);
    m_painterPtr->drawText(790, 640, m_infoPtr->btn_off());
    m_painterPtr->setPen(m_unoPtr->isDraw2StackRule() ? PEN_GREEN : PEN_WHITE);
    m_painterPtr->drawText(970, 640, m_infoPtr->btn_on());
    m_painterPtr->setPen(PEN_WHITE);

    update();
}


void SettingWindow::paintEvent(QPaintEvent *) {
    static QRect roi(0, 0, 0, 0);

    roi.setWidth(this->width());
    roi.setHeight(this->height());
    QPainter(this).drawImage(roi, m_screen);
}


void SettingWindow::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        // Only response to left-click events, and ignore the others
        int x = 1280 * event->x() / this->width();
        int y = 720 * event->y() / this->height();

        // Do special behaviors when configuring game options
        if (270 <= y && y <= 450) {
            if (150 <= x && x <= 270) {
                // SND OFF button
                m_sndPtr->setEnabled(false);
                this->refreshScreen();
            }
            else if (330 <= x && x <= 450) {
                // SND ON button
                m_sndPtr->setEnabled(true);
                m_sndPtr->play(Sound::SND_PLAY);
                this->refreshScreen();
            }
            else if (790 <= x && x <= 910) {
                // 3 players
                m_unoPtr->setPlayers(3);
                this->refreshScreen();
            }
            else if (970 <= x && x <= 1090) {
                // 4 players
                m_unoPtr->setPlayers(4);
                this->refreshScreen();
            }
        }
        else if (519 <= y && y <= 540) {
            if (800 <= x && x <= 927) {
                // Force play, <KEEP> button
                m_unoPtr->setForcePlay(false);
                this->refreshScreen();
            }
            else if (980 <= x && x <= 1104) {
                // Force play, <PLAY> button
                m_unoPtr->setForcePlay(true);
                this->refreshScreen();
            }
        }
        else if (569 <= y && y <= 590) {
            if (800 <= x && x <= 906) {
                // 7-0, <OFF> button
                m_unoPtr->setSevenZeroRule(false);
                this->refreshScreen();
            }
            else if (980 <= x && x <= 1072) {
                // 7-0, <ON> button
                m_unoPtr->setSevenZeroRule(true);
                this->refreshScreen();
            }
        }
        else if (619 <= y && y <= 640) {
            if (800 <= x && x <= 906) {
                // +2 stack, <OFF> button
                m_unoPtr->setDraw2StackRule(false);
                this->refreshScreen();
            }
            else if (980 <= x && x <= 1072) {
                // +2 stack, <ON> button
                m_unoPtr->setDraw2StackRule(true);
                this->refreshScreen();
            }
        }
    }
}
