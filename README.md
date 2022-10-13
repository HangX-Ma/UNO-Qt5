#  :rocket:`ZJUER:` UNO-Qt5
UNO card game based on Qt5. Original Repository is here [shiawasenahikari/UnoCard](https://github.com/shiawasenahikari/UnoCard)

## Platform
- Ubuntu 20.04
- Qt 5.15.2
- Doxygen and graphviz


## Usage
If you want to generate Doxygen file, `DBUILD_DOCUMENTATION` needs to be set `True`. You will find `index.html` in `build/docs/html` directory.
```shell
cd UNO-Qt5 && mkdir build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_DOCUMENTATION=True -B build -S .
cmake --build build
```

## Game Rules

1. Each player draws 7 cards to set up a new UNO game. Player to the left of the dealer plays first. Play passes to the left to start (**YOU->WEST->NORTH->EAST)**.

2. Match the top card on the **DISCARD** pile either by color or content. For example, if the card is a Green 7 [#1](UNORes/front_g7.png), you must play a Green card [#2](UNORes/front_g9.png), or a 7 card with another color [#3](UNORes/front_b7.png). Or, you may play any `[wild]` [#4](UNORes/front_kw.png) or `[wild +4]` [#5](UNORes/front_kw+.png) card. If you don't have anything that matches, or you just don't want to play any of your playable cards, you must pick a card from the DRAW pile. If you draw a card you can play, you may play it immediately. Otherwise, play moves to the next person.

<p align="center">
<img src="https://github.com/HangX-Ma/UNO-Qt5/raw/main/UNORes/front_g7.png" />
<img src="https://github.com/HangX-Ma/UNO-Qt5/raw/main/UNORes/front_g9.png" />
<img src="https://github.com/HangX-Ma/UNO-Qt5/raw/main/UNORes/front_b7.png" />
<img src="https://github.com/HangX-Ma/UNO-Qt5/raw/main/UNORes/front_kw.png" />
<img src="https://github.com/HangX-Ma/UNO-Qt5/raw/main/UNORes/front_kw+.png" />
</p>

1. When you play a +2 (**Draw Two**) card, the next person to play must draw 2 cards and forfeit his/her turn. This card may only be played on a matching color or on another +2 card.

<p align="center">
<img src="https://github.com/HangX-Ma/UNO-Qt5/raw/main/UNORes/front_r+.png" />
</p>

4. When you play a 🔄 (**Reverse**), reverse direction of play. Play to the left now passes to the right, and vice versa. This card may only be played on a matching color or on another 🔄 card.

<p align="center">
<img src="https://github.com/HangX-Ma/UNO-Qt5/raw/main/UNORes/front_yS.png" />
</p>

5. When you play a 🚫 (**Skip**) card, the next person to play loses his/her turn and is "skipped". This card may only be played on a matching color or on another 🚫 card.

<p align="center">
<img src="https://github.com/HangX-Ma/UNO-Qt5/raw/main/UNORes/front_bF.png" />
</p>

6. When you play a ⊕ (**Wild**) card, you may change the color being played to any color (including current color) to continue play. You may play a ⊕ card even if you have another playable card in hand.

<p align="center">
<img src="https://github.com/HangX-Ma/UNO-Qt5/raw/main/UNORes/front_kw.png" />
</p>

7. When you play a +4 **(Wild Draw Four)** card, call the next color played, require the next player to pick 4 cards from the **DRAW** pile, and for his/her turn. However, there is a hitch! You can only play this card when you don't have a card in your hand that matches the color of the previously played card. If you suspect that your previous player has played a +4 card illegally, you may challenge him/her. A challenged player must show his/her hand to the player who challenged. If the challenged player actually used the +4 card illegally, the challenged player draws 4 cards instead. On the other hand, if the challenged player is not guilty, the challenger must draw the 4 cards, plus 2 additional cards.

<p align="center">
<img src="https://github.com/HangX-Ma/UNO-Qt5/raw/main/UNORes/front_kw+.png" />
</p>

8. Before playing your next-to-last card, you must say "**UNO**". In this application, "**UNO**" will be said automatically. Who successfully played his/her last card becomes the winner.

### Special Rules

#### How to Enable
Click the `<SETTING>` button in the left-bottom corner, and enable your favorite special rules.

#### 7-0

- When someone plays a 7, that player must swap hands with another player.
- When anyone plays a 0, everyone rotates hands in the direction of play.

#### Stack
- +2 cards can be stacked. A player that can't add to the stack must draw the total.

## Code Structure

## Contributor
