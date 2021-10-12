/*
 * gofish.cpp
 *
 *  Created on: Oct 11, 2021
 *      Author: TannerDolby
 */
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include <array>
#include <list>
#include <algorithm>
#include <iterator>
#include <set>
#include <assert.h>
#include <numeric>
#include <random>
#include <chrono>
using namespace std;


// What is Go Fish?
// Go Fish is a card game usually played by 2-5 players

// A standard deck of cards has four suites: hearts, clubs, spades, diamonds.
// Each suite has thirteen cards: ace, 2, 3, 4, 5, 6, 7, 8, 9, 10, jack, queen and king.
// Thus the entire deck has 52 cards total.

// Go Fish is over when one player runs out of cards or there are no more cards in the pool.
// The winner is then determined by who has the most piles or suits of cards in front of them.
// Where a "suit" is 4 of the same card number
// https://www.ducksters.com/games/go_fish_rules.php


// Cards are the nodes in the linked list
struct Node
{
	int cardNum;
	string cardVal;
	string cardSuit;
	Node *link;

};
typedef Node *nodePtr;


struct Player
{
	string name;
	bool isDealer;
	int numCards;
	int numWins;
	int movesToWin;
	Node cards[7]; // make it a big number
	vector<int> hand;
	int suits;
	Player *link;
};
typedef Player *playerPtr;

struct suits
{
	string name;
};

struct move
{
	int askingNum;
	string askingSuit;
	bool hasCard;
};

// function prototypes
void setSuitNames(suits[]);
void insertNode(nodePtr&, int, string, string);
void initGame(Player[], int);
string getNthNodeSuit(nodePtr&, int);
int getNthNodeNum(nodePtr&, int);
void deleteNode(nodePtr&, int);
void displayHand(Player[], int);
void eraseElem(vector<int>&, int);
void dealStartingHand(vector<int>&, nodePtr&, Player[]);
void populateVec(vector<int>&, int);
void classifyCard(int, bool);
void printCard(Player[], int, int, bool); // use or remove!
int choiceToNum(string choice);
string askMove(Player[], bool);
void ask(Player[], bool, string);
void handle(Player[], bool, string, int);
int exchange(Player[], int, int, int);
void checkForSuits(Player Players[], bool playerOneTurn);
int isAHandEmpty(Player[], int);
void getCurrentTime();

int STANDARD_DECK = 52;

int main()
{
	vector<int> indexes;
	bool playerOneTurn = true;
	bool gameOver = false;
	char playAgain = 'N';
	int cardsInDeck = 0;
	int cardTaken = 0;
	int cardGiven = 0;
	int numPlayers = 2; // Setup for one player versus the computer
	vector<int> asks;
	int count = 1;

	// Create an output stream to write match history file
	ofstream outStream;
	outStream.open("./gofish-match-histoy.txt", ios::app);

	// initialize array of structs representing the 4 card suits
	suits Suits[4];
	setSuitNames(Suits);

	// player array (one player vs the AI)
	Player Players[2];

	// create a new Card pointer
	nodePtr head;
	head = new Node;

	// Create a new Player pointer
	playerPtr playerHead;
	playerHead = new Player;

	// seed the random number generator
	srand(time(0));

	// Populate the linked list with our deck of cards data
	do
	{
		string royalty[3] = {"jack", "queen", "king"};
		string v = "";
		string suit = "";
		if (count == 1)
		{
			v = "ace";
		}
		else if (count < 11)
		{
			v = 1;
		}
		else
		{
			if (count == 11)
			{
				v = royalty[0];
			}
			else if (count == 12)
			{
				v = royalty[1];
			}
			else
			{
				v = royalty[2];
			}
		}
		insertNode(head, count, v, Suits[0].name);
		insertNode(head, count, v, Suits[1].name);
		insertNode(head, count, v, Suits[2].name);
		insertNode(head, count, v, Suits[3].name);
		count++;
	} while (count < 14);

	// After insertion the nodePtr temp pointer
	// is deleted, so we need another temp pointer
	// to get back to the front of the list
	nodePtr tempPtr;
	tempPtr = head;
	cardsInDeck = STANDARD_DECK;

	// purge extra node at the tail end of linked list
	deleteNode(head, 52);

	// initialize game
	initGame(Players, numPlayers);

	// each player starts with 7 cards if there are 3 or fewer players (Player One vs AI)
	Players[0].numCards = 7;
	Players[1].numCards = 7;
	Players[0].suits = 0;
	Players[1].suits = 0;

	populateVec(indexes, 52);

	// shuffle vector representing "choices" e.g. 0-51
	// while the linked lists is where the actual deck is,
	// this vector `indexes` keeps track of cards drawn
	// so I can fetch nodes at the given index in the linked list

	// add ability to reshuffle or "fresh shuffle" so every game starts with a different 7 starting cards
	random_shuffle(indexes.begin(), indexes.end());

	// deal both players 7 cards to start (2 players)
	// if 3 or more players deal 5 cards to each
	dealStartingHand(indexes, head, Players);

	cardsInDeck = indexes.size(); // 38 (52 - (7 + 7))

	cout << "7 cards have been randomly dealt to each player" << endl;
	cout << "There are " << cardsInDeck << " cards left in the deck" << endl;

	// game loop (meant for two players, future work will be adding more players, maybe introduce some classes)

	// do while deck isn't empty, alternate from player 1 to 2 asking
	while (gameOver == false)
	{
		// display Player One Cards
		cout << Players[0].name << "'s cards: (" << Players[0].hand.size() << ") ";
		displayHand(Players, 0);
		cout << endl << Players[1].name << "'s cards: (" << Players[1].hand.size() << ") ";
		displayHand(Players, 1);
		cout << "\nPlayer 2 (" << Players[1].name << ") cards: its a secret!" << endl;

		string choice = "";
		cout << indexes.size() << " size" << endl;
		int choiceNum = 0;
		string fishingCard = askMove(Players, playerOneTurn);

		// Start fishing for cards
		cout << endl;
		ask(Players, playerOneTurn, fishingCard);
		choiceNum = choiceToNum(fishingCard);

		int cardsToGive = 0;

		// handle making piles (matches of 4)
		vector<string> gameLog;

		// Exchange cards
		if (playerOneTurn)
		{

			cardsToGive = exchange(Players, 1, 0, choiceNum);
			// check for suits after and exchange happens
			checkForSuits(Players, true);
		}
		else
		{
			cardsToGive = exchange(Players, 0, 1, choiceNum);
			checkForSuits(Players, false);
		}

		cout << "Cards to give: " << cardsToGive << endl;

		// Otherwise go fish
		if (cardsToGive == 0)
		{
		    cout << "Go fish!" << endl;

		    // take card from the pool
		    // get indexes vector and get the next number in the shuffled vector
		    cout << indexes.front() << endl;
		    // get nthNodeNum and push it into the players hand vector
		    int cardTakenNum = getNthNodeNum(head, indexes.front());

		    if (playerOneTurn)
		    {
		        Players[0].hand.push_back(cardTakenNum);
		        // erase the num from vector as that card is "no longer" in deck
		        indexes.erase(indexes.begin());
		    } else
		    {
		        Players[1].hand.push_back(cardTakenNum);
		        // erase the num from vector as that card is "no longer" in deck
		        // cout << indexes.size() << " b1" << endl;
		        indexes.erase(indexes.begin());
		    }
		    playerOneTurn = !playerOneTurn;

		}
		else
		{
			handle(Players, playerOneTurn, fishingCard, cardsToGive);

		}


		// if deck is empty OR when one player runs out of cards
		cout << indexes.size() << " deck size" << endl;

		// It would also be nice to know which player had the empty hand
		if (Players[0].hand.size() == 0 || Players[1].hand.size() == 0)
		{
			gameOver = true;
			int playerHandEmpty = isAHandEmpty(Players, 2);
			// determine which players hand was empty
			cout << "Game Over! ";
			if (playerHandEmpty == 0)
			{
				cout << Players[0].name << " has run out of cards" << endl;
				cout << Players[1].name << " wins!" << endl;
				// todo: show number of moves (game turns) winning player had, and number of suits acquired
				outStream << Players[1].name << " wins!" << endl;

			}
			else if (playerHandEmpty == 1)
			{
				cout << Players[1].name << " has run out of cards" << endl;
				cout << Players[0].name << " wins!" << endl;
				// todo: player N wins in 'X' turns with 'X' suits
				outStream << Players[0].name << " wins!" << endl;
			}
			cout  << "P1: " << Players[0].hand.size() << endl;
			cout  << "P2: " << Players[1].hand.size() << endl;
		}
		else if (indexes.size() == 0)
		{
			// check to see who has the most "suits"
			// that is 4 of the same number card
			// suits should be made for players during their
			// turn
			// todo: add logic to properly look for matches of 4
			// and update cards in console

		}



	}

	// When indexes.size() is 0 OR a players hand is 0, the game is over and
	// the winner is determined by
	cout << indexes.size() << " indexes left to choose from!" << endl;

	// iterate through the linked list representing original stack of cards
	//while (tempPtr != NULL)
	//{
		//cout << "Card Num/Val: " << tempPtr -> cardNum << " Suit: " << tempPtr -> cardSuit;
		//cout << " - Spec: " << tempPtr -> cardVal << endl;
		//tempPtr = tempPtr -> link;
		//len++;
	//}

	return 0;
}
// Check if a players hand is empty and return the player index
int isAHandEmpty(Player Players[], int numPlayers)
{
	int counter = 0;
	int playerIdx = -1;
	bool isEmpty = false;
	for (int i = 0; i < numPlayers; i++)
	{
		cout << Players[i].hand.size() << endl;
		if (Players[i].hand.size() == 0)
		{
			isEmpty = true;
			playerIdx = counter;
		}
		counter++;
	}
	return playerIdx;
}

// TODO
// check for suits (that is 4 of the same number card)
void checkForSuits(Player Players[], bool playerOneTurn)
{
	int cardToRemove = 0;
	int counter = 0;
	vector<int> occArr;
	vector<int> suitLog;
	if (playerOneTurn)
	{
		for (int i = 0; i < Players[0].hand.size(); i++)
		{
			int occurences = count(Players[0].hand.begin(), Players[0].hand.end(), Players[0].hand[i]);
			occArr.push_back(occurences);
			cout << Players[0].hand[i] << " - OCC: " << occurences << endl;
		}

		// iterate the occurrence array
		// todo: handle when card added to hand is apart of a suit (4 of the same number)
		// and is the end of the vector.
		for (int j = 0; j < occArr.size(); j++)
		{
			cout << occArr[j] << " OCC ARR" << endl;
			// When an item in the occurence array is >= 4 then remove 4 of that card num from hand
			if (occArr[j] >= 2)
			{

				cardToRemove = Players[0].hand[j];
				cout << "CARD TO REMOVE " << cardToRemove << endl;

				// keep track of which suits are accumulated for match history
				suitLog.push_back(cardToRemove);
				Players[0].suits += 1;

				// there is a suit (4 of the same number cards
				vector<int>::iterator itr = Players[0].hand.begin() + j;
				cout << "removing card" << cardToRemove << endl;

				    // remove cards (add limit to removed # of cards to ensure only the expected cards are removed)
				    while (Players[0].hand[j] == cardToRemove)
				    {
				    	// AND the number of cards to take matches
				    	cout << "Counter: " << counter << endl;

				    	if (itr == Players[0].hand.end()-1)
				    	        {
				    	            Players[0].hand.pop_back();
				    	            break;
				    	        }
				    	        else
				    	        {
				    	            if (itr != Players[0].hand.end()-1)
				    	            {
				    	                Players[0].hand.erase(itr);
				    	            }
				    	        }
				    	counter++;
				    }
			}

		}


		cout << Players[0].suits << " SUITS " << endl;
		displayHand(Players, 0);
		cout << suitLog.size() << " suitLog size" << endl;
//		for (int j = 0; j < suitLog.size(); j++)
//		{
//			cout << suitLog[j] << " SUIT LOG" << endl;
//		}
	}
}

void getCurrentTime()
{
	auto start = std::chrono::system_clock::now();
	std::time_t time = std::chrono::system_clock::to_time_t(start);
	cout << "Date: " << ctime(&time);
}


int exchange(Player Players[], int giver, int taker, int choiceNum)
{
	int cardsToGive = 0;
	for (int i = 0; i < Players[giver].hand.size(); i++)
	{
	    while (Players[giver].hand[i] == choiceNum)
	    {
	        cardsToGive++;
	        // add cards to asking players hand
	        Players[taker].hand.push_back(Players[giver].hand[i]);

	        // and remove them from the giving players hand
	        vector<int>::iterator it = Players[giver].hand.begin() + i;

	        // if the element to be erased is the last element in vector
	        // erase it with pop_back()
	        if (it == Players[giver].hand.end()-1)
	        {
	        	Players[giver].hand.pop_back();
	        	break;
	        }
	        else
	        {
	        	// otherwise if the card still matches choiceNum
	        	// and its not the last element, erase it with .erase()
	        	if (it != Players[giver].hand.end()-1)
	        	{
	        		Players[giver].hand.erase(it);
	        	}
	        }
	    }
	}
	return cardsToGive;
}

void handle(Player Players[], bool playerOneTurn, string choice, int cardsToGive)
{
	if (playerOneTurn)
	{
		cout << Players[1].name << " gives " << Players[0].name << " all of their " << choice << "'s (" << cardsToGive << ")" << endl;
	}
	else
	{
		cout << Players[0].name << " gives " << Players[1].name << " all of their " << choice << "'s (" << cardsToGive << ")" << endl;
	}
}

void ask(Player Players[], bool playerOneTurn, string choice)
{
	if (playerOneTurn)
	{
		cout <<  Players[1].name << ", do you have any " << choice << "'s " << "?"<< endl;
	}
	else
	{
		cout << Players[0].name << ", do you have any " << choice << "'s " << "?" << endl;
	}

}

string askMove(Player Players[], bool playerOneTurn)
{
	string choice = "";
	if (playerOneTurn)
	{
		cout << Players[0].name << "'s cards: (" << Players[0].hand.size() << ") ";
		displayHand(Players, 0);
		cout << endl << Players[0].name << ": what card do you wish to ask " << Players[1].name << " for?" << endl;
		cin >> choice;
	}
	else
	{
		cout << endl << Players[1].name << ": what card do you wish to ask " << Players[0].name << " for?" << endl;
		cin >> choice;
	}
	return choice;
}


// Erase an element at the specified index in a vector<T>
void eraseElem(vector<int>& vec, int index)
{
	for (int i = 0; i < vec.size(); i++)
	{
		// erase the nth element
		vec.erase(vec.begin() + index);
	}
}

void populateVec(vector<int>& indexes, int size)
{
	for (int i = 0; i < size; i++)
	{
		indexes.push_back(i);
	}
}


// Delete node from the linked list representing card deck
void deleteNode(nodePtr& head, int index)
{
	// end of the linked list (or empty)
	if (head == NULL)
	{
		return; // bail
	}

	// store head node, get temp to the front of the list
	Node* temp = head;

	if (index == 0)
	{
		// move to the next node
		head = temp -> link;

		// free up old pointer
		free(temp);
		return;
	}
	// Find previous node of the node to be deleted
	for (int i = 0; temp != NULL && i < index - 1; i++)
	{
		temp = temp -> link;
	}
	// if position is out of the range of nodes
	if (temp == NULL || temp -> link == NULL)
	{
		cout << "pointer is out of linked list range" << endl;
		return;
	}

	// Card temp -> link is the node to be deleted
	// so we store the pointer to the next node
	Node *link = temp -> link -> link;

	// Unlink the node and free memory
	free(temp -> link);

	// Unlink the deleted node
	temp -> link = link;
}

void dealStartingHand(vector<int>& indexes, nodePtr& head, Player Players[])
{
	int tempNum;
	string tempSuit;

	// create a temporary Node struct to populate each players initial hand of 7 cards
	Node tempCard;

	// set to ensure 7 unique and random numbers occur for a hand
	set<int> s1;
	// a vector to store unique random numbers from set, so I can easily use random_shuffle()
	vector<int> s1Help;

	for (int i = 0; i < 14; i++)
	{

		int r = rand() % indexes.size() + 1;
		if (r == 52) r -= 1;

		// insert random numbers to set in order
		// to create a uniqe set of 7 random numbers
		s1.insert(r);
	}

	while (s1.size() != 14)
	{
		int rn = (rand() % indexes.size()) + 1;
		if (rn == 52) rn -= 1;
		s1.insert(rn);
	}

	int ct = 0;
	set<int>::iterator itr;
	int p2 = 0;
	// iterate through the set of unique random numbers
	// adding them to the helper vector s1Help
	for (itr = s1.begin(); itr != s1.end(); itr++)
	{
		s1Help.push_back(*itr);
	}

	// shuffle the vector containing unique random numbers
	// from the set
	random_shuffle(s1Help.begin(), s1Help.end());

	//for (itr = s1.begin(); itr != s1.end(); itr++)
	for (int i = 0; i < s1Help.size(); i++)
	{
		//cout << *itr << endl;
		//cout << s1Help[i] << endl;
		int pos = 0;

		// Clean this up! =)
		if (ct < 7) {
			// refactor this and abstract into a function
			pos = find(indexes.begin(), indexes.end(), s1Help[i]) - indexes.begin();
			tempNum = getNthNodeNum(head, s1Help[i]);
			tempSuit = getNthNodeSuit(head, s1Help[i]);
			tempCard.cardNum = tempNum;
			tempCard.cardSuit = tempSuit;
			Players[0].hand.push_back(tempNum);
			Players[0].cards[ct] = tempCard;
			indexes.erase(indexes.begin() + pos);
		} else {
			pos = find(indexes.begin(), indexes.end(), s1Help[i]) - indexes.begin();
			// Handle player 2 (AI) random cards, this
			// is where more players could get random cards assigned too
			// this logic would need to be updated to better scale
			tempNum = getNthNodeNum(head, s1Help[i]);
			tempSuit = getNthNodeSuit(head, s1Help[i]);
			tempCard.cardNum = tempNum;
			tempCard.cardSuit = tempSuit;
			Players[1].hand.push_back(tempNum);
			Players[1].cards[p2] = tempCard;
			indexes.erase(indexes.begin() + pos);
			p2++;
		}
		ct++;
	}
}

// pass vector by reference as parameter to keep track of
// random numbers
void pushNum(vector<int> &indexes, int index)
{
  indexes.push_back(index);
}

string getNthNodeSuit(nodePtr& head, int index)
{
	Node* current = head;
	// the index of the node were currently looking at
	int count = 0;

	while (current != NULL)
	{
		if (count == index)
		{
			return current -> cardSuit;
		}
		count++;
		current = current -> link;
	}
	// bail when trying to get a non existent node
	assert(0);
}

int getNthNodeNum(nodePtr& head, int index)
{
	// front of the list
	Node* current = head;
	int count = 0;

	// While were not at the end of the list
	while (current != NULL)
	{
		if (count == index)
		{
			return current -> cardNum;
		}
		count++;
		current = current -> link;
	}

	// bail when node is not in the list
	assert(0);
}

void classifyCard(int card, bool last)
{
	if (last)
	{
		if (card == 1)
		{
		    cout << "Ace";
		}
		else if (card > 1 & card < 11) {
		    cout << card;
		} else {
		    if (card == 11)
		    {
		        cout << "Jack";
		    }
		    else if (card == 12)
		    {
		        cout << "Queen";
		    }
		    else if (card == 13) {
		        cout << "King";
		    }
		}
	}
	else
	{
	if (card == 1)
		{
			cout << "Ace" << ", ";
		}
		else if (card > 1 & card < 11) {
			cout << card << ", ";
		} else {
			if (card == 11)
			{
				cout << "Jack" << ", ";
			}
			else if (card == 12)
			{
				cout << "Queen" << ", ";
			}
			else if (card == 13) {
				cout << "King" << ", ";
			}
		}
	}
}

// no need for card suit, use the vector `hand` instead for easier tracking to show up-to-date hand
void printCard(Player Players[], int playerIndex, int index, bool last) {
	int card = Players[playerIndex].hand[index];

	if (last)
	{
		classifyCard(card, true);
	}
	else
	{
		classifyCard(card, false);
	}

}

void displayHand(Player Players[], int index)
{
	cout << "[ ";
	for (int i = 0; i < Players[index].hand.size(); i++)
	{

		if (i == Players[index].hand.size()-1)
		{
			printCard(Players, index, i, true);
		}
		else
		{
			printCard(Players, index, i, false);
		}

	}
	cout << " ]";
}

void insertNode(nodePtr& head, int cardNum, string cardVal, string cardSuit)
{
	// declare new pointer
	nodePtr tempPtr;
	tempPtr = new Node;

	// Assign the data
	tempPtr -> cardNum = cardNum;
	tempPtr -> cardVal = cardVal;
	tempPtr -> cardSuit = cardSuit;
	tempPtr -> link = head;

	// redirect pointer to front of the list
	head = tempPtr;
}

void initGame(Player Players[], int numPlayers)
{
	cout << "Welcome to C++ Go Fish!" << endl;
	getCurrentTime();
	cout << "Enter a name for Player 1: " << endl;
	cin >> Players[0].name;
	Players[1].name = "AI";
	cout << "Player 1: " << Players[0].name << endl;
	cout << "Player 2: " << Players[1].name << endl;
	cout << "Goodluck!" << endl;
	for (int i = 0; i < numPlayers; i++)
	{
		Players[i].suits = 0;
	}
}

int choiceToNum(string choice)
{
	int choiceNum = 0;
	if (choice == "Ace")
	{
		choiceNum = 1;
	}
	else if (choice == "2")
	{
		choiceNum = 2;
	}
	else if (choice == "3")
	{
		choiceNum = 3;
	}
	else if (choice == "4")
	{
		choiceNum = 4;
	}
	else if (choice == "5")
	{
		choiceNum = 5;
	}
	else if (choice == "6")
	{
		choiceNum = 6;
	}
	else if (choice == "7")
	{
		choiceNum = 7;
	}
	else if (choice == "8")
	{
		choiceNum = 8;
	}
	else if (choice == "9")
	{
		choiceNum = 9;
	}
	else if (choice == "10")
	{
		choiceNum = 10;
	}
	else if (choice == "Jack")
	{
		choiceNum = 11;
	}
	else if (choice == "Queen")
	{
		choiceNum = 12;
	}
	else if (choice == "King")
	{
		choiceNum = 13;
	}
	return choiceNum;
}

void setSuitNames(suits Suits[])
{
	string suiteNames[4] = {"hearts", "clubs", "spades", "diamonds"};
	for (int i = 0; i < 4; i++)
	{
		Suits[i].name = suiteNames[i];
	}
}
