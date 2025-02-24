#include <iostream>
#include <string>
#include <random>

using namespace std;

int main() {

	unsigned int seed;
	
	//get valid seed
	cout << "Input a seed: ";
	while(!(cin >> seed)){

		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Input a seed: ";
	}
	
	cout << "\n";

	//make generator
	default_random_engine rng(seed);
	uniform_int_distribution<int> rand_int(2,7);

	//init needed vars
	unsigned int coins = 100;
	unsigned int wager;
	unsigned int winnings;
	unsigned int minWager = 0;
	unsigned int maxWager = 0;
	unsigned int minPrize = 0;
	unsigned int maxPrize = 0;
	int temp[3];
	vector<int> vals;
	vector<vector<int>> rounds; //in each vector, first val is wager, second is winnings

	//round loop (end with 0 wager)
	while(coins != 0){

		//current coin count
		cout << "You currently possess " << coins << " coins.\n";
		
		//get valid wager or end with 0
		cout << "Bet how many coins? ";
		while(!(cin >> wager) || wager > coins){

			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Bet how many coins? ";
		}

		if(wager == 0){
		
			cout << "\n";
			break;
		
		}

		//first run assigns minWager
		if(minWager == 0) minWager = wager;
		
		//test for min or max wager, assign if applicable
		if(wager < minWager) minWager = wager;
		if(wager > maxWager) maxWager = wager;

		//take wager from coins amount
		coins -= wager;

		//store wager value
		vals.push_back(wager);

		//generate numbers
		for(int i = 0; i < 3; i++){

			temp[i] = rand_int(rng);
			cout << " " << temp[i];

		}
		cout << "\n";

		//figure winnings
		if(temp[0] == 7 && temp[1] == 7 && temp[2] == 7) winnings = wager * 10;
		else if(temp[0] == temp[1] && temp[1] == temp[2]) winnings = wager * 5;
		else if(temp[0] == temp[1] || temp[1] == temp[2] || temp[2] == temp[0]) winnings = wager * 2;
		else winnings = 0;

        //test for min or max wager, assign if applicable
        if(winnings < minPrize) minPrize = winnings;
        if(winnings > maxPrize) maxPrize = winnings;

		//gain winnings
		coins += winnings;

		//print and store result
		if(winnings != 0) cout << "You won " << winnings << " coins!\n\n";
		else cout << "You did not win.\n\n";

		vals.push_back(winnings);

		//put wager and winnings into rounds vector, reset vals for next round
		rounds.push_back(vals);
		vals.resize(0);

	}

	//No play summary if no rounds
	if(rounds.size() == 0){
	
		return 0;

	}

	//summary
	cout << "Play Summary:\n";
	
	//round recap
	for(int i = 0; i < rounds.size(); i++){

		cout << "You wagered " << rounds.at(i).at(0) << " coins and won " << rounds.at(i).at(1) << " coins.\n";

	}
	cout << "\n";

	//max and mins
	cout << "Your minimum wager was " << minWager << " coins.\n";
	cout << "Your maximum wager was " << maxWager << " coins.\n";
	cout << "Your minimum prize was " << minPrize << " coins.\n";
	cout << "Your maximum prize was " << maxPrize << " coins.\n";

	//end line
	cout << "\n";

	return 0;

}
