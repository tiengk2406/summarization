#include "pageRank.hpp"
#include "utils.hpp"

const int NUM_OF_SENTENCES_OUT = 20;
const float EPSILON = 0.0000000000056;
const float THRESHOLD_PAGE_RANK = 0.1;

int summurize(const std::filesystem::path& input, const std::filesystem::path& output,
              const std::filesystem::path& stopWordPath) {
  int ret = FAILURE;
  std::map<std::string, bool> stopWordMap;
  std::vector<std::unique_ptr<Sentence>> sentenceList;
  if (utils::parseStopWordFile(stopWordPath, &stopWordMap) == FAILURE) {
    std::cerr << "donot use stop word..." << std::endl;
  }

  if (!std::filesystem::exists(output)) {
    std::filesystem::create_directory(output);
  }

  for (const auto& entry : std::filesystem::directory_iterator(input)) {
    if (std::filesystem::is_directory(entry.path())) {
      std::cout << entry.path().c_str() << " is a directory. skip it\n";
      continue;
    }
    std::cout << "Path for training: " << entry.path().c_str() << std::endl;
    utils::parseOneFile(entry.path().c_str(), stopWordMap, &sentenceList);
    std::cout << "sentence vector size = " << sentenceList.size() << std::endl;
    PageRank pageRank(&sentenceList);
    //convert to tf-idf
    std::vector<std::vector<float>> tfidfMattrix = pageRank.calTfidfMatrix();
    std::cout << "tfidf matrix: row=" << tfidfMattrix.size() << std::endl;
    std::cout << "tfidf matrix: col=" << tfidfMattrix[0].size() << std::endl;

    // calculated centroid
    std::vector<float> centroid = pageRank.calCentroid(tfidfMattrix);

    //Convert TF-idf to consine
    std::vector<std::vector<float>> consineMatrix = pageRank.tfidf2ConsineMat(tfidfMattrix, tfidfMattrix);
    // tfidfMattrix.clear();
    //Calculate PageRank
    size_t cosineSize = consineMatrix.size();
    std::cout << "Consine matrix: [Row=" << cosineSize << "]\n\t\t[Col=" << consineMatrix[0].size() << "]\n";

    //Convert to linkMatrix
    std::vector<std::vector<int>> linkMatrix = pageRank.createLinkMatrix(consineMatrix, THRESHOLD_PAGE_RANK);

    utils::printMatrix(linkMatrix);
    std::vector<float> pageRankVal(cosineSize, 1.0 / cosineSize);
    float dampingFactor = 0.85, epsilon = EPSILON;
    int iterations = 100;
    pageRank.calculatePagerank(linkMatrix, pageRankVal, dampingFactor, iterations, epsilon);

    std::vector<float> pageRankAndRadVal;
    float alpha = 0.2;
    pageRank.calculateCompositeScore(centroid, tfidfMattrix, pageRankVal, alpha, pageRankAndRadVal);

    //print output
    int numOutputSentence = NUM_OF_SENTENCES_OUT;
    std::string fileOut = output.c_str() + (std::string)"/" + entry.path().stem().c_str();
    utils::writeToFile(sentenceList, pageRankAndRadVal, numOutputSentence, fileOut);
    // utils::writeToFile(sentenceList, pageRankVal, numOutputSentence, fileOut);

    sentenceList.clear();
    tfidfMattrix.clear();
    pageRankVal.clear();
    consineMatrix.clear();
    linkMatrix.clear();
    centroid.clear();
    pageRankAndRadVal.clear();
  }

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
