#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <random>
#include <algorithm>
#include <iomanip>
#include <cmath>

#include "functions.h"

//check arguments from commandline
bool checkArgs(int argc, char *argv[])
{
	if(argc != 3 ){
		std::cerr << "Arguments should be ./AAT number-of-artefacts number-of-jumps" << std::endl;
		return false;
	}
	int noOfArtefacts = atoi(argv[1]);
	if (noOfArtefacts < 1 || noOfArtefacts > 5){
		std::cerr << "The number of artefacts to be traced should be an integer in range 1 to 5 inclusive" << std::endl;
		return false;
	}
	int noOfJumps = atoi(argv[2]);
	if (noOfJumps < 1 || noOfJumps > 10){
		std::cerr << "The number of jumps to be made should be an integer in range 1 to 10 inclusive" << std::endl;
		return false;
	}
	return true;
}
//generate random numbers and bool
int randInt(int start, int end) //generate random int in a range
{
	static std::mt19937 eng(time(0));
	std::uniform_int_distribution<> uniform (start, end);
	return uniform(eng);
}
int randInt(double weights[], int noOfElements) //generate random int according to a predefined probability
{
	static std::mt19937 eng(time(0));
    std::discrete_distribution<> d(weights, weights+(noOfElements-1));
	return d(eng);
}
double randDouble(double start, double end) //generate random double in a range
{
	static std::mt19937 eng(time(0));
	std::uniform_real_distribution<> uniform (start, end);
	return uniform(eng);
}
int randClue(int current, int sourceTime)
{
	int r = (current - sourceTime)*0.6;//making sure the clue never takes them back before the source time
	return randInt(9*(r/10), r);
}
bool randBool(int falseProb)
{
	//there's a small chance the artefact is fake
	static std::mt19937 eng(time(0));
	std::bernoulli_distribution distribution(falseProb);
	return distribution(eng);
}

//Chrononaut member functions
Chrononaut::Chrononaut(std::string Name, CHRONO_TYPE t, int aAge, double aLvl)
	: name(Name), type(t), actualAge(aAge), abilityLevel(aLvl)
{
	travelAge = 0;
	alive = true;
}
void Chrononaut::display()
{
	std::cout << name << " is " << actualAge << " year(s) old and can " << specialty << std::endl;
}
void Chrononaut::jumpReport()
{
	std::cout << "The " << position << ", whose ability level is now at " << abilityLevel << ", has spent " << travelAge << " years back in time." << std::endl;
}
void Chrononaut::step(double improveFactor)
{
	abilityLevel += improveFactor;
	++travelAge;
}
//ChronoCrew member functions
ChronoCrew::ChronoCrew()
{
	//human chronoauts
	createChrononautList(crew);
	determineChrononautSpecialty(crew);
	//chrono-pet
	addChronopet(crew);
	determinePetSpecialty(crew);
}
void ChronoCrew::display() //display details of the initial crew
{
	std::cout << "The initial chrono-crew includes: \n";
	for(Chrononaut c: crew)
	{
		std::cout << "\t";
		c.display();
	}
}
void ChronoCrew::jumpReport() //report on the survivers before and after each jump period
{
	std::cout << "The survivers of the chrono-crew now include: " << std::endl;
	for(int i = 0; i < 5; i++)
	{
		if(crew[i].isAlive())
		{
			std::cout << "\t";
			crew[i].jumpReport();
		}
	}
}
void ChronoCrew::step(TechEra era)
{
	for (int i = 0; i < 5; i++){
		if(crew[i].isAlive()){
			crew[i].step(era.getAbilityImproveFactor());
		}
	}		
}
//Artefact member functions
//Constructor
Artefact::Artefact(std::string n, std::string des, int sourceTime)
	: name(n), description(des), estimateSourceTime(sourceTime)
{
	totalInfoFound = 0;	
	fake = false;
}
void Artefact::infoFound(double info, bool f)
{
	fake = f;
	totalInfoFound += info;
	if (totalInfoFound > 1){
		totalInfoFound = 1;
	}
}
bool Artefact::searchCompleted()
{
	return (fake || totalInfoFound >= 1);
}
void Artefact::display()
{
	std::cout << "It's a " << description << " " << name << " which can be traced back to the year ";
	if (estimateSourceTime == 0){
		std::cout << "Zero" << std::endl;
	}else if (estimateSourceTime > 0){
		std::cout << estimateSourceTime << " AD" << std::endl;
	}else if (estimateSourceTime < 0){
		std::cout << (-estimateSourceTime) << " BC" << std::endl;
	}
}
int Artefact::getEstimatedSource()
{
	return estimateSourceTime;
}
void Artefact::addMajorEvent(Event e)
{
	majorEvents.push_back(e);
}
void Artefact::addInfoElement(FindInfo i)
{
	infoElements.push_back(i);
}

void Artefact::printInfoElements()
{
	if(infoElements.size() > 0){
		std::cout << std::fixed << std::setprecision(2) << std::right <<std::boolalpha;
		std::cout << std::setw(10) << " " << "-----------------------------------------------" << std::endl;
		std::cout << std::setw(10) << "|" << std::left << std::setw(8) << "Year";
		std::cout << std::right << std::setw(1) << "|" << std::setw(18) << "Is Fake" << std::setw(1) << "|" << std::setw(18) << "Percentage" << std::setw(1) << "|" << std::endl;
		
		for(FindInfo i: infoElements){
			std::cout << std::setw(10) << " " << "-----------------------------------------------" << std::endl;
			std::cout << std::setw(10) << "|" << std::left << std::setw(8) << i.getYear();
			std::cout << std::right << std::setw(1) << "|" << std::setw(18) << i.artefactIsFake() << std::setw(1) << "|" << std::setw(17) << (i.getInfo() * 100) << "%" << std::setw(1) << "|" << std::endl;
		}
	}
	std::cout << std::right << std::setw(10) << " " << "-----------------------------------------------" << std::endl;
	std::cout << std::right << std::setw(10) << "|" << std::left << std::setw(20) << "Total: ";
	std::cout << std::right << std::setw(25) << (getInfoPercentage()*100)  << "%" << std::setw(1) << "|" << std::endl;
	std::cout << std::setw(10) << " " << "-----------------------------------------------" << std::endl;
}
void Artefact::printMajorEvents()//print the major events occurred while finding the info about the artefact
{
	if (majorEvents.size() == 0){
		std::cout << "\tThere was no major event." << std::endl;
	}else{
		std::sort(majorEvents.begin(), majorEvents.end(), compareYear);
		std::cout << std::right;
		std::cout << std::setw(10) << " " << "-----------------------------------------------" << std::endl;
		std::cout <<std::setw(10) << "|" << std::setw(4) << "Year";
		std::cout << std::setw(5) << "|" << std::setw(19) << "Event" << std::setw(19) << "|" << std::endl;
		std::cout << std::setw(10) << " " << "-----------------------------------------------" << std::endl;
		for(Event e: majorEvents){
			std::cout << std::setw(10) << "|" << e.getYear();
			std::cout << std::setw(2) << "|" << std::setw(36) << e.getEventName() << std::setw(2) << "|" << std::endl;
			std::cout << std::setw(9) << " " << "-----------------------------------------------" << std::endl;
		}
	}
}

//TechEra member functions
TechEra::TechEra(std::string n, double techGrowth, double popGrowth, double aImprove): name(n)
{
	techGrowthRate = techGrowth;
	popGrowthRate = popGrowth;
	abilityImprovement = aImprove;
}

void TechEra::display()
{
	std::cout << name << std::endl;
}
TechEra& TechEra::operator=(const TechEra& obj) //overload for deep copy
{
	if(this == &obj)
	{
		return *this;
	}
	name = obj.name;
	techGrowthRate = obj.techGrowthRate;
	popGrowthRate = obj.popGrowthRate;
	abilityImprovement = obj.abilityImprovement;
	return *this;
}

//SettlementType member functions
SettlementType::SettlementType(std::string n, SETTLEMENT t, std::string f)
	: type(n), typeEnum(t), facility(f) {}
	
void SettlementType::display()
{
	std::cout << "a " << type << " which has " << facility << std::endl;
}
SettlementType::SettlementType(const SettlementType &obj) //overload for deep copy
{
	type = obj.type;
	typeEnum = obj.typeEnum;
	facility = obj.facility;
}

//PopulationCentre member functions
PopulationCentre::PopulationCentre(TechEra era, SettlementType s, int pop, int tech)
	:inTechEra(era), ofType(s)
{
	year = 2525;
	techLvl = tech;
	population = pop;
}

void PopulationCentre::display()
{
	std::cout << "The population centre is ";
	ofType.display();
	std::cout << "This is the ";
	inTechEra.display();
}
void PopulationCentre::displayPopulation()
{
	std::cout << "The population is " << population << " people." << std::endl;
}
void PopulationCentre::displayTechLevel()
{
	std::cout << "The techonology level is at " << techLvl << std::endl;
}

void PopulationCentre::operator++() //overload for stepping year by year
{	
	year++;
	if (year == -3000){
		inTechEra = TechEra("Bronze Age", 0.00000401, 0.000000101, 0.0002); //as tech grows, they learn things easier -> ability improves faster
	}else if(year == -1200){
		inTechEra = TechEra("Middle Age", 0.0000501, 0.0000403, 0.0003);
	}else if(year == 1492){
		inTechEra =  TechEra("Age of Enlightenment", 0.000709, 0.000301, 0.0004);
	}else if(year == 1789){
		inTechEra = TechEra("Modern Period", 0.0207, 0.004, 0.0005);
	}
	population += population * inTechEra.getPopGrowthRate();
	techLvl += techLvl * inTechEra.getTechGrowthRate();
	ofType = settlementGenerator(population);
}

void PopulationCentre::changeTechAndPop(int tech, int pop)
{
	techLvl += tech;
	population += pop;
}

PopulationCentre::PopulationCentre(const PopulationCentre &obj) //overloaded copy constructor
	:ofType(obj.ofType), inTechEra(obj.inTechEra)
{
	year = obj.year;
	population = obj.population;
	techLvl = obj.techLvl;
}

PopulationCentre& PopulationCentre::operator-=(const int &y) //overloaded operator for jumping back y years
{
	//the current year will be the upperbound
	int current = year;
	int goalYear = current-y; //the lower bound
	//print info before the jump
	
	//jumping through adjacent era
	if(goalYear < 1789 && current > 1789){ //current year is in morder era
		techLvl = growthCalc(techLvl, -(inTechEra.getTechGrowthRate()), current-1789);
		population = growthCalc(population, -(inTechEra.getPopGrowthRate()), current-1789);
		current = 1789;
		SettlementType t = settlementGenerator(population); //ofType is determined by the population
		ofType = SettlementType(t); //copy constructor to move to the adjacent level
		printJumpJourney();		
		inTechEra = TechEra("Age of Enlightenment", 0.000709, 0.000301, 0.0004); //reset inTechEra
	}
	if(goalYear < 1492 && current > 1492 && current <= 1789){ //current year is in enlightenment		
		techLvl = growthCalc(techLvl, -(inTechEra.getTechGrowthRate()), current-1492);
		population = growthCalc(population, -(inTechEra.getPopGrowthRate()), current-1492);
		current = 1492;
		SettlementType t = settlementGenerator(population); //ofType is determined by the population
		ofType = SettlementType(t); //copy constructor to move to the adjacent level
		printJumpJourney();	
		inTechEra = TechEra("Middle Age", 0.0000501, 0.0000403, 0.0003);  //reset inTechEra
	}
	if(goalYear < -1200 && current > -1200 && current <= 1492){ //current year is in middle age		
		techLvl = growthCalc(techLvl, -(inTechEra.getTechGrowthRate()), abs(current-(-1200)));
		population = growthCalc(population, -(inTechEra.getPopGrowthRate()), abs(current-(-1200)));
		current = -1200;
		SettlementType t = settlementGenerator(population); //ofType is determined by the population
		ofType = SettlementType(t); //copy constructor to move to the adjacent level
		printJumpJourney();	
		inTechEra = TechEra("Bronze Age", 0.00000401, 0.000000101, 0.0002); //reset inTechEra
	}
	if(goalYear < -3000 && current > -3000 && current <= -1200){ //current year is in bronze and goal in is stone age
		techLvl = growthCalc(techLvl, -(inTechEra.getTechGrowthRate()), current-(-3000));
		population = growthCalc(population, -(inTechEra.getPopGrowthRate()), current-(-3000));	
		current = -3000;
		SettlementType t = settlementGenerator(population); //ofType is determined by the population
		ofType = SettlementType(t); //copy constructor to move to the adjacent level
		printJumpJourney();	
		inTechEra = TechEra("Stone Age", 0.0000000500, 0.00000000103, 0.0001);
	}
	//when both upper bound and lower bound are in the same era
	int tech = growthCalc(techLvl, -(inTechEra.getTechGrowthRate()), abs(current-goalYear));
	int pop = growthCalc(population, -(inTechEra.getPopGrowthRate()), abs(current-goalYear));
	techLvl = tech;
	population = pop;
	year -= y;
	SettlementType t = settlementGenerator(population);
	ofType = SettlementType(t);
	printJumpJourney();
	
	return *this;
}
void PopulationCentre::printJumpJourney()
{
	std::cout << "Travelling to ";
	inTechEra.display();
	std::cout << "\tThe population centre is now ";
	ofType.display();
}	
//Event member functions
Event::Event(int y, std::string n, EVENT t, double tech, double pop, bool chrononaut, std::string des)
	: name(n), type(t), techChange(tech), populationChange(pop), affectsChrononaut(chrononaut)
{
	year = y;
	description = des;
}
void Event::display(Artefact &art, PopulationCentre &pop, std::vector<Chrononaut> &chronos)
{
}
void Event::eventName()
{
	std::cout << name << std::endl;
}
void Event::eventDescription()
{
	std::cout << "\t" << description << std::endl;
}
void Event::affectTechAndPop(PopulationCentre& pop, std::vector<Chrononaut> &chronos, Artefact &a)
{
	int popChange = pop.getPopulation() * populationChange; //negative if the event has negative effect
	int tech = pop.getTechLevel() * techChange;
	int chronoNo = randInt(0,4);
	switch (type)
	{
		case FindClue: case FindPieceOfInfo:
			break;
		case SocialRevolution: case Interaction: case SomeoneQuits:
			a.addMajorEvent(*this);
			break;
		case Plague: case Skirmish:
			if (affectsChrononaut){ //there's a chance a chrononaut dies
				affectChronos(chronos);
			}
			negativeEffectOnPop(chronos, pop, popChange);	
			increaseTech();
			a.addMajorEvent(*this);
			break;				
		case TechBreakthrough:
			description = "Technology level increases by " + std::to_string(tech);
			a.addMajorEvent(*this);
			break;
	}
	pop.changeTechAndPop(tech, popChange);
}
void Event::negativeEffectOnPop(std::vector<Chrononaut> &chronos, PopulationCentre &pop, int &popChange)
{
	int peopleSaved = 0;
	if (chronos[Doctor].isAlive()){
		description += "The Doctor's first instinct is to help people.\n";
		double factor = chronos[Doctor].getAbility();
		if (chronos[ChronoPet].isAlive() && chronos[ChronoPet].getType() == GuineaPig)
		{
			description += "\tThe Guinea Pig helps him with his experiment to find a way to cure people.\n";
			factor += chronos[ChronoPet].getAbility();
		}
		peopleSaved += popChange * factor; //popChange is negative -> peopleSaved is neg		
		if(chronos[Security].isAlive()){
			description += "\tHowever, the Security also tries to limit his interaction with the locals.\n";
			double prevention = peopleSaved * chronos[Security].getAbility();
			if(chronos[ChronoPet].isAlive() && chronos[ChronoPet].getType() == Dog)
			{
				description += "\tThe Dog also tries to help the Security to limit the Doc's effect.\n";
				prevention += chronos[ChronoPet].getAbility();
			}
			peopleSaved -= peopleSaved * prevention;
		}
		if (peopleSaved < 0){
			peopleSaved = 0;
		}
		popChange += abs(peopleSaved); //because popChange is currently negative
		description += "\t" + std::to_string(abs(peopleSaved)) + " people were saved.\n";
	}
}
void Event::affectChronos(std::vector<Chrononaut> &chronos)
{	
	int chronoNo = randInt(0,4);
	while (!chronos[chronoNo].isAlive()){ // if the chrono is already not alive, randomly choose another
		chronoNo = randInt(0,4);
	}
	chronos[chronoNo].setAliveFlag(false);
	description += chronos[chronoNo].getName() + " - the " + chronos[chronoNo].getPosition() + " is no longer with us.\n";
	
}
void Event::increaseTech()
{
	std::string reason;
	if(type == Plague){
		reason = "as they do researches to find a cure.";
	}else if (type == Skirmish){
		reason = "as they develop weapons.";
	}		
	description += "\tTechnology also improves " + reason;
}
std::string Event::getYear()
{
	if(year >= 0){
		return std::to_string(year) + " AD";
	}else{
		return std::to_string(abs(year)) + " BC";
	}
}
FindInfo::FindInfo(int y, bool f, double infoPercentage)
	: Event(y,"a piece of information about the artefact", FindPieceOfInfo,0, 0, false, "")
{
	info = infoPercentage;
	fake = f;	
}
std::string FindInfo::getDescription()
{
	std::string des;
	std::ostringstream infoStr;
	infoStr << std::fixed << std::setprecision(3) << info;
	des = "It is worth " + infoStr.str() + " and says the artefact is ";
	if (fake){
		des += "fake";
	}else{
		des += "not fake";
	} 
	return des;
}
void FindInfo::findInfoPiece(Artefact &a)
{
	if(info > 1){
		info = 1;
	}
	a.infoFound(info, fake);
}

FindJumpClue::FindJumpClue(int y, int c)
	: Event(y,"a jump clue found", FindClue, 0,0,false, "")
{
	clue = c;
}
std::string FindJumpClue::getDescription()
{
	return "It says the next clue is about " + std::to_string(clue) + " year(s) from now.";
}

//non-member functions
//to initialise data
void createArtefactList (std::vector<Artefact> &arts, int noOfArts)
{
	std::vector<Artefact> artefacts; //list of all existing artefacts
	//open data file
	std::ifstream inFile;
	inFile.open("Artefacts.txt");
	if(!inFile)
	{
		std::cerr << "Error opening Artefacts.txt file. Terminating..." << std::endl;
		exit(1);
	}
	
	std::string details;
	while (getline(inFile, details))
	{
		std::stringstream ss(details);
		std::string description, name, source;
		getline(ss, description, ',');
		getline(ss, name, ',');
		getline(ss, source);
		Artefact a(name, description, atoi(source.c_str()));
		artefacts.push_back(a);
	}
	inFile.close();
	
	//generate random artefacts
	std::vector<int> addedArt;
	int artefactNo = randInt(0, noOfArts-1);
	addedArt.push_back(artefactNo);
	arts.push_back(artefacts[artefactNo]);
	while(arts.size() < noOfArts)
	{
		artefactNo = randInt(0, noOfArts-1);
		if (std::find(addedArt.begin(), addedArt.end(), artefactNo) == addedArt.end())
		{					
			addedArt.push_back(artefactNo);
			arts.push_back(artefacts[artefactNo]);
		}
	}
}

//add human chrononauts to the crew
void createChrononautList(std::vector<Chrononaut> &list)
{
	std::vector<Chrononaut> people; //list of all existing people
	//open data file
	std::ifstream inFile;
	inFile.open("Chrononauts.txt");
	if(!inFile)
	{
		std::cerr << "Error opening Chrononauts.txt file. Terminating..." << std::endl;
		exit(1);
	}
	
	std::string details;
	while (getline(inFile, details))
	{
		//each line is in format: name,actualAge,abilityLvl
		std::stringstream ss(details);
		std::string name, ageStr, abilityStr;
		getline(ss, name, ',');
		getline(ss, ageStr, ',');
		getline(ss, abilityStr);
		Chrononaut c(name, Human, atoi(ageStr.c_str()), atof(abilityStr.c_str()));
		people.push_back(c);
	}
	inFile.close();
	
	//generate random chrononauts
	std::vector<int> addedChrono;
	int totalPeople = people.size();
	int chronoNo = randInt(0, totalPeople-1);
	addedChrono.push_back(chronoNo);
	list.push_back(people[chronoNo]);
	while(list.size() < 4)
	{
		chronoNo = randInt(0, totalPeople-1);
		if (std::find(addedChrono.begin(), addedChrono.end(), chronoNo) == addedChrono.end())
		{					
			addedChrono.push_back(chronoNo);
			list.push_back(people[chronoNo]);
		}
	}
	addedChrono.clear();
}
void determineChrononautSpecialty(std::vector<Chrononaut> &list)
{
	//Each chronoaut has a role according to their position in the list
	//JumpEngineer = 0, Doctor = 1, Historian = 2, Security = 3
	list[JumpEngineer].setSpecialty("control the time jump");
	list[JumpEngineer].setPosition("Jump Engineer");
	list[Doctor].setSpecialty("determine the likelihood of introducing or suffering from disease/death");
	list[Doctor].setPosition("Doctor");
	list[Historian].setSpecialty("determine the values of each clue");
	list[Historian].setPosition("Historian");
	list[Security].setSpecialty("determine the impact of the interactions between the crew and the locals");
	list[Security].setPosition("Security");
}

//add the chronopet to the crew
void addChronopet(std::vector<Chrononaut> &list)
{
	std::vector<Chrononaut> pets; //list of all existing people
	//open data file
	std::ifstream inFile;
	inFile.open("Chronopets.txt");
	if(!inFile)
	{
		std::cerr << "Error opening Chronopets.txt file. Terminating..." << std::endl;
		exit(1);
	}
	std::string details;
	int lineNo = 1;
	while (getline(inFile, details)) //Guinea Pig = 1, Cat = 2, Dog = 3
	{
		//each line is in format: name,actualAge,abilityLvl
		std::stringstream ss(details);
		std::string name, ageStr, abilityStr;
		getline(ss, name, ',');
		getline(ss, ageStr, ',');
		getline(ss, abilityStr);
		CHRONO_TYPE type = GuineaPig;
		if (lineNo == 2){
			type = Cat;
		}else if (lineNo == 3){
			type = Dog;
		}
		Chrononaut c(name, type, atoi(ageStr.c_str()), atof(abilityStr.c_str()));
		pets.push_back(c);
		lineNo++;
	}
	inFile.close();
	
	//randomly choose 1 pet and add to the crew
	int random = randInt(0,2);
	list.push_back(pets[random]);
}
void determinePetSpecialty(std::vector<Chrononaut> &list)
{
	CHRONO_TYPE type = list[ChronoPet].getType();
	list[ChronoPet].setPosition("Chrono-pet");
	if(type == GuineaPig){
		list[ChronoPet].setSpecialty("help the doctor in experimenting cures to help people");
	}else if (type == Cat){
		list[ChronoPet].setSpecialty("keep the chance of a chrononaut quitting the adventure low");
	}else if (type == Dog){
		list[ChronoPet].setSpecialty("help the Security limiting their interactions with the locals");
	}
}

//functions that generate random events
Event eventGenerator(int yearStep,int &clue, bool &clueFound, double &clueProbability, bool &infoFound, double &infoProbability, Artefact &art, int current, std::vector<Chrononaut> &crew)
{
	if(yearStep == 9 && clueFound == false && infoFound == false){ //making sure the clue and info found within 10 years
		clueFound = true;
		clue = randClue(current, art.getEstimatedSource());
		FindJumpClue c(current,clue);
		return  Event(current, "A jump clue found", FindClue, 0,0,false, c.getDescription());
	}else if(yearStep == 10 && clueFound == false){
		clueFound = true;
		clue = randClue(current, art.getEstimatedSource());
		FindJumpClue c(current,clue);
		return  Event(current, "A jump clue found", FindClue, 0,0,false, c.getDescription());
	}else if (yearStep == 10 && infoFound == false){
		infoFound = true;
		double info = randDouble(0, (1-art.getInfoPercentage())*0.1);
		bool fake = randBool(0.3);
		info = historianDeterminesInfoValue(crew[Historian], info, 1-art.getInfoPercentage());
		FindInfo t (current,fake, info);
		t.findInfoPiece(art);
		art.addInfoElement(t);
		return Event(current, "A piece of information found", FindPieceOfInfo,0, 0, false, t.getDescription());
	}else{
		//determine the probability of a chrononaut quitting
		double quitProb = 0.07;
		if(crew[ChronoPet].isAlive() && crew[ChronoPet].getType() == Cat){
			quitProb = crew[ChronoPet].getAbility();
		}
		//increase probability of FindInfo event and FindClue event
		if (!infoFound && yearStep >= 5){
			infoProbability = infoProbability + 0.5;
		}else{
			infoProbability = 0.01;
		}
		if (!clueFound && yearStep >= 5){
			clueProbability = clueProbability + 0.5;
		}else{
			clueProbability = 0.01;
		}
		return randomEvent(clue, clueFound, clueProbability, infoFound, infoProbability, art, current, crew, quitProb);
	}
		
}

Event randomEvent(int &clue, bool &clueFound, double &clueProbability,bool &infoFound, double &infoProbability, Artefact &art, int current, std::vector<Chrononaut> &crew, double quitProb)
{
	double weights[] =
            {0.6, // number 0 at 0.6 probability
             clueProbability, // number 1 at clue probability
             infoProbability, // number 2 at info probability
             0.07, 0.07, 0.07, 0.07, 0.07, quitProb}; // 3..8 at 0.07 probability
			 
	int event = randInt(weights, 9);
	double popChange = 0;
	double techChange = 0;
	bool fake = false;
	double info = 0;
	bool chronoAffected = false;
	if (event == 0){
			return Event(current, "Nothing happened",Nothing,0,0,false, "");
	}else if(event == 1){
		clueFound = true;
		clue = randClue(current, art.getEstimatedSource());
		FindJumpClue c(current, clue);
		return Event(current, "A jump clue found", FindClue, 0,0,false, c.getDescription());
	}else if (event == 2){
		infoFound = true;
		info = randDouble(0, (1-art.getInfoPercentage())*0.1); //random info
		fake = randBool(0.3);
		info = historianDeterminesInfoValue(crew[Historian], info, 1-art.getInfoPercentage());
		FindInfo t (current, fake, info);
		t.findInfoPiece(art);
		art.addInfoElement(t);
		return Event(current, "A piece of information found", FindPieceOfInfo,0, 0, false, t.getDescription());
	}else if (event == 3){
		return interactionGenerator(current, crew[Security]);
	}else if (event == 4){
		popChange = randDouble(-0.1, -0.2);
		techChange = randDouble(0.05, 0.15);
		chronoAffected = randBool(0.4); //chrononaut affected to be determined...
		return Event(current, "Plague", Plague,techChange,popChange ,chronoAffected, "");
	}else if(event == 5){ 
		popChange = randDouble(-0.2, -0.3); //pop can decrease faster than a plague
		techChange = randDouble(0.17, 0.2); //tech increases faster than a plague
		chronoAffected = randBool(0.4); //chrononaut affected to be determined...
		return Event(current, "Skirmish", Skirmish, techChange ,popChange ,chronoAffected, "");
	}else if(event == 6){ 
		return socialRevolutionGenerator(current);
	}else if(event == 7){ 
		techChange = randDouble(0.1, 0.55);
		return Event(current, "Technological Breakthrough", TechBreakthrough, techChange ,0 ,chronoAffected, "");
	}else if (event == 8){
		int chronoNo;
		do{ // if the chrono is already not alive, randomly choose another
			chronoNo = randInt(0,4);
		}while (!crew[chronoNo].isAlive());
		crew[chronoNo].setAliveFlag(false); //consider them dead for simplycity
		std::string eventName = crew[chronoNo].getName() + "Quits";
		std::string des = crew[chronoNo].getName() + " decides to stop his journey here.";
		return Event(current, eventName, SomeoneQuits,0 ,0 ,true, des);
	}
}
double historianDeterminesInfoValue(Chrononaut &historian, double info, double percentageLeft)
{
	if (historian.isAlive()){
		double infoAcquired = info + historian.getAbility();
		if (infoAcquired >= percentageLeft){ //making sure the total info percentage doesn't exceed 100%
			return percentageLeft;
		}
		return infoAcquired;
	}
	return info; //otherwise return the info itself
}
Event socialRevolutionGenerator(int year)
{
	double techChange, popChange;
	if (year < -3000){ //stone age
		techChange = randDouble(0.01, 0.03);
		popChange = randDouble(0.03, 0.04);
		std::string des = "Pastoral and horticultural practices have replaced hunting and gathering practices.";
		return Event (year, "A Domestication Revolution", SocialRevolution, techChange, popChange, false, des);
		
	}else if(year >= -3000 && year < 1765){ 
		techChange = randDouble(0.06, 0.1);
		popChange = randDouble(0.4, 0.6); //pop grows fast as many cities appeared during 2nd half of middle age
		std::string des = "Agricultural systems haved replaced pastoral and horticultural systems.";
		return Event (year, "An Agricultural Revolution", SocialRevolution, techChange, popChange, false, des);
		
	}else if (year >= 1765 && year <= 2525){ //from the 1st industrial revolution
		techChange = randDouble(0.3, 0.6); //tech changes fastest
		popChange = randDouble(0.1, 0.2);
		std::string des = "It has seen a dramatic shift from agriculture to manufacturing as the major sources of power, wealth and prestige.";
		return Event (year, "An Industrial Revolution", SocialRevolution, techChange, popChange, false, des);
		
	}
}

Event interactionGenerator(int year, Chrononaut &security) //security may limit these
{
	double techChange = 0;
	double popChange = 0;
	if (year < -3000){ //stone age
		techChange = randDouble(0.0001, 0.0003);
		popChange = randDouble(0.0001, 0.0001);
		if(security.isAlive()){
			techChange -= security.getAbility();
			popChange -= security.getAbility();
		}
		std::string des = "The chrononauts teach the locals a few tricks to hunt and gather food easier.";
		return Event (year, "An Interaction with the locals", Interaction, techChange, popChange, false, des);		
	}else if (year >= -3000 && year < 1765){
		techChange = randDouble(0.0000, 0.0003);
		popChange = randDouble(0.0003, 0.0004);
		if(security.isAlive()){
			techChange -= security.getAbility();
			popChange -= security.getAbility();
		}
		std::string des = "The chrononauts teach the locals to grow crops.";
		return Event(year, "An Interaction with the locals", Interaction, techChange, popChange, false, des);
	}else if (year >= 1765 && year <= 2525){
		techChange = randDouble(-0.0003, -0.0001);
		if(security.isAlive()){
			techChange += security.getAbility(); //because it's negative already
		}
		std::string des = "They experiment a new idea with some of the locals and fail. This causes some setback as a huge amount of resources was used.";
		return Event(year, "An Interaction with the locals", Interaction, techChange, popChange, false, des);
		
	}
}

//this function returns a settlement type according to the current population
SettlementType settlementGenerator(int currentPopulation)
{
	if(currentPopulation > 0 && currentPopulation <= 100){
		return SettlementType("Hamlet", Hamlet, "isolated farms");
	}else if(currentPopulation > 100 && currentPopulation <= 1000){
		return SettlementType ("Village", Village, "a village shop");
	}else if(currentPopulation > 1000 && currentPopulation <= 100000){
		return SettlementType ("Town", Town, "a medical centre and shops");
	}else if(currentPopulation > 10000 && currentPopulation <= 1000000){
		return SettlementType ("City", City, "a hospital and buses");
	}else{
		return SettlementType ("Metropolis", Metropolis, "skyscrapers and metro trains");
	}
}

//calculating the population/techlevel according to the growth/decay rate
unsigned int growthCalc(int original, double rate, int year)
{
	int result = original * pow(1 + rate, year);
	return result;
}
bool compareYear(Event e1, Event e2)
{
	return (e1.getYearInt() < e2.getYearInt());
}

//functions called in main
void yearStep(int &clue, Artefact &art, int &currentYear, PopulationCentre &popCentre, ChronoCrew &crew)
{
	//generate events
	bool infoFound = false;
	bool clueFound = false;
	double clueProbability = 0.001;
	double infoProbability = 0.001;
	for(int step = 1; (!infoFound || !clueFound) && !art.isFake(); step++)
	{
		//forward the popcentre
		++popCentre;
		crew.step(popCentre.isInEra());
		
		//event each year
		std::cout << std::endl;
		std::cout << "Year " << currentYear << " - ";
		Event e = eventGenerator(step,clue,clueFound,clueProbability,infoFound,infoProbability, art, currentYear, crew.getCrew());
		e.eventName();
		e.affectTechAndPop(popCentre, crew.getCrew(), art);
		e.eventDescription();	

		//details on the popCentre after the step
		currentYear++;
		std::cout << "\t";				
		popCentre.displayPopulation();
		std::cout << "\t";	
		popCentre.displayTechLevel();
	}
}

void search(Artefact &art, const int noOfJumps, int &jumpTaken, PopulationCentre &popCentre, ChronoCrew &crew, int &currentYear, int &clue)
{
	while(!art.searchCompleted() && jumpTaken < noOfJumps && crew.engineerAlive())
	{
		crew.jumpReport();
		int jump;
		do{
			jump = clue+randInt(5, 10);
		}while(art.getEstimatedSource() > currentYear - jump);
		currentYear -= jump;
		popCentre.display(); //before the jump
		std::cout << "Jumping back " << jump << " years from now.......(/ ^o^)/" << std::endl;
		jumpTaken++; //decrease the number of jumps
		//the current tech era is...
		
		popCentre -= jump;
		std::cout << "They have reached their destination. ";
		popCentre.display();//after the jump
			
		yearStep(clue, art, currentYear, popCentre, crew);
		crew.jumpReport();
		std::cout << "++++++++++++End of jump. Only " << noOfJumps - jumpTaken << " jump(s) left++++++++++++" << std::endl;	
	}
}

void wrapUp(std::vector<Artefact> &arts, const int noOfArtefacts)
{
	for (int i=0; i<noOfArtefacts; i++){
		std::cout << "Artefact " << i+1 << ": ";
		arts[i].display();
		arts[i].printMajorEvents();
		arts[i].printInfoElements();
	}
}
