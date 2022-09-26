/**
 * @file newButton.h
 * @author HangX-Ma (m-contour@qq.com)
 * @brief UNO card content enum
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

#ifndef CONTENT_H
#define CONTENT_H

typedef enum {
    NUM0, NUM1, NUM2, NUM3, NUM4,
    NUM5, NUM6, NUM7, NUM8, NUM9,
    DRAW2, SKIP, REV, WILD, WILD_DRAW4
} Content;

#endif