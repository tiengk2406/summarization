#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include "tfidf.hpp"
#include <valarray>


tfidf::tfidf(std::vector<std::vector<std::string>> & input):rawDataSet(input)
{
  calMat();
}

void tfidf::createVocabList()
{
  std::set<std::string> vocabListSet;
  for (std::vector<std::string> document : rawDataSet)
  {
    for (std::string word : document)
      vocabListSet.insert(word);
  }
  std::copy(vocabListSet.begin(), vocabListSet.end(), std::back_inserter(vocabList));
}

inline std::vector<float> tfidf::bagOfWords2VecMN(const std::vector<std::string> & inputSet)
{
  std::vector<float> returnVec(vocabList.size(), 0);
  for (std::string word : inputSet)
  {
    size_t idx = std::find(vocabList.begin(), vocabList.end(), word) - vocabList.begin();
    if (idx == vocabList.size())
      std::cout << "word: " << word << "not found" << std::endl;
    else
      returnVec.at(idx) += 1;
  }
  return returnVec;
}

void tfidf::vec2mat()
{
  int cnt(0);
  for (auto it = rawDataSet.begin(); it != rawDataSet.end(); ++ it)
  {
    cnt ++;
    std::cout << cnt << "\r";
    std::cout.flush();
    dataMat.push_back(bagOfWords2VecMN(*it));
    numOfTerms.push_back(it->size());
    it->clear();
  }
  std::cout << std::endl;
  ncol = dataMat[0].size();
  nrow = dataMat.size();
  rawDataSet.clear(); // release memory
}

inline std::vector<float> tfidf::vecSum(const std::vector<float>& a, const std::vector<float>& b)
{
  assert(a.size() == b.size());
  std::vector<float> result;
  result.reserve(a.size());
  std::transform(a.begin(), a.end(), b.begin(), 
                  std::back_inserter(result), std::plus<float>());
  return result;
}

void tfidf::calMat()
{
  createVocabList();
  vec2mat();

  std::vector<std::vector<float>> dataMat2(dataMat);
  std::vector<float> termCount;
  termCount.resize(ncol);

  for (unsigned int i = 0; i != nrow; ++i)
  {
    for (unsigned int j = 0; j != ncol; ++j)
    {
      if (dataMat2[i][j] > 1) // only keep 1 and 0
        dataMat2[i][j] = 1;
    }
    termCount = vecSum(termCount, dataMat2[i]); // no. of doc. each term appears
  }
  dataMat2.clear(); //release

  std::vector<float> row_vec;
  for (unsigned int i = 0; i != nrow; ++i)
  {
    for (unsigned int j = 0; j != ncol; ++j)
    {
      float tf = dataMat[i][j] / numOfTerms[i];
      float idf = log((float)nrow / (termCount[j]));
      row_vec.push_back(tf * idf); // TF-IDF equation
    }
    weightMat.push_back(row_vec);
    row_vec.clear();
  }
  nrow = weightMat.size();
  ncol = weightMat[0].size();
}

void tfidf::printMat() {
  std::cout << "Print TFIDF matrix[" << nrow << "][" << ncol << "]\n";
  for (unsigned int i = 0; i < nrow; i++) {
    for (unsigned int j = 0; j < ncol; j++) {
      std::cout << weightMat[i][j] << "     ";
    }
    std::cout << std::endl;
  }
}

void tfidf::printVocabList() {
  for (auto i : vocabList) {
    std::cout << i << "    ";
  }
  std::cout << std::endl;
}