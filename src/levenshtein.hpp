#ifndef LEVENSHTEIN_H
#define LEVENSHTEIN_H

#include <string>

int levenshtein(std::string string1, std::string string2);

int levenshtein_(std::string string1, std::string string2,
	int swap_penalty, int substitution_penalty,
	int insertion_penalty, int deletion_penalty);

#endif
