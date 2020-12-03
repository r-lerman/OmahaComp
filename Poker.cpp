#include "Poker.h"

namespace Poker
{

// ------------------------------------- PlayingCard -------------------------------------------------------

PlayingCard::PlayingCard(const std::string& r)
{
	m_rank = 0;
	m_suit = Suit::suit_unknown;
	if ( !r.length() || r.length() > 2 ) return;
	
	if ( isdigit(r[0]) )
	{
		m_rank = atoi( r.substr(0,1).c_str() );
		if ( m_rank == 1) m_rank = 0;
	}
	else
		switch( tolower(r[0]) )
		{
			case 't': m_rank = 10; break;
			case 'j': m_rank = 11; break;
			case 'q': m_rank = 12; break;
			case 'k': m_rank = 13; break;
			case 'a': m_rank = 14; break;
		}

	if ( r.length() == 2 )
		switch( tolower(r[1]) ) 
		{
			case 'd': m_suit = Suit::suit_diamonds; break;
			case 'c': m_suit = Suit::suit_clubs; break;
			case 'h': m_suit = Suit::suit_hearts; break;
			case 's': m_suit = Suit::suit_spades; break;
			case '*': m_suit = Suit::suit_unknown; break;
		}
}

std::string PlayingCard::ToString() const
{
	std::ostringstream oss;
	if ( empty() )
		oss << "##";
	else
	{
		if ( m_rank >=2 && m_rank <=9 )
			oss << m_rank;
		else
			switch( m_rank )
			{
				case 10: oss << 'T'; break;
				case 11: oss << 'J'; break;
				case 12: oss << 'Q'; break;
				case 13: oss << 'K'; break;
				case 1: 
				case 14: oss << 'A'; break;
			}

		switch( m_suit ) 
		{
			case Suit::suit_diamonds: oss << 'd'; break;
			case Suit::suit_clubs:    oss << 'c'; break;
			case Suit::suit_hearts:   oss << 'h'; break;
			case Suit::suit_spades:   oss << 's'; break;
		}
	}
	return oss.str();
}

// ------------------------------------- PokerCardSet -----------------------------------------------------

PokerCardSet::PokerCardSet(const std::string& s)
{
	std::string::size_type pos1 = s.find(":"), pos2;
	if ( pos1 == std::string::npos )
		pos1 = 0;
	else
		m_set_name = s.substr(0, pos1++);

	while ( pos1 < s.length() && (pos2 = s.find("-", pos1)) != std::string::npos )
	{
		m_cards.push_back( s.substr(pos1, pos2-pos1) );
		pos1 = pos2 + 1;
	}
	m_cards.push_back( s.substr(pos1) );
}

std::string PokerCardSet::ToString() const
{
	std::ostringstream oss;
	if ( !m_set_name.empty() )
		oss << m_set_name << ':';
	for(PokerCardArray::size_type i = 0; i < Cards(); i++)
	{
		oss << m_cards[i];
		if ( m_cards[i].HasSuit() && i < Cards() - 1 ) oss << '-';
	}
	return oss.str();
}

bool PokerCardSet::MakeStraightFlush() // rank: 9
{
	if ( Cards() != 5 || !AllCardsOfTheSameSuit() ) 
		return false;
	
	SortSet();
	if ( InConsecutiveOrder() ) 
		return true;

	if ( m_cards[0].GetCardRank() == 14 )
	{
		PokerCardArray old(m_cards);
		m_cards[0].AceToLowestCard();
		MoveFirstCardToTheEnd();
		if ( InConsecutiveOrder() )
			return true;
		m_cards = old;
	}
	return false;
}

bool PokerCardSet::Make4ofKind() // rank: 8
{
	if ( Cards() != 5 ) 
		return false;

	SortSet();
	if ( m_cards[0].GetCardRank() !=  m_cards[1].GetCardRank() )
		MoveFirstCardToTheEnd();
		
	return m_cards[0].GetCardRank() == m_cards[1].GetCardRank() &&
		   m_cards[0].GetCardRank() == m_cards[2].GetCardRank() &&
		   m_cards[0].GetCardRank() == m_cards[3].GetCardRank();
}

bool PokerCardSet::MakeFullHouse() // rank: 7
{
	if ( Cards() != 5 ) 
		return false;
	
	SortSet();

	if ( m_cards[0].GetCardRank() == m_cards[1].GetCardRank() &&
		 m_cards[0].GetCardRank() == m_cards[2].GetCardRank() &&
		 m_cards[3].GetCardRank() == m_cards[4].GetCardRank() ) 
		 return true;

	if ( m_cards[0].GetCardRank() == m_cards[1].GetCardRank() &&
		 m_cards[2].GetCardRank() == m_cards[3].GetCardRank() &&
		 m_cards[2].GetCardRank() == m_cards[4].GetCardRank() ) 
	{
		MoveFirstCardToTheEnd();
		MoveFirstCardToTheEnd();
		return true;
	}
	return false;
}

bool PokerCardSet::MakeFlush() // rank: 6
{ 
	if ( Cards() == 5 && AllCardsOfTheSameSuit() ) 
	{ 
		SortSet();	
		return InConsecutiveOrder() ? false : true; 
	}
	return false;
}

bool PokerCardSet::MakeStraight() // rank: 5
{
	if ( Cards() != 5 || AllCardsOfTheSameSuit() ) 
		return false;
	
	SortSet();
	if ( InConsecutiveOrder() ) 
		return true;

	if ( m_cards[0].GetCardRank() == 14 )
	{
		PokerCardArray old(m_cards);
		m_cards[0].AceToLowestCard();
		MoveFirstCardToTheEnd();
		if ( InConsecutiveOrder() )
			return true;
		m_cards = old;
	}
	return false;
}

bool PokerCardSet::Make3ofKind() // rank: 4
{
	if ( Cards() != 5 ) 
		return false;
	
	SortSet();
	PokerCardArray::iterator it = adjacent_find( m_cards.begin(), m_cards.end() );
    if ( it == m_cards.end() ) 
		return false;

	while ( m_cards[0].GetCardRank() !=  m_cards[1].GetCardRank() )
		MoveFirstCardToTheEnd();

	if( m_cards[0].GetCardRank() == m_cards[1].GetCardRank() &&
	    m_cards[0].GetCardRank() == m_cards[2].GetCardRank() &&
	    m_cards[3].GetCardRank() != m_cards[4].GetCardRank() )
	{
		sort(m_cards.begin()+3, m_cards.end(), std::greater<PokerCard>());
		return true;
	}
	return false;
}

bool PokerCardSet::MakeTwoPair()  // rank: 3
{
	if ( Cards() != 5 ) return false;
	SortSet();

	PokerCardArray::iterator it = adjacent_find( m_cards.begin(), m_cards.end() );
    if ( it == m_cards.end() ) 
		return false;

	while ( m_cards[0].GetCardRank() != m_cards[1].GetCardRank() )
		MoveFirstCardToTheEnd();

	if ( m_cards[2].GetCardRank() == m_cards[3].GetCardRank() &&
		 m_cards[2].GetCardRank() != m_cards[4].GetCardRank() &&
		 m_cards[2].GetCardRank() != m_cards[0].GetCardRank() )
		 return true;

	if ( m_cards[3].GetCardRank() == m_cards[4].GetCardRank() &&
		 m_cards[2].GetCardRank() != m_cards[3].GetCardRank() &&
		 m_cards[2].GetCardRank() != m_cards[0].GetCardRank() )
	{
		std::swap( m_cards[2], m_cards[4] );
		return true;
	}
	return false;
}

bool PokerCardSet::MakeOnePair() // rank: 2
{
	if ( Cards() != 5 ) return false;
	SortSet();

	PokerCardArray::iterator it = adjacent_find( m_cards.begin(), m_cards.end() );
    if ( it == m_cards.end() ) 
		return false;

	while ( m_cards[0].GetCardRank() != m_cards[1].GetCardRank() )
		MoveFirstCardToTheEnd();

	PokerCardArray t(m_cards);
	if ( distance( t.begin(), unique(t.begin(), t.end()) ) == 4 )
	{
		sort(m_cards.begin()+2, m_cards.end(), std::greater<PokerCard>());
		return true;
	}
	return false;
}

// ------------------------------------- PokerHandLow --------------------------------------------------

PokerHandLow::PokerHandLow(PokerPlayerCards a, PokerBoardCards b)
{
	a.MakeLow8();
	b.MakeLow8();
	if ( a.Cards() >= 2 && b.Cards() >= 3 )
	{
		PokerCardArray ar(5);
		PokerCardArray::size_type p1, p2, pb1, pb2, pb3;
		for(p1 = 0; p1 < a.Cards(); p1++)
			for(ar[0] = a[p1], p2 = p1 + 1; p2 < a.Cards(); p2++)
				for(ar[1] = a[p2], pb1 = 0; pb1 < b.Cards(); pb1++)
					for(ar[2] = b[pb1], pb2 = pb1 + 1; pb2 < b.Cards(); pb2++)
						for(ar[3] = b[pb2], pb3 = pb2 + 1; pb3 < b.Cards(); pb3++)
						{
							ar[4] = b[pb3];

							PokerCardArray candidate(ar);
							sort(candidate.begin(), candidate.end());
							if ( distance( candidate.begin(), unique(candidate.begin(), candidate.end()) ) == 5 )
							{
								reverse( candidate.begin(), candidate.end() );
								if ( !Cards() || candidate < m_cards ) m_cards = candidate;
							}
						}
	}
	m_qualified = Cards() == 5;
	if ( m_qualified ) 
		m_rank_name = ToString();
}

// ------------------------------------- PokerHandHigh ------------------------------------------------

PokerHandHigh::PokerHandHigh(PokerPlayerCards a, PokerBoardCards b)
{
	m_hand_rank = 0;
	if ( a.Cards() >= 2 && b.Cards() >= 3 )
	{
		PokerCardArray ar(5);
		unsigned int ar_rank = 0;
		PokerCardArray::size_type p1, p2, pb1, pb2, pb3;
		for(p1 = 0; p1 < a.Cards(); p1++)
			for(ar[0] = a[p1], p2 = p1 + 1; p2 < a.Cards(); p2++)
				for(ar[1] = a[p2], pb1 = 0; pb1 < b.Cards(); pb1++)
					for(ar[2] = b[pb1], pb2 = pb1 + 1; pb2 < b.Cards(); pb2++)
						for(ar[3] = b[pb2], pb3 = pb2 + 1; pb3 < b.Cards(); pb3++)
						{
							ar[4] = b[pb3];

							PokerCardSet candidate(ar);

							if ( candidate.MakeStraightFlush() ) ar_rank = 9;
							else if ( m_hand_rank <= 8 && candidate.Make4ofKind() )   ar_rank = 8;	
							else if ( m_hand_rank <= 7 && candidate.MakeFullHouse() ) ar_rank = 7;	
							else if ( m_hand_rank <= 6 && candidate.MakeFlush() )     ar_rank = 6;	
							else if ( m_hand_rank <= 5 && candidate.MakeStraight() )  ar_rank = 5;	
							else if ( m_hand_rank <= 4 && candidate.Make3ofKind() )   ar_rank = 4;	
							else if ( m_hand_rank <= 3 && candidate.MakeTwoPair() )   ar_rank = 3;	
							else if ( m_hand_rank <= 2 && candidate.MakeOnePair() )   ar_rank = 2;	
							else 
							{
								candidate.SortSet();
								ar_rank = 1;	
							}

							if ( !Cards() || ar_rank > m_hand_rank || 
								 ar_rank == m_hand_rank && candidate > m_cards ) 
							{ 
								m_cards = candidate.GetCards(); 
								m_hand_rank = ar_rank;
							}
						}

		m_rank_name = GetRankNameForHighHand(m_hand_rank);
	}
}

std::string PokerHandHigh::ToString() const // for testing
{
	std::ostringstream oss;
	if ( !m_set_name.empty() )
		oss << m_set_name << ':';
	for(PokerCardArray::size_type i = 0; i < Cards(); i++)
	{
		oss << m_cards[i];
		switch( m_cards[i].GetCardSource() )
		{
			case CardSource::source_from_player: oss << "/P"; break;
			case CardSource::source_from_board:  oss << "/B"; break;
		}
		if ( m_cards[i].HasSuit() && i < Cards() - 1 ) oss << ' ';
	}
	oss << " - Rank: " << m_hand_rank << "(" << m_rank_name << ")";
	return oss.str();
}

std::string PokerHandHigh::GetRankNameForHighHand(unsigned int n)
{
	switch( n )
	{
		case 9: return "Straight Flush"; break;
		case 8: return "4-of-a-Kind"; break;
		case 7: return "Full House"; break;
		case 6: return "Flush"; break;
		case 5: return "Straight"; break;
		case 4: return "3-of-a-Kind"; break;
		case 3: return "Two Pair"; break;
		case 2: return "One Pair"; break;
		case 1: return "High card"; break;
		case 0:
		default: return "Unknown rank"; break;
	}
}

} // End Namespace Poker