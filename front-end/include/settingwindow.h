/**
 * @file settingwindow.h
 * @author HangX-Ma (m-contour@qq.com)
 * @brief UNO setting Window
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

#ifndef __SETTINGWINDOW_H__
#define __SETTINGWINDOW_H__

#include <QtWidgets/QMainWindow>
#include "info.h"
#include "uno.h"
#include "sound.h"
#include "newbutton.h"

class SettingWindow : public QMainWindow 
{
    Q_OBJECT;

public:
    SettingWindow(int argc, char* argv[], Info* info, QWidget *parent = nullptr);
    ~SettingWindow();

    void start();

private:
    QImage m_screen;
    
    UNO* m_unoPtr;
    Sound* m_sndPtr;
    Info* m_infoPtr;
    QPainter* m_painterPtr;
    
    void refreshScreen(const QString& message = "");

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent* event);

signals:
    // back to caller
    void SIG_setting_win_back();
};


#endif