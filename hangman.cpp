#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <algorithm>
#include <string>
#include <sstream>

#include <dirent.h>

bool ends_with(const std::string &str, const std::string &ending){
		if(ending.size() > str.size()){
			return false;
		}
		return std::equal(std::rbegin(ending), std::rend(ending), std::rbegin(str));
}

class Figure {
protected:
	std::string directory;
	std::vector<std::string> frames;

public:
	Figure(const std::string &directory) : directory(directory) {
		// get list of files
		std::vector<std::string> files;
		DIR *d = opendir(directory.c_str());
		if(d){
			struct dirent *dir = nullptr;
			while((dir = readdir(d))){
				if(dir->d_type == DT_REG){
					files.emplace_back(dir->d_name);
				}
			}
			closedir(d);
		}
		// sort in descending order
		std::sort(std::rbegin(files), std::rend(files));
		// read files to frames vector
		for(const auto &file : files){
			std::ifstream ifile(directory + "/" + file);
			if(ifile){
				std::ostringstream osframe;
				osframe << ifile.rdbuf();
				frames.push_back(osframe.str());
			}
		}
	}

	std::size_t size() const {
		return frames.size();
	}

	std::string operator[](std::size_t index){
		return frames.at(index);
	}
};

class Dict {
protected:
	std::string path;
	std::vector<std::string> lines;
	std::default_random_engine rng;

	std::string randomWord() {
		auto it = std::begin(lines);
		std::uniform_int_distribution<> distribution(0, std::distance(std::begin(lines), std::end(lines)) - 1);
		std::advance(it, distribution(rng));
		return *it;
	}

	bool validWord(const std::string &word){
		if(ends_with(word, "'s")){
			return false;
		}
		for(const auto &c : word){
			if(c < 'a' || c > 'z'){
				return false;
			}
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
			lowerWord(word);
			if(validWord(word)){
				lines.push_back(word);
			}
		}
	}

	std::string getWord() {
		auto word = randomWord();
		return word;
	}
};

class Hangman {
protected:
	std::string word;
	Figure figure;
	int lives;
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
		return lives <= 0;
	}

	bool validGuess(char guess){
		return guess >= 'a' && guess <= 'z';
	}

	void handleGuess(char guess){
		if(validGuess(guess)){
			auto it = std::find(std::begin(guessed), std::end(guessed), guess);
			if(it == std::end(guessed)){
				auto it = std::find(std::begin(word), std::end(word), guess);
				if(it != std::end(word)){
					guessed.push_back(guess);
					std::cout << "Correct!" << std::endl;
				}
				else{
					misses.push_back(guess);
					--lives;
					std::cout << "Wrong!" << std::endl;
				}
			}
			else {
				std::cout << "You already guessed '" << guess << "'" << std::endl;
			}
		}
		else{
			std::cout << "'" << guess << "' is not a valid guess" << std::endl;
		}
	}

	void printFigure() {
		auto frame = figure[static_cast<std::size_t>(lives)];
		if(frame.size()){
			std::cout << frame << std::endl;
		}
	}

	void printWord() {
		std::string display(word.size(), '_');
		for(std::size_t i = 0; i < word.size(); ++i){
			char c = word[i];
			auto it = std::find(std::begin(guessed), std::end(guessed), c);
			if(it != std::end(guessed)){
				display[i] = c;
			}
		}
		std::cout << display << std::endl;
	}

public:
	Hangman(const std::string &word, const Figure &fig, uint32_t lives) : word(word), figure(fig), lives(lives) {
	}

	void play(){
		std::string line;
		while(!lost() && !won()){
			printFigure();
			printWord();
			std::getline(std::cin, line);
			if (line.size() == 0){
				std::cout << "Expected input" << std::endl;
			}
			else if (line.size() == 1){
				// got exactly 1 character
				char guess = line[0];
				handleGuess(guess);
				if(won()){
					std::cout << "You won!  The word was '" << word << "'" << std::endl;
				}
				if(lost()){
					std::cout << "Game over!" << std::endl;
					printFigure();
				}
			}
			else {
				std::cout << "Please guess only 1 character" << std::endl;
			}
		}
	}
};

bool hasFlag(const std::vector<std::string> &args, const std::string &flag){
	auto it = std::find(std::begin(args), std::end(args), flag);
	return it != std::end(args);
}

std::pair<bool, std::string> hasFlagAndValue(const std::vector<std::string> &args, const std::string &flag){
	std::pair<bool, std::string> result;
	auto it = std::find(std::begin(args), std::end(args), flag);
	if(it != std::end(args)){
		std::advance(it, 1);
		if(it != std::end(args)){
			result = std::make_pair(true, *it);
		}
		else{
			result = std::make_pair(false, "");
		}
	}
	else{
		result = std::make_pair(false, "");
	}
	return result;
}

void printUsage(){

}

int main(int argc, char **argv){
	std::vector<std::string> args(argv, argv + argc);
	std::cout << args[0] << std::endl;

	bool help = hasFlag(args, "-h");
	if(help){
		printUsage();
		exit(0);
	}

	int lives = 10;
	auto livesResult = hasFlagAndValue(args, "-l");
	if(livesResult.first){
		try{
			lives = std::stoi(livesResult.second);
		}
		catch(std::logic_error &err){
			std::cout << "'-l' flag passed invalid integer" << std::endl;
			exit(0);
		}
	}

	std::string word;
	auto wordResult = hasFlagAndValue(args, "-w");
	if(wordResult.first){
		word = wordResult.second;
	}
	if(word.size() == 0){
		std::string file = "/usr/share/dict/words";
		auto fileResult = hasFlagAndValue(args, "-f");
		if(fileResult.first && wordResult.first){
			std::cout << "an error" << std::endl;
			exit(-1);
		}
		if(fileResult.first){
			file = fileResult.second;
		}
		Dict dict(file);
		word = dict.getWord();
	}
	Figure fig("figure/");
	if(livesResult.first){
		if(static_cast<int>(fig.size()) - 1 != lives){
			std::cout << "figures does not match lives" << std::endl;
			exit(-1);
		}
	}
	else{
		lives = static_cast<int>(fig.size()) - 1;
	}
	Hangman hm(word, fig, lives);
	hm.play();
	return 0;
}