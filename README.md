# Go Fish
Play the classic card game "Go Fish" against a computer. Developed using a standard 52 card deck.

## Demo
```
Welcome to C++ Go Fish!
Date: Wed Oct 27 17:09:48 2021
Enter a name for Player 1: 
Tanner
Player 1: Tanner
Player 2: AI
Goodluck!
7 cards have been randomly dealt to each player
There are 38 cards left in the deck
Tanner's cards: (7)
AI's cards: (7)
Let the games begin!
Tanner's cards: (7) Books: 0
[ Jack, 9, 2, 2, Ace, 9, King ]
Tanner: what card do you wish to ask AI for?
9
AI: do you have any 9's ?
Go fish!
... and the game continues
```

```
Go fish!
Tanner draws a card from the deck: 8
1 cards left in the deck
AI's cards: (7) Books: 2
AI: what card do you wish to ask Tanner for?
Tanner: do you have any King's ?
AI has a book! Obtained four of card: King
Tanner gives AI all of their King's (1)
AI's cards: (4) Books: 3
AI: what card do you wish to ask Tanner for?
Tanner: do you have any 3's ?
Go fish!
AI draws a card from the deck: 7
0 cards left in the deck
Game over! The deck is empty
Tanner wins! 
Results: AI: 3 books and Tanner: 6 books
Do you want to play again? (Y/N)
N
```

## Win Condition
Go Fish is over when one of the players runs out of cards or there are no more cards in the deck. The winner is determined by who has the most piles of cards or "books".

_Note: A "book" is four of the same number card_

## Game History
The game history will be written to the game-log.txt file after every game and contains:

- Winning player name
- Number of books obtained
- Date of match

```
Tanner wins with 2 books - Date: Tue Oct 26 18:06:47 2021
AI wins with 2 books - Date: Tue Oct 26 18:17:30 2021
Finch wins with 3 books - Date: Tue Oct 26 18:26:48 2021
Tanner wins with 1 book - Date: Tue Oct 26 21:11:58 2021
AI wins with 4 books - Date: Tue Oct 26 22:01:46 2021
```

# Resources
[Wikipedia - Go Fish](https://en.wikipedia.org/wiki/Go_Fish)

[Rules and Overview of Go Fish](https://www.ducksters.com/games/go_fish_rules.php)