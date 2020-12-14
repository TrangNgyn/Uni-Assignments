//enum of the positions of chrononauts
enum POSITION{JumpEngineer = 0, Doctor = 1, Historian = 2, Security = 3, ChronoPet = 4};
enum CHRONO_TYPE {Human = 0, GuineaPig = 1, Cat = 2, Dog = 3};
enum SETTLEMENT{Hamlet = 0, Village = 1, Town = 2, City = 3, Metropolis = 4};
enum EVENT{Nothing = 0, FindClue = 1, FindPieceOfInfo = 2, Interaction = 3, Plague = 4, Skirmish = 5, SocialRevolution = 6, TechBreakthrough = 7, SomeoneQuits = 8};

//Chrononauts and pets
class Chrononaut
{
	private:
		const std::string name;
		std::string position;
		CHRONO_TYPE type;
		const int actualAge;
		int travelAge;
		double abilityLevel;
		std::string specialty;
		bool alive;
	public:
		Chrononaut(std::string Name, CHRONO_TYPE t, int aAge, double aLvl);
		std::string getName(){return name;}
		CHRONO_TYPE getType() const {return type;}
		double getAbility() const {return abilityLevel;}
		void step(double abilityModifier); //year by year
		void setSpecialty(std::string s) {specialty = s;}
		void setPosition(std::string p) {position = p;}
		std::string getPosition(){return position;}
		void display();
		void jumpReport();
		bool isAlive() const {return alive;}
		void setAliveFlag(bool a) {alive = a;}
};

class TechEra;
class ChronoCrew
{
	private:
		std::vector<Chrononaut> crew;
	public:
		ChronoCrew();
		~ChronoCrew() = default;
		void step(TechEra era); //step year by year
		std::vector<Chrononaut> &getCrew(){return crew;}
		bool engineerAlive() const {return crew[JumpEngineer].isAlive();}
		void display();
		void jumpReport();
};

//Technological era
class TechEra
{
	private:
		std::string name;
		double techGrowthRate;
		double popGrowthRate; //different tech growth rate affects the pop growth rate (better medicine, better living conditions etc)
		double abilityImprovement; //different eras determine how fast their abilities improve year by year
	public:
		TechEra(std::string n, double techgrowth, double popgrowth, double aImprove);
		~TechEra() = default;
		void display();
		double getAbilityImproveFactor() const {return abilityImprovement;}
		double getTechGrowthRate() const {return techGrowthRate;}
		double getPopGrowthRate() const {return popGrowthRate;}
		TechEra& operator=(const TechEra&); //overload for deep copy
};

//Population centre
class SettlementType
{
	private:
		std::string type;
		SETTLEMENT typeEnum;
		std::string facility;
	public:
		SettlementType(std::string n, SETTLEMENT te, std::string f);
		~SettlementType() = default;
		void display();
		SETTLEMENT getType() const {return typeEnum;}
		SettlementType(const SettlementType &obj);
};

class PopulationCentre
{
	private:
		int year;
		unsigned int population;
		unsigned int techLvl;
		SettlementType ofType;
		TechEra inTechEra;
	public:
		PopulationCentre(TechEra,SettlementType,int,int);
		~PopulationCentre(){};
		TechEra isInEra() const {return inTechEra;}
		void display();
		void displayPopulation();
		void displayTechLevel();
		void operator++(); //overload ++
		void changeTechAndPop(int tech, int pop);
		void printJumpJourney();
		unsigned int getPopulation(){return population;}
		unsigned int getTechLevel(){return techLvl;}
		PopulationCentre(const PopulationCentre &obj);
		PopulationCentre& operator-=(const int &y);
};

class Artefact;
//Events
class Event
{
	protected:
		int year;
		std::string name;
		EVENT type;
		std::string description;
		double techChange;   //the effect the event has on the tech level, in range [0,1]
		double populationChange; //effect of the event on the population, in range [0,1]
		bool affectsChrononaut; //true if a specific chrononaut is affected
	
	public:
		Event(int y, std::string n, EVENT t,double tech, double pop, bool chrononaut, std::string des);
		virtual ~Event() = default;
		void eventName();
		void eventDescription();
		void affectTechAndPop(PopulationCentre &pop, std::vector<Chrononaut> &c, Artefact &a);
		EVENT getType() const {return type;}
		void affectChronos(std::vector<Chrononaut> &c);		
		void negativeEffectOnPop(std::vector<Chrononaut> &chronos, PopulationCentre &pop, int &popChange);
		void increaseTech();
		void display(Artefact &art, PopulationCentre &pop, std::vector<Chrononaut> &chronos);
		std::string getYear();
		int getYearInt() {return year;}
		std::string getEventName() const {return name;}
};

class FindInfo: public Event
{
	private:
		bool fake;
		double info;
	public:
		FindInfo(int y, bool f, double i);
		virtual ~FindInfo() = default;
		std::string getDescription();
		double getInfo() const {return info;}
		void findInfoPiece(Artefact &a);
		bool artefactIsFake() const {return fake;}
};

class FindJumpClue: public Event
{
	private:
		int clue;
	public:
		FindJumpClue(int y, int clue);
		virtual ~FindJumpClue() = default;
		std::string getDescription();
};
class Artefact
{
	private:
		std::string name;
		std::string description;
		int estimateSourceTime;
		double totalInfoFound;
		bool fake;
		std::vector<Event> majorEvents;
		std::vector<FindInfo> infoElements;
	public:
		Artefact(std::string name, std::string des, int sourceTime);
		void infoFound(double info, bool f);
		bool searchCompleted();
		void display();
		int getEstimatedSource();
		double getInfoPercentage() const {return totalInfoFound;}
		bool isFake() const{return this->fake;}
		void addMajorEvent(Event e);
		void addInfoElement(FindInfo i);
		void printMajorEvents();
		void printInfoElements();
};

//non-member functions
//check arguments from commandline
bool checkArgs(int argc, char *argv[]);

//random int generator
int randInt(int start, int end);
int randomInt(double *weights, int noOfElements);
double randDouble(double start, double end);
int randClue(int current, int sourceTime);
bool randBool(int falseProb);

//read the file and create a list of artefacts to be  found
void createArtefactList (std::vector<Artefact> &arts, int noOfArts);
void createChrononautList(std::vector<Chrononaut> &list);
void determineChrononautSpecialty(std::vector<Chrononaut> &list);
void addChronopet(std::vector<Chrononaut> &list);
void determinePetSpecialty(std::vector<Chrononaut> &list);

//generate events randomly
Event eventGenerator(int yearStep,int &clue, bool &clueFound, double &clueProb, bool &infoFound, double &infoProb, Artefact &art, int current, std::vector<Chrononaut> &crew);
Event randomEvent(int &clue, bool &clueFound, double &clueProb,bool &infoFound, double &infoProb, Artefact &art, int current, std::vector<Chrononaut> &crew, double quitProb);
Event socialRevolutionGenerator(int year);
Event interactionGenerator(int year, Chrononaut &security);
double historianDeterminesInfoValue(Chrononaut &historian, double info, double percentageLeft);

bool compareYear(Event e1, Event e2);

SettlementType settlementGenerator(int currentPopulation);
unsigned int growthCalc(int original, double rate, int year);

//functions called in main
void yearStep(int &clue, Artefact &art, int &currentYear, PopulationCentre &popCentre, ChronoCrew &crew);
void search(Artefact &art, const int noOfJumps, int &jumpTaken, PopulationCentre &popCentre, ChronoCrew &crew, int &currentYear, int &clue);
void wrapUp(std::vector<Artefact> &arts, const int noOfArtefacts);
