#ifndef __INFO_H__
#define __INFO_H__

#include <QtCore/QString>

class Info 
{

public:
    static inline Info* getInstance() {
        static Info instance;
        return &instance;
    }

    inline QString act_drawCard(int i, const QString& s) {
        return p(i) + ": Draw " + s;
    }

    inline QString act_drawCardCount(int i1, int i2) {
        return i2 == 1
            ? p(i1) + ": Draw a card"
            : p(i1) + ": Draw " + QString::number(i2) + " cards";
    }

    inline QString act_pass(int i) {
        return p(i) + ": Pass";
    }

    inline QString act_playCard(int i, const QString& s) {
        return p(i) + ": " + s;
    }

    inline QString act_playDraw2(int i1, int i2, int i3) {
        return p(i1) + ": Let " + p(i2) + " draw "
            + QString::number(i3) + " cards";
    }

    inline QString act_playRev(int i) {
        return p(i) + ": Change direction";
    }

    inline QString act_playSkip(int i1, int i2) {
        return i2 == 0
            ? p(i1) + ": Skip your turn"
            : p(i1) + ": Skip " + p(i2) + "'s turn";
    }

    inline QString act_playWild(int i1, int i2) {
        return p(i1) + ": Change color to " + c(i2);
    }

    inline QString act_playWildDraw4(int i1, int i2) {
        return p(i1) + ": Change color & let " + p(i2) + " draw 4";
    }

    inline QString ask_challenge(int i) {
        return "^ Do you think your previous player still has " + c(i) + "?";
    }

    inline QString ask_color() {
        return "^ Specify the following legal color";
    }

    inline QString ask_target() {
        return "^ Specify the target to swap hand cards with";
    }

    inline QString btn_auto() {
        return "<AUTO>";
    }

    inline QString btn_keep() {
        return "<KEEP>";
    }

    inline QString btn_off() {
        return "<OFF>";
    }

    inline QString btn_on() {
        return "<ON>";
    }

    inline QString btn_play() {
        return "<PLAY>";
    }

    inline QString btn_settings() {
        return "<SETTINGS>";
    }

    inline QString info_0_rotate() {
        return "Hand cards transferred to next";
    }

    inline QString info_7_swap(int i1, int i2) {
        return p(i1) + " swapped hand cards with " + p(i2);
    }

    inline QString info_cannotDraw(int i1, int i2) {
        return p(i1) + " cannot hold more than "
            + QString::number(i2) + " cards";
    }

    inline QString info_cannotPlay(const QString& s) {
        return "Cannot play " + s;
    }

    inline QString info_challenge(int i1, int i2, int i3) {
        return i2 == 0
            ? p(i1) + " doubted that you still have " + c(i3)
            : p(i1) + " doubted that " + p(i2) + " still has " + c(i3);
    }

    inline QString info_challengeFailure(int i) {
        return i == 0
            ? "Challenge failure, you draw 6 cards"
            : "Challenge failure, " + p(i) + " draws 6 cards";
    }

    inline QString info_challengeSuccess(int i) {
        return i == 0
            ? "Challenge success, you draw 4 cards"
            : "Challenge success, " + p(i) + " draws 4 cards";
    }

    inline QString info_clickAgainToPlay(const QString& s) {
        return "Click again to play " + s;
    }

    inline QString info_dirChanged() {
        return "Direction changed";
    }

    inline QString info_gameOver() {
        return "Game Over. Click the card deck to restart";
    }

    inline QString info_ready() {
        return "GET READY";
    }

    inline QString info_ruleSettings() {
        return "RULE SETTINGS";
    }

    inline QString info_skipped(int i) {
        return p(i) + ": Skipped";
    }

    inline QString info_welcome() {
        return "WELCOME TO UNO CARD GAME, CLICK UNO TO START";
    }

    inline QString info_yourTurn() {
        return "Select a card to play, or draw a card from deck";
    }

    inline QString info_yourTurn_stackDraw2(int i) {
        return "Stack a +2 card, or draw "
            + QString::number(i) + " cards from deck";
    }

    inline QString label_7_0() {
        return "7 to swap, 0 to rotate:";
    }

    inline QString label_bgm() {
        return "BGM";
    }

    inline QString label_draw2Stack() {
        return "+2 can be stacked:";
    }

    inline QString label_forcePlay() {
        return "When you draw a playable card:";
    }

    inline QString label_level() {
        return "LEVEL";
    }

    inline QString label_no() {
        return "NO";
    }

    inline QString label_players() {
        return "PLAYERS";
    }

    inline QString label_remain_used(int i1, int i2) {
        return "Remain/Used: " + QString::number(i1)
            + "/" + QString::number(i2);
    }

    inline QString label_score() {
        return "SCORE";
    }

    inline QString label_snd() {
        return "SND";
    }

    inline QString label_yes() {
        return "YES";
    }

private:
    Info() = default;

    inline static const QString& c(int i) {
        static const QString UNKNOWN = "????";
        static const QString S[] = { "NONE", "RED", "BLUE", "GREEN", "YELLOW" };
        return 0 <= i && i <= 4 ? S[i] : UNKNOWN;
    }

    inline static const QString& p(int i) {
        static const QString UNKNOWN = "????";
        static const QString S[] = { "YOU", "WEST", "NORTH", "EAST" };
        return 0 <= i && i <= 3 ? S[i] : UNKNOWN;
    }

};


#endif