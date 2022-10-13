#include "gamewindow.h"
#include "newbutton.h"

#include <QtGui/QPen>
#include <QtGui/QMouseEvent>
#include <QtCore/QThread>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QPainter>

static const QPen PEN_RED(QColor(0xFF, 0x55, 0x55));
static const QPen PEN_GREEN(QColor(0x55, 0xAA, 0x55));
static const QPen PEN_WHITE(QColor(0xCC, 0xCC, 0xCC));
static const QPen PEN_YELLOW(QColor(0xFF, 0xAA, 0x11));

static const QBrush BRUSH_RED(QColor(0xFF, 0x55, 0x55));
static const QBrush BRUSH_BLUE(QColor(0x55, 0x55, 0xFF));
static const QBrush BRUSH_GREEN(QColor(0x55, 0xAA, 0x55));
static const QBrush BRUSH_YELLOW(QColor(0xFF, 0xAA, 0x11));

GameWindow::GameWindow(int argc, char* argv[], Info* info, QWidget *parent)
    : m_unoPtr(nullptr), m_infoPtr(info), m_soundPtr(nullptr), m_AI_Ptr(nullptr), m_painterPtr(nullptr) {
    if (argc > 1) {
        unsigned seed = unsigned(atoi(argv[1]));
        m_unoPtr = UNO::getInstance(seed);
    }
    else {
        m_unoPtr = UNO::getInstance();
    }
    
    m_soundPtr = Sound::getInstance();
    m_AI_Ptr = AI::getInstance();

    /* Game windows property */
    setFixedSize(1280,720);
    setWindowIcon(QIcon(":/UNORes/icon_128x128.ico"));
    setWindowTitle("UNO Game");

    m_selectedIndex = -1;
    m_hideFlag = 0x00;
    m_isAIRunning = false;
    m_winner = Player::YOU;

    for (int i = 0; i <= 3; ++i) {
        m_backup[i] = QImage(1280, 720, QImage::Format_RGB888);
        m_bkPainterList[i] = new QPainter(&m_backup[i]);
    }
    m_screen = QImage(1280, 720, QImage::Format_RGB888);
    m_painterPtr = new QPainter(&m_screen);
    m_painterPtr->setPen(PEN_WHITE);
    m_painterPtr->setFont(QFont("Arial", 20));

    /* SETTING button */
    NewButton *btnSetting = new NewButton("SETTING", 120, 60);
    btnSetting->setParent(this);
    btnSetting->move(20, 650);
    btnSetting->setFont(QFont("Arial", 18, QFont::ExtraBold));

    connect(this, &GameWindow::SIG_new_game, btnSetting, [=]()
    {
        btnSetting->setVisible(false);
    });


    connect(this, &GameWindow::SIG_game_over, btnSetting, [=]()
    {
        btnSetting->setVisible(true);
    });


    connect(btnSetting, &NewButton::clicked, this, [=]()
    {
        emit SIG_enter_setting();
    });

}

GameWindow::~GameWindow() {
    ; // Empty
}


void GameWindow::start() {
    emit SIG_new_game();
    this->updateStatus(STAT_NEW_GAME);
}


void GameWindow::hardAI() {
    int idxBest;
    Color bestColor[1];

    if (!m_isAIRunning) {
        m_isAIRunning = true;
        while (m_status == Player::COM1
            || m_status == Player::COM2
            || m_status == Player::COM3) {
            updateStatus(STAT_IDLE); // block mouse click events when idle
            idxBest = m_AI_Ptr->hardAI_bestCardIndex4NowPlayer(bestColor);
            if (idxBest >= 0) {
                // Found an appropriate card to play
                play(idxBest, bestColor[0]);
            }
            else {
                // No appropriate cards to play, or no card to play
                draw();
            }
        }

        m_isAIRunning = false;
    }
} // hardAI()


void GameWindow::sevenZeroAI() {
    int idxBest;
    Color bestColor[1];

    if (!m_isAIRunning) {
        m_isAIRunning = true;
        while (m_status == Player::COM1
            || m_status == Player::COM2
            || m_status == Player::COM3) {
            updateStatus(STAT_IDLE); // block mouse click events when idle
            idxBest = m_AI_Ptr->sevenZeroAI_bestCardIndex4NowPlayer(bestColor);
            if (idxBest >= 0) {
                // Found an appropriate card to play
                play(idxBest, bestColor[0]);
            }
            else {
                // No appropriate cards to play, or no card to play
                draw();
            }
        }

        m_isAIRunning = false;
    }
}


void GameWindow::play(int index, Color color) {
    Card* card;
    AnimateLayer layer[1];
    int c, now, size, recentSize, next;

    updateStatus(STAT_IDLE); // block mouse click events when idle
    now = m_unoPtr->getNow();
    size = m_unoPtr->getCurrPlayer()->getHandSize();
    card = m_unoPtr->play(now, index, color);
    m_selectedIndex = -1;
    m_soundPtr->play(Sound::SND_PLAY);
    if (card != nullptr) {
        layer[0].elem = card->normalImg;
        switch (now) {
        case Player::COM1:
            layer[0].startLeft = 160;
            layer[0].startTop = 290 - 20 * size + 40 * index;
            break; // case Player::COM1

        case Player::COM2:
            layer[0].startLeft = (1205 - 45 * size + 90 * index) / 2;
            layer[0].startTop = 50;
            break; // case Player::COM2

        case Player::COM3:
            layer[0].startLeft = 1000;
            layer[0].startTop = 290 - 20 * size + 40 * index;
            break; // case Player::COM3

        default:
            layer[0].startLeft = (1205 - 45 * size + 90 * index) / 2;
            layer[0].startTop = 500;
            break; // default
        } // switch (now)

        recentSize = int(m_unoPtr->getRecent().size());
        layer[0].endLeft = (45 * recentSize + 1419) / 2;
        layer[0].endTop = 270;
        animate(1, layer);
        if (size == 1) {
            // The player in action becomes winner when it played the
            // final card in its hand successfully
            if (now == Player::YOU) {
                m_soundPtr->play(Sound::SND_WIN);
            }
            else {
                m_soundPtr->play(Sound::SND_LOSE);
            }

            m_winner = now;
            updateStatus(STAT_GAME_OVER);
        } // if (size == 1)
        else {
            // When the played card is an action card or a wild card,
            // do the necessary things according to the game rule
            if (size == 2) {
                m_soundPtr->play(Sound::SND_UNO);
            } // if (size == 2)

            switch (card->content) {
            case DRAW2:
                next = m_unoPtr->switchNow();
                if (m_unoPtr->isDraw2StackRule()) {
                    c = m_unoPtr->getDraw2StackCount();
                    refreshScreen(m_infoPtr->act_playDraw2(now, next, c));
                    threadWait(1500);
                    updateStatus(next);
                } // if (m_unoPtr->isDraw2StackRule())
                else {
                    refreshScreen(m_infoPtr->act_playDraw2(now, next, 2));
                    threadWait(1500);
                    draw(2, /* force */ true);
                } // else
                break; // case DRAW2

            case SKIP:
                next = m_unoPtr->switchNow();
                refreshScreen(m_infoPtr->act_playSkip(now, next));
                threadWait(1500);
                updateStatus(m_unoPtr->switchNow());
                break; // case SKIP

            case REV:
                m_unoPtr->switchDirection();
                refreshScreen(m_infoPtr->act_playRev(now));
                threadWait(1500);
                updateStatus(m_unoPtr->switchNow());
                break; // case REV

            case WILD:
                refreshScreen(m_infoPtr->act_playWild(now, color));
                threadWait(1500);
                updateStatus(m_unoPtr->switchNow());
                break; // case WILD

            case WILD_DRAW4:
                next = m_unoPtr->getNext();
                refreshScreen(m_infoPtr->act_playWildDraw4(now, next));
                threadWait(1500);
                updateStatus(STAT_DOUBT_WILD4);
                break; // case WILD_DRAW4

            case NUM7:
                if (m_unoPtr->isSevenZeroRule()) {
                    refreshScreen(m_infoPtr->act_playCard(now, card->name));
                    threadWait(750);
                    updateStatus(STAT_SEVEN_TARGET);
                    break; // case NUM7
                }
                // else fall through

            case NUM0:
                if (m_unoPtr->isSevenZeroRule()) {
                    refreshScreen(m_infoPtr->act_playCard(now, card->name));
                    threadWait(750);
                    cycle();
                    break; // case NUM0
                }
                // else fall through

            default:
                refreshScreen(m_infoPtr->act_playCard(now, card->name));
                threadWait(1500);
                updateStatus(m_unoPtr->switchNow());
                break; // default
            }
        }
    }
}


void GameWindow::draw(int count, bool force) {
    Card* drawn;
    QString message;
    AnimateLayer layer[1];
    int i, index, c, now, size;

    updateStatus(STAT_IDLE); // block mouse click events when idle
    c = m_unoPtr->getDraw2StackCount();
    if (c > 0) {
        count = c;
        force = true;
    } 

    index = -1;
    drawn = nullptr;
    now = m_unoPtr->getNow();
    m_selectedIndex = -1;
    for (i = 0; i < count; ++i) {
        index = m_unoPtr->draw(now, force);
        if (index >= 0) {
            drawn = m_unoPtr->getCurrPlayer()->getHandCards().at(index);
            size = m_unoPtr->getCurrPlayer()->getHandSize();
            layer[0].startLeft = 338;
            layer[0].startTop = 270;
            switch (now) {
            case Player::COM1:
                layer[0].elem = m_unoPtr->getBackImage();
                layer[0].endLeft = 20;
                layer[0].endTop = 290 - 20 * size + 40 * index;
                message = m_infoPtr->act_drawCardCount(now, count);
                break; // case Player::COM1

            case Player::COM2:
                layer[0].elem = m_unoPtr->getBackImage();
                layer[0].endLeft = (1205 - 45 * size + 90 * index) / 2;
                layer[0].endTop = 20;
                message = m_infoPtr->act_drawCardCount(now, count);
                break; // case Player::COM2

            case Player::COM3:
                layer[0].elem = m_unoPtr->getBackImage();
                layer[0].endLeft = 1140;
                layer[0].endTop = 290 - 20 * size + 40 * index;
                message = m_infoPtr->act_drawCardCount(now, count);
                break; // case Player::COM3

            default:
                layer[0].elem = drawn->normalImg;
                layer[0].endLeft = (1205 - 45 * size + 90 * index) / 2;
                layer[0].endTop = 520;
                message = m_infoPtr->act_drawCard(now, drawn->name);
                break;
            }

            m_soundPtr->play(Sound::SND_DRAW);
            animate(1, layer);
            refreshScreen(message);
            threadWait(300);
        }
        else {
            message = m_infoPtr->info_cannotDraw(now, UNO::MAX_HOLD_CARDS);
            refreshScreen(message);
            break;
        }
    }

    threadWait(750);
    if (count == 1 &&
        drawn != nullptr &&
        m_unoPtr->isForcePlay() &&
        m_unoPtr->isLegalToPlay(drawn)) {
        // Player drew one card by itself, the drawn card
        // can be played immediately if it's legal to play
        if (!drawn->isWild()) {
            play(index);
        }
        else if (now != Player::YOU) {
            play(index, m_AI_Ptr->calcBestColor4NowPlayer());
        }
        else {
            // Store index value as global value. This value
            // will be used after the wild color determined.
            m_selectedIndex = index;
            updateStatus(STAT_WILD_COLOR);
        }
    }
    else {
        refreshScreen(m_infoPtr->act_pass(now));
        threadWait(750);
        updateStatus(m_unoPtr->switchNow());
    }
}


void GameWindow::swapWith(int whom) {
    static int x[] = { 580, 160, 580, 1000 };
    static int y[] = { 490, 270, 50, 270 };
    AnimateLayer layer[2];
    int curr;

    updateStatus(STAT_IDLE);
    curr = m_unoPtr->getNow();
    m_hideFlag = (1 << curr) | (1 << whom);
    refreshScreen(m_infoPtr->info_7_swap(curr, whom));
    layer[0].elem = layer[1].elem = m_unoPtr->getBackImage();
    layer[0].startLeft = x[curr];
    layer[0].startTop = y[curr];
    layer[0].endLeft = x[whom];
    layer[0].endTop = y[whom];
    layer[1].startLeft = x[whom];
    layer[1].startTop = y[whom];
    layer[1].endLeft = x[curr];
    layer[1].endTop = y[curr];
    animate(2, layer);
    m_hideFlag = 0x00;
    m_unoPtr->swap(curr, whom);
    refreshScreen(m_infoPtr->info_7_swap(curr, whom));
    threadWait(1500);
    updateStatus(m_unoPtr->switchNow());
}


void GameWindow::cycle() {
    static int x[] = { 580, 160, 580, 1000 };
    static int y[] = { 490, 270, 50, 270 };
    int curr, next, oppo, prev;
    AnimateLayer layer[4];

    updateStatus(STAT_IDLE);
    m_hideFlag = 0x0f;
    refreshScreen(m_infoPtr->info_0_rotate());
    curr = m_unoPtr->getNow();
    next = m_unoPtr->getNext();
    oppo = m_unoPtr->getOppo();
    prev = m_unoPtr->getPrev();
    layer[0].elem = m_unoPtr->getBackImage();
    layer[0].startLeft = x[curr];
    layer[0].startTop = y[curr];
    layer[0].endLeft = x[next];
    layer[0].endTop = y[next];
    layer[1].elem = m_unoPtr->getBackImage();
    layer[1].startLeft = x[next];
    layer[1].startTop = y[next];
    layer[1].endLeft = x[oppo];
    layer[1].endTop = y[oppo];
    if (m_unoPtr->getPlayers() == 3) {
        layer[2].elem = m_unoPtr->getBackImage();
        layer[2].startLeft = x[oppo];
        layer[2].startTop = y[oppo];
        layer[2].endLeft = x[curr];
        layer[2].endTop = y[curr];
        animate(3, layer);
    } // if (m_unoPtr->getPlayers() == 3)
    else {
        layer[2].elem = m_unoPtr->getBackImage();
        layer[2].startLeft = x[oppo];
        layer[2].startTop = y[oppo];
        layer[2].endLeft = x[prev];
        layer[2].endTop = y[prev];
        layer[3].elem = m_unoPtr->getBackImage();
        layer[3].startLeft = x[prev];
        layer[3].startTop = y[prev];
        layer[3].endLeft = x[curr];
        layer[3].endTop = y[curr];
        animate(4, layer);
    } // else

    m_hideFlag = 0x00;
    m_unoPtr->cycle();
    refreshScreen(m_infoPtr->info_0_rotate());
    threadWait(1500);
    updateStatus(m_unoPtr->switchNow());
}


/**
 * Triggered on challenge chance. When a player played a [wild +4], the next
 * player can challenge its legality. Only when you have no cards that match
 * the previous played card's color, you can play a [wild +4].
 * Next player does not challenge: next player draw 4 cards;
 * Challenge success: current player draw 4 cards;
 * Challenge failure: next player draw 6 cards.
 */
void GameWindow::onChallenge() {
    int now, challenger;
    bool challengeSuccess;

    updateStatus(STAT_IDLE); // block mouse click events when idle
    now = m_unoPtr->getNow();
    challenger = m_unoPtr->getNext();
    challengeSuccess = m_unoPtr->challenge(now);
    refreshScreen(m_infoPtr->info_challenge(
        challenger, now, m_unoPtr->next2lastColor()));
    threadWait(1500);
    if (challengeSuccess) {
        // Challenge success, who played [wild +4] draws 4 cards
        refreshScreen(m_infoPtr->info_challengeSuccess(now));
        threadWait(1500);
        draw(4, /* force */ true);
    } // if (challengeSuccess)
    else {
        // Challenge failure, challenger draws 6 cards
        refreshScreen(m_infoPtr->info_challengeFailure(challenger));
        threadWait(1500);
        m_unoPtr->switchNow();
        draw(6, /* force */ true);
    }
}


void GameWindow::threadWait(int millis) {
    QEventLoop loop;

    QTimer::singleShot(millis, &loop, SLOT(quit()));
    loop.exec();
}


void GameWindow::updateStatus(int status) {
    switch (m_status = status) {
    case STAT_NEW_GAME:
        m_unoPtr->start();
        m_selectedIndex = -1;
        refreshScreen(m_infoPtr->info_ready());
        threadWait(2000);
        switch (m_unoPtr->getRecent().at(0)->content) {
        case DRAW2:
            // If starting with a [+2], let dealer draw 2 cards.
            draw(2, /* force */ true);
            break; // case DRAW2

        case SKIP:
            // If starting with a [skip], skip dealer's turn.
            refreshScreen(m_infoPtr->info_skipped(m_unoPtr->getNow()));
            threadWait(1500);
            updateStatus(m_unoPtr->switchNow());
            break; // case SKIP

        case REV:
            // If starting with a [reverse], change the action
            // sequence to COUNTER CLOCKWISE.
            m_unoPtr->switchDirection();
            refreshScreen(m_infoPtr->info_dirChanged());
            threadWait(1500);
            updateStatus(m_unoPtr->getNow());
            break; // case REV

        default:
            // Otherwise, go to dealer's turn.
            updateStatus(m_unoPtr->getNow());
            break; // default
        }
        break; // case STAT_NEW_GAME

    case Player::YOU:
        // Your turn, select a hand card to play, or draw a card
        if (m_unoPtr->legalCardsCount4NowPlayer() == 0) {
            draw();
        }
        else {
            auto hand = m_unoPtr->getPlayer(Player::YOU)->getHandCards();
            if (hand.size() == 1) {
                play(0);
            }
            else if (m_selectedIndex < 0) {
                int c = m_unoPtr->getDraw2StackCount();
                refreshScreen(c == 0
                    ? m_infoPtr->info_yourTurn()
                    : m_infoPtr->info_yourTurn_stackDraw2(c));
            }
            else {
                Card* card = hand.at(m_selectedIndex);
                refreshScreen(m_unoPtr->isLegalToPlay(card)
                    ? m_infoPtr->info_clickAgainToPlay(card->name)
                    : m_infoPtr->info_cannotPlay(card->name));
            }
        }
        break; // case Player::YOU

    case STAT_WILD_COLOR:
        // Need to specify the following legal color after played a
        // wild card. Draw color sectors in the center of screen
        refreshScreen(m_infoPtr->ask_color());
        break; // case STAT_WILD_COLOR

    case STAT_DOUBT_WILD4:
        if (m_unoPtr->getNext() != Player::YOU) {
            // Challenge or not is decided by AI
            if (m_AI_Ptr->needToChallenge()) {
                onChallenge();
            }
            else {
                m_unoPtr->switchNow();
                draw(4, /* force */ true);
            }
        }
        else {
            // Challenge or not is decided by you
            refreshScreen(m_infoPtr->ask_challenge(m_unoPtr->next2lastColor()));
        }
        break; // case STAT_DOUBT_WILD4

    case STAT_SEVEN_TARGET:
        // In 7-0 rule, when someone put down a seven card, the player
        // must swap hand cards with another player immediately.
        if (m_unoPtr->getNow() != Player::YOU) {
            // Seven-card is played by AI. Select target automatically.
            swapWith(m_AI_Ptr->calcBestSwapTarget4NowPlayer());
        }
        else {
            // Seven-card is played by you. Select target manually.
            refreshScreen(m_infoPtr->ask_target());
        }
        break; // case STAT_SEVEN_TARGET

    case Player::COM1:
    case Player::COM2:
    case Player::COM3:
        // AI players' turn
        if (m_unoPtr->isSevenZeroRule()) {
            sevenZeroAI();
        }
        else {
            hardAI();
        } 
        break; // case Player::COM1, Player::COM2, Player::COM3

    case STAT_GAME_OVER:
        refreshScreen(m_infoPtr->info_gameOver());
        emit SIG_game_over();
        break; // case STAT_GAME_OVER

    default:
        break;
    } 
}


void GameWindow::refreshScreen(const QString& message = "") {
    QImage image;
    int i, size, status;

    // get status
    status = m_status;

    // Clear previous background
    m_painterPtr->drawImage(0, 0, m_unoPtr->getBackgroundImage());

    // Message area
    int width = m_painterPtr->fontMetrics().horizontalAdvance(message);
    m_painterPtr->drawText(640 - width / 2, 487, message);

    this->showDeckRecent();
    this->showRemainUsed();

    /* -------- Left-center: Hand cards of Player West (COM1) -------- */
    if (status == state::STAT_GAME_OVER && m_winner == Player::COM1) {
        // Played all hand cards, it's winner
        m_painterPtr->setPen(PEN_YELLOW);
        width = m_painterPtr->fontMetrics().horizontalAdvance("WIN");
        m_painterPtr->drawText(80 - width / 2, 461, "WIN");
        m_painterPtr->setPen(PEN_WHITE);
    }
    else if (((m_hideFlag >> 1) & 0x01) == 0x00) {
        Player* p = m_unoPtr->getPlayer(Player::COM1);
        auto hand = p->getHandCards();
        size = int(hand.size());
        for (i = 0; i < size; ++i) {
            image = p->isOpen(i) ? hand.at(i)->normalImg : m_unoPtr->getBackImage();
            m_painterPtr->drawImage(20, 290 - 20 * size + 40 * i, image);
        }

        if (size == 1) {
            // Show "UNO" warning when only one card in hand
            m_painterPtr->setPen(PEN_YELLOW);
            width = m_painterPtr->fontMetrics().horizontalAdvance("UNO");
            m_painterPtr->drawText(80 - width / 2, 494, "UNO");
            m_painterPtr->setPen(PEN_WHITE);
        }
    }

    /* -------- Top-center: Hand cards of Player North (COM2) -------- */
    if (status == STAT_GAME_OVER && m_winner == Player::COM2) {
        // Played all hand cards, it's winner
        m_painterPtr->setPen(PEN_YELLOW);
        width = m_painterPtr->fontMetrics().horizontalAdvance("WIN");
        m_painterPtr->drawText(640 - width / 2, 121, "WIN");
        m_painterPtr->setPen(PEN_WHITE);
    }
    else if (((m_hideFlag >> 2) & 0x01) == 0x00) {
        Player* p = m_unoPtr->getPlayer(Player::COM2);
        auto hand = p->getHandCards();
        size = int(hand.size());
        for (i = 0; i < size; ++i) {
            image = p->isOpen(i) ? hand.at(i)->normalImg : m_unoPtr->getBackImage();
            m_painterPtr->drawImage((1205 - 45 * size + 90 * i) / 2, 20, image);
        }

        if (size == 1) {
            // Show "UNO" warning when only one card in hand
            m_painterPtr->setPen(PEN_YELLOW);
            width = m_painterPtr->fontMetrics().horizontalAdvance("UNO");
            m_painterPtr->drawText(560 - width, 121, "UNO");
            m_painterPtr->setPen(PEN_WHITE);
        }
    }

    /* -------- Right-center: Hand cards of Player East (COM3) -------- */
    if (status == STAT_GAME_OVER && m_winner == Player::COM3) {
        // Played all hand cards, it's winner
        m_painterPtr->setPen(PEN_YELLOW);
        width = m_painterPtr->fontMetrics().horizontalAdvance("WIN");
        m_painterPtr->drawText(1200 - width / 2, 461, "WIN");
        m_painterPtr->setPen(PEN_WHITE);
    }
    else if (((m_hideFlag >> 3) & 0x01) == 0x00) {
        Player* p = m_unoPtr->getPlayer(Player::COM3);
        auto hand = p->getHandCards();
        size = int(hand.size());
        for (i = 0; i < size; ++i) {
            image = p->isOpen(i) ? hand.at(i)->normalImg : m_unoPtr->getBackImage();
            m_painterPtr->drawImage(1140, 290 - 20 * size + 40 * i, image);
        }

        if (size == 1) {
            // Show "UNO" warning when only one card in hand
            m_painterPtr->setPen(PEN_YELLOW);
            width = m_painterPtr->fontMetrics().horizontalAdvance("UNO");
            m_painterPtr->drawText(1200 - width / 2, 494, "UNO");
            m_painterPtr->setPen(PEN_WHITE);
        }
    }

    /* -------- Bottom: Your hand cards -------- */
    if (status == STAT_GAME_OVER && m_winner == Player::YOU) {
        // Played all hand cards, it's winner
        m_painterPtr->setPen(PEN_YELLOW);
        width = m_painterPtr->fontMetrics().horizontalAdvance("WIN");
        m_painterPtr->drawText(640 - width / 2, 621, "WIN");
        m_painterPtr->setPen(PEN_WHITE);
    }
    else if ((m_hideFlag & 0x01) == 0x00) {
        // Show your all hand cards
        auto hand = m_unoPtr->getPlayer(Player::YOU)->getHandCards();
        size = int(hand.size());
        for (i = 0; i < size; ++i) {
            Card* card = hand.at(i);
            image = status == STAT_GAME_OVER
                || (status == Player::YOU
                    && m_unoPtr->isLegalToPlay(card))
                ? card->normalImg : card->darkImg;
            m_painterPtr->drawImage(
                /* x     */ (1205 - 45 * size + 90 * i) / 2,
                /* y     */ i == m_selectedIndex ? 500 : 520,
                /* image */ image
            );
        }

        if (size == 1) {
            // Show "UNO" warning when only one card in hand
            m_painterPtr->setPen(PEN_YELLOW);
            m_painterPtr->drawText(720, 621, "UNO");
            m_painterPtr->setPen(PEN_WHITE);
        }
    }

    // Extra sectors in special status
    switch (status) {
    case STAT_WILD_COLOR:
        // Need to specify the following legal color after played a
        // wild card. Draw color sectors in the center of screen
        // Draw blue sector
        m_painterPtr->setPen(Qt::NoPen);
        m_painterPtr->setBrush(BRUSH_BLUE);
        m_painterPtr->drawPie(270, 180, 271, 271, 0, 90 * 16);

        // Draw green sector
        m_painterPtr->setBrush(BRUSH_GREEN);
        m_painterPtr->drawPie(270, 180, 271, 271, 0, -90 * 16);

        // Draw red sector
        m_painterPtr->setBrush(BRUSH_RED);
        m_painterPtr->drawPie(270, 180, 271, 271, 180 * 16, -90 * 16);

        // Draw yellow sector
        m_painterPtr->setBrush(BRUSH_YELLOW);
        m_painterPtr->drawPie(270, 180, 271, 271, 180 * 16, 90 * 16);
        m_painterPtr->setPen(PEN_WHITE);
        break;
    
    case STAT_DOUBT_WILD4:
        // Ask whether you want to challenge your previous player
        // Draw YES button
        m_painterPtr->setPen(Qt::NoPen);
        m_painterPtr->setBrush(BRUSH_GREEN);
        m_painterPtr->drawPie(270, 180, 271, 271, 0, 180 * 16);
        m_painterPtr->setPen(PEN_WHITE);
        width = m_painterPtr->fontMetrics().horizontalAdvance(m_infoPtr->label_yes());
        m_painterPtr->drawText(405 - width / 2, 268, m_infoPtr->label_yes());

        // Draw NO button
        m_painterPtr->setPen(Qt::NoPen);
        m_painterPtr->setBrush(BRUSH_RED);
        m_painterPtr->drawPie(270, 180, 271, 271, 0, -180 * 16);
        m_painterPtr->setPen(PEN_WHITE);
        width = m_painterPtr->fontMetrics().horizontalAdvance(m_infoPtr->label_no());
        m_painterPtr->drawText(405 - width / 2, 382, m_infoPtr->label_no());
        break;

    case STAT_SEVEN_TARGET:
        // Ask the target you want to swap hand cards with
        // Draw west sector (red)
        m_painterPtr->setPen(Qt::NoPen);
        m_painterPtr->setBrush(BRUSH_RED);
        m_painterPtr->drawPie(270, 180, 271, 271, -90 * 16, -120 * 16);
        m_painterPtr->setPen(PEN_WHITE);
        width = m_painterPtr->fontMetrics().horizontalAdvance("W");
        m_painterPtr->drawText(338 - width / 2, 350, "W");

        // Draw east sector (green)
        m_painterPtr->setPen(Qt::NoPen);
        m_painterPtr->setBrush(BRUSH_GREEN);
        m_painterPtr->drawPie(270, 180, 271, 271, -90 * 16, 120 * 16);
        m_painterPtr->setPen(PEN_WHITE);
        width = m_painterPtr->fontMetrics().horizontalAdvance("E");
        m_painterPtr->drawText(472 - width / 2, 350, "E");

        // Draw north sector (yellow)
        m_painterPtr->setPen(Qt::NoPen);
        m_painterPtr->setBrush(BRUSH_YELLOW);
        m_painterPtr->drawPie(270, 180, 271, 271, 150 * 16, -120 * 16);
        m_painterPtr->setPen(PEN_WHITE);
        width = m_painterPtr->fontMetrics().horizontalAdvance("N");
        m_painterPtr->drawText(405 - width / 2, 270, "N");
        break;

    default:
        break;
    }

    // Show screen
    update();
}

void GameWindow::showDeckRecent() {
    /* Center: card deck & recent played card */
    auto recentColors = m_unoPtr->getRecentColors();
    auto recent = m_unoPtr->getRecent();
    int size = int(recent.size());
    int width = 45 * size + 75;
    QImage image = m_unoPtr->getBackImage();

    m_painterPtr->drawImage(338, 270, image);
    for (int i = 0; i < size; ++i) {
        if (recent.at(i)->content == WILD) {
            image = m_unoPtr->getColoredWildImage(recentColors.at(i));
        }
        else if (recent.at(i)->content == WILD_DRAW4) {
            image = m_unoPtr->getColoredWildDraw4Image(recentColors.at(i));
        }
        else {
            image = recent.at(i)->normalImg;
        }
        m_painterPtr->drawImage(792 - width / 2 + 45 * i, 270, image);
    }
}


void GameWindow::showRemainUsed() {
    /* Left-top corner: remain / used */
    int remain = m_unoPtr->getDeckCount();
    int used = m_unoPtr->getUsedCount();
    QString info = m_infoPtr->label_remain_used(remain, used);
    m_painterPtr->drawText(20, 42, info);
}


void GameWindow::animate(int layerCount, AnimateLayer layer[]) {
    int i, j;
    QRect roi;

    for (i = 0; i < 5; ++i) {
        for (j = 0; j < layerCount; ++j) {
            AnimateLayer& l = layer[j];
            roi.setX(l.startLeft + (l.endLeft - l.startLeft) * i / 5);
            roi.setY(l.startTop + (l.endTop - l.startTop) * i / 5);
            roi.setWidth(l.elem.width());
            roi.setHeight(l.elem.height());
            m_bkPainterList[j]->drawImage(roi, m_screen, roi);
        } // for (j = 0; j < layerCount; ++j)

        for (j = 0; j < layerCount; ++j) {
            AnimateLayer& l = layer[j];
            m_painterPtr->drawImage(
                /* x     */ l.startLeft + (l.endLeft - l.startLeft) * i / 5,
                /* y     */ l.startTop + (l.endTop - l.startTop) * i / 5,
                /* image */ l.elem
            );
        }

        update();
        threadWait(30);
        for (j = 0; j < layerCount; ++j) {
            AnimateLayer& l = layer[j];
            roi.setX(l.startLeft + (l.endLeft - l.startLeft) * i / 5);
            roi.setY(l.startTop + (l.endTop - l.startTop) * i / 5);
            roi.setWidth(l.elem.width());
            roi.setHeight(l.elem.height());
            m_painterPtr->drawImage(roi, m_backup[j], roi);
        }
    }
}



void GameWindow::paintEvent(QPaintEvent* event) {
    static QRect roi(0, 0, 0, 0);

    roi.setWidth(this->width());
    roi.setHeight(this->height());
    QPainter(this).drawImage(roi, m_screen);
}


void GameWindow::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        // Only response to left-click events, and ignore the others
        int x = 1280 * event->x() / this->width();
        int y = 720 * event->y() / this->height();
        switch (m_status) {
        case Player::YOU:
            if (520 <= y && y <= 700) {
                Player* now = m_unoPtr->getPlayer(Player::YOU);
                auto hand = now->getHandCards();
                int size = int(hand.size());
                int width = 45 * size + 75;
                int startX = 640 - width / 2;
                if (startX <= x && x <= startX + width) {
                    // Hand card area
                    // Calculate which card clicked by the X-coordinate
                    int index = qMin((x - startX) / 45, size - 1);
                    Card* card = hand.at(index);

                    // Try to play it
                    if (index != m_selectedIndex) {
                        m_selectedIndex = index;
                        updateStatus(m_status);
                    }
                    else if (m_unoPtr->isLegalToPlay(card)) {
                        if (card->isWild() && size > 1) {
                            updateStatus(STAT_WILD_COLOR);
                        }
                        else {
                            play(index);
                        }
                    }
                }
                else {
                    // Blank area, cancel your selection
                    m_selectedIndex = -1;
                    updateStatus(m_status);
                }
            }
            else if (270 <= y && y <= 450 && 338 <= x && x <= 458) {
                // Card deck area, draw a card
                draw();
            }
            break; // case Player::YOU

        case STAT_WILD_COLOR:
            if (220 < y && y < 315) {
                if (310 < x && x < 405) {
                    // Red sector
                    play(m_selectedIndex, RED);
                }
                else if (405 < x && x < 500) {
                    // Blue sector
                    play(m_selectedIndex, BLUE);
                }
            }
            else if (315 < y && y < 410) {
                if (310 < x && x < 405) {
                    // Yellow sector
                    play(m_selectedIndex, YELLOW);
                }
                else if (405 < x && x < 500) {
                    // Green sector
                    play(m_selectedIndex, GREEN);
                }
            }
            break; // case STAT_WILD_COLOR

        case STAT_DOUBT_WILD4:
            // Asking if you want to challenge your previous player
            if (310 < x && x < 500) {
                if (220 < y && y < 315) {
                    // YES button, challenge wild +4
                    onChallenge();
                }
                else if (315 < y && y < 410) {
                    // NO button, do not challenge wild +4
                    m_unoPtr->switchNow();
                    draw(4, /* force */ true);
                }
            }
            break; // case STAT_DOUBT_WILD4

        case STAT_SEVEN_TARGET:
            if (198 < y && y < 276 && m_unoPtr->getPlayers() == 4) {
                if (338 < x && x < 472) {
                    // North sector
                    swapWith(Player::COM2);
                }
            }
            else if (315 < y && y < 410) {
                if (310 < x && x < 405) {
                    // West sector
                    swapWith(Player::COM1);
                }
                else if (405 < x && x < 500) {
                    // East sector
                    swapWith(Player::COM3);
                }
            }
            break; // case STAT_SEVEN_TARGET

        case STAT_GAME_OVER:
            if (270 <= y && y <= 450) {
                if (338 <= x && x <= 458) {
                    // Card deck area, start a new game
                    emit SIG_new_game();
                    updateStatus(STAT_NEW_GAME);
                }
            }
            break; // case STAT_GAME_OVER

        default:
            break; // default
        }
    }
}