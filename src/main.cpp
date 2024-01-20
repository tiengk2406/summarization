#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <cstdio>
#include <map>
#include <functional>
#include <numeric>
#include "pageRank.hpp"

#ifdef USE_UTILS_FILE
#include "utils.hpp"

#else

std::string tolowerStr(std::string s)
{
  std::transform(s.begin(), s.end(), s.begin(),
                  [](unsigned char c){ return std::tolower(c); }
                );
  return s;
}
int numOfWord = 0;
std::vector<std::string> textParse(const std::string & bigString, const std::string& delimiter, std::map<std::string,bool>& stopWordMap) {
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
    numOfWord ++;
    res.push_back(token);
  }

  return res;
}

void parseOneFile(const char *filePath, std::map<std::string,bool> stopWordMap, std::vector<std::unique_ptr<Sentence>>* sentenceList) {
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

int parseData(const std::filesystem::path& path, std::map<std::string,bool> stopWordMap, std::vector<std::unique_ptr<Sentence>>* sentenceList){
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
#endif

template<typename A, typename B>
static std::pair<B,A> flip_pair(const std::pair<A,B> &p)
{
    return std::pair<B,A>(p.second, p.first);
}

template<typename A, typename B>
std::multimap<B,A> flip_map(const std::map<A,B> &src)
{
    std::multimap<B,A> dst;
    std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()),
                  flip_pair<A,B>);
    return dst;
}

void writeToFile(const std::vector<std::unique_ptr<Sentence>>& sentenceList, const std::vector<float>& pageRank,
                  int numOutputSentence, const std::filesystem::path& output) {
  std::map<std::string, float> resultMap;
  size_t size = pageRank.size();
  for (size_t i = 0; i < size; ++i) {
    resultMap.insert({sentenceList[i].get()->docID, pageRank[i]});
  }
  std::multimap<float, std::string> dst = flip_map(resultMap);

  std::cout << "\nsummariztion order:\n" << std::endl;
  int index = 0;
  std::ofstream summurizeOutputFile;
  std::string path = output.c_str() + std::string("sumurize_output.txt");
  if (!std::filesystem::exists(output)) {
    std::filesystem::create_directory(output);
  }

  summurizeOutputFile.open(path.c_str());
  for(std::multimap<float, std::string>::const_reverse_iterator it = dst.rbegin(); it != dst.rend(); ++it) {
    summurizeOutputFile << "[pageRank value = " << it->first << "]:[docID=]" << it->second << "]:";
     std::vector<std::string>* val = &sentenceList[index].get()->wordList;
    for (auto str : *val) {
      summurizeOutputFile << str << " ";
    }
    summurizeOutputFile << std::endl;
    if (index > numOutputSentence)
    {
      break;
    }
    index ++;
  }
  summurizeOutputFile.close();
}

int summurize(const std::filesystem::path& input, const std::filesystem::path& output,
              const std::filesystem::path& stopWordPath) {
  int ret = FAILURE;
  std::vector<std::unique_ptr<Sentence>> sentenceList;
  std::map<std::string, bool> stopWordMap;
  if (parseStopWordFile(stopWordPath, &stopWordMap) == FAILURE) {
    std::cerr << "donot use stop word..." << std::endl;
  }

  ret = parseData(input, stopWordMap, &sentenceList);
  if (ret == FAILURE) {
    std::cout << "Can not parser data. [path=" << input.stem().string() << "]\n";
    return ret;
  }

  std::cout << "sentence vector size = " << sentenceList.size() << std::endl;
  PageRank pageRank(&sentenceList);
  //convert to tf-idf
  std::vector<std::vector<float>> tfidfMattrix = pageRank.calTfidfMatrix();
  std::cout << "tfidf matrix: row=" << tfidfMattrix.size() << std::endl;
	std::cout << "tfidf matrix: col=" << tfidfMattrix[0].size() << std::endl;
  //Convert TF-idf to consine
  std::vector<std::vector<float>> consineMatrix = pageRank.tfidf2ConsineMat(tfidfMattrix, tfidfMattrix);
  tfidfMattrix.clear();
  //Calculate PageRank
  size_t cosineSize = consineMatrix.size();
  std::cout << "Consine matrix: [Row=" << cosineSize << "]\n\t\t[Col=]" << consineMatrix[0].size() << "]\n";

  std::vector<float> pageRankVal(cosineSize, 1.0 / cosineSize);
  float dampingFactor = 0.85, epsilon = 0.0000056;
  int iterations = 100;
  pageRank.calculatePagerank(consineMatrix, pageRankVal, dampingFactor, iterations, epsilon);
  consineMatrix.clear();
  //print output
  int numOutputSentence = 10;
  writeToFile(sentenceList, pageRankVal, numOutputSentence, output);
  return ret;
}


int main(int argc, char* argv[]) {
    if (argc < 4) {
      std::cout << "missing" << std::endl;
      return -1;
    }
    return summurize(std::filesystem::path(argv[1]), std::filesystem::path(argv[2]), std::filesystem::path(argv[3]));
}
