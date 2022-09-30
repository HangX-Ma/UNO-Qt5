/**
 * @file mainwindow.h
 * @author HangX-Ma (m-contour@qq.com)
 * @brief UNO main Window
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

#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QtWidgets/QMainWindow>

#include "settingwindow.h"
#include "gamewindow.h"
#include "info.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int argc, char* argv[], QWidget *parent = nullptr);
    ~MainWindow();

private:
    GameWindow* m_gameWinPtr = nullptr;
    SettingWindow* m_settingWinPtr = nullptr;
    
    QImage m_screen;
    QPainter* m_painterPtr = nullptr;

    QImage m_bg_welcome; //!< welcome image
    QImage m_bg_back;    //!< uno card back image

    Info* m_infoPtr = nullptr;

    void refreshScreen();

protected:
    /* Rewrite paint event to draw background */
    void paintEvent(QPaintEvent *);
    /* turn to different state */
    void mousePressEvent(QMouseEvent* event);

signals:
  // back to caller
  void SIG_game_win_open();

};

#endif