#include "pageRank.hpp"
#include "utils.hpp"

const int NUM_OF_SENTENCES_OUT = 10;
const float EPSILON = 0.00000056;
const float THRESHOLD_PAGE_RANK = 0.3666;
int summurize(const std::filesystem::path& input, const std::filesystem::path& output,
              const std::filesystem::path& stopWordPath) {
  int ret = FAILURE;
  std::vector<std::unique_ptr<Sentence>> sentenceList;
  std::map<std::string, bool> stopWordMap;
  if (utils::parseStopWordFile(stopWordPath, &stopWordMap) == FAILURE) {
    std::cerr << "donot use stop word..." << std::endl;
  }

  ret = utils::parseData(input, stopWordMap, &sentenceList);
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
  std::cout << "Consine matrix: [Row=" << cosineSize << "]\n\t\t[Col=" << consineMatrix[0].size() << "]\n";

  //Convert to linkMatrix
  std::vector<std::vector<int>> linkMatrix = pageRank.createLinkMatrix(consineMatrix, THRESHOLD_PAGE_RANK);
  consineMatrix.clear();
  utils::printMatrix(linkMatrix);
  std::vector<float> pageRankVal(cosineSize, 1.0 / cosineSize);
  float dampingFactor = 0.85, epsilon = EPSILON;
  int iterations = 100;
  pageRank.calculatePagerank(linkMatrix, pageRankVal, dampingFactor, iterations, epsilon);
  linkMatrix.clear();
  //print output
  int numOutputSentence = NUM_OF_SENTENCES_OUT;
  utils::writeToFile(sentenceList, pageRankVal, numOutputSentence, output);
  return ret;
}

int main(int argc, char* argv[]) {
  if (argc < 4) {
    std::cout << "missing params:  " << argc << std::endl;
    std::cout << "USAGE: summarizaion_app [param1] [param2] [param3]\n";
    std::cout << "       [param1]: input folder that contains n files need summurize.\n";
    std::cout << "       [param2]: output folder that contains output file\n";
    std::cout << "       [param3]: path to stopwork.txt file\n.";
    std::cout << "exmample: summarizaion_app train/test/ ./out/ train/stopwords.txt\n";
    return -1;
  }

  return summurize(std::filesystem::path(argv[1]), std::filesystem::path(argv[2]), std::filesystem::path(argv[3]));
}
