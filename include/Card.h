#ifndef CARD_H
#define CARD_H

#include <string>

class Card {
public:
    enum class Suit { HEARTS, DIAMONDS, CLUBS, SPADES };

    enum class Rank {
        TWO = 2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN,
        JACK, QUEEN, KING, ACE
    };

    Card(Suit suit, Rank rank);

    std::string toString() const;

    Suit getSuit() const;
    Rank getRank() const;

    bool operator<(const Card& other) const;

private:
    Suit suit_;
    Rank rank_;
};

#endif