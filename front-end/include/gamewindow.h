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

#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QtWidgets/QMainWindow>


class GameWindow : public QMainWindow 
{
    Q_OBJECT;

public:
    GameWindow(QWidget *parent = nullptr);
    ~GameWindow();
};


#endif