#include <cstdio>
#include <vector>
#include <iostream>
#include "fraction.hpp"
using namespace std;

/* keno.cpp
 * Ethan Maness
 * October 17, 2021
 *
 * Allows for the analysis of odds in Keno games with any bets, catches and payouts as defined by the user.  Uses the Fraction class for more precise probabilistic calculations.
 */

int main(){
	double bet;
	int balls;
	vector<int> catches;
	vector<double> payouts, probabilities, returns;
	double total_return;
	int c;
	double p;
	Fraction frac;

	//read in bet and balls
	cin >> bet;
	cin >> balls;
	
	//continually read in catch-and-payout pairs
	while(1){
		//if one fails, end loop
		if(!(cin >> c && cin >> p)) break;
		catches.push_back(c);
		payouts.push_back(p);
		
		//calculate probability for a given catch count
		frac.Clear();
		frac.Multiply_Binom(80-balls, 20-c);
		frac.Multiply_Binom(balls, c);
		frac.Divide_Binom(80,20);
		probabilities.push_back(frac.Calculate_Product());
	
		//also calculate average return for the given catch
		returns.push_back(probabilities.back() * payouts.back());
	}

	//print info
	printf("Bet: %.2f\n", bet);
	printf("Balls Picked: %d\n", balls);
	//for each catch-payout pair, print the probability of success and the average return of that one pair
	for(size_t i = 0; i < catches.size(); i++){
		cout << "  Probability of catching " << catches.at(i) << " of " << balls << ": " << probabilities.at(i);
		cout << " -- Expected return: " << returns.at(i) << endl;
		//also, sum up all returns to get total average return
		total_return += returns.at(i);
	}

	//calculate net loss (or gain)
	printf("Your return per bet: %.2f\n", total_return - bet);
	//calculate net loss per dollar spent
	printf("Normalized: %.2f\n", (total_return - bet)/bet);

}
