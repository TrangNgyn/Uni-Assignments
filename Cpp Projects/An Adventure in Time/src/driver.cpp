#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "functions.h"

int main(int argc, char* argv[])
{
	//check for arguments
	if(!checkArgs(argc, argv))
	{
		return -1;
	}
	
	//start the simulation
	int noOfArtefacts = atoi(argv[1]);
	int noOfJumps = atoi(argv[2]);
	
	//list of chrononauts
	ChronoCrew crew;
	crew.display();
	
	//List of artefacts to be found
	std::vector<Artefact> arts;
	createArtefactList (arts, noOfArtefacts);
	
	//The original population centre	
	int pop = randInt(3000000, 5000000);
	int techLvl = randInt(5000000, 10000000);
	TechEra t("Modern Period", 0.0107, 0.004, 0.0005);
	SettlementType s("Metropolis", Metropolis, "skyscrapers and metro trains");
	PopulationCentre originalPop(t,s,pop,techLvl);
	
	int jumpTaken = 0;

	for(int j = 0; j < noOfArtefacts; j++)
	{		
		PopulationCentre popCentre(originalPop);
		int currentYear = 2525;
		int clue = randClue(currentYear, arts[j].getEstimatedSource());
		std::cout << "====================Artefact number " << j+1 << "====================" << std::endl;
		arts[j].display();
		
		search(arts[j], noOfJumps, jumpTaken, popCentre, crew, currentYear, clue);
	}
	
	//wrapping things up
	std::cout << "\nWrapping things up...\n" << std::endl;
	wrapUp(arts, noOfArtefacts);
	std::cout << "\nEnd of simulation!" << std::endl;
	

	return 0;
}