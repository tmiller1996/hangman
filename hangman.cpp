#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <memory>
#include <algorithm>
#include <string>

template<typename Iter, typename RandomGenerator>
Iter random_select(Iter start, Iter end, RandomGenerator &rng){
	std::uniform_int_distribution<> distribution(0, std::distance(start, end) - 1);
	std::advance(start, distribution(rng));
	return start;
}

bool ends_with(const std::string &str, const std::string &ending){
    if(ending.size() > str.size()){
    	return false;
    }
    return std::equal(std::rbegin(ending), std::rend(ending), std::rbegin(str));
}

template<typename Iter, typename T>
bool contains(Iter begin, Iter end, const T &elem){
	Iter it = std::find(begin, end, elem);
	return it != end;
}

template<typename Container, typename T>
bool contains(Container container, const T &elem){
	return contains(std::begin(container), std::end(container), elem);
}

class Dict {
protected:
	std::string path;
	std::vector<std::string> lines;
	std::default_random_engine rng;

	std::string randomWord() {
		auto it = random_select(std::begin(lines), std::end(lines), rng);
		lowerWord(*it);
		return *it;
	}

	bool wordValid(const std::string &word){
		if(ends_with(word, "'s")){
			return false;
		}
		return true;
	}

	void lowerWord(std::string &word){
		for(auto &c : word){
			c = std::tolower(c);
		}
	}

public:
	Dict(const std::string &path) : path(path), lines(), rng(std::random_device()()){
		std::ifstream ifile(path);
		std::string word;
		while(std::getline(ifile, word)){
			lines.push_back(word);
		}
	}

	std::string getWord() {
		auto word = randomWord();
		while(!wordValid(word)){
			word = randomWord();
		}
		return word;
	}
};

class Hangman {
protected:
	std::string word;
	uint32_t lives;
	std::vector<char> guessed;
	std::vector<char> misses;

	bool won() {
		for(const char &c : word){
			auto it = std::find(std::begin(guessed), std::end(guessed), c);
			if(it == std::end(guessed)){
				return false;
			}
		}
		return true;
	}

	bool lost() {
		return lives == 0;
	}

	bool validGuess(char guess){
		return guess >= 'a' && guess <= 'z';
	}

public:
	Hangman(const std::string &word, uint32_t lives) : word(word), lives(lives) {
	}

	void play(){
		while(true){
			char guess = std::cin.get();
			if(guess != '\n'){
				if(validGuess(guess)){
					if(!contains(guessed, guess)){
					}
					else{
						std::cout << "You already guessed '" << guess << "'" << std::endl;
					}
				}
				else{
					std::cout << "'" << guess << "' is not a valid guess" << std::endl;
				}
			}
		}
	}
};

int main(){
	Dict dict("/usr/share/dict/words");
	Hangman hm(dict.getWord(), 10);
	hm.play();
	return 0;
}