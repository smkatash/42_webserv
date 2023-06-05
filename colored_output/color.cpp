#include "color.hpp"
#include <iostream> 

void printHeader()
{
	std::cout << BRIGHT_PINK 
			  << "____________________________________________________________________________\n" 
			  << "\n\t\t\t  WEB" \
			  << LILLA << "SERV IS" \
			  << LAVANDA << " REA" \
			  << LIGHT_BLUE << "DY 4U" \
			  << BRIGHT_CYAN << "\n\t\t\t...   ╭∩╮" \
			  << CYAN << "ʕ•ᴥ•ʔ" \
			  << CERULEAN << "╭∩╮   <><   " \
			  << ""
			  << "\n____________________________________________________________________________\n" 
			  << std::endl;
}

static void printCredits()
{
	std::cout << CERULEAN 
			  << "____________________________________________________________________________\n" 
			  << "\n\t\t\tWE WOUL" \
			  << BRIGHT_CYAN << "D LIKE T" \
			  << LIGHT_BLUE << "O SAY" \
			  << LAVANDA << " TY" \
			  << LILLA << "\n\t\t\tcredits by"
			  << BRIGHT_PINK << " JFK at 42Hn   " \
			  << ""
			  << "\n____________________________________________________________________________\n" 
			  << std::endl;
}

bool printError(std::string message, int value){
	if (value == 0)
		std::cout << ERROR \
				  << message \
				  << CERULEAN \
				  << std::endl;
	else
		std::cout << ERROR \
			  	  << message \
			  	  << value \
			  	  << CERULEAN \
			  	  << std::endl;
	return false;
}

void printAction(std::string message, int value){
	std::cout << ACTION \
			  << message \
			  << value \
			  << CERULEAN \
			  << std::endl;
}

void printLaDemande(std::string message, int value){
	std::cout << DEMANDE \
			  << "LA DEMANDE ---------------------------------------------------------------------------------------------------------------->>" \
			  << RESET_COLOR \
			  << message \
			  << DEMANDE \
			  << "<<------------------------------------------------------------------------------------------------------------------------END"
			  << value \
			  << CERULEAN \
			  << std::endl;
}

void printLaResponse(std::string message, int value){
	std::cout << RESPONSE \
			  << "LA RESPONSE --------------------------------------------------------------------------------------------------------------->>" \
			  << RESET_COLOR \
			  << message \
			  << DEMANDE \
			  << "<<------------------------------------------------------------------------------------------------------------------------END"
			  << value \
			  << CERULEAN \
			  << std::endl;
}

void printFooter()
{
	std::cout << CERULEAN <<  "\n└---t" \
			  << CYAN << "his" \
			  << BRIGHT_CYAN<< " is " \
			  << LIGHT_BLUE << "a ct" \
			  << LAVANDA  << "rl+C" \
			  << LILLA << " ... " \
			  << BRIGHT_PINK  << "bye bye" \
			  << std::endl;
	printCredits();
}