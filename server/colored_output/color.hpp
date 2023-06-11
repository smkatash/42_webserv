#pragma once

#include <iostream> 
// #define RED "\033[1;31m"
#define BRIGHT_PINK "\033[38;5;225m"
#define LILLA "\033[38;5;189m"
#define LAVANDA "\033[38;5;153m"
#define LIGHT_BLUE "\033[38;5;153m"
#define BRIGHT_CYAN "\033[38;5;117m"
#define CYAN "\033[38;5;81m"
#define CERULEAN "\033[38;5;45m"
#define GREEN "\033[1;31m"
#define RESET_COLOR "\033[38;5;231m"

#define DEMANDE "\033[38;5;69m"
#define RESPONSE "\033[38;5;69m"

#define ERROR "\033[38;5;210m"
#define ACTION "\033[38;5;49m"
#define NEW_CONNECTION "\033[38;5;229m"
#define CLOSE_CONNECTION "\033[38;5;197m"

#define SERVER "\033[38;5;45m"
#define CLIENT "\033[38;5;189m"

void printHeader();
void printFooter();
void printLaDemande(std::string message, int value);
void printLaResponse(std::string message, int value);
bool printError(std::string message, int value);
void printAction(std::string message, int value);