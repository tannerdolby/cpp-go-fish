/*
 * gofish.cpp
 *
 *  Created on: Oct 27, 2021
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


/*
 * What is Go Fish?
 * Go Fish is a card game usually played by 2-5 players
 * A standard deck of cards has four suites: hearts, clubs, spades, diamonds.
 * Each suite has thirteen cards: ace, 2, 3, 4, 5, 6, 7, 8, 9, 10, jack, queen and king.
 * Thus the entire deck has 52 cards total.
 *
 * Go Fish is over when one player runs out of cards or there are no more cards in the pool.
 * The winner is then determined by who has the most piles or suits of cards in front of them.
 * Where a "book" is 4 of the same card number - Reference: https://www.ducksters.com/games/go_fish_rules.php
 */

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
	Node cards[7];
	vector<int> hand;
	int books;
	Player *link;
};
typedef Player *playerPtr;

struct suits
{
	string name;
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
void printCard(Player[], int, int, bool);
int choiceToNum(string choice);
string numToChoice(int choice);
string askMove(Player[], bool, int);
void ask(Player[], bool, string);
void handle(Player[], bool, string, int);
int exchange(Player[], int, int, int);
void checkForBooks(Player[], int);
int isAHandEmpty(Player[], int);
void getCurrentTime();
void displayEmptyHandMsg(Player[], int);

int STANDARD_DECK = 52;

int main()
{
//	vector<int> indexes;
	bool playerOneTurn = true;
	bool gameOver = false;
	char playAgain;
	int cardsInDeck = 0;
	int numPlayers = 2;
	vector<int> asks;

	// create an output stream to write match history file
	ofstream outStream;
	outStream.open("match-history.txt", ios::app);

	do {
		gameOver = false;
		int count = 1;
		vector<int> indexes;
		suits Suits[4];
		setSuitNames(Suits);

		// player array (one player vs the AI)
		Player Players[2];

		// create a new Card pointer
		nodePtr head;
		head = new Node;

		// create a new Player pointer
		playerPtr playerHead;
		playerHead = new Player;

		// seed the random number generator
		srand(time(0));

		// Populate the linked list with a deck of cards
		// 13 cards in each of the 4 suits
		do {
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

		// populate the vector that keeps track
		// of random indexes mirroring the linked list positions
		populateVec(indexes, 52);

		// random shuffle the vector representing card "choices" e.g. 0-51
		// while the linked lists is where the actual deck is
		random_shuffle(indexes.begin(), indexes.end());

		// deal both players 7 cards to start (2 players)
		// if 3 or more players deal 5 cards to each
		dealStartingHand(indexes, head, Players);
		cardsInDeck = indexes.size();

		cout << "7 cards have been randomly dealt to each player" << endl;
		cout << "There are " << cardsInDeck << " cards left in the deck" << endl;

		// Display initial hand of cards for both players
		cout << Players[0].name << "'s cards: (" << Players[0].hand.size() << ")" << endl;
		cout << Players[1].name << "'s cards: (" << Players[1].hand.size() <<  ")" << endl;
		cout << "Let the games begin!" << endl;

		while (gameOver == false)
		{
			string choice = "";
			int choiceNum = 0;
			string word = "books";
			int rootsChoice = (rand() % 13) + 1;
			string fishingCard = askMove(Players, playerOneTurn, rootsChoice);

			// Start fishing for cards
			ask(Players, playerOneTurn, fishingCard);
			choiceNum = choiceToNum(fishingCard);

			int cardsToGive = 0;

			// Exchange cards
			if (playerOneTurn)
			{
				cardsToGive = exchange(Players, 1, 0, choiceNum);
				checkForBooks(Players, 0);
			}
			else
			{
				cardsToGive = exchange(Players, 0, 1, choiceNum);
				checkForBooks(Players, 1);
			}

			if (cardsToGive == 0)
			{
			    cout << "Go fish!" << endl;

			    // take card from the pool
			    // get indexes vector and get the next number in the shuffled vector
			    // get nthNodeNum and push it into the players hand vector
			    int cardTakenNum = getNthNodeNum(head, indexes.front());

			    if (playerOneTurn)
			    {
			    	cout << Players[0].name << " draws a card from the deck: " << cardTakenNum << endl;
			        Players[0].hand.push_back(cardTakenNum);
			        // erase the num from the front of the vector as that card is "no longer" in deck
			        indexes.erase(indexes.begin());
			    } else
			    {
			    	cout << Players[1].name << " draws a card from the deck: " << cardTakenNum << endl;
			        Players[1].hand.push_back(cardTakenNum);
			        indexes.erase(indexes.begin());
			    }
			    playerOneTurn = !playerOneTurn;
				cout << indexes.size() << " cards left in the deck" << endl;
			}
			else
			{
				handle(Players, playerOneTurn, fishingCard, cardsToGive);

			}

			auto start = std::chrono::system_clock::now();
			std::time_t time = std::chrono::system_clock::to_time_t(start);

			if (Players[0].books == 1 || Players[1].books == 1) {
			    word = "book";
			}

			if (Players[0].hand.size() == 0 || Players[1].hand.size() == 0)
			{
				cout << "Game Over! A player has run out of cards" << endl;
				gameOver = true;

				if (Players[0].books > Players[1].books)
				{
					cout << Players[0].name << " wins! \nResults: " << Players[0].name << ": " << Players[0].books << " books and "<< Players[1].name << ": " << Players[1].books << " books" <<  endl;
					displayEmptyHandMsg(Players, 0);
					outStream << Players[0].name << " wins with  " << Players[0].books << " " << word << " - Date: " << ctime(&time) << endl;
				}
				else if (Players[0].books < Players[1].books)
				{
					cout << Players[1].name << " wins! \nResults: " << Players[1].name << ": " << Players[1].books << " books and "<< Players[0].name << ": " << Players[0].books << " books" <<  endl;
					displayEmptyHandMsg(Players, 1);
					outStream << Players[1].name << " wins with " << Players[1].books << " " << word << " - Date: " << ctime(&time) << endl;
				} else
				{
					cout << "Its a tie!" << endl;
					cout << Players[1].name << ": " << Players[1].books << " books and "<< Players[0].name << ": " << Players[0].books << "books" <<  endl;
				}
			}
			else if (indexes.size() == 0)
			{
				cout << "Game over! The deck is empty" << endl;

				if (Players[0].books > Players[1].books)
				{
					cout << Players[0].name << " wins! \nResults: " << Players[1].name << ": " << Players[1].books << " books and "<< Players[0].name << ": " << Players[0].books << " books" <<  endl;
					outStream << Players[0].name << " wins with " << Players[0].books << " " << word << " - Date: " << ctime(&time) << endl;
				}
				else if (Players[0].books < Players[1].books)
				{
					cout << Players[1].name << " wins! \nResults: " << Players[1].name << ": " << Players[1].books << " books and "<< Players[0].name << ": " << Players[0].books << " books" <<  endl;
					outStream << Players[1].name << " wins with " << Players[1].books << " " << word << " - Date: " << ctime(&time) << endl;
				} else
				{
					cout << "Its a tie!" << endl;
					cout << Players[1].name << ": " << Players[1].books << " books and "<< Players[0].name << ": " << Players[0].books << "books" <<  endl;

				}
				gameOver = true;
			}
	    }

		cout << "Do you want to play again? (Y/N)" << endl;
		cin >> playAgain;
		playAgain = toupper(playAgain);

	} while (playAgain == 'Y');

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
		if (Players[i].hand.size() == 0)
		{
			isEmpty = true;
			playerIdx = counter;
		}
		counter++;
	}
	return playerIdx;
}

void checkForBooks(Player Players[], int index)
{
	vector<int> occVec;
	vector<int> occVecPreserved;
	vector<int> suitLog;
	int cardToRemove = 0;
	int counter = 0;
	int offset = 0;
	string word = "books";
	int book = 4;

	if (Players[0].books == 1 || Players[1].books == 1) {
	    word = "book";
	}

	for (int i = 0; i < Players[index].hand.size(); i++)
	{
	    int occurences = count(Players[index].hand.begin(), Players[index].hand.end(), Players[index].hand[i]);
	    occVec.push_back(occurences);
	    occVecPreserved.push_back(Players[index].hand[i]);
	}

	for (int j = 0; j < occVec.size(); j++)
	{
	    if (occVec[j] >= book)
	    {
	        counter++;
	        cardToRemove = occVecPreserved[j];
	        suitLog.push_back(cardToRemove);

	        if (counter == book || counter == book * 2 || counter == book * 3 || counter == book * 4)
	        {
	            Players[index].books += 1;
	            cout << Players[index].name << " has a book! Obtained four of card: ";
	            classifyCard(cardToRemove, true);
	            cout << endl;
	        }

	        vector<int>::iterator itr = Players[index].hand.begin() + (j - offset);

	        while (Players[index].hand[j - offset] == cardToRemove)
	        {
	            offset++;
	            if (itr == Players[index].hand.end()-1)
	            {
	                Players[index].hand.pop_back();
	            }
	            else
	            {
	                while ((itr != Players[index].hand.end()-1) & (*itr == cardToRemove))
	                {
	                    if (itr != Players[index].hand.end())
	                    {
	                        Players[index].hand.erase(itr);
	                    }
	                    break;
	                }
	            }
	        }
	    }
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
		cout << Players[1].name << ": do you have any " << choice << "'s " << "?" << endl;
	}
	else
	{
		cout << Players[0].name << ": do you have any " << choice << "'s " << "?" << endl;
	}

}

string askMove(Player Players[], bool playerOneTurn, int randomChoice)
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
		cout << Players[1].name << "'s cards: (" << Players[1].hand.size() << ") " << "Books: " << Players[1].books;
//		displayHand(Players, 1);
		cout << endl << Players[1].name << ": what card do you wish to ask " << Players[0].name << " for?" << endl;
		choice = numToChoice(randomChoice);
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

void displayEmptyHandMsg(Player Players[], int playerWithEmptyHand) {
    if (playerWithEmptyHand == 0) {
        cout << Players[0].name << " has run out of cards" << endl;
    } else {
        cout << Players[1].name << " has run out of cards" << endl;
    }
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

	for (int i = 0; i < s1Help.size(); i++)
	{
		int pos = 0;

		if (ct < 7) {
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
	cout << "Books: " << Players[index].books << endl;
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
	// each player starts with 7 cards if there are 3 or fewer players (Player One vs AI)
	for (int i = 0; i < numPlayers; i++)
	{
		Players[i].books = 0;
		Players[i].numCards = 7;
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

string numToChoice(int choice)
{
	string choiceNum = "";
	if (choice == 1)
	{
		choiceNum = "Ace";
	}
	else if (choice == 2)
	{
		choiceNum = "2";
	}
	else if (choice == 3)
	{
		choiceNum = "3";
	}
	else if (choice == 4)
	{
		choiceNum = "4";
	}
	else if (choice == 5)
	{
		choiceNum = "5";
	}
	else if (choice == 6)
	{
		choiceNum = "6";
	}
	else if (choice == 7)
	{
		choiceNum = "7";
	}
	else if (choice == 8)
	{
		choiceNum = "8";
	}
	else if (choice == 9)
	{
		choiceNum = "9";
	}
	else if (choice == 10)
	{
		choiceNum = "10";
	}
	else if (choice == 11)
	{
		choiceNum = "Jack";
	}
	else if (choice == 12)
	{
		choiceNum = "Queen";
	}
	else if (choice == 13)
	{
		choiceNum = "King";
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