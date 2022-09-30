#ifndef __SOUND_H__
#define __SOUND_H__

#include <QtCore/QUrl>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtMultimedia/QSoundEffect>


class Sound : public QObject {
    Q_OBJECT

private:
    bool enabled;
    QThread thread;
    QSoundEffect* sndUno;
    QSoundEffect* sndWin;
    QSoundEffect* sndLose;
    QSoundEffect* sndDraw;
    QSoundEffect* sndPlay;

    Sound(QObject* = nullptr) {
        sndUno = new QSoundEffect;
        sndWin = new QSoundEffect;
        sndLose = new QSoundEffect;
        sndDraw = new QSoundEffect;
        sndPlay = new QSoundEffect;

        sndUno->setSource(QUrl::fromLocalFile(":/UNORes/snd_uno.wav"));
        sndWin->setSource(QUrl::fromLocalFile(":/UNORes/snd_win.wav"));
        sndLose->setSource(QUrl::fromLocalFile(":/UNORes/snd_lose.wav"));
        sndDraw->setSource(QUrl::fromLocalFile(":/UNORes/snd_draw.wav"));
        sndPlay->setSource(QUrl::fromLocalFile(":/UNORes/snd_play.wav"));

        sndUno->moveToThread(&thread);
        sndWin->moveToThread(&thread);
        sndLose->moveToThread(&thread);
        sndDraw->moveToThread(&thread);
        sndPlay->moveToThread(&thread);

        connect(this, SIGNAL(playSndUno()), sndUno, SLOT(play()));
        connect(this, SIGNAL(playSndWin()), sndWin, SLOT(play()));
        connect(this, SIGNAL(playSndLose()), sndLose, SLOT(play()));
        connect(this, SIGNAL(playSndDraw()), sndDraw, SLOT(play()));
        connect(this, SIGNAL(playSndPlay()), sndPlay, SLOT(play()));

        connect(&thread, &QThread::finished, this, &QObject::deleteLater);
        
        enabled = true;
        thread.start();
    }

    ~Sound() {
        thread.quit();
        thread.wait();
    }

public:
    static const int SND_UNO = 0;
    static const int SND_WIN = 1;
    static const int SND_LOSE = 2;
    static const int SND_DRAW = 3;
    static const int SND_PLAY = 4;

    static Sound* getInstance() {
        static Sound instance;
        return &instance;
    }

    inline void setEnabled(bool enabled) {
        this->enabled = enabled;
    }

    inline bool isEnabled() {
        return enabled;
    }

signals:
    void playSndUno();
    void playSndWin();
    void playSndLose();
    void playSndDraw();
    void playSndPlay();

public slots:
    inline void play(int which) {
        if (enabled) {
            switch (which) {
                case SND_UNO: emit this->playSndUno(); break; // case SND_UNO
                case SND_WIN: emit this->playSndWin(); break; // case SND_WIN
                case SND_LOSE: emit this->playSndLose(); break; // case SND_LOSE
                case SND_DRAW: emit this->playSndDraw(); break; // case SND_DRAW
                case SND_PLAY: emit this->playSndPlay(); break; // case SND_PLAY
                default: break;
            }
        }
    }
};

#endif