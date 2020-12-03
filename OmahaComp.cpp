#include "Poker.h"
#include <iostream>
#include <fstream>

bool LoadCards(const std::string& s, Poker::PokerPlayerCards& h1, Poker::PokerPlayerCards& h2, Poker::PokerBoardCards& b)
{
	std::string::size_type idx1 = s.find(' ');
	if (idx1 == std::string::npos)	return false;
	std::string::size_type idx2 = s.find(' ', idx1 + 1);
	if (idx2 == std::string::npos)	return false;

	h1 = Poker::PokerPlayerCards(s.substr(0, idx1));
	h2 = Poker::PokerPlayerCards(s.substr(idx1 + 1, idx2 - idx1 - 1));
	b  = Poker::PokerBoardCards(s.substr(idx2 + 1, s.length() - idx2));
	return h1.Cards() == 4 && h2.Cards() == 4 && b.Cards() == 5 ? true : false;
}

std::string HandEvaluation(const Poker::PokerHand& h1, const Poker::PokerHand& h2)
{
	if (h1.ObjectSuffix() != h2.ObjectSuffix())
		return "Cannot compare hands with different type";

	if (!h1.qualified() && !h2.qualified())
		return "No hand qualified for Low";
	else if (h1 == h2)
		return "Split Pot " + h1.ObjectSuffix() + " (" + h1.GetRankName() + ")";
	else
	{
		if (h1 > h2)
			return "HandA wins " + h1.ObjectSuffix() + " (" + h1.GetRankName() + ")";
		else
			return "HandB wins " + h2.ObjectSuffix() + " (" + h2.GetRankName() + ")";
	}
}

// ---------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cerr << "Missing application parameters." << std::endl;
		return EXIT_FAILURE;
	}

	std::ifstream ifs(argv[1]);
	if (!ifs)
	{
		std::cerr << "Cannot find the specified input file." << std::endl;
		return EXIT_FAILURE;
	}

	std::ofstream ofs;
	ofs.open(argv[2]);
	if (!ofs)
	{
		std::cerr << "Cannot open the specified output file." << std::endl;
		return EXIT_FAILURE;
	}

	Poker::PokerPlayerCards p1, p2;
	Poker::PokerBoardCards b;
	std::string sCardCombination;

	while (!ifs.eof())
	{
		std::getline(ifs, sCardCombination);
		if (sCardCombination.empty()) continue;

		if (!LoadCards(sCardCombination, p1, p2, b))
		{
			std::cerr << "Wrong syntax in the input file." << std::endl;
			return EXIT_FAILURE;
		}

		Poker::PokerHandHigh hi1(p1, b), hi2(p2, b);

		ofs << p1 << ' ' << p2 << ' ' << b << std::endl
			//  << "A: " << hi1 << endl << "B: " << hi2 << endl // for testing
			<< "=> " << HandEvaluation(hi1, hi2) << "; "
			<< HandEvaluation(Poker::PokerHandLow(p1, b), Poker::PokerHandLow(p2, b)) << std::endl << std::endl;
	}

	ifs.close();
	ofs.close();
	return EXIT_SUCCESS;
}
