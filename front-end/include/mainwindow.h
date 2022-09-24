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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

#include "settingwindow.h"
#include "gamewindow.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    GameWindow* m_GameWinPtr = nullptr;
    SettingWindow* m_SettingWinPtr = nullptr;

protected:
    /* Rewrite paint event to draw background */
    void paintEvent(QPaintEvent *);
    /* turn to different state */
    void mousePressEvent(QMouseEvent* event);

};

#endif