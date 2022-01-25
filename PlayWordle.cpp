// Gather_Concordance.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <string>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cctype>

using namespace std;

int main()
{
	ifstream OneLengthDictionary;
	string dictionaryFolderPath="C:\\Users\\paul.gontarz\\OneDrive - Thermo Fisher Scientific\\Desktop\\WordleDictionary\\";
	string filePath = "";
	string word, guessResultString, singleLetter;
	int gamechoice=0, wordLength=0, guessNum=0;
	int currentscores[11], guessResultInt[11] = { 0,0,0,0,0,0,0,0,0,0,0 }, totalValuesForLength = 1, currentSolutionValue = 0, multiplier = 1;
	vector <int> possibilityMatrix(177147);
	int worstCaseCount = 0, bestWorstCase = 0, bestGuessIndex=0, countInGuess=0, countInSolution=0;
	vector <string> Solutions, Guesses, tempSolutions;
	bool FoundSolution=false, inputCorrect = true, keepWord=true, inSolution=false;

	cout << "Let's play wordle! Are you playing original wordle or hellowordl.net" << endl;
	cout << "(1) for original wordle." << endl;
	cout << "(2) for hellowordl.net" << endl;
	cin >> gamechoice;
	while (gamechoice != 1 && gamechoice != 2)
	{
		cout << "Yo retard. You have to hit 1 and then enter or 2 and then enter. Anthing else and you've fucked up." << endl;
		cout << "(1) for original wordle." << endl;
		cout << "(2) for hellowordl.net" << endl;
		cin >> gamechoice;
	}
	if (gamechoice == 1)
	{
		cout << "Ok! Playing original wordle." << endl;
		cout << "Loading possible solutions and guesses." << endl;
		filePath = dictionaryFolderPath + "WordleGuesses.txt";
		OneLengthDictionary.open(filePath);
		while (getline(OneLengthDictionary, word))
		{
			std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
			Guesses.push_back(word);
		}
		OneLengthDictionary.close();
		filePath = dictionaryFolderPath + "WordleSolutions.txt";
		OneLengthDictionary.open(filePath);
		while (getline(OneLengthDictionary, word))
		{
			std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
			Solutions.push_back(word);
			Guesses.push_back(word); //Guesses list does not contain solutions words.
		}
		OneLengthDictionary.close();
		cout << "Loaded " << Guesses.size() << " guess words and " << Solutions.size() << " solution words." << endl;
		wordLength = 5;

	}
	else if (gamechoice == 2)
	{
		cout << "Ok! Playing hellowordl.net" << endl;
		cout << "How many letters are in the word? " << endl;
		std::cin >> wordLength;
		cout << "Ok, playing HelloWordl with a " << wordLength << " letter word." << endl;
		filePath = dictionaryFolderPath + "HelloWordlGuesses" + to_string(wordLength) + ".txt";
		OneLengthDictionary.open(filePath);
		while (getline(OneLengthDictionary, word))
		{
			std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
			Guesses.push_back(word);
		}
		OneLengthDictionary.close();
		filePath = dictionaryFolderPath + "HelloWordlSolutions" + to_string(wordLength) + ".txt";
		OneLengthDictionary.open(filePath);
		while (getline(OneLengthDictionary, word))
		{
			std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
			Solutions.push_back(word);
		}
		OneLengthDictionary.close();
		cout << "Loaded " << Guesses.size() << " guess words and " << Solutions.size() << " solution words with " << wordLength << " letters." << endl;
	}
	for (int i = 0; i < wordLength; i++) //So we don't have to reset full matrix if not needed
		totalValuesForLength = totalValuesForLength * 3;

	//Scoring. Match at location: 2. Present but no match: 1. Not present: 0.
	while (FoundSolution==false)
	{
		bestWorstCase = 1000000;
		guessNum++;
		cout << "Choosing guess #" << guessNum << endl;
		for (int i = 0; i < Guesses.size(); i++) //Go through all Guesses.
		{
			inSolution = false;
			for (int j = 0; j < totalValuesForLength; j++) //Set counts for each possibility to 0.
			{
				possibilityMatrix[j] = 0;
			}
			for (int j = 0; j < Solutions.size(); j++)//Go through all Solutions.
			{
				if (Guesses[i].compare(Solutions[j])==0)
					inSolution = true;
				for (int k = 0; k < wordLength; k++)
				{
					currentscores[k] = -1; //Reset all scores.
					if (Guesses[i][k] == Solutions[j][k]) //If a value matches, set as 2 (green).
						currentscores[k] = 2;
				}
				for (int k = 0; k < wordLength; k++)
				{
					if (currentscores[k] != 2)
					{
						countInGuess= 0;
						for (int l = 0; l <=k; l++) //Find how many of the letter are in the word up to that point.
						{
							if (Guesses[i][k] == Guesses[i][l]) //Count how many times the letter is found in the guess up to the letter's point. Needed for multiple occurrences of letter in word.
								countInGuess++;
						}
						countInSolution = 0;
						for (int l = 0; l < wordLength; l++) //Count number of guesses in solution.
						{
							if (Guesses[i][k] == Solutions[j][l] && currentscores[l]!=2) //Do not check matched locations to avoid things like guess aah vs cat calling first 'a' yellow.
								countInSolution++;
						}
						if (countInGuess !=0 && countInGuess <= countInSolution) //Since can't be a match, if letter occurs in solution more or equal to guess, letter in wrong place (yellow). Assign 1. 
							currentscores[k] = 1;
						else //If not 2 and not 1, assign 0 (letter not found).
							currentscores[k] = 0;
					}
				}
				//Convert base-3 number into base-10 number.
				currentSolutionValue = 0;
				multiplier = 1;
				for (int k = wordLength-1; k>=0; k--)
				{
					currentSolutionValue = currentSolutionValue + (multiplier * currentscores[k]);
					multiplier = multiplier * 3;
				}
				possibilityMatrix[currentSolutionValue]++; //Increment solutions of this score.
			}
			worstCaseCount = 0; //Look for what the worst number of words kept can be.
			for (int j = 0; j < totalValuesForLength; j++)
			{
				if (possibilityMatrix[j] > worstCaseCount)
					worstCaseCount = possibilityMatrix[j];
			}
			if (worstCaseCount < bestWorstCase) //If the worst case for this word is better than the bestWorstCase word, take new word.
			{
				bestWorstCase = worstCaseCount;
				bestGuessIndex = i;
			}
			else if (worstCaseCount == bestWorstCase)
			{
				if (inSolution == true)
				{
					bestWorstCase = worstCaseCount;
					bestGuessIndex = i;
				}
			}
		}
		cout << "Guess: " << Guesses[bestGuessIndex] << ". Worst case scenario is " << bestWorstCase << " possible words left." << endl;
		cout << "Enter numeric guess result. 2 for green, 1 for yellow, 0 for grey. Press [enter] after last number. No spaces." << endl;
		cin >> guessResultString;
		inputCorrect = true;
		for (int i = 0; i < wordLength; i++)
		{
			singleLetter = guessResultString[i];
			guessResultInt[i] = atoi(singleLetter.c_str());
			if (guessResultInt[i] > 2 || guessResultInt < 0)
				inputCorrect = false;
		}

		while (guessResultString.size() != wordLength || inputCorrect==false )
		{
			if (guessResultString.size() != wordLength)
				cout << "You entred a result " << guessResultString.size() << " characters long. You need a " << wordLength << " letter result. Try again." << endl;
			else
				cout << "You entered a number that wasn't 0, 1, or 2. Try again." << endl;
			cout << "Enter numeric guess result. 2 for green, 1 for yellow, 0 for grey. Press [enter] after last number. No spaces." << endl;
			cin >> guessResultString;
			inputCorrect = true;
			for (int i = 0; i < wordLength; i++)
			{
				singleLetter = guessResultString[i];
				guessResultInt[i] = atoi(singleLetter.c_str());
				if (guessResultInt[i] > 2 || guessResultInt < 0)
					inputCorrect = false;
			}
		}
		//Check if we found solution.
		FoundSolution = true;
		for (int i = 0; i < wordLength; i++)
		{
			if (guessResultInt[i] != 2)
			{
				FoundSolution = false;
				i=wordLength;
			}
		}
		//Keep only words that match guess result.
		tempSolutions.clear();
		for (int j = 0; j < Solutions.size(); j++)//Go through all Solutions.
		{
			for (int k = 0; k < wordLength; k++)
			{
				currentscores[k] = -1; //Reset all scores.
				if (Guesses[bestGuessIndex][k] == Solutions[j][k]) //If a value matches, set as 2 (green).
					currentscores[k] = 2;
			}
			for (int k = 0; k < wordLength; k++)
			{
				if (currentscores[k] != 2)
				{
					countInGuess = 0;
					for (int l = 0; l <= k; l++) //Find how many of the letter are in the word up to that point.
					{
						if (Guesses[bestGuessIndex][k] == Guesses[bestGuessIndex][l]) //Count how many times the letter is found in the guess up to the letter's point. Needed for multiple occurrences of letter in word.
							countInGuess++;
					}
					countInSolution = 0;
					for (int l = 0; l < wordLength; l++) //Count number of guesses in solution.
					{
						if (Guesses[bestGuessIndex][k] == Solutions[j][l])
							countInSolution++;
					}
					if (countInGuess <= countInSolution) //Since can't be a match, if letter occurs in solution more or equal to guess, letter in wrong place (yellow). Assign 1. 
						currentscores[k] = 1;
					else //If not 2 and not 1, assign 0 (letter not found).
						currentscores[k] = 0;
				}
			}
			keepWord = true;
			for (int k = 0; k < wordLength; k++)
			{
				if (guessResultInt[k] != currentscores[k])
					keepWord = false;
			}
			if (keepWord == true)
				tempSolutions.push_back(Solutions[j]);
		}
		//Shrink Solutions.
		Solutions = tempSolutions;
		cout << "After guess #" << guessNum << ", there are " << Solutions.size() << " possible solutions remaining." << endl;
		for (int i = 0; i < Solutions.size(); i++)
			cout << Solutions[i] << " ";
		cout << endl;
	}

	cout << "Congratulations! You solved the wordle!" << endl;
	return 0;
}