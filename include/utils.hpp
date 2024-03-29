#ifndef utils_HPP
#define utils_HPP

#include <vector>
#include <string>
#include <map>
#include <filesystem>
#include "sentence.hpp"

class utils {
  public:
    template<typename A, typename B>
    static std::pair<B,A> flip_pair(const std::pair<A,B> &p)
    {
        return std::pair<B,A>(p.second, p.first);
    }

    template<typename A, typename B>
    static std::multimap<B,A> flip_map(const std::map<A,B> &src)
    {
        std::multimap<B,A> dst;
        std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()),
                      flip_pair<A,B>);
        return dst;
    }
  template<typename A>
  static void printMatrix(const std::vector<std::vector<A>>& matrix) {
    size_t nrow = matrix.size();
    size_t ncol = matrix[0].size();
    std::cout << "Print matrix[" << nrow << "][" << ncol << "]\n";
    for (unsigned int i = 0; i < nrow; i++) {
      for (unsigned int j = 0; j < ncol; j++) {
        std::cout << matrix[i][j] << "    ";
      }
      std::cout << std::endl;
    }
  }

  static std::string tolowerStr(std::string s);
  static std::vector<std::string> textParse(const std::string & bigString, const std::string& delimiter, std::map<std::string,bool>& stopWordMap);
  static void parseOneFile(const char *filePath, std::map<std::string,bool> stopWordMap, std::vector<std::unique_ptr<Sentence>>* sentenceList);
  static int parseData(const std::filesystem::path& path, std::map<std::string,bool> stopWordMap, std::vector<std::unique_ptr<Sentence>>* sentenceList);
  static int parseStopWordFile(const std::filesystem::path& stopWordPath, std::map<std::string, bool> *stopWordMap);
  static void writeToFile(const std::vector<std::unique_ptr<Sentence>>& sentenceList, const std::vector<float>& pageRank,
                  int numOutputSentence, const std::string& path);

};

#endif