// David Lopez Barcenas wrote the code bellow, only Godand I knew what we did, and know only God knows
#include "DieRoller.h"

#include<iostream>  //Standard IO
#include<random>    //Random

//constructor that seeds the rng with a std::random_device
DieRoller::DieRoller() {
	std::random_device rd;
	rng.seed(rd());
}

//seeds the rng with the seedVal
void DieRoller::SeedRNG(unsigned int seedVal) {
	rng.seed(seedVal);
}

// returns the result of an N-sided die roll (between 1 and N);
int DieRoller::RollDN(int N) {
	std::uniform_int<unsigned int> uint_distlowmax(1, N);
	return(uint_distlowmax(rng));
}

// rolls 4 six-sided dice and sums the top 3, dropping the lowest roll
int DieRoller::Roll4D6DropLowest() {
	std::uniform_int<unsigned int> uint_distlowmax(1, 6);

	int roll[4];
	int minimum = 7;
	int sumDice = 0;
	for (int i = 0; i < 4; i++) {
		roll[i] = uint_distlowmax(rng);
		if (roll[i] < minimum) {
			minimum = roll[i];
		}
		sumDice = sumDice + roll[i];
		//std::cout << "Dice " << i << " = " << roll[i] << std::endl;
	}
	return(sumDice - minimum);
}