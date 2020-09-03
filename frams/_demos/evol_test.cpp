// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 2019-2020  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.


#include <vector>
#include <numeric> //std::accumulate()
#include "common/loggers/loggertostdout.h"
#include "frams/genetics/preconfigured.h"
#include "frams/genetics/genman.h"
#include "frams/model/model.h"
#include "frams/model/geometry/modelgeometryinfo.h"


struct Individual
{
	Geno geno;
	double fitness;
};

double criterion(char symbol, double value)
{
	return isupper(symbol) ? value : -value;
}

double get_fitness(const Individual &ind, const char *fitness_def)
{
	SString genotype = ind.geno.getGenes();
	Model model = Model(ind.geno, Model::SHAPETYPE_UNKNOWN);
	double fitness = 0;
	const char *p = fitness_def;

	Orient axes;
	Pt3D sizes;
	double area = 0;
	double volume = 0;
	while (*p)
	{
		switch (*p)
		{
			case '0':
				break;
			case '!': //special symbol for current fitness (used only in printing population stats)
				fitness += ind.fitness;
				break;
			case 'l':
			case 'L':
				fitness += criterion(*p, genotype.length());
				break;
			case 'p':
			case 'P':
				fitness += criterion(*p, model.getPartCount());
				break;
			case 'j':
			case 'J':
				fitness += criterion(*p, model.getJointCount());
				break;
			case 'n':
			case 'N':
				fitness += criterion(*p, model.getNeuroCount());
				break;
			case 'c':
			case 'C':
				fitness += criterion(*p, model.getConnectionCount());
				break;
			case 'b':
			case 'B':
				fitness += criterion(*p, model.size.x * model.size.y * model.size.z);
				break;
			case 's':
			case 'S':
				fitness += criterion(*p, ModelGeometryInfo::area(model, 1.0));
				break;
			case 'v':
			case 'V':
				fitness += criterion(*p, ModelGeometryInfo::volume(model, 1.0));
				break;
			case 'h':
			case 'H':
				ModelGeometryInfo::findSizesAndAxes(model, 1.0, sizes, axes);
				fitness += criterion(*p, sizes.x);
				break;
			case 'w':
			case 'W':
				ModelGeometryInfo::findSizesAndAxes(model, 1.0, sizes, axes);
				fitness += criterion(*p, sizes.y);
				break;
			case 'd':
			case 'D':
				ModelGeometryInfo::findSizesAndAxes(model, 1.0, sizes, axes);
				fitness += criterion(*p, sizes.z);
				break;
			case 'u':
			case 'U':
				ModelGeometryInfo::findSizesAndAxes(model, 1.0, sizes, axes);
				volume = sizes.x * sizes.y * sizes.z;
				area = ModelGeometryInfo::area(model, 1.0);
				fitness += criterion(*p, model.getPartCount() < 20 ? pow(area, 1.5) / volume : 0);
				break;
			default:
				printf("Unknown fitness criterion symbol: '%c'\n", *p);
				exit(3);
		}
		p++;
	}
	return fitness;
}

void update_fitness(Individual &ind, const char *fitness_def)
{
	ind.fitness = get_fitness(ind, fitness_def);
}

void print_stats(const vector<Individual> &population, char criterion)
{
	vector<double> criterion_values;
	char crit[2] = { 0 };
	crit[0] = criterion;
	for (const Individual& ind : population)
		criterion_values.push_back(get_fitness(ind, crit));
	printf("%g,%g,%g", *std::min_element(criterion_values.begin(), criterion_values.end()),
		   std::accumulate(criterion_values.begin(), criterion_values.end(), 0.0) / criterion_values.size(),
		   *std::max_element(criterion_values.begin(), criterion_values.end()));
}

int tournament(const vector<Individual> &population, int tournament_size)
{
	int best = -1;
	for (int i = 0; i < tournament_size; i++)
	{
		int rnd = rndUint(population.size());
		if (best == -1) best = rnd;
		else if (population[rnd].fitness > population[best].fitness) //assume maximization
			best = rnd;
	}
	return best;
}


// A minimalistic steady-state evolutionary algorithm.
int main(int argc, char *argv[])
{
	PreconfiguredGenetics genetics;
	LoggerToStdout messages_to_stdout(LoggerBase::Enable);
	GenMan genman;

	bool deterministic;
	int pop_size, nr_evals;
	double prob_mut, prob_xover;
	const char* format;
	const char* fitness_def;

	if (argc < 8)
	{
		printf("Too few parameters!\n");
		printf("Command line: <deterministic?_0_or_1> <population_size> <nr_evaluations> <prob_mut> <prob_xover> <genetic_format> <fitness_definition>\n");
		printf("Example: 1 10 50 0.6 0.4 4 NC\n\n");
		printf("Fitness definition is a sequence of capital (+1 weight) and small (-1 weight) letters.\n");
		printf("Each letter corresponds to one fitness criterion, and they are all weighted and added together.\n");
		printf("  0      - a constant value of 0 that provides a flat fitness landscape (e.g. for testing biases of genetic operators).\n");
		printf("  l or L - genotype length in characters.\n");
		printf("  p or P - the number of Parts.\n");
		printf("  j or J - the number of Joints.\n");
		printf("  n or N - the number of Neurons.\n");
		printf("  c or C - the number of neural Connections.\n");
		printf("  b or B - the bounding box volume.\n");
		printf("  s or S - the surface area.\n");
		printf("  v or V - volume.\n");
		printf("  h or H - height.\n");
		printf("  w or W- width.\n");
		printf("  d or D - depth.\n");

		printf("\nThe output consists of 7 columns separated by the TAB character.\n");
		printf("The first column is the number of mutated or crossed over and evaluated genotypes.\n");
		printf("The remaining columns are triplets of min,avg,max (in the population) of fitness, Parts, Joints, Neurons, Connections, genotype characters.\n");
		printf("Finally, the genotypes in the last population are printed with their fitness values.\n");
		return 1;
	}

	deterministic = atoi(argv[1]) == 1;
	pop_size = atoi(argv[2]);
	nr_evals = atoi(argv[3]);
	prob_mut = atof(argv[4]);
	prob_xover = atof(argv[5]);
	format = argv[6];
	fitness_def = argv[7];

	if (!deterministic)
		rndGetInstance().randomize();

	vector<Individual> population(pop_size);
	for (Individual& ind : population)
	{
		ind.geno = genman.getSimplest(format);
		if (ind.geno.getGenes() == "")
		{
			printf("Could not get the simplest genotype for format '%s'\n", format);
			return 2;
		}
		update_fitness(ind, fitness_def);
	}
	for (int i = 0; i < nr_evals; i++)
	{
		int selected_positive = tournament(population, max(2, int(sqrt(population.size()) / 2))); //moderate positive selection pressure
		int selected_negative = rndUint(population.size()); //random negative selection

		double rnd = rndDouble(prob_mut + prob_xover);
		if (rnd < prob_mut)
		{
			Geno mutant = genman.mutate(population[selected_positive].geno);
			if (mutant.getGenes() == "")
			{
				printf("Failed mutation (%s) of '%s'\n", mutant.getComment().c_str(), population[selected_positive].geno.getGenes().c_str());
			}
			else
			{
				population[selected_negative].geno = mutant;
				update_fitness(population[selected_negative], fitness_def);
			}
		}
		else
		{
			int selected_positive2 = tournament(population, max(2, int(sqrt(population.size()) / 2)));
			Geno xover = genman.crossOver(population[selected_positive].geno, population[selected_positive2].geno);
			if (xover.getGenes() == "")
			{
				printf("Failed crossover (%s) of '%s' and '%s'\n", xover.getComment().c_str(), population[selected_positive].geno.getGenes().c_str(), population[selected_positive2].geno.getGenes().c_str());
			}
			else
			{
				population[selected_negative].geno = xover;
				update_fitness(population[selected_negative], fitness_def);
			}
		}

		if (i % population.size() == 0 || i == nr_evals - 1)
		{
			printf("Evaluation %d", i);
			for (char c : string("!PJNCL"))
			{
				printf("\t");
				print_stats(population, c);
			}
			printf("\n");
		}
	}
	for (const Individual& ind : population)
	{
		printf("%.1f\t", ind.fitness);
		printf("%s\n", ind.geno.getGenesAndFormat().c_str());
	}

	return 0;
}
