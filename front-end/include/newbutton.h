/**
 * @file newButton.h
 * @author HangX-Ma (m-contour@qq.com)
 * @brief self design button class
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

#ifndef __NEWBUTTON_H__
#define __NEWBUTTON_H__

#include <QtWidgets/QPushButton>
#include <QtCore/QPropertyAnimation>

class NewButton : public QPushButton
{
    Q_OBJECT

public:
    inline NewButton(QString name, int btnW, int btnH) {
        this->setText(name);
        this->setObjectName("btn_" + name);
        this->setStyleSheet("QPushButton{color: white; border:none;}"
                            "QPushButton::hover{color: #BDB76B; border:none;}"
                            "QPushButton::pressed{color: #BDB76B; border:none;}");
        this->setFixedSize(btnW, btnH);
    }
    inline ~NewButton() {};

    // animation
    inline void btnPressed() {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
        
        animation->setDuration(100);
        /* change position */
        animation->setStartValue(QRect(this->x(), this->y(), this->width(), this->height()));
        animation->setEndValue(QRect(this->x(), this->y() + 5, this->width(), this->height()));
        /* bounce curve */
        animation->setEasingCurve(QEasingCurve::OutBounce);
        animation->start();
    }

    inline void btnReleased() {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
        
        animation->setDuration(100);
        /* change position */
        animation->setStartValue(QRect(this->x(), this->y() + 5, this->width(), this->height()));
        animation->setEndValue(QRect(this->x(), this->y(), this->width(), this->height()));
        /* bounce curve */
        animation->setEasingCurve(QEasingCurve::OutBounce);
        animation->start();
    }
};

#endif
