#pragma once

#include <vector>
#include <algorithm>
#include <sstream>

namespace Poker
{
	enum class Suit { suit_unknown, suit_diamonds, suit_clubs, suit_hearts, suit_spades };
	enum class CardSource { source_unknown, source_from_player, source_from_board };

	// -------------------------------------------------------------------------------------------------------

	class PlayingCard
	{
	protected:

		unsigned int m_rank; // A=1(low),2,3,4,5,6,7,8,9,T=10,J=11,Q=12,K=13,A=14(high)
		Suit m_suit; // d,c,h,s

	public:

		PlayingCard() : m_rank(0), m_suit(Suit::suit_unknown) { }
		PlayingCard(const std::string&);
		PlayingCard(const PlayingCard& other) : m_rank(other.m_rank), m_suit(other.m_suit) { }

		PlayingCard& operator= (const PlayingCard& rValue) 
		{ 
			m_rank = rValue.m_rank; m_suit = rValue.m_suit; 
			return *this; 
		}

		virtual bool operator == (const PlayingCard& rValue) const { return m_rank == rValue.m_rank && m_suit == rValue.m_suit; }
		virtual bool operator != (const PlayingCard& rValue) const { return m_rank != rValue.m_rank || m_suit != rValue.m_suit; }
		virtual std::string ToString() const;

		unsigned int GetCardRank() const { return m_rank; }
		Suit GetSuit() const { return m_suit; }
		bool empty() const { return m_rank ? false : true; }
	};

	inline std::ostream& operator<<(std::ostream& out, const PlayingCard& c) { return out << c.ToString(); }

	// -------------------------------------------------------------------------------------------------------

	class PokerCard : public PlayingCard
	{
	protected:
	
		CardSource m_source;

	public:

		PokerCard() : PlayingCard(), m_source(CardSource::source_unknown) { }
		PokerCard(std::string r) : PlayingCard(r), m_source(CardSource::source_unknown) { }

		PokerCard(const PokerCard& other) : PlayingCard(other), m_source(other.m_source) { }
	
		PokerCard& operator= (const PokerCard& rValue) 
		{ 
			m_rank = rValue.m_rank; m_suit = rValue.m_suit; m_source = rValue.m_source; 
			return *this; 
		}

		void transformForLow8() { m_suit = Suit::suit_unknown; if ( m_rank == 14 ) m_rank = 1; }
		void AceToLowestCard() { if ( m_rank == 14 ) m_rank = 1; }

		void SetAsFromPlayer() { m_source = CardSource::source_from_player; }
		void SetAsFromBoard() { m_source = CardSource::source_from_board; }
		CardSource GetCardSource() const { return m_source; }

		bool IsLow8() const { return m_rank <= 8; }
		bool HasSuit() const { return m_suit != Suit::suit_unknown; }

		virtual bool operator == (const PokerCard& rValue) const { return m_rank == rValue.m_rank; }
		virtual bool operator != (const PokerCard& rValue) const { return m_rank != rValue.m_rank; }
		virtual bool operator >  (const PokerCard& rValue) const { return m_rank >  rValue.m_rank; }
		virtual bool operator <  (const PokerCard& rValue) const { return m_rank <  rValue.m_rank; }
		virtual bool operator >= (const PokerCard& rValue) const { return m_rank >= rValue.m_rank; }
		virtual bool operator <= (const PokerCard& rValue) const { return m_rank <= rValue.m_rank; }
	};

	using PokerCardArray = std::vector<PokerCard>;

	// -------------------------------------------------------------------------------------------------------

	class PokerCardSet
	{
	protected:

		PokerCardArray m_cards;
		std::string m_set_name;

	private:

		static bool notQualifyForLow8(PokerCard c) { return !c.IsLow8(); }

	public:

		PokerCardSet() { }
		PokerCardSet(const PokerCardArray& ar) : m_cards(ar) { }
		PokerCardSet(const std::string&);
		PokerCardSet(const PokerCardSet& other) : m_cards(other.m_cards), m_set_name(other.m_set_name) { }

		~PokerCardSet() { }

		virtual std::string ToString() const;
		unsigned int Cards() const { return m_cards.size(); }

		std::string GetName() const { return m_set_name; }
		void SetName(std::string& s) { m_set_name = s; }

		const PokerCardArray& GetCards() const { return m_cards; }
		const PokerCard& operator [] (PokerCardArray::size_type i) const { return m_cards[i]; }
	
		PokerCardSet& operator= (const PokerCardSet& rValue) 
		{ 
			m_cards = rValue.m_cards; m_set_name = rValue.m_set_name; 
			return *this; 
		}

		PokerCardSet& operator + (const PokerCardSet& rValue) 
		{ 
			m_cards.insert(m_cards.end(), rValue.m_cards.begin(), rValue.m_cards.end()); 
			return *this; 
		}

		bool operator == (const PokerCardSet& rValue) const { return Cards() == rValue.Cards() && m_cards == rValue.m_cards; }
		bool operator > (const PokerCardSet& rValue) const  { return Cards() == rValue.Cards() && m_cards > rValue.m_cards; }
		bool operator < (const PokerCardSet& rValue) const  { return Cards() == rValue.Cards() && m_cards < rValue.m_cards; }

		void SortSet() { sort(m_cards.begin(), m_cards.end(), std::greater<PokerCard>()); }

		bool InConsecutiveOrder() const
		{
			PokerCardArray::const_iterator first = m_cards.begin(), next = m_cards.begin(); ++next;
			while ( next != m_cards.end() ) 
			{
				if ( first->GetCardRank() - next->GetCardRank() != 1 ) return false;
				++first; ++next;
			}
			return true;
		}

		bool AllCardsOfTheSameSuit() const
		{ 
			if ( Cards() < 2 ) return true;
			Suit s = m_cards[0].GetSuit();
			for(PokerCardArray::size_type i = 1; i < Cards(); i++)
				if ( s != m_cards[i].GetSuit() ) return false;
			return true;
		}

		void MakeLow8()
		{
			for(PokerCardArray::iterator pos = m_cards.begin(); pos != m_cards.end(); ++pos) 
				pos->transformForLow8();

			PokerCardArray::iterator it = remove_if(m_cards.begin(), m_cards.end(), PokerCardSet::notQualifyForLow8);
			sort(m_cards.begin(), it);
			m_cards.resize( distance(m_cards.begin(), unique(m_cards.begin(), it) ) );
		}	

		void MoveFirstCardToTheEnd()
		{
			PokerCard c = m_cards[0];
			for(PokerCardArray::size_type i = 1; i < Cards(); i++)
				m_cards[i-1] = m_cards[i];
			m_cards[Cards()-1] = c;
		}

		bool MakeStraightFlush();
		bool Make4ofKind();
		bool MakeFullHouse();
		bool MakeFlush();
		bool MakeStraight();
		bool Make3ofKind();
		bool MakeTwoPair();
		bool MakeOnePair();
	};

	inline std::ostream& operator<<(std::ostream& out, const PokerCardSet& cs) { return out << cs.ToString(); }

	// -------------------------------------------------------------------------------------------------------

	class PokerPlayerCards : public PokerCardSet
	{
	public:

		PokerPlayerCards() { }
		PokerPlayerCards(const std::string& s) : PokerCardSet(s) 
		{ 
			for(PokerCardArray::iterator pos = m_cards.begin(); pos != m_cards.end(); ++pos) 
				pos->SetAsFromPlayer(); 
		}
	};

	class PokerBoardCards : public PokerCardSet
	{
	public:

		PokerBoardCards() { }
		PokerBoardCards(const std::string& s) : PokerCardSet(s)
		{ 
			for(PokerCardArray::iterator pos = m_cards.begin(); pos != m_cards.end(); ++pos) 
				pos->SetAsFromBoard(); 
		}
	};

	// -------------------------------------------------------------------------------------------------------

	class PokerHand : public PokerCardSet  // Abstract Class
	{
	protected:

		std::string m_rank_name;

	public:

		std::string GetRankName() const { return m_rank_name; }

		virtual bool operator == (const PokerHand& rValue) const 
		{ return qualified() && rValue.qualified() && m_cards == rValue.m_cards; }
	
		virtual bool operator > (const PokerHand& rValue) const
		{ return qualified() && rValue.qualified() && m_cards > rValue.m_cards; }

		virtual bool qualified() const { return Cards() == 5; }
		virtual unsigned int GetRank() const { return 0; }

		virtual std::string ObjectSuffix() const=0;
	};

	/* -------------------------------------------------------------------------------------------------------
		9 - Straight Flush: Five cards in sequence, of the same suit. (A,K,Q,J,10 is known as a Royal Flush)
		8 - Four of a Kind: Four cards of the same rank, and one side card or 'kicker'.
		7 - Full House: Three cards of the same rank, and two cards of a different, matching rank.
		6 - Flush: Five cards of the same suit.
		5 - Straight: Five cards in sequence.
		4 - Three of a kind: Three cards of the same rank, and two unrelated side cards.
		3 - Two pair: Two cards of a matching rank, another two cards of a different matching rank, and one side card.
		2 - One pair: Two cards of a matching rank, and three unrelated side cards.
		1 - High card: Any hand that does not qualify under a category listed above.
	*/

	class PokerHandHigh : public PokerHand
	{
	protected:

		unsigned int m_hand_rank;

	public:

		PokerHandHigh(PokerPlayerCards, PokerBoardCards);
		PokerHandHigh(const PokerHandHigh& other) : m_hand_rank(other.m_hand_rank) 
		{ 
			m_rank_name = other.m_rank_name; 
			m_cards = other.m_cards; 
			m_set_name = other.m_set_name;
		}

		PokerHandHigh& operator= (const PokerHandHigh& rValue) 
		{
			m_hand_rank = rValue.m_hand_rank; 
			m_rank_name = rValue.m_rank_name; 
			m_cards = rValue.m_cards; 
			m_set_name = rValue.m_set_name;
			return *this; 
		}

		static std::string GetRankNameForHighHand(unsigned int);

		virtual std::string ToString() const;
		virtual unsigned int GetRank() const { return m_hand_rank; }
		virtual std::string ObjectSuffix() const { return std::string("Hi"); }

		virtual bool operator == (const PokerHand& rValue) const 
		{ return m_hand_rank == rValue.GetRank() && m_cards == rValue.GetCards(); }

		virtual bool operator > (const PokerHand& rValue) const
		{ return m_hand_rank > rValue.GetRank() || m_hand_rank == rValue.GetRank() && m_cards > rValue.GetCards(); }
	};

	/* -------------------------------------------------------------------------------------------------------
		10 - 5, 4, 3, 2, A
		9  - 6, 4, 3, 2, A
		8  - 6, 5, 4, 3, 2
		7  - 7, 5, 4, 3, 2
		6  - 7, 6, 5, 2, A
		5  - 7, 6, 5, 4, 2
		4  - 8, 4, 3, 2, A
		3  - 8, 6, 4, 2, A
		2  - 8, 7, 6, 5, 3
		1  - 8, 7, 6, 5, 4
		0  - Cannot qualify for Low
	*/

	class PokerHandLow : public PokerHand
	{
		bool m_qualified;
	
	public:

		PokerHandLow(PokerPlayerCards, PokerBoardCards);
		PokerHandLow(const PokerHandLow& other) : m_qualified(other.m_qualified)
		{ 
			m_rank_name = other.m_rank_name; 
			m_cards = other.m_cards; 
			m_set_name = other.m_set_name;
		}

		PokerHandLow& operator= (const PokerHandLow& rValue) 
		{ 
			m_qualified = rValue.m_qualified;
			m_rank_name = rValue.m_rank_name; 
			m_cards = rValue.m_cards; 
			m_set_name = rValue.m_set_name;
			return *this; 
		}

		virtual bool qualified() const { return m_qualified; }
		virtual std::string ObjectSuffix() const { return std::string("Lo"); }

		virtual bool operator > (const PokerHand& rValue) const
		{ return m_qualified && ( !rValue.qualified() || rValue.qualified() && m_cards < rValue.GetCards() ); }
	};

} // End Namespace Poker
