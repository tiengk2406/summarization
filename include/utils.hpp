#ifndef utils_HPP
#define utils_HPP

#include <vector>
#include <string>
#include <map>
#include <filesystem>
#include "sentence.hpp"


std::vector<std::string> textParse(const std::string & bigString, const std::string& delimiter, const std::map<std::string,bool> &stopWordMap);
void parseOneFile(const char *filePath, const std::map<std::string,bool> &stopWordMap, std::vector<std::unique_ptr<Sentence>>* sentenceList);
int parseData(const std::filesystem::path& path, const std::map<std::string,bool> stopWordMap, std::vector<std::unique_ptr<Sentence>>* sentenceList);
std::string tolowerStr(std::string s);
int parseStopWordFile(const std::filesystem::path& stopWordPath, std::map<std::string, bool> *stopWordMap);



#endif