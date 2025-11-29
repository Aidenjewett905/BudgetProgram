//Name:			Budget Program
//Description:	Program allows user to save and load a budget, with specific categories
//				Includes multiple utility options in a menu
//Author:		Aiden Jewett
//Version:		v1.1.1
//Version Date: 11/28/2025

#include <iostream>
#include <string>
#include <array>
#include <fstream>
#include <cctype>
#include <iomanip>
#include <limits>

using namespace std;

//Category of the budget with a balance, name, and percentage of income assigned to budget.
//I.E. Savings, Wants, Needs
int numOfCategories = 0; //Counts the number of categories currently in existence
int nextIDNumber = 0;
class BudgetCategory {
	string name = "default";
	double balance = 0.0;
	double budgetPercentage = 0.0; // 0-1
	int idNumber = 0; //Wont change, even if the ID beforehand is removed
public:
	BudgetCategory() {
	}
	BudgetCategory(string name, double balance, double budgetPercentage) {
		this->name = name;
		this->balance = balance;
		this->budgetPercentage = budgetPercentage;
		numOfCategories++;
		idNumber = ++nextIDNumber;
	}
	void setName(string name) {
		this->name = name;
	}
	string getName() const {
		return name;
	}
	void setBalance(double balance)	{
		this->balance = balance;
	}
	double getBalance() const {
		return balance;
	}
	void setPercentOfBudget(double budgetPercentage) {
		this->budgetPercentage = budgetPercentage;
	}
	double getPercentOfBudget() const {
		return budgetPercentage;
	}
	void setIDNumber(int id) {
		idNumber = id;
	}
	int getIDNumber() const {
		return idNumber;
	}
	void operator + (double number) {
		balance += number;
	}
	bool operator == (const BudgetCategory& cat2) {
		if (name == cat2.getName())
			return true;
		else
			return false;
	}

};

//The clearBuffer function clears the cin buffer. This will clear bad inputs, or do nothing if there is no bad input.
static void clearBuffer() {
	std::cin.clear();	//Clear errors due to invalid input
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //Clear buffer to allow for new input
}

//This function clears all of the whitespace from a string
//This is performed by cutting all characters after the first whitespace, so it also clears anything after a space
//Parameters: string& word - The string to have its whitespace removed
void static cutWhiteSpace(string& word) {
	for (int i = 0; i < word.length(); i++)
	{
		if (word.at(i) == ' ')
		{
			//If there is a whitespace, erase the rest of the word as spaces are not allowed so
			//this must be the end of the word
			word.erase(i, (word.length() - i)); 
		}
	}
}

//This function splits off every character in a string that is not a digit or .
//This allows the resulting string to be converted into a double or float
//Parameters: string& word - The input string to be reduced to a double
void static cutToDouble(string& word) {
	for (int i = 0; i < word.length(); i++)
	{
		if (!isdigit(word.at(i)) && word.at(i) != '.')
		{
			word.erase(i);
			i--; //removed the index, so it must go back by one or it will skip a character
		}
	}
}

//This function displays the budget categories to the screen in a readable format
//Parameters:	BudgetCategory* catPointer - Pointer to an array that contains all of the budget categories
//				to be output to the screen.
void static displayToScreen(BudgetCategory* catPointer) {
	printf("%2s | %14s %14s %14s\n", "ID", "Category    |", "Balance    |",
		"Percentage   |");
	for (int i = 0; i < numOfCategories; i++)
	{
		printf("%-2d | %-12s | %-12.2f | %-12.2f |\n", (catPointer + i)->getIDNumber(),
			(catPointer + i)->getName().data(),
			(catPointer + i)->getBalance(),
			(catPointer + i)->getPercentOfBudget() * 100);
	}
}

//This function saves all of the categories to a file, in roughly the same format as displayToScreen
//Ideally this function will be saved as a .txt file
//Parameters:	fstream& file - A pointer that leads to the output file
//				BudgetCategory* catPointer - A pointer that leads to the array of all budget categories to be saved
//				int numOfCategories - Stores the total number of categories, including main, in the array
void static saveToFile(fstream& file, BudgetCategory* catPointer, int numOfCategories) {
	//ID |   Category    |     Balance    |     Percentage
	file << setw(2) << right << "ID | "
		<< setw(14) << right << "Category     | " << setw(14) 
		<< right << "Balance    | " << setw(14) << right << "Percentage   |\n";
	for (int i = 0; i < numOfCategories; i++)
	{
		//Name, Balance, Percentage
		file << setw(2) << left << (catPointer + i)->getIDNumber() << " | "
			<< setw(12) << left << (catPointer + i)->getName() << " | " <<
			setw(11) << left << fixed << showpoint << setprecision(2) << (catPointer + i)->getBalance() << " | "
			<< setw(13) << left << fixed << showpoint << setprecision(2) << (catPointer + i)->getPercentOfBudget() * 100
			<< "|";

		//Add a newline as long as this is not the final line, as this messes up loading saves
		if (i != numOfCategories - 1)
			file << "\n";
	}
}

//This function modifies the balance contained within all budget categories
//The balance is distributed based on the percentageOfBudget variable for each category, all adding up to 100%
//Parameters:	double balanceModification - Variable that contains the amount of money to be added/removed
//				BudgetCategory* catPointer - Pointer that leads to the array of budget categories
void static modifyBalance(double balanceModification, BudgetCategory* catPointer) {
	for (int i = 0; i < numOfCategories; i++)
	{
		//Sets the new balance as currentBalance + (balanceModification * percentOfBudget)
		//Aka it adds the income, taking account the category percentage, into each category
		(catPointer + i)->setBalance((catPointer + i)->getBalance() +
		((catPointer + i)->getPercentOfBudget() * balanceModification));
	}
}

//This function gets the accumulated percentage of all budget categories
//This is useful for ensuring that all of the categories add up to 100%
//Parameters:	BudgetCategory* catPointer - Pointer that leads to the array of budget categories
//Returns:	The total accumulated percentage of all categories as a double
double static getTotalPercentage(BudgetCategory* catPointer) {
	double totalPercent = 0.0; //Accumulator
	for (int i = 1; i < numOfCategories; i++)
	{
		totalPercent += (catPointer + i)->getPercentOfBudget();
	}

	return totalPercent;
}

//This function gets the total balance value of each budget category
//This is useful for allocating a balance to the total category
//Parameters:	BudgetCategory* catPointer - Pointer that leads to the array containing the budget categories
//Returns:		The total accumulated balance in all of the budget categories
double static getTotalValue(BudgetCategory* catPointer) {
	double totalBalance = 0.0;
	for (int i = 1; i < numOfCategories; i++)
	{
		totalBalance += (catPointer + i)->getBalance();
	}

	return totalBalance;
}

//This function determines if the inputted integer value refers to the main category
//The main category is defined as being ID 1
//If the category is main, an error message is sent, and if not then the buffer is cleared
//Parameters:	int i - The integer ID to check if equal to 1
//Returns:		if i = 1 true is returned
//				if i != 1 false is returned
bool static isMainCat(int i) {
	if (i == 1)
	{
		cout << "Cannot modify main directly, please select another category.\n";
		return true;
	}
	else
	{
		clearBuffer();
		return false;
	}
}

//This function outputs list of all budget categories and their associated IDs
//Parameters:	BudgetCategory* catPointer - A pointer to the array of budget categories
void static outputIDAndCats(BudgetCategory* catPointer) {
	
	printf("%2s | %-14s\n", "ID", "Category");
	for (int i = 0; i < numOfCategories; i++)
	{
		printf("%2d | %-14s\n", (catPointer + i)->getIDNumber(),
			(catPointer+i)->getName().data());
	}
}

//This function prompts the user to select how much balance they wish to add/subtract from a specific category
//The specific category is decided by the parameters
//The buffer is cleared after the user input, which prevents an invalid input from crashing the program
//In order to find the ID, first the index equal to the ID is checked, and then if not found, a manual 
//search is performed to find the valid ID
//Parameters:	int IDChoice - the ID of the category to be modified
//				BudgetCategory* catPointer - Pointer that leads to the array of budget categories
void static addToCategory(int IDChoice, BudgetCategory* catPointer) {
	double modification = 0;
	bool foundID = false;
	
	cout << "Enter the amount to add to the category, enter a negative value to subtract: ";
	std::cin >> modification;
	clearBuffer(); //Cut off invalid input

	//If the IDs are in order this avoids searching the entire array
	//First part of the statement will prevent an out of bounds exception by ensuring that
	//IDChoice - 1 is a valid index
	if ((IDChoice <= numOfCategories && IDChoice > 0)
		&& (catPointer + (IDChoice - 1))->getIDNumber() == IDChoice)
	{
		(catPointer + (IDChoice - 1))->operator+(modification);
		foundID = true;
	}
	else //Have to search for the right ID number
	{
		//Start at index 1, as index 0 is total which should not be modified
		for (int i = 1; i < numOfCategories; i++)
		{
			if ((catPointer + i)->getIDNumber() == IDChoice)
			{
				(catPointer + i)->operator+(modification);
				foundID = true;
			}
		}
	}

	if (!foundID)
		cout << "ID not found, no addition/subtraction performed";
	else
		catPointer->setBalance(getTotalValue(catPointer));
}

//This function outputs a list of each category and their current percentage of the budget
//Parameters:	BudgetCategory* catPointer - Pointer to the array of budget categories
void static outputCatsAndPercents(BudgetCategory* catPointer) {
	for (int i = 0; i < numOfCategories; i++)
	{
		//Multiply percent by 100 to get the full digit percent (I.E. 45.9%)
		printf("%-14s | %-4.1f%%\n", (catPointer + i)->getName().data(), 
			((catPointer + i)->getPercentOfBudget() * 100));
	}
}

//This function checks if a percentage is not 100%, defined as being a double value == 1.0
//If the value is not 100%, an error message will be displayed
//Parameters:		double percent - the double percentage to be checked
//Returns:			if double != 1.0 true is returned
//					else false is returned
bool static notOneHundredPercent(double percent) {
	if (percent != 1.0)
	{
		cout << "Error: Total percent does not equal 100%. Please re-enter the values.\n";
		return true;
	}
	else
		return false;
}

//This function prompts the user to enter a new percentage for each individual category within the array
//The buffer is cleared after each input, preventing invalid inputs from crashing the program
//If the total value is equal to 100%, the function returns, otherwise it loops again until the total percent is 100
//Parameters:	BudgetCategory* catPointer - Pointer that leads to the list of budget categories
void static setNewPercentages(BudgetCategory* catPointer) {
	cout << "List of Categories:\n";
	outputCatsAndPercents(catPointer);
	cout << "\n";

	double categoryPercent;
	do {
		//Start at index 1 to avoid setting total
		for (int i = 1; i < numOfCategories; i++)
		{
			cout << "Enter the percentage for the " << (catPointer + i)->getName() << 
				" category in decimal format (ex: 45.2): ";
			std::cin >> categoryPercent;
			clearBuffer(); //If an invalid input gets stuck, this will clear it and allow the program to continue.
			categoryPercent /= 100.0;
			(catPointer + i)->setPercentOfBudget(categoryPercent);
		}
	} while (notOneHundredPercent(getTotalPercentage(catPointer)));

	catPointer->setPercentOfBudget(getTotalPercentage(catPointer));

	cout << "\nPercentages set:\n";
	outputCatsAndPercents(catPointer);

}

//This function adds a new category to the list of categories
//The function prompts the user to enter a name and balance, and will initialize the percentage as
//100% if it is the first non-main category, otherwise as 0%
//The buffer is cleared after each input, preventing a bad input from crashing the program
//Parameters:	BudgetCategory* catPointer - Pointer to the array of budget categories
void static addNewCategory(BudgetCategory* catPointer) {
	string catName;
	double startingBalance;
	double startingPercentage;
	cout << "Enter a name for the new category (Do not include spaces): ";
	std::cin >> catName;
	clearBuffer(); //Clear the buffer just in case user included a space, this prevents it from jamming other inputs
	cout << "Enter a starting balance for the category: ";
	std::cin >> startingBalance;
	clearBuffer(); //Clear buffer in case non-int is inputted, will result to some sort of int in startingBalance
	cout << "Percent of Budget will be initialized as 0 or 100, the ID will be automatically chosen.\n";

	catPointer->operator+(startingBalance); //Add the new balance to main

	catPointer += numOfCategories;

	//If this is the only category besides main, it is 100%
	if ((numOfCategories - 1) == 0)
		startingPercentage = 1.0;
	else
		startingPercentage = 0.0;

	*catPointer = BudgetCategory(catName, startingBalance, startingPercentage);
}

//This function erases a budget category from the array, removing the percentages and balances from the total
//The function also moves every single category after the removed category back by 1, so there are no gaps
//Parameters:	BudgetCategory* catPointer - Pointer to the array of categories
//				int index - index (not ID) of the category to be removed
void static eraseArrayCategory(BudgetCategory* catPointer, int index) {
	numOfCategories--;

	catPointer->setBalance(catPointer->getBalance() 
		- ((catPointer + index))->getBalance()); //Remove this cat balance from total
	catPointer->setPercentOfBudget(catPointer->getPercentOfBudget() 
		- ((catPointer + index))->getPercentOfBudget()); //Remove this cat percentage from total
	
	catPointer += index; //Set the pointer to the index to be erased

	//Move everything past the index back by one space
	for (index; index < numOfCategories; index++)
	{
		*catPointer = *(catPointer + 1);
		catPointer++;
	}

	//Now that everything has been moved back, there is one extra category at the last pointer location
	//that will be set to default
	*catPointer = BudgetCategory();

}

//This function facilitates the removal of a category within the array of categories
//First the category is found by ID, first by checking if the category is at the index = ID, then by checking
//through the array.
//If the category is found, eraseArrayCategory is called, and a message is output confirming the removal
//If the category is not found, an error message is output
//Parameters:	int IDChoice - The ID of the category to be removed
//				BudgetCategory* catPointer - Pointer to the array of budget categories
void static removeCategory(int IDChoice, BudgetCategory* catPointer) {
	int indexOfRemoval = 0;
	bool foundID = false;

	//Check the ID in order first, in case the IDs are still ordered in the array
	if ((catPointer + (IDChoice - 1))->getIDNumber() == IDChoice)
	{
		foundID = true;
		indexOfRemoval = IDChoice - 1;
	}
	else //Otherwise it will have to be found manually
	{
		while (indexOfRemoval < numOfCategories && !foundID)
		{
			if ((catPointer + indexOfRemoval)->getIDNumber() == IDChoice)
				foundID = true;
			else
				indexOfRemoval++;
		}
	}

	if (foundID)
	{
		eraseArrayCategory(catPointer, indexOfRemoval);
		cout << "ID " << IDChoice << " found at index " << indexOfRemoval << " and removed.\n";
	}
	else
		cout << "ID " << IDChoice << " not found.\n";
}

int main() {
	bool menu1 = true; //Loop menu 1
	bool menu2 = true; //Loop menu 2
	const int MAX_CATEGORIES = 10; //Max number of categories by default //In the future this can be dynamically expanded when needed.
	array<BudgetCategory, MAX_CATEGORIES> categories;
	fstream file; //The file used for output and input

	bool fileInput = false;
	//First menu, for loading file or creating new file
	while (menu1)
	{
		int choice;
		cout << "Budget Program\n1. New File\n2. Load File\n3. Exit\nChoice: ";
		std::cin >> choice;

		string fileName;
		switch (choice)
		{
			case 1:
				categories[0] = BudgetCategory("main", 0.0, 1.0);
				menu1 = false;
				break;
			case 2:
				cout << "Enter the name of your file: ";
				std::getline(std::cin, fileName); //Clear previous input
				std::getline(std::cin, fileName);
				file.open(fileName, ios::in);
				menu1 = false;
				fileInput = true;
				break;
			case 3:
				menu1 = false;
				menu2 = false;
				break;
			default:
				cout << "Error, invalid choice\n";
				clearBuffer(); //Clear the buffer to make way for the next choice
				break;
		}

	}

	if (fileInput)
	{
		string output;
		double totalBalance = 0.0;
		double totalPercentage = 0.0;
		//Input file info
		BudgetCategory* currentCat = categories.data(); //Pointer for array
		//cout << "CurrentPos = " << file.tellg() << "\n";
		std::getline(file, output); //Clear the first line, as it is for formatting
		currentCat->setName("main"); //Add main, which will not be imported and instead re-created
		currentCat->setIDNumber(1);
		nextIDNumber = currentCat->getIDNumber() + 1; //Set to the next valid ID after this one
		numOfCategories++; //Increment the number of categories to include main

		currentCat++; //Start adding to the next category
		std::getline(file, output); //Clear the second line, as it is for main

		while (!file.eof())
		{
			//First category is always main, even if user manually changed the name
			//main budget percent will be calculated through accumulator, and the balance will be
			//calculated through accumulator. This prevents manual meddling
			//ID
			//cout << "CurrentPos = " << file.tellg() << "\n";
			std::getline(file, output, '|'); //Extract until | character
			cutToDouble(output); //Cut to a number, cutToDouble will still work correcly
			//cout << "DEBUG: cut to double results in " << output << "\n";
			currentCat->setIDNumber(stoi(output));
			//cout << "CurrentPos = " << file.tellg() << "\n";
			//Clear the space character, as the cut functions will delete the entire output if
			//they detect it.
			std::getline(file, output, ' ');
				
			//name
			std::getline(file, output, '|'); //Extract until | character
			//cout << "DEBUG: getline results in " << output << "\n";
			cutWhiteSpace(output); //Cut off the white space from the string, as this is the name
			//cout << "DEBUG: name results in " << output << "\n";
			currentCat->setName(output);
			std::getline(file, output, ' ');

			//balance
			std::getline(file, output, '|'); //Extract until | character
			cutToDouble(output); //Cut all chars that are not digits or .
			//cout << "DEBUG: cut to double 2 results in " << output << "\n";
			currentCat->setBalance(stod(output));
			totalBalance += currentCat->getBalance();
			std::getline(file, output, ' ');

			//budgetPercentage
			std::getline(file, output, '|'); //Extract until | character
			cutToDouble(output); //Cut all chars that are not digits or .
			//cout << "DEBUG: cut to double 3 results in " << output << "\n";
			currentCat->setPercentOfBudget(stod(output) / 100.0); //Divide by 100 to get the 0-1 percentage
			totalPercentage += currentCat->getPercentOfBudget();

			std::getline(file, output); //Clear the rest of the line
			numOfCategories++;
			nextIDNumber = currentCat->getIDNumber(); //Whatever this ID is is the most recent valid ID
			currentCat++;
			//cout << "DEBUG: End of loop\n";
		}

		currentCat = categories.data();
		currentCat->setBalance(totalBalance); //Assign the total balance
		currentCat->setPercentOfBudget(totalPercentage); //Assign the total percentage

		file.close();

	}


	while (menu2) //Second menu for manipulating file
	{
		int choice;
		cout << "\n1. Display\n2. Add/Subtract balance\n3. Add/Subtract from category\n" << 
			"4. Modify category percentage\n5. Add/Remove category\n" << 
			"6. Save File\n7. Exit\nChoice: ";
		std::cin >> choice;

		char yOrNChoice;
		double moneyAmount;
		int IDChoice;
		string fileName;
		fstream file2 = fstream();
		switch (choice)
		{
		case 1: //Display
			displayToScreen(categories.data());
			break;
		case 2: //Add/Subtract balance
			cout << "How much balance are you adding? (enter a negative value for subtraction): ";
			std::cin >> moneyAmount;
			clearBuffer(); //If the user entered an invalid value, it will get cut off of the input.
			modifyBalance(moneyAmount, categories.data());
			break;
		case 3: //Add/Subtract from category
			cout << "List of Categories:\n";
			outputIDAndCats(categories.data());
			do {
				cout << "Select a category ID other than main to modify: ";
				std::cin >> IDChoice;
			} while (isMainCat(IDChoice)); //Buffer clear is present in this function, invalid IDChoice becomes 0.
			addToCategory(IDChoice, categories.data());
			break;
		case 4: //Modify category percentage
			if (numOfCategories > 1)
			{
				setNewPercentages(categories.data());
			}
			else
			{
				cout << "No categories except main exist, cannot modify percentages.\n";
			}
			break;
		case 5: //Add/Remove category
			cout << "List of Categories:\n";
			outputIDAndCats(categories.data());
			do {
				cout << "Select a category ID to remove, or select a negative value to add " << 
					"a new category (You cannot select main): ";
				std::cin >> IDChoice;
			} while (isMainCat(IDChoice)); //Makes sure it is not main, and clears buffer in case of error
			if (IDChoice <= 0 && numOfCategories < 10)
			{
				addNewCategory(categories.data());
			}
			else if (IDChoice <= 0 && numOfCategories >= 10)
			{
				cout << "ERROR: There is a limit of 10 categories. Please remove a category to add a new one.\n";
			}
			else
			{
				removeCategory(IDChoice, categories.data());
			}
			break;
		case 6: //Save File
			cout << "Enter the name of the new file (This will override a file if it already exists): ";
			std::getline(std::cin, fileName);
			std::getline(std::cin, fileName);
			file2.open(fileName, ios::out); //Deletes any old data then writes to file
			saveToFile(file2, categories.data(), numOfCategories);
			file2.close();
			break;
		case 7: //Exit
			cout << "Are you sure you wish to exit? If you have not saved a file " <<
				"your data will be lost (y/n): ";
			std::cin >> yOrNChoice;
			if (yOrNChoice == 'y' || yOrNChoice == 'Y')
				menu2 = false;
			break;
		default:
			cout << "Error, invalid choice\n";
			clearBuffer(); //Clear the buffer to make room for next choice
			break;
		}

	}
	
	return 0;
}