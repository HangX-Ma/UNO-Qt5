#include "uno.h"

static const QString A[] = {
    "k", "r", "b", "g", "y"
};

static const QString B[] = {
    "0", "1", "2", "3", "4",
    "5", "6", "7", "8", "9",
    "+", "F", "S", "w", "w+"
};

static const char* BROKEN_IMAGE_RESOURCES_EXCEPTION =
    "One or more image resources are broken. Re-install this app.";


UNO::UNO(unsigned int seed) {
    QImage br, dk;
    int i, done, total;

    // Preparations
    done = 0;
    total = 120;

    /* Load background image resources */
    if (m_bg_welcome.load(":/UNORes/bg_welcome.png") &&
        m_bg_welcome.width() == 1280 && m_bg_welcome.height() == 720 &&
        m_bg_counter_clockwise.load(":/UNORes//bg_counter.png") &&
        m_bg_counter_clockwise.width() == 1280 && m_bg_counter_clockwise.height() == 720 &&
        m_bg_clockwise.load(":/UNORes/bg_clockwise.png") &&
        m_bg_clockwise.width() == 1280 && m_bg_clockwise.height() == 720) {
        done += 3;
        qDebug("Loading... (%d%%)", 100 * done / total);
    }
    else {
        qDebug(BROKEN_IMAGE_RESOURCES_EXCEPTION);
        exit(1);
    }
    

    /* Load card back image resource */
    if (m_backImg.load(":/UNORes/back.png") &&
        m_backImg.width() == 121 && m_backImg.height() == 181) {
        ++done;
        qDebug("Loading... (%d%%)", 100 * done / total);
    }
    else {
        qDebug(BROKEN_IMAGE_RESOURCES_EXCEPTION);
        exit(1);
    }

    // Generate 54 types of cards
    for (i = 0; i < 54; ++i) {
        Color a = i < 52 ? Color(i / 13 + 1) : Color(0);
        Content b = i < 52 ? Content(i % 13) : Content(i - 39);
        if (br.load(":/UNORes/front_" + A[a] + B[b] + ".png") &&
            br.width() == 121 && br.height() == 181 &&
            dk.load(":/UNORes/dark_" + A[a] + B[b] + ".png") &&
            dk.width() == 121 && dk.height() == 181) {
            done += 2;
            tableCards.push_back(Card(br, dk, a, b));
            qDebug("Loading... (%d%%)", 100 * done / total);
        }
        else {
            qDebug(BROKEN_IMAGE_RESOURCES_EXCEPTION);
            exit(1);
        }
    }

    // Load colored wild & wild +4 image resources (kw and kw+4 have been loaded before)
    m_wildCardImg[0] = tableCards.at(39 + WILD).normalImg;
    m_wild4CardImg[0] = tableCards.at(39 + WILD_DRAW4).normalImg;
    for (i = 1; i < 5; ++i) {
        if (br.load(":/UNORes/front_" + A[i] + B[WILD] + ".png") &&
            br.width() == 121 && br.height() == 181 &&
            dk.load(":/UNORes/front_" + A[i] + B[WILD_DRAW4] + ".png") &&
            dk.width() == 121 && dk.height() == 181) {
            done += 2;
            m_wildCardImg[i] = br;
            m_wild4CardImg[i] = dk;
            qDebug("Loading... (%d%%)", 100 * done / total);
        }
        else {
            qDebug(BROKEN_IMAGE_RESOURCES_EXCEPTION);
            exit(1);
        }
    }

    // Generate a random seed based on the current time stamp
    if (seed == 0U) {
        seed = unsigned(time(nullptr));
    }

    qDebug("Random seed is %d", seed);
    srand(seed);

    // Initialize other members
    players = 3;
    now = rand() % 4;
    legality = 0;

    direction = 0;
    draw2StackCount = 0;

    forcePlayRule   = true;
    draw2StackRule  = false;
    sevenZeroRule   = false;
}


void UNO::start() {
    Card* card;
    int i, size;

    // Reset direction
    direction = DIR_LEFT;

    // In +2 stack rule, reset the stack counter
    draw2StackCount = 0;

    // Clear the analysis data
    memset(colorAnalysis, 0, 5 * sizeof(int));
    memset(contentAnalysis, 0, 15 * sizeof(int));

    // Clear card deck, used card deck, recent played cards,
    // everyone's hand cards, and everyone's strong/weak colors
    deckCards.clear();
    usedCards.clear();
    recentCards.clear();
    recentColors.clear();
    for (i = Player::YOU; i <= Player::COM3; ++i) {
        player[i].open = 0x00;
        player[i].handCards.clear();
        player[i].weakColor = NONE;
        player[i].strongColor = NONE;
    }

    // Generate a temporary sequenced card deck
    for (i = 0; i < 54; ++i) {
        card = &tableCards.at(i);
        switch (card->content) {
            case WILD:
            case WILD_DRAW4:
                deckCards.push_back(card);
                deckCards.push_back(card);
                // fall through

            default:
                deckCards.push_back(card);
                // fall through

            case NUM0:
                deckCards.push_back(card);
        }
    }

    // Shuffle cards
    size = int(deckCards.size());
    while (size > 0) {
        i = rand() % size--;
        card = deckCards[i]; 
        deckCards[i] = deckCards[size]; 
        deckCards[size] = card;
    }

    // Determine a start card as the previous played card
    do {
        card = deckCards.back();
        deckCards.pop_back();
        if (card->isWild()) {
            // Start card cannot be a wild card, so return it
            // to the bottom of card deckCards and pick another card
            deckCards.insert(deckCards.begin(), card);
        }
        else {
            // Any non-wild card can be start card
            // Start card determined
            recentCards.push_back(card);
            ++colorAnalysis[card->color];
            ++contentAnalysis[card->content];
            recentColors.push_back(card->color);
        }
    } while (recentCards.empty());

    // Let everyone draw 7 cards
    for (i = 0; i < 7; ++i) {
        draw(Player::YOU,  /* force */ true);
        draw(Player::COM1, /* force */ true);
        if (players == 4) draw(Player::COM2, /* force */ true);
        draw(Player::COM3, /* force */ true);
    }

    // In the case of (last winner = NORTH) & (game mode = 3 player mode)
    // Re-specify the dealer randomly
    if (players == 3 && now == Player::COM2) {
        now = (3 + rand() % 3) % 4;
    }

    // Write log
    qDebug("Game starts with %s", qPrintable(card->name));
}


int UNO::draw(int who, bool force) {
    Card* card;
    int i, index, size;
    std::vector<Card*>* hand;

    i = -1;
    if (who >= Player::YOU && who <= Player::COM3) {
        if (draw2StackCount > 0) {
            --draw2StackCount;
        }
        else if (!force) {
            // Draw a card by player itself, register weak color
            player[who].weakColor = lastColor();
            if (player[who].weakColor == player[who].strongColor) {
                // Weak color cannot also be strong color
                player[who].strongColor = NONE;
            }
        }

        hand = &(player[who].handCards);
        if (hand->size() < MAX_HOLD_CARDS) {
            // Draw a card from card deckCards, and put it to an appropriate position
            qDebug("Player %d draw a card", who);
            card = deckCards.back();
            deckCards.pop_back();
            if (who == Player::YOU) {
                auto j = std::upper_bound(hand->begin(), hand->end(), card);
                i = int(j - hand->begin());
                hand->insert(j, card);
                player[who].open = (player[who].open << 1) | 0x01;
            }
            else {
                i = int(hand->size());
                hand->push_back(card);
            }

            player[who].recentCards = nullptr;
            if (deckCards.empty()) {
                // Re-use the used cards when there are no more cards in deckCards
                qDebug("Re-use the used cards");
                size = int(usedCards.size());
                while (size > 0) {
                    index = rand() % size--;
                    deckCards.push_back(usedCards.at(index));
                    --colorAnalysis[usedCards.at(index)->color];
                    --contentAnalysis[usedCards.at(index)->content];
                    usedCards.erase(usedCards.begin() + index);
                }
            }
        }
        else {
            // In +2 stack rule, if someone cannot draw all of the required
            // cards because of the max-hold-card limitation, force reset
            // the counter to zero.
            draw2StackCount = 0;
        }

        if (draw2StackCount == 0) {
            // Update the legality binary when necessary
            card = recentCards.back();
            legality = card->isWild()
                ? 0x30000000000000LL | (0x1fffLL << 13 * (lastColor() - 1))
                : 0x30000000000000LL | (0x1fffLL << 13 * (lastColor() - 1)) | (0x8004002001LL << card->content);
        }
    }

    return i;
}


int UNO::legalCardsCount4NowPlayer() {
    int count = 0;

    for (Card* card : player[now].handCards) {
        if (isLegalToPlay(card)) {
            ++count;
        }
    }

    return count;
}


Card* UNO::play(int who, int index, Color color) {
    int size;
    Card* card;
    std::vector<Card*>* hand;

    card = nullptr;
    if (who >= Player::YOU && who <= Player::COM3) {
        hand = &(player[who].handCards);
        size = int(hand->size());
        if (index < size) {
            card = hand->at(index);
            qDebug("Player %d played %s", who, qPrintable(card->name));
            hand->erase(hand->begin() + index);
            if (card->isWild()) {
                // When a wild card is played, register the specified
                // following legal color as the player's strong color
                player[who].strongColor = color;
                player[who].strongCount = 1 + size / 3;
                if (color == player[who].weakColor) {
                    // Strong color cannot also be weak color
                    player[who].weakColor = NONE;
                }
            }
            else if (card->color == player[who].strongColor) {
                // Played a card that matches the registered
                // strong color, strong counter counts down
                --player[who].strongCount;
                if (player[who].strongCount == 0) {
                    player[who].strongColor = NONE;
                }
            }
            else if (player[who].strongCount >= size) {
                // Correct the value of strong counter when necessary
                player[who].strongCount = size - 1;
            }

            if (card->content == DRAW2 && draw2StackRule) {
                draw2StackCount += 2;
            }

            player[who].open = who == Player::YOU
                ? (player[who].open >> 1)
                : (player[who].open & MASK_BEGIN_TO_I(index))
                | (player[who].open & MASK_I_TO_END(index + 1)) >> 1;
            player[who].recentCards = card;
            recentCards.push_back(card);
            ++colorAnalysis[card->color];
            ++contentAnalysis[card->content];
            recentColors.push_back(card->isWild() ? color : card->color);
            qDebug("colorAnalysis & contentAnalysis:");
            qDebug(qPrintable(array2string(colorAnalysis, 5)));
            qDebug(qPrintable(array2string(contentAnalysis, 15)));
            if (recentCards.size() > 5) {
                usedCards.push_back(recentCards.front());
                recentCards.erase(recentCards.begin());
                recentColors.erase(recentColors.begin());
            }

            // Update the legality binary
            legality = draw2StackCount > 0
                ? (0x8004002001LL << DRAW2)
                : card->isWild()
                ? 0x30000000000000LL
                | (0x1fffLL << 13 * (lastColor() - 1))
                : 0x30000000000000LL
                | (0x1fffLL << 13 * (lastColor() - 1))
                | (0x8004002001LL << card->content);
            if (hand->size() == 0) {
                // Game over, change background & show everyone's hand cards
                direction = 0;
                for (int i = Player::COM1; i <= Player::COM3; ++i) {
                    player[i].sort();
                    player[i].open = MASK_ALL(this, i);
                } // for (int i = Player::COM1; i <= Player::COM3; ++i)

                qDebug("======= WINNER IS PLAYER %d =======", who);
            }
        }
    }

    return card;
}


bool UNO::challenge(int whom) {
    bool result = false;

    if (whom >= Player::YOU && whom <= Player::COM3) {
        if (whom != Player::YOU) {
            player[whom].sort();
            player[whom].open = MASK_ALL(this, whom);
        } // if (whom != Player::YOU)

        for (Card* card : player[whom].handCards) {
            if (card->color == next2lastColor()) {
                result = true;
                break;
            }
        }
    }

    qDebug("Player %d is challenged. Result = %d", whom, result);
    return result;
}


void UNO::swap(int a, int b) {
    Player store = player[a];
    player[a] = player[b];
    player[b] = store;
    if (a == Player::YOU || b == Player::YOU) {
        player[Player::YOU].sort();
        player[Player::YOU].open = MASK_ALL(this, Player::YOU);
    }

    qDebug("Player %d swapped hand cards with Player %d", a, b);
}


 void UNO::cycle() {
    int curr = now, next = getNext(), oppo = getOppo(), prev = getPrev();
    Player store = player[curr];
    player[curr] = player[prev];
    player[prev] = player[oppo];
    player[oppo] = player[next];
    player[next] = store;
    player[Player::YOU].sort();
    player[Player::YOU].open = MASK_ALL(this, Player::YOU);
    qDebug("Everyone passed hand cards to the next player");
}