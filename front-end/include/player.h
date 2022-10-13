#ifndef __PLAYER_H__
#define __PLAYER_H__


#include "card.h"
#include "color.h"
#include <vector>

/**
 * @brief Store a player's real-time information,
 * such as hand cards, and recent played card. 
 */
class Player {

public:

    static const int YOU = 0;
    static const int COM1 = 1;
    static const int COM2 = 2;
    static const int COM3 = 3;

    /**
     * @return This player's all hand cards.
     */
    inline const std::vector<Card*>& getHandCards() {
        return handCards;
    }


    /**
     * @return How many cards in this player's hand.
     */
    inline int getHandSize() {
        return int(handCards.size());
    }


    /**
     * @return This player's recent played card, or nullptr if this player drew
     *         one or more cards in its previous action.
     */
    inline Card* getRecent() {
        return recentCards;
    }


    /**
     * @brief Check whether this player's hand cards are known by you, i.e. the unique
     * non-AI player. In 7-0 rule, when a seven or zero card is put down, and
     * your hand cards are transferred to someone else (for example, A), then
     * A's all hand cards are known by you.
     *
     * @param index Index of the card to check (0 ~ this->handCards.size() - 1).
     *              If you pass -1, check all hand cards.
     * @return Whether this player's specified card is known by you. If index is
     *         -1, this function will return true only when ALL OF THIS PLAYER'S
     *         HAND CARDS are known by you.
     */
    inline bool isOpen(int index) {
        return index < 0
            ? open == (~(0xffffffffU << handCards.size()))
            : 0x01 == (0x01 & (open >> index));
    }

    /**
     * Call this function to rearrange this player's hand cards.
     * The cards with same color will be arranged together.
     */
    inline void sort() {
        std::sort(handCards.begin(), handCards.end());
    }


    /**
     * @brief When this player played a wild card, record the specified color as this
     * player's strong color. The strong color will be remembered until this
     * player played a number of card matching that color. You can use this
     * value to defend this player's UNO dash.
     *
     * @return This player's strong color, or Color::NONE if no available
     *         strong color.
     */
    inline Color getStrongColor() {
        return strongColor;
    }


    /**
     * @brief When this player draw a card in action, record the previous played card's
     * color as this player's weak color. What this player did means that this
     * player probably do not have cards in that color. You can use this value
     * to defend this player's UNO dash.
     *
     * @return This player's weak color, or Color::NONE if no available weak
     *         color.
     */
    inline Color getWeakColor() {
        return weakColor;
    }

private:
    Player() = default;

    /**
     * @brief UNO player cards in hand
     */
    std::vector<Card*> handCards;

    /**
     * @brief Recent played card. If the player drew one or more cards in its last
     * action, this member will be nullptr.
     */
    Card* recentCards = nullptr;


    Color strongColor = NONE;
    Color weakColor = NONE;

     /**
     * How many dangerous cards (cards in strong color) in hand. THIS IS AN
     * ESTIMATED VALUE, NOT A REAL VALUE! This value is estimated by player's
     * actions, such as which color this player selected when playing a wild
     * card, and how many dangerous cards are played after that wild card.
     */
    int strongCount = 0;

    /**
     * This binary values shows the visibility of your cards. The card of
     * handCards.at(i) is known by you when 0x01 == ((open >> i) & 0x01).
     */
    unsigned open = 0x00000000U;

    /**
     * Grant Uno class to access our constructors (to create Player instances)
     * and our private fields (to change players' real-time information).
     */
    friend class UNO;
};

#endif