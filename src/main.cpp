#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <cstdio>
#include "word2vec.hpp"

const int FAILURE = 1;
const int SUCCESS = 0;

class Sentence {
public:
  std::string docID;
  std::string content;
  int wdCount;
  
};


class Graph {
  private:
    std::string* vericeList;
    float **data;
  public:
    Graph() {}
    ~Graph(){}
    //void createGraph(
};

void parseOneFile(const char *filePath, std::vector<std::unique_ptr<Sentence>>* sentenceList) {
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
    sentence->content = content;
    sentence->wdCount = wdCount;
    sentenceList->push_back(std::move(sentence));
  } 
  file.close();  
}

int parseData(const std::filesystem::path& path, std::vector<std::unique_ptr<Sentence>>* sentenceList){
  int result = FAILURE;
  result = std::filesystem::exists(path);
  if (result) {
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
      if (std::filesystem::is_directory(entry.path())) {
        std::cout << entry.path().c_str() << " is a directory. skip it\n";
        continue;
      }

      parseOneFile(entry.path().c_str(), sentenceList);
    }

  }
  std::cout << "vector size = " << sentenceList->size() << std::endl; 
  return SUCCESS;
}

void convert2Graph(const std::vector<std::unique_ptr<Sentence>>& sentenceList, Graph* graph) {
  
}

void calPageRank(const Graph& graph, int numFactor, std::vector<std::unique_ptr<Sentence>>* sentenceOutList) {

}
void preprocessData(std::vector<std::unique_ptr<Sentence>>* data) {
   //remove stop word

   // separage word
}

int summurize(const std::filesystem::path& input, const std::filesystem::path& output) {
  int ret = FAILURE;
  std::vector<std::unique_ptr<Sentence>> sentenceList, sentenceOutput;
  ret = parseData(input, &sentenceList);
  if (ret == FAILURE) {
    std::cout << "Can not parser data. [path=" << input.stem().string() << "]\n"; 
    return ret;
  }
  preprocessData(&sentenceList);
  Graph graph;
  convert2Graph(sentenceList, &graph);
  calPageRank(graph, 5, &sentenceOutput);
  // exportResult(output);


  return ret;
}


int main(int argc, char* argv[]) {
    if (argc < 3) {
      std::cout << "missing" << std::endl;
      return -1;
    }
    return summurize(std::filesystem::path(argv[1]), std::filesystem::path(argv[2]));
}
