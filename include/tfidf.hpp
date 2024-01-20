#ifndef tfidf_HPP
#define tfidf_HPP

#include <vector>
#include <string>

class tfidf {
private:
	std::vector<std::vector<float>> dataMat; // converted bag of words matrix
	unsigned int nrow; // matrix row number
	unsigned int ncol; // matrix column number
	std::vector<std::vector<std::string>> rawDataSet; // raw data
	std::vector<std::string> vocabList; // all terms
	std::vector<int> numOfTerms; // used in tf calculation

	void createVocabList();
	inline std::vector<float> bagOfWords2VecMN(const std::vector<std::string> & inputSet);
	void vec2mat();
	inline std::vector<float> vecSum(const std::vector<float>& a, const std::vector<float>& b);
	void calMat();

public:
	std::vector<std::vector<float>> weightMat; // TF-IDF weighting matrix
	tfidf(std::vector<std::vector<std::string>> & input);
    void printMat();
    void printVocabList();
};

#endif