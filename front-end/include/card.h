#ifndef __CARD_H__
#define __CARD_H__


#include <QtGui/QImage>
#include <QtCore/QString>
#include "color.h"
#include "content.h"

class Card
{

public:
    const Color   color;        //!< Card's color, e.g. Color::BLUE
    const Content content;      //!< Card's content, e.g. Content::NUM3
    const QString name;         //!< Card's name, e.g. "Blue 3"

    const QImage  normalImg;    //!< Card's normal image resource
    const QImage  darkImg;      //!< Card's dark image resource


    /**
     * @brief Override relation operator "<".
     *
     * @param that Provide another Card instance.
     * @return true if this->id < that.id.
     */
    inline bool operator<(const Card& that) {
        return this->id < that.id;
    }

    /**
     * @brief Override relation operator "<=".
     *
     * @param that Provide another Card instance.
     * @return true if this->id <= that.id.
     */
    inline bool operator<=(const Card& that) {
        return this->id <= that.id;
    }

    /**
     * @brief Override relation operator "==".
     *
     * @param that Provide another Card instance.
     * @return true if this->id == that.id.
     */
    inline bool operator==(const Card& that) {
        return this->id == that.id;
    }

    /**
     * @brief Override relation operator ">=".
     *
     * @param that Provide another Card instance.
     * @return true if this->id >= that.id.
     */
    inline bool operator>=(const Card& that) {
        return this->id >= that.id;
    }

    /**
     * @brief  Override relation operator ">".
     *
     * @param that Provide another Card instance.
     * @return true if this->id > that.id.
     */
    inline bool operator>(const Card& that) {
        return this->id > that.id;
    }

    /**
     * @brief Override relation operator "!=".
     *
     * @param that Provide another Card instance.
     * @return true if this->id != that.id.
     */
    inline bool operator!=(const Card& that) {
        return this->id != that.id;
    }

    /**
     * @return Whether the card is a [wild] or [wild +4].
     */
    inline bool isWild() {
        return color == NONE;
    }

private:
    /**
     * @brief Card's ID, has the value of 39 + content (for a wild card)
     * or 13 * (color - 1) + content (for a non-wild card)
     */
    const int id;

    /**
     * @brief Color part of card name
     * 
     * @param color color enum
     * @return color name list pointer
     */
    inline static const QString& A(Color color) {
        static QString a[] = { "", "Red ", "Blue ", "Green ", "Yellow " };
        return a[color];
    }

    /**
     * @brief content part of card name
     * 
     * @param content content enum
     * @return content name list pointer
     */
    inline static const QString& B(Content content) {
        static QString b[] = {
            "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
            "+2", "Skip", "Reverse", "Wild", "Wild +4"
        };
        return b[content];
    }

    /**
     * Constructor. Provide parameters for an Uno card and create its instance.
     */
    inline Card(QImage normalImg, QImage darkImg, Color color, Content content) :
        color(color),
        content(content),
        name(A(color) + B(content)),
        normalImg(normalImg),
        darkImg(darkImg),
        id(isWild() ? 39 + content : 13 * (color - 1) + content) {
    }

    /**
     * @brief UNO class to access our constructors (to create Card instances) and
     * our private fields (to change the wild color when necessary).
     */
    friend class UNO;
};



#endif