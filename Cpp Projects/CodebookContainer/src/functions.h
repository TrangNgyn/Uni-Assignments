#include <iostream>
#include <iomanip>
#include <vector>

class Mint
{
	private:
		int value;
		int modulus;
		static const int specialSymbol = 0;
	public:
		Mint() = default;
		Mint(int val, int mod);
		~Mint() = default;
		static int getSpecialSymbol();
		int getValue() const;
		bool equalToZero();
		int operator-(const Mint& obj) const;
};

class Melt
{
	private:
		char value;
		static const char specialSymbol = 'a';
	public:
		Melt() = default;
		~Melt() = default;
		Melt(char val);
		static char getSpecialSymbol();
		char getValue() const;
		bool equalToZero();
		int operator-(const Melt& obj) const;
};

template<typename T>
class Codeword
{
	private:
		T *symbol;
		int size;
	public:
		Codeword(){size = 0;}
		Codeword(T *sym, int s){
			size = s;
			symbol = new T[size];
			for(int i=0; i<size; i++){
				symbol[i] = sym[i];
			}
		}
		int getSize() const {return size;}
		~Codeword(){
			if(symbol != NULL){
				delete [] symbol;
			}
		}
		int Weight(){
			int weight = 0;
			for(int i=0; i < size; i++){
				if(!symbol[i].equalToZero()){
					weight++;
				}
			}
			return weight;
		}
		int Distance(Codeword<T> cw2){
			int distance = 0;
			for(int i=0; i < size; i++){
				distance += (symbol[i] - cw2.symbol[i]);
			}
			return distance;
		}
		void Display(){
			std::cout << std::left;
			for(int i=0; i < size; i++){
				std::cout << std::setw(6) << symbol[i].getValue();
			}
			std::cout << std::setw(6) << "Weight: " << Weight() << std::endl;
		}
		
		Codeword(const Codeword &obj){ //for deep copy when using vector in driver
			size = obj.size;
			symbol = new T[size];
			for(int i=0; i<size; i++){
				symbol[i] = obj.symbol[i];
			}
		}
};
template<typename T>
class Codebook
{
	private:
		T *codewords;
		int size;
	public:
		Codebook(T *cw, int s) {
			size = s;
			codewords = new T[size];
			for(int i=0; i<size; i++){
				codewords[i] = cw[i];
			}
		}
		~Codebook(){
			if(codewords != NULL){
				delete [] codewords;
			}
		}
		int minimumWeight(){
			if(size == 1){
				return 0;
			}
			int min = codewords[1].Weight();
			for(int i=2; i < size; i++){
				if(min > codewords[i].Weight()){
					min = codewords[i].Weight();
				}
			}
			return min;
		}
		int** calcDistance(){
			int **d = new int*[size];
			for(int i=0; i<size; i++){
				d[i] = new int[size];
				for(int j=0; j<size; j++){
					d[i][j] = codewords[i].Distance(codewords[j]);
				}
			}
			return d;
		}
		int minimumDistance(){
			if(size == 1){
				return 0;
			}
			int **d = calcDistance();		
			int min =  d[0][1];		
			for(int i=0; i<size; i++){
				for(int j=0; j<(size-i); j++){
					if(min > d[i][j] && i != j){
						min = d[i][j];
					}
				}
			}
			for(int i=0; i<size; i++){
				delete [] d[i];
			}
			delete [] d;
			return min;
		}
		void Display(){
			for(int i=0; i<(size); i++){
				codewords[i].Display();
			}
			std::cout << "Minimum weight: " << minimumWeight() << std::endl;
			std::cout << "Minimum distance: " << minimumDistance() << std::endl;
			
			printDistanceTable();
		}
		void printDistanceTable(){
			std::cout << "Distance table: " << std::endl;
			int **d = calcDistance();
			std::cout << std::right;
			
			for(int i=0; i< size; i++){
				for(int j=0; j<size; j++){
					std::cout << std::setw(6) << d[i][j];
				}
				std::cout << std::endl;
			}
			for(int i=0; i<size; i++){
				delete [] d[i];
			}
			delete [] d;
		}
};

bool checkArgs(int argc, char *argv[]);
bool isPositiveInt(char cstring[]);
void populateBook(std::vector<Codeword<Mint>> &cw, int length, int size, int seed, int mod);
void populateBook(std::vector<Codeword<Melt>> &cw, int length, int size, int seed);
