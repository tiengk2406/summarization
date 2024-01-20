#include <iostream>
#include <fstream>
#include <string>

#include "utils.hpp"

std::string tolowerStr(std::string s)
{
  std::transform(s.begin(), s.end(), s.begin(),
                  [](unsigned char c){ return std::tolower(c); }
                );
  return s;
}

std::vector<std::string> textParse(const std::string & bigString, const std::string& delimiter, std::map<std::string,bool>& stopWordMap) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = bigString.find_first_of(delimiter, pos_start)) != std::string::npos) {
      token = bigString.substr (pos_start, pos_end - pos_start);
      pos_start = pos_end + delim_len;
      token.erase(remove(token.begin(), token.end(), ' '), token.end());
      token = tolowerStr(token);
      if (stopWordMap[token] == true) {
        continue;
      }

      res.push_back(token);
  }

  res.push_back(bigString.substr(pos_start));
  return res;
}

void parseOneFile(const char *filePath, const std::map<std::string,bool> &stopWordMap, std::vector<std::unique_ptr<Sentence>>* sentenceList) {
  std::ifstream file;
  file.open(filePath, std::ifstream::in);
  while (!file.eof()) {
    std::unique_ptr<Sentence> sentence = std::make_unique<Sentence>();
    std::string lineContent;
    char docID[200] = {}, content[5000] = {};
    int num = 0, wdCount = 0;
    std::getline(file, lineContent);
    // format for each line: ex
    // <s docid="AP900118-0029" num="15" wdcount="9"> ``Honecker took things into his own hands,'' Schabowski said.</s>
    sscanf(lineContent.c_str(), "<s docid=\"%s\" num=\"%d\" wdcount=\"%d\">%s<s>",
            docID, &num, &wdCount, content);


    sentence->docID = docID + (std::string)("_") + std::to_string(num);
    sentence->wordList = textParse(content, " ,.`", stopWordMap);
    sentence->wdCount = wdCount;
    sentenceList->push_back(std::move(sentence));
  }
  file.close();
}

int parseData(const std::filesystem::path& path, const std::map<std::string,bool> stopWordMap, std::vector<std::unique_ptr<Sentence>>* sentenceList){
  int result = FAILURE;
  result = std::filesystem::exists(path);
  if (result) {
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
      if (std::filesystem::is_directory(entry.path())) {
        std::cout << entry.path().c_str() << " is a directory. skip it\n";
        continue;
      }

      parseOneFile(entry.path().c_str(), stopWordMap, sentenceList);
    }

  }
  std::cout << "vector size = " << sentenceList->size() << std::endl;
  return SUCCESS;
}

int parseStopWordFile(const std::filesystem::path& stopWordPath, std::map<std::string, bool> *stopWordMap) {
  if (!std::filesystem::exists(stopWordPath)) {
    std::cerr << "[path=" << stopWordPath.c_str() << "] did not exist\n";
    return FAILURE;
  }

  std::ifstream file;
  file.open(stopWordPath.c_str(), std::ifstream::in);
  while (!file.eof()) {
    std::unique_ptr<Sentence> sentence = std::make_unique<Sentence>();
    std::string lineContent;
    std::getline(file, lineContent);
    if (lineContent.length() > 0)
      stopWordMap->insert({tolowerStr(lineContent), true});
  }

  file.close();
  return SUCCESS;
}

