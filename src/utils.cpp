#include <iostream>
#include <fstream>
#include <string>

#include "utils.hpp"

std::string utils::tolowerStr(std::string s)
{
  std::transform(s.begin(), s.end(), s.begin(),
                  [](unsigned char c){ return std::tolower(c); }
                );
  return s;
}

std::vector<std::string> utils::textParse(const std::string & bigString, const std::string& delimiter, std::map<std::string,bool>& stopWordMap) {
  std::string token;
  std::vector<std::string> res;
  size_t beg, pos = 0;
  while ((beg = bigString.find_first_not_of(delimiter, pos)) != std::string::npos)
  {
    pos = bigString.find_first_of(delimiter, beg + 1);
    token = tolowerStr(bigString.substr(beg, pos - beg));
    if (stopWordMap[token] == true) {
      continue;
    }

    res.push_back(token);
  }

  return res;
}

void utils::parseOneFile(const char *filePath, std::map<std::string,bool> stopWordMap, std::vector<std::unique_ptr<Sentence>>* sentenceList) {
  std::ifstream file;
  file.open(filePath, std::ifstream::in);
  std::cout << "File name for training: "<< filePath << std::endl;
  while (!file.eof()) {
    std::unique_ptr<Sentence> sentence = std::make_unique<Sentence>();
    std::string lineContent;
    char docID[200] = {}, content[5000] = {};
    int num = 0, wdCount = 0;
    std::getline(file, lineContent);
    // format for each line: ex  " \"%[^\"]\"",
    // <s docid="AP900118-0029" num="15" wdcount="9"> ``Honecker took things into his own hands,'' Schabowski said.</s>
    sscanf(lineContent.c_str(), "<s docid=\"%100[^\"]\" num=\"%d\" wdcount=\"%d\">%[^\n]s</s>",
            docID, &num, &wdCount, content);
    if (wdCount <= 5 || std::strlen(content) <= 10) {
      continue;
    }
    // remove </s>
    if (std::strlen(content) > 4)
      content[std::strlen(content) - 4] = 0;
    sentence->docID = docID + (std::string)("_") + std::to_string(num);
    sentence->wordList = textParse(content, {" .,:;!?`"}, stopWordMap);

    sentence->wdCount = wdCount;
    sentenceList->push_back(std::move(sentence));
  }
  file.close();
}

int utils::parseData(const std::filesystem::path& path, std::map<std::string,bool> stopWordMap, std::vector<std::unique_ptr<Sentence>>* sentenceList){
  int result = FAILURE;
  std::cout << "Path for training: " << path.c_str() << std::endl;
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

  return SUCCESS;
}

int utils::parseStopWordFile(const std::filesystem::path& stopWordPath, std::map<std::string, bool> *stopWordMap) {
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

void utils::writeToFile(const std::vector<std::unique_ptr<Sentence>>& sentenceList, const std::vector<float>& pageRank,
                  int numOutputSentence, const std::string& path) {
  std::map<std::string, float> resultMap;
  size_t size = pageRank.size();
  for (size_t i = 0; i < size; ++i) {
    resultMap.insert({sentenceList[i].get()->docID, pageRank[i]});
  }
  std::multimap<float, std::string> dst = utils::flip_map(resultMap);

  int index = 0;
  std::ofstream summurizeOutputFile;
  std::cout << "\nOutput to file : " <<  path << std::endl;
  std::cout << "page size " << pageRank.size() << std::endl;
  summurizeOutputFile.open(path.c_str());

  // for (size_t ii = 0; ii < pageRank.size(); ++ii) {
  //   std::cout << "[PageRanks Score=" << pageRank[ii] << "][DocID=" << sentenceList[ii].get()->docID << "]\n";
  // }

  for(std::multimap<float, std::string>::const_reverse_iterator it = dst.rbegin(); it != dst.rend(); ++it) {
    if (index < numOutputSentence)
    {
      index++;
    } else {
      break;
    }

    summurizeOutputFile << "[docID=" << it->second << "]";
    int sentence_index = 0;
    for (size_t i = 0; i < sentenceList.size(); ++i) {
      if (sentenceList[i].get()->docID == it->second) {
        sentence_index = i;
        break;
      }
    }

    std::vector<std::string>* val = &sentenceList[sentence_index].get()->wordList;
    for (auto str : *val) {
      summurizeOutputFile << str << " ";
    }
    summurizeOutputFile << std::endl;

  }
  summurizeOutputFile.close();
}
