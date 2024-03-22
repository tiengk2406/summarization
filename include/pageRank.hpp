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
    std::vector<std::unique_ptr<Sentence>>* sentenceList;
    int convertDoc2Vec(const std::string docMem);
    float cosineSimilarity(const float* MatA, const float*  MatB, size_t lengh);
    float calNorm(const std::vector<float>& v1, const std::vector<float>& v2);

  public:
    PageRank() {
      sentenceList = nullptr;
    }

    PageRank(std::vector<std::unique_ptr<Sentence>>* sentenceList_):sentenceList(sentenceList_) {
    }

    ~PageRank(){

    }

    std::vector<float> calCentroid(const std::vector<std::vector<float>>& data);
    std::vector<std::vector<float>> calTfidfMatrix();
    std::vector<std::vector<float>> tfidf2ConsineMat(const std::vector<std::vector<float>> &tfidfMat, const std::vector<std::vector<float>> &tfidfMat1);
    void calculatePagerank(std::vector<std::vector<int>>& graph, std::vector<float>& pagerank, float dampingFactor, int iterations, float epsilon);
    std::vector<std::vector<int>> createAdjacencyMatrix(const std::vector<std::vector<float>>& cosineMatrix, float threshold);
    void calculateCompositeScore(const std::vector<float>& centroid, const std::vector<std::vector<float>>& tfidfMatrix,
                                 const std::vector<float>& pageRankScore, float alpha, std::vector<float>& result);

};

#endif