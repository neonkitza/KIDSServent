#include <string>

#pragma once

bool hasEnding(std::string const &fullString, std::string const &ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else {
		return false;
	}
}
std::string getIthDigitFromEnd(std::string text, int i)
{
	return text.substr(text.length() - i, text.length() - i + 1);
}

std::string getLastIDigits(std::string text, int i)
{
	return text.substr(text.length() - i);
}

std::string getStartID(std::string text)
{
	return text.substr(0, 1);
}

std::string getIStartingID(std::string text, int i)
{
	return text.substr(0, i);
}
std::string getEndID(std::string text)
{
	return text.substr(text.length() - 1);
}
bool StartsWith(const std::string& text, const std::string& token)
{
	if (text.length() < token.length())
		return false;
	return (text.compare(0, token.length(), token) == 0);
}
