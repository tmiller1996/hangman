#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <memory>

template<typename Iter, typename RandomGenerator>
Iter random_select(Iter start, Iter end, RandomGenerator &rng){
	std::uniform_int_distribution<> distribution(0, std::distance(start, end) - 1);
	std::advance(start, distribution(rng));
	return start;
}

class Dict {
public:
	Dict() = default;
	virtual ~Dict() = default;

	virtual std::string getWord() = 0;
};

class DictFile : public Dict {
protected:
	std::string path;
	std::vector<std::string> lines;
	std::default_random_engine rng;

	std::string getWord() override {
		auto it = random_select(std::begin(lines), std::end(lines), rng);
		return *it;
	}
public:
	DictFile(const std::string &path) : Dict(), path(path), rng(std::random_device()()) {
		std::ifstream ifile(path);
		std::string word;
		while(std::getline(ifile, word)){
			lines.push_back(word);
		}
	}

	~DictFile() override = default;
};

int main(){
	std::unique_ptr<Dict> dict = std::make_unique<DictFile>("/usr/share/dict/words");
	for(int i = 0; i < 5; ++i){
		std::cout << dict->getWord() << std::endl;
	}
	return 0;
}