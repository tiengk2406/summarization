#ifndef SENTENCE_HPP
#define SENTENCE_HPP

#include <vector>
#include <string>

const int FAILURE = 1;
const int SUCCESS = 0;
class Sentence {
public:
  std::string docID;
  std::vector<std::string> wordList;
  int wdCount;

};

#endif