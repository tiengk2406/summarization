#include "pageRank.hpp"
#include "tfidf.hpp"

void PageRank::calculatePagerank(std::vector<std::vector<int>>& adjacencyMatrix, std::vector<float>& pagerank, float dampingFactor, int iterations, float epsilon) {
  int numPages = adjacencyMatrix.size();
  std::vector<float> newPagerank(numPages, 1.0 / numPages);

  for (int iter = 0; iter < iterations; ++iter) {
    for (int i = 0; i < numPages; ++i) {
      float incomingPR = 0.0;
      for (int j = 0; j < numPages; ++j) {
        if (adjacencyMatrix[j][i] == 1) {
          incomingPR += pagerank[j] / static_cast<float>(adjacencyMatrix[j].size());
        }
      }
      newPagerank[i] = (1.0 - dampingFactor) / numPages + dampingFactor * incomingPR;
    }

    if (calNorm(pagerank, newPagerank) < epsilon) {
      std::cout << "PageRank stop at: " << iter << std::endl;
      break;
    }

    pagerank = newPagerank;
  }
}


std::vector<std::vector<float>> PageRank::tfidf2ConsineMat(const std::vector<std::vector<float>> &tfidfMatA, const std::vector<std::vector<float>> &tfidfMatB) {
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

float PageRank::calNorm(const std::vector<float>& v1, const std::vector<float>& v2)
{
  float ret = 0;
  for (size_t i = 0; i < v1.size(); i++) {
      ret += (v1[i] - v2[i]) * (v1[i] - v2[i]);
  }
  return sqrt(ret);
}

float PageRank::cosineSimilarity(const float* MatA, const float*  MatB, size_t lengh)
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

std::vector<std::vector<float>> PageRank::calTfidfMatrix() {
  std::vector<std::vector<std::string>>  data;
  for (auto it = std::begin(*sentenceList); it != std::end(*sentenceList); ++it)
  {
    Sentence* stemp = it->get();
    data.push_back(stemp->wordList);
  }

  tfidf ins(data);
  // ins.printMat();
  return ins.weightMat;
}

std::vector<std::vector<int>> PageRank::createAdjacencyMatrix(const std::vector<std::vector<float>>& cosineMatrix, float threshold) {
  size_t numRows = cosineMatrix.size();
  size_t numCols = cosineMatrix[0].size();

  std::vector<std::vector<int>> adjacencyMatrix(numRows, std::vector<int>(numCols, 0));

  for (size_t i = 0; i < numRows; ++i) {
    for (size_t j = i; j < numCols; ++j) {
      if (cosineMatrix[i][j] >= threshold) {
        adjacencyMatrix[i][j] = 1;  // link
        adjacencyMatrix[j][i] = adjacencyMatrix[i][j];
      }
    }
  }

  return adjacencyMatrix;
}

std::vector<float> PageRank::calCentroid(const std::vector<std::vector<float>>& data) {
  int nrow = data.size();
  int ncol = data[0].size();
  std::vector<float> ret;
  for (int i = 0; i < ncol; ++i) {
    float val = 0.0;
    for (int j = 0; j < nrow; ++j) {
      val += data[j][i];
    }

    ret.push_back(val / nrow);
  }

  return ret;
}

void PageRank::calculateCompositeScore(const std::vector<float>& centroid, const std::vector<std::vector<float>>& tfidfMatrix,
                                       const std::vector<float>& pageRankScore, float alpha, std::vector<float>& ret) {
  // cal cosine from tfidf to centail
  size_t size = pageRankScore.size();
  for (size_t i = 0; i < size; ++i) {
    float consineVal = cosineSimilarity(tfidfMatrix[i].data(), centroid.data(), centroid.size());
    float val = alpha * consineVal + (1 - alpha) * pageRankScore[i];
    ret.push_back(val);
  }
}