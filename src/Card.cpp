#include "Card.h"
#include <string>

Card::Card(const Suit suit, const Rank rank): suit_(suit), rank_(rank) {
}

std::string Card::toString() const {
    std::string result;

    switch (suit_) {
        case Suit::HEARTS: result = "♥";
            break;
        case Suit::DIAMONDS: result = "♦";
            break;
        case Suit::CLUBS: result = "♣";
            break;
        case Suit::SPADES: result = "♠";
            break;
    }

    switch (rank_) {
        case Rank::TWO: result += "2";
            break;
        case Rank::THREE: result += "3";
            break;
        case Rank::FOUR: result += "4";
            break;
        case Rank::FIVE: result += "5";
            break;
        case Rank::SIX: result += "6";
            break;
        case Rank::SEVEN: result += "7";
            break;
        case Rank::EIGHT: result += "8";
            break;
        case Rank::NINE: result += "9";
            break;
        case Rank::TEN: result += "10";
            break;
        case Rank::JACK: result += "J";
            break;
        case Rank::QUEEN: result += "Q";
            break;
        case Rank::KING: result += "K";
            break;
        case Rank::ACE: result += "A";
            break;
    }

    return result;
}

Card::Suit Card::getSuit() const {
    return suit_;
}

Card::Rank Card::getRank() const {
    return rank_;
}

bool Card::operator<(const Card &other) const {
    return static_cast<int>(rank_) < static_cast<int>(other.rank_);
}
