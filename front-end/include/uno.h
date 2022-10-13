#ifndef __UNO_H__
#define __UNO_H__

#include "color.h"
#include "content.h"
#include "card.h"
#include "player.h"


constexpr int DIR_LEFT = 1; //!< Direction value (clockwise).
constexpr int DIR_RIGHT = 3; //!< Direction value (counter-clockwise).

class UNO {

public:
    /**
     * @brief In this application, everyone can hold 14 cards at most.
     */
    static const int MAX_HOLD_CARDS = 14;


    /**
     * @return Reference of our meyers singleton.
     */
    inline static UNO* getInstance(unsigned int seed = 0U) {
        static UNO instance(seed);
        return &instance;
    } // getInstance(unsigned)


    /**
     * @brief Start a new Uno game. Shuffle cards, let everyone draw 7 cards,
     * then determine our start card.
     */
    void start();

    /**
     * @brief Call this function when someone needs to draw a card.
     * 
     * NOTE: Everyone can hold 14 cards at most in this program, so even if this
     * function is called, the specified player may not draw a card as a result.
     *
     * @param who   Who draws a card. Must be one of the following values:
     *              Player::YOU, Player::COM1, Player::COM2, Player::COM3.
     * @param force Pass true if the specified player is required to draw cards,
     *              i.e. previous player played a [+2] or [wild +4] to let this
     *              player draw cards. Or false if the specified player draws a
     *              card by itself in its action.
     * @return Index of the drawn card in hand, or -1 if the specified player
     *         didn't draw a card because of the limitation.
     */
    int draw(int who, bool force);


    /**
     * @brief Call this function when someone needs to play a card. The played card
     * replaces the "previous played card", and the original "previous played
     * card" becomes a used card at the same time.
     * 
     * NOTE: Before calling this function, you must call isLegalToPlay(Card*)
     * function at first to check whether the specified card is legal to play.
     * This function will play the card directly without checking the legality.
     *
     * @param who   Who plays a card. Must be one of the following values:
     *              Player::YOU, Player::COM1, Player::COM2, Player::COM3.
     * @param index Play which card. Pass the corresponding card's index of the
     *              specified player's hand cards.
     * @param color Optional, available when the card to play is a wild card.
     *              Pass the specified following legal color.
     * @return Reference of the played card.
     */
    Card* play(int who, int index, Color color);


    /**
     * @brief When you think your previous player used a [wild +4] card illegally,
     * i.e. it holds at least one card matching the next-to-last color,
     * call this function to make a challenge.
     *
     * @param whom Challenge whom. Must be one of the following:
     *             Player::YOU, Player::COM1, Player::COM2, Player::COM3.
     * @return Tell the challenge result, true if challenge success,
     *         or false if challenge failure.
     */
    bool challenge(int whom);

    /**
     * @brief In 7-0 rule, when someone put down a seven card, then the player must
     * swap hand cards with another player immediately.
     *
     * @param a Who put down the seven card. Must be one of the following:
     *          Player::YOU, Player::COM1, Player::COM2, Player::COM3.
     * @param b Exchange with whom. Must be one of the following:
     *          Player::YOU, Player::COM1, Player::COM2, Player::COM3.
     *          Cannot exchange with yourself.
     */
    void swap(int a, int b);

    /**
     * @brief In 7-0 rule, when a zero card is put down, everyone need to pass the hand
     * cards to the next player.
     */
    void cycle();

    /**
     * @return How many legal cards (the cards that can be played legally)
     *         in now player's hand.
     */
    int legalCardsCount4NowPlayer();


    /**
     * @return Card back image resource.
     */
    inline const QImage& getBackImage() {
        return m_backImg;
    }


    /**
     * @return Background image resource in current direction.
     */
    inline const QImage& getBackgroundImage() {
        return (direction == DIR_LEFT) ? 
            m_bg_clockwise : 
            (direction == DIR_RIGHT) ? 
                m_bg_counter_clockwise : 
                m_bg_welcome;
    }


    /**
     * @brief When a player played a wild card and specified a following legal color,
     * get the corresponding color-filled image here, and show it in recent
     * card area.
     *
     * @param color The wild image with which color filled you want to get.
     * @return Corresponding color-filled image.
     */
    inline const QImage& getColoredWildImage(Color color) {
        return m_wildCardImg[color];
    }


    /**
     * @brief When a player played a wild +4 card and specified a following legal
     * color, get the corresponding color-filled image here, and show it in
     * recent card area.
     *
     * @param color The wild +4 image with which color filled you want to get.
     * @return Corresponding color-filled image.
     */
    inline const QImage& getColoredWildDraw4Image(Color color) {
        return m_wild4CardImg[color];
    }


    /**
     * @brief Players in turn.
     * @return player id 
     */
    inline int getNow() { 
        return now;
    } 


    /**
     * @brief Switch to next player's turn.
     * @return Player in turn after switched.
     */
    inline int switchNow() {
        return (now = getNext());
    }


    /**
     * @return Current player's next player.
     */
    inline int getNext() {
        int next = (now + direction) % 4;
        if (players == 3 && next == Player::COM2) {
            next = (next + direction) % 4;
        }

        return next;
    }


    /**
     * @return Current player's opposite player.
     * @note When only 3 players in game, getOppo() == getPrev().
     */
    inline int getOppo() {
        int oppo = (getNext() + direction) % 4;
        if (players == 3 && oppo == Player::COM2) {
            oppo = (oppo + direction) % 4;
        }

        return oppo;
    }


    /**
     * @return Current player's previous player. 
     */
    inline int getPrev() {
        int prev = (4 + now - direction) % 4;
        if (players == 3 && prev == Player::COM2) {
            prev = (4 + prev - direction) % 4;
        }

        return prev;
    }


   /**
     * @param who Get which player's instance. Must be one of the following:
     *            Player::YOU, Player::COM1, Player::COM2, Player::COM3.
     * @return Specified player's instance.
     */
    inline Player* getPlayer(int who) {
        return who < Player::YOU || who > Player::COM3 ? nullptr : &player[who];
    }


    /**
     * @return &this->player[this->getNow()].
     */
    inline Player* getCurrPlayer() {
        return &player[getNow()];
    }


    /**
     * @return &this->player[this->getNext()].
     */
    inline Player* getNextPlayer() {
        return &player[getNext()];
    }


    /**
     * @return &this->player[this->getOppo()].
     */
    inline Player* getOppoPlayer() {
        return &player[getOppo()];
    }


    /**
     * @return &this->player[this->getPrev()].
     */
    inline Player* getPrevPlayer() {
        return &player[getPrev()];
    }

    /**
     * @return How many players in game (3 or 4).
     */
    inline int getPlayers() {
        return players;
    }


    /**
     * @brief Set the amount of players in game.
     *
     * @param players Supports 3 and 4.
     */
    inline void setPlayers(int players) {
        if (players == 3 || players == 4) {
            this->players = players;
        }
    }


    /**
     * @brief Switch current action sequence. The value of [direction] will be
     * switched between DIR_LEFT and DIR_RIGHT.
     */
    inline void switchDirection() {
        direction = 4 - direction;
    }


    /**
     * @return This value tells that what's the next step
     *         after you drew a playable card in your action.
     *         When force play is enabled, play the card immediately.
     *         When force play is disabled, keep the card in your hand.
     */
    inline bool isForcePlay() {
        return forcePlayRule;
    }


    /**
     * @param enabled Enable/Disable the force play rule.
     */
    inline void setForcePlay(bool enabled) {
        forcePlayRule = enabled;
    }


    /**
     * @return Whether the 7-0 rule is enabled. In 7-0 rule, when a seven card
     *         is put down, the player must swap hand cards with another player
     *         immediately. When a zero card is put down, everyone need to pass
     *         the hand cards to the next player.
     */
    inline bool isSevenZeroRule() {
        return sevenZeroRule;
    }


    /**
     * @param enabled Enable/Disable the 7-0 rule.
     */
    inline void setSevenZeroRule(bool enabled) {
        sevenZeroRule = enabled;
    }


    /**
     * @return Can or cannot stack +2 cards. If can, when you put down a +2
     *         card, the next player may transfer the punishment to its next
     *         player by stacking another +2 card. Finally the first one who
     *         does not stack a +2 card must draw all of the required cards.
     */
    inline bool isDraw2StackRule() {
        return draw2StackRule;
    }


    /**
     * @param enabled Enable/Disable the +2 stacking rule.
     */
    inline void setDraw2StackRule(bool enabled) {
        draw2StackRule = enabled;
    }


    /**
     * @brief Only available in +2 stack rule. In this rule, when a +2 card is put
     * down, the next player may transfer the punishment to its next player
     * by stacking another +2 card. Finally the first one who does not stack
     * a +2 card must draw all of the required cards.
     *
     * @return This counter records that how many required cards need to be
     *         drawn by the final player. When this value is not zero, only
     *         +2 cards are legal to play.
     */
    inline int getDraw2StackCount() {
        return draw2StackCount;
    }


    /**
     * @brief Find a card instance in card table.
     *
     * @param color   Color of the card you want to get.
     * @param content Content of the card you want to get.
     * @return Corresponding card instance.
     */
    inline Card* findCard(Color color, Content content) {
        return color == NONE && content == WILD
            ? &tableCards.at(39 + WILD)
            : color == NONE && content == WILD_DRAW4
            ? &tableCards.at(39 + WILD_DRAW4)
            : color != NONE && content != WILD && content != WILD_DRAW4
            ? &tableCards.at(13 * (color - 1) + content)
            : nullptr;
    }


    /**
     * @return How many cards in deck (haven't been used yet).
     */
    inline int getDeckCount() {
        return int(deckCards.size());
    }


    /**
     * @return How many cards have been used.
     */
    inline int getUsedCount() {
        return int(usedCards.size() + recentCards.size());
    }


    /**
     * @return Recent played cards.
     */
    inline const std::vector<Card*>& getRecent() {
        return recentCards;
    }


    /**
     * @return Colors of recent played cards.
     */
    inline const std::vector<Color>& getRecentColors() {
        return recentColors;
    }


    /**
     * @return Color of the last played card.
     */
    inline Color lastColor() {
        return recentColors.back();
    }


    /**
     * @return Color of the next-to-last played card.
     */
    inline Color next2lastColor() {
        return recentColors.at(recentColors.size() - 2);
    }


    /**
     * @brief Access colorAnalysis[A] to get how many cards in color A are used.
     *
     * @param A Provide the parameter A.
     * @return Value of colorAnalysis[A].
     */
    inline int getColorAnalysis(Color A) {
        return colorAnalysis[A];
    }


    /**
     * @brief Access contentAnalysis[B] to get how many cards in content B are used.
     *
     * @param B Provide the parameter B.
     * @return Value of contentAnalysis[B].
     */
    inline int getContentAnalysis(Content B) {
        return contentAnalysis[B];
    }


    /**
     * Check whether the specified card is legal to play. It's legal only when
     * it's wild, or it has the same color/content to the previous played card.
     *
     * @param card Check which card's legality.
     * @return Whether the specified card is legal to play.
     */
    inline bool isLegalToPlay(Card* card) {
        return ((legality >> card->id) & 0x01LL) == 0x01LL;
    }

private:
    QImage m_backImg; //!< Card back image.
    QImage m_bg_welcome; //!< Background welcome image
    QImage m_bg_clockwise; //!< Background image with clockwise sign
    QImage m_bg_counter_clockwise; //!< Background image with counter clockwise sign

    QImage m_wildCardImg[5]; //!< Wild card images
    QImage m_wild4CardImg[5]; //!< Wild +4 card images


    int now;  //!< Player in turn.
    int players; //!< How many players in game. Supports 3 or 4.
    int direction; //!< Current action sequence (DIR_LEFT / DIR_RIGHT).

    bool forcePlayRule; //!< Whether the force play rule is enabled.
    bool sevenZeroRule; //!< Whether the 7-0 rule is enabled.
    bool draw2StackRule; //!< Can or cannot stack +2 cards.

    /**
     * @brief Only available in +2 stack rule. In this rule, when a +2 card is put
     * down, the next player may transfer the punishment to its next player
     * by stacking another +2 card. Finally the first one who does not stack
     * a +2 card must draw all of the required cards. This counter records
     * that how many required cards need to be drawn by the final player.
     * When this value is not zero, only +2 cards are legal to play.
     */
    int draw2StackCount;

    /**
     * @brief Game players.
     */
    Player player[4];

    /**
     * @brief This binary value shows that which cards are legal to play. When
     * 0x01LL == ((legality >> i) & 0x01LL), the card with id number i
     * is legal to play. When the recent-played-card queue changes,
     * this value will be updated automatically.
     */
    long long legality;

    /**
     * @brief Access colorAnalysis[A] to get how many cards in color A are used
     */
    int colorAnalysis[5];

    /**
     * @brief Access contentAnalysis[B] to get how many cards in content B are used.
     */
    int contentAnalysis[15];

    std::vector<Card*> deckCards; //!< Ready to be used cards.
    std::vector<Card> tableCards; //!< Card map. table[i] stores the card instance of id number i.
    std::vector<Card*> usedCards; //!< Used cards.
    std::vector<Card*> recentCards; //!< recent played cards
    std::vector<Color> recentColors; //!< recent played cards'color

    // Singleton, hide default constructor.
    UNO(unsigned int seed);

    // MACRO
    inline static unsigned MASK_I_TO_END(int i) {
        return 0xffffffffU << i;
    }

    // MACRO
    inline static unsigned MASK_BEGIN_TO_I(int i) {
        return ~(0xffffffffU << i);
    }

    // MACRO
    inline static unsigned int MASK_ALL(UNO* u, int p) {
        return MASK_BEGIN_TO_I(u->getPlayer(p)->getHandSize());
    }

    inline static QString array2string(int arr[], int size) {
        QString s(size > 0 ? QString::number(arr[0]) : "");
        for (int i = 1; i < size; ++i) {
            s = s + ", " + QString::number(arr[i]);
        }

        return "[" + s + "]";
    }  
};

#endif