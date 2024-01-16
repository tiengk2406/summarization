#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <cstdio>
#include <map>
#include <functional>
#include <numeric>

#include "tfidf.hpp"

#ifdef USE_UTILS_FILE
#include "utils.hpp"

#else
const int FAILURE = 1;
const int SUCCESS = 0;
class Sentence {
public:
  std::string docID;
  std::vector<std::string> wordList;
  int wdCount;
  
};

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

class Graph {
  private:

    std::vector<std::vector<float>> tfidf2ConsineMat(const std::vector<std::vector<float>> &tfidfMat, const std::vector<std::vector<float>> &tfidfMat1);
    int convertDoc2Vec(const std::string docMem);
    float cosineSimilarity(const float* MatA, const float*  MatB, size_t lengh);

  public:
    Graph() {}
    ~Graph(){}
    int createGraph(const std::vector<std::unique_ptr<Sentence>>& sentenceList);
    void calculatePagerank(std::vector<std::vector<float>>& graph, std::vector<float>& pagerank, float dampingFactor, int iterations);

};

void Graph::calculatePagerank(std::vector<std::vector<float>>& graph, std::vector<float>& pagerank, float dampingFactor, int iterations) {
  int numPages = graph.size();
  std::vector<float> newPagerank(numPages, 1.0 / numPages);

  for (int iter = 0; iter < iterations; ++iter) {
    for (int i = 0; i < numPages; ++i) {
      float incomingPR = 0.0;
      for (int j = 0; j < numPages; ++j) {
        if (graph[j][i] == 1) {
          incomingPR += pagerank[j] / static_cast<float>(graph[j].size());
        }
      }
      newPagerank[i] = (1.0 - dampingFactor) / numPages + dampingFactor * incomingPR;
    }

    pagerank = newPagerank;
  }
}


std::vector<std::vector<float>> Graph::tfidf2ConsineMat(const std::vector<std::vector<float>> &tfidfMatA, const std::vector<std::vector<float>> &tfidfMatB) {
  std::vector<std::vector<float>> ret;
  float val = 0.0;
  std::vector<float> valRow;
  size_t nrow = tfidfMatA.size();
  size_t ncol = tfidfMatA[0].size();
  for (size_t i = 0; i < nrow; i++) {
    for (size_t j = 0; j < nrow; j++) {
      val = cosineSimilarity(tfidfMatA[i].data(), tfidfMatB[j].data(), ncol);
      valRow.push_back(val);
    }
    ret.push_back(valRow);
    valRow.clear();
  }
  std::cout << "CosineMatrix: [row = " << ret.size() << "].[Col=]" << ret[0].size() << std::endl;

  return ret;
}


float Graph::cosineSimilarity(const float* MatA, const float*  MatB, size_t lengh)
{
  float dot = 0.0, normA = 0.0, normB = 0.0, ret = 0.0 ;
  for (size_t i = 0; i < lengh; ++i) {
    dot += MatA[i] * MatB[i] ;
    normA += MatA[i] * MatB[i] ;
    normB += MatB[i] * MatB[i] ;
  }

  if (normA != 0 && normB != 0) {
    ret = dot/(sqrt(normA) * sqrt(normB));
  }

  return ret;
}


int Graph::createGraph(const std::vector<std::unique_ptr<Sentence>> &sentenceList) {
  std::vector<std::vector<std::string>>  data;
  for (auto it = std::begin(sentenceList); it != std::end(sentenceList); ++it)
  {
    Sentence* stemp = it->get();
    data.push_back(stemp->wordList);
  }

  tfidf ins(data);
	std::vector<std::vector<float>> mat = ins.weightMat;
  std::vector<std::vector<float>> consineMat = tfidf2ConsineMat(mat, mat);
  std::cout << "vector size = " << sentenceList.size() << std::endl; 
  std::cout << "The number of word = " << numOfWord << std::endl;
  std::cout << "tfidf matrix: total row=" << ins.weightMat.size() << std::endl;
	std::cout << "tfidf matrix: total col=" << ins.weightMat[0].size() << std::endl;
  // ins.printMat();
  // ins.printVocabList();
  // std::cout << "so dong = " << consineMat.size() << "so cot = " << consineMat[0].size() << std::endl;
  size_t cosineSize = consineMat.size();
  std::vector<float> pagerank(consineMat.size(), 1.0 / consineMat.size());
  float dampingFactor = 0.85;
  int iterations = 100;
  calculatePagerank(consineMat, pagerank, dampingFactor, iterations);
  std::cout << "Pagerank values:\n";
  for (size_t i = 0; i < cosineSize; ++i) {
    std::cout << "Page " << i + 1 << ": " << pagerank[i] << "\n";
  }

  return SUCCESS;
}

int summurize(const std::filesystem::path& input, const std::filesystem::path& /*output*/, 
              const std::filesystem::path& stopWordPath) {
  int ret = FAILURE;
  std::vector<std::unique_ptr<Sentence>> sentenceList, sentenceOutput;
  std::map<std::string, bool> stopWordMap;
  if (parseStopWordFile(stopWordPath, &stopWordMap) == FAILURE) {
    std::cerr << "donot use stop word..." << std::endl;
  }

  ret = parseData(input, stopWordMap, &sentenceList);
  if (ret == FAILURE) {
    std::cout << "Can not parser data. [path=" << input.stem().string() << "]\n"; 
    return ret;
  }
  
  Graph graph;
  graph.createGraph(sentenceList);


  return ret;
}


int main(int argc, char* argv[]) {
    if (argc < 4) {
      std::cout << "missing" << std::endl;
      return -1;
    }
    return summurize(std::filesystem::path(argv[1]), std::filesystem::path(argv[2]), std::filesystem::path(argv[3]));
}
