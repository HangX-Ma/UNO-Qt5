/**
 * @file gamewindow.h
 * @author HangX-Ma (m-contour@qq.com)
 * @brief UNO game Window
 * @version 0.1
 * @date 2022-09-24
 * @copyright Copyright (c) 2022 HangX-Ma
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __GAMEWINDOW_H__
#define __GAMEWINDOW_H__

#include <QtWidgets/QMainWindow>
#include "uno.h"
#include "info.h"
#include "sound.h"
#include "ai.h"

class AnimateLayer;

class GameWindow : public QMainWindow 
{
    Q_OBJECT

public:
    GameWindow(int argc, char* argv[], Info* info, QWidget *parent = nullptr);
    ~GameWindow();

    void start();

private:
    UNO* m_unoPtr;
    Info* m_infoPtr;
    Sound* m_soundPtr;
    AI* m_AI_Ptr;
    QPainter* m_painterPtr;
    QPainter* m_bkPainterList[4];

    QImage m_screen;
    QImage m_backup[4];
    int m_status;


    bool m_isAIRunning;
    int m_winner;
    int m_selectedIndex;
    int m_hideFlag;

    /**
     * @brief The local event loop will block the thread until it is returned.
     * 
     * @param millis How many milli seconds to wait. 
     */
    void threadWait(int millis);

    /**
     * @brief The player in action play a card.
     *
     * @param index Play which card. Pass the corresponding card's index of the
     *              player's hand cards.
     * @param color Optional, available when the card to play is a wild card.
     *              Pass the specified following legal color.
     */
    void play(int index, Color color = NONE);

    /**
     * @brief The player in action draw one or more cards.
     *
     * @param count How many cards to draw.
     * @param force Pass true if the specified player is required to draw cards,
     *              i.e. previous player played a [+2] or [wild +4] to let this
     *              player draw cards. Or false if the specified player draws a
     *              card by itself in its action.
     */
    void draw(int count = 1, bool force = false);

    /**
     * The player in action swap hand cards with another player.
     *
     * @param whom Swap with whom. Must be one of the following:
     *             Player::YOU, Player::COM1, Player::COM2, Player::COM3
     */
    void swapWith(int whom);

    /**
     * In 7-0 rule, when a zero card is put down, everyone need to pass
     * the hand cards to the next player.
     */
    void cycle();

    /**
     * Triggered on challenge chance. When a player played a [wild +4], the next
     * player can challenge its legality. Only when you have no cards that match
     * the previous played card's color, you can play a [wild +4].
     * Next player does not challenge: next player draw 4 cards;
     * Challenge success: current player draw 4 cards;
     * Challenge failure: next player draw 6 cards.
     */
    void onChallenge();

    void refreshScreen(const QString& message);
    void animate(int layerCount, AnimateLayer layer[]);
    void updateStatus(int status);

    /* used in refreshScreen */
    void showDeckRecent();
    void showRemainUsed();

    void hardAI();
    void sevenZeroAI();

protected:
    // Implemented Listeners
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);

signals:
    void SIG_game_over();
    void SIG_new_game();
    void SIG_enter_setting();
};


class AnimateLayer {
public:
    QImage elem;
    int startLeft, startTop, endLeft, endTop;
};


typedef enum {
    STAT_IDLE = 100,
    STAT_NEW_GAME,
    STAT_GAME_OVER,
    STAT_WILD_COLOR,
    STAT_DOUBT_WILD4,
    STAT_SEVEN_TARGET,
} state;


#endif