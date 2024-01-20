#ifndef PAGERANK_HPP
#define PAGERANK_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <cstdio>
#include <map>
#include <functional>
#include <numeric>
#include "sentence.hpp"

class PageRank {
  private:

    int numOfWord;
    std::vector<std::unique_ptr<Sentence>>* sentenceList;
    int convertDoc2Vec(const std::string docMem);
    float cosineSimilarity(const float* MatA, const float*  MatB, size_t lengh);
    float calNorm(const std::vector<float>& v1, const std::vector<float>& v2);

  public:
    PageRank() {
      numOfWord = 0;
      sentenceList = nullptr;
    }

    PageRank(std::vector<std::unique_ptr<Sentence>>* sentenceList_):sentenceList(sentenceList_) {
    }

    ~PageRank(){

    }

    std::vector<std::vector<float>> calTfidfMatrix();
    std::vector<std::vector<float>> tfidf2ConsineMat(const std::vector<std::vector<float>> &tfidfMat, const std::vector<std::vector<float>> &tfidfMat1);
    void calculatePagerank(std::vector<std::vector<float>>& graph, std::vector<float>& pagerank, float dampingFactor, int iterations, float epsilon);

};

#endif