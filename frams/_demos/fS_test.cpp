#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <chrono>
#include <common/nonstd_math.h>
#include "frams/genetics/fS/fS_general.h"
#include "frams/genetics/fS/fS_conv.h"
#include "frams/genetics/fS/fS_oper.h"
#include "frams/genetics/preconfigured.h"

using std::cout;
using std::endl;

void ensure(bool condition)
{
	if(!condition)
	{
		std::cout<<"ensure() failed!"<<std::endl;
		exit(0);
	}
}

int countChars(SString genotype, string chars, int count)
{
	int result = 0;
	for (int i = 0; i < genotype.length(); i++)
	{
		for (int j = 0; j < count; j++)
		{
			if (genotype[i] == chars[j])
				result += 1;
		}
	}
	return result;
}

int countJoints(SString genotype)
{
	return countChars(genotype, JOINTS, 2);
}

int countParams(SString genotype)
{
	return countChars(genotype, "=", 1);
}

int countModifiers(SString genotype)
{
	return countChars(genotype, "IiFfSsTt", 8);
}

int countNeuroConnections(fS_Genotype &geno)
{
	vector < fS_Neuron * > neurons = geno.getAllNeurons();
	int result = 0;
	for (int i = 0; i < int(neurons.size()); i++)
		result += neurons[i]->inputs.size();
	return result;
}

void testRearrangeBeforeCrossover()
{
	GenoOper_fS operators;
	string test_cases[] = {
			"1.1:EE[N]",
			"1.1:E[N;N;N]E[N;N;N]",
			"1.1:E[N;N;N]E[N'3;N'4'5;N'3]",
			"1.1:E[N'3;4;]E[N'3;N'4'5;N'3]",
			"1.1:E[N'1;2;0]E[N;N;N]",
			"1.1:E[N'1'3;N'2'4;N]E[N'3;N'4'5;N'3]",
			"1.1:E[Sin;N;G]E[Rnd;N;T]",
			"1.1:E[N'1'3;N'2'4;N](E[N'3;N'4'5;N'3]^E[N'3;N'4'6'7])",
			"1.1:E[N'1'3;N'2'4;N](E[N'0'3;N'4'5;N'3'6]^E[N'3;N'4'6'7])",
	};
	int expectedSubStart[] = {
			0, 3, 3, 3, 3, 3, 3, 3, 3
	};
	for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
	{
		fS_Genotype geno(test_cases[i]);
		Node *subtree = geno.getAllNodes()[1];
		int subStart;

		operators.rearrangeConnectionsBeforeCrossover(&geno, subtree, subStart);

		cout<<geno.getGeno().c_str()<<endl;
		ensure(subStart == expectedSubStart[i]);
	}
}

void testRearrangeAfterCrossover()
{
	GenoOper_fS operators;
	string test_cases[] = {
			"1.1:E[N'0'1;N'0]E[N]",
			"1.1:E[N'0'1;N'0]E[Rnd;N;N]",
			"1.1:E[N'0'1;N'0]E[Rnd;N;N]E[N;N]",
			"1.1:E[N'0'1;N'0](E[Rnd;N;N]^E[N'2'3;N'2])",
			"1.1:E[N'0'1;N'0](E[Rnd;N;N]^E[N'2'3;N'2]C[N'2'4])",
	};
	int subStart[] {
			0, 0, 0, 0, 0,
	};
	for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
	{
		fS_Genotype geno(test_cases[i]);
		Node *subtree = geno.getAllNodes()[1];

		operators.rearrangeConnectionsAfterCrossover(&geno, subtree, subStart[i]);

		cout<<geno.getGeno().c_str()<<endl;
	}
}

double EPSILON = 0.01;

bool doubleCompare(double a, double b)
{
	return fabs(a - b) < EPSILON;
}

const vector<Part::Shape> availablePartShapes{
		Part::Shape::SHAPE_ELLIPSOID,
		Part::Shape::SHAPE_CUBOID,
		Part::Shape::SHAPE_CYLINDER,
};
void testAddPart()
{
	GenoOper_fS operators;
	string test_cases[] = {
			"1.1:E",
			"1.1:SE",
			"1.1:sE",
			"1.1:SSSSSSSE",
			"1.1:sssssssE",
			"1.1:SSSSSSSSSE",    // More than max
			"1.1:sssssssssE", // Less than min
	};
	double expectedVolume[] = {
			4.19,
			5.58,
			3.15,
			20.94,
			0.84,
			20.94,
			0.84

	};

	for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
	{
		fS_Genotype geno(test_cases[i]);

		operators.addPart(geno, availablePartShapes, false);

		geno.getState(false);
		Node *newNode = geno.getAllNodes()[1];
		ensure(doubleCompare(newNode->calculateVolume(), expectedVolume[i]));
	}
}

void testChangePartType()
{
	GenoOper_fS operators;
	string test_cases[] = {
			"1.1:C",
			"1.1:E",
			"1.1:R",
			"1.1:SSSSC",
			"1.1:sssssE",
			"1.1:sssssC{x=0.3;y=2.3;z=1.1}",
			"1.1:SSSSSSE{x=0.3;y=2.3;z=1.1}",
	};

	for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
	{
		fS_Genotype geno(test_cases[i]);
		geno.getState(false);
		double oldVolume = geno.startNode->calculateVolume();

		operators.changePartType(geno, availablePartShapes);

		geno.getState(false);
		ensure(doubleCompare(geno.startNode->calculateVolume(), oldVolume));
	}

}
void testUsePartType()
{
	GenoOper_fS operators;
	vector<Part::Shape> cuboid;
	cuboid.push_back(Part::Shape::SHAPE_CUBOID);
	vector<Part::Shape> ellipsoid;
	ellipsoid.push_back(Part::Shape::SHAPE_ELLIPSOID);
	vector<Part::Shape> cylinder;
	cylinder.push_back(Part::Shape::SHAPE_CYLINDER);

	fS_Genotype geno("1.1:E");
	ensure(geno.getAllNodes()[0]->partType == Part::Shape::SHAPE_ELLIPSOID);
	operators.changePartType(geno, cylinder);
	ensure(geno.getAllNodes()[0]->partType == Part::Shape::SHAPE_CYLINDER);
	operators.addPart(geno, cylinder);
	ensure(geno.getAllNodes()[1]->partType == Part::Shape::SHAPE_CYLINDER);
	operators.removePart(geno);
	operators.addPart(geno, cuboid);
	ensure(geno.getAllNodes()[1]->partType == Part::Shape::SHAPE_CUBOID);
	operators.removePart(geno);
	operators.addPart(geno, ellipsoid);
	ensure(geno.getAllNodes()[1]->partType == Part::Shape::SHAPE_ELLIPSOID);

}

void testTurnWithRotation()
{
	GenoConv_fS0s converter = GenoConv_fS0s();
	fS_Genotype::TURN_WITH_ROTATION = true;
	MultiMap map;
	SString test_cases[]{
			"1.1:EE{ty=1.56}",
			"1.1:EE{ty=0.78;ry=0.78}",
			"1.1:EE{tx=30;ty=1.56;tz=45}",
	};

	for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
	{
		SString genotype_str = test_cases[i];
		/// Test translate
		cout << converter.convert(genotype_str, &map, false).c_str() << endl;
	}
	fS_Genotype::TURN_WITH_ROTATION = false;

}

/**
 * Cases when exchanging trees with similar size aways makes children of the equal parents equal to them
 * Test cases will almost always work when crossoverTries is big enough
 */
void testCrossoverSimilarTrees()
{
	GenoOper_fS operators;
	string test_cases[] = {
			"1.1:EE",
			"1.1:E(E^E)",
			"1.1:EEEE",
			"1.1:ECRE",
			"1.1:E(RE^CRE)",
			"1.1:E(EEE^EEE^EEE)",
			"1.1:E(CRE^CRE^CRE)",
			"1.1:EEEEEECRE(CRE^CRE^CRE)",
	};

	float f1, f2;
	// Repeat the test several times as crossover is not deterministic
	for (int z = 0; z < 10; z++)
	{
		for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
		{
			char *arr1 = strdup(test_cases[i].c_str());
			char *arr2 = strdup(arr1);

			operators.crossOver(arr1, arr2, f1, f2);

			cout<<arr1<<endl;
			free(arr1);
			free(arr2);
		}
	}
}

void testAllPartSizesValid()
{
	string test_cases[] = {
			"1.1:C{x=2000.0}",    // Too big dimension
			"1.1:C{y=2000.0}",
			"1.1:C{z=2000.0}",
			"1.1:C{x=0.0005}",    // Too small dimension
			"1.1:C{y=0.0005}",
			"1.1:C{z=0.0005}",
			"1.1:E{x=1.1}",        // Invalid size params
			"1.1:E{y=1.1}",
			"1.1:E{z=1.1}",
			"1.1:R{z=1.1;y=1.2}",
			"1.1:R{z=1.1}",
			"1.1:R{y=1.1}",
			"1.1:SR{z=999.0}",
			"1.1:C(R^E{z=1.1})",
			"1.1:C{x=1.5;y=1.5;z=1.5}",    // Test volume
			"1.1:C{x=1.8;y=1.8}",
	};

	for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
	{
		fS_Genotype geno(test_cases[i]);
		ensure(geno.checkValidityOfPartSizes() != 0);
	}
}

void testOneGenotype(SString test, int expectedPartCount)
{
	GenoOper_fS operators;
	GenoConv_fS0s converter = GenoConv_fS0s();
	MultiMap map;
	int tmp = -1;
	SString tmpStr;

	/// Test get geno
	fS_Genotype geno(test.c_str());
	cout << geno.getGeno().c_str() << endl;

	/// Test translate
	cout << converter.convert(test, &map, false).c_str() << endl;

	// Test mapping
	map.print();

	// Test checkpoints
	int checkpointCount = geno.buildModel(true).getCheckpointCount();
	ensure(checkpointCount == geno.getNodeCount());

	////Test operations
	// Test part count
	ensure(geno.getNodeCount() == expectedPartCount);

	// Test add part
	tmp = geno.getNodeCount();
	operators.addPart(geno, availablePartShapes);
	ensure(tmp + 1 == geno.getNodeCount());

	// Test change part
	tmp = geno.getNodeCount();
	tmpStr = geno.getGeno();
	if (operators.changePartType(geno, availablePartShapes))
	{
		ensure(geno.getNodeCount() == tmp);
		ensure(geno.getGeno() != tmpStr);
	}
	// Test remove part
	tmp = geno.getNodeCount();
	if (operators.removePart(geno))
		ensure(tmp == 1 + geno.getNodeCount());

	// Test change joint
	char firstJoint;
	if(geno.getNodeCount() == 2)
		firstJoint = geno.getAllNodes()[1]->joint;
	tmpStr = geno.getGeno();
	if (operators.changeJoint(geno))
	{
		ensure(tmpStr != geno.getGeno());
		if(geno.getNodeCount() == 2)
			// If there are only 2 nodes, we know which joint has been changed
			ensure(geno.getAllNodes()[1]->joint != firstJoint);
	}

	// Test add param
	tmp = countParams(geno.getGeno());
	if (operators.addParam(geno))
		ensure(tmp + 1 == countParams(geno.getGeno()));

	// Test change param
	tmpStr = geno.getGeno();
	tmp = countParams(geno.getGeno());
	fS_Genotype::precision = 6;
	if (operators.changeParam(geno))
	{
		SString resultGeno = geno.getGeno();
		ensure(tmp == countParams(resultGeno));
		ensure(tmpStr != resultGeno);
	}

	// Test remove param
	tmp = countParams(geno.getGeno());
	if (operators.removeParam(geno))
		ensure(tmp == 1 + countParams(geno.getGeno()));

	// Test add modifier
	tmp = countModifiers(geno.getGeno());
	if (operators.changeModifier(geno))
		ensure(tmp != countModifiers(geno.getGeno()));

	// Test add neuro
	tmp = geno.getAllNeurons().size();
	if (operators.addNeuro(geno))
		ensure(tmp + 1 == int(geno.getAllNeurons().size()));

	// Test add neuro connections
	tmp = countNeuroConnections(geno);
	if (operators.addNeuroConnection(geno))
		ensure(tmp + 1 == countNeuroConnections(geno));

	// Test change neuro connection
	tmpStr = geno.getGeno();
	if (operators.changeNeuroConnection(geno))
		ensure(test != geno.getGeno());

	// Test remove neuro connections
	tmp = countNeuroConnections(geno);
	if (operators.removeNeuroConnection(geno))
		ensure(tmp - 1 == countNeuroConnections(geno));

	// Test remove neuro
	tmp = geno.getAllNeurons().size();
	if (operators.removeNeuro(geno))
		ensure(tmp - 1 == int(geno.getAllNeurons().size()));

	// Test change neuro params
	tmpStr = geno.getGeno();
	if(operators.changeNeuroParam(geno))
		ensure(tmpStr != geno.getGeno());
}

void validationTest()
{
	GenoConv_fS0s converter = GenoConv_fS0s();
	GenoOper_fS operators;
	SString invalidGenotypes[] = {
			"1.1:FFF",    // No part type
			"1.1:FFF{x=5.0}",    // No part type
			"1.1:qqE",    // Invalid modifier
			"1.1:E{f}",    // No equal sign
			"1.1:E{qw=1.0}",    // Wrong param key
			"1.1:E{f=}",    // Wrong param value
			"1.1:E{f=fr}",    // Wrong param value
			"1.1:E[G'w'2]",    // Invalid neuro connection key
			"1.1:E[G'1:w'2]",    // Invalid neuro connection value
			"1.1:E{",    // Lacking param end
			"1.1:E[",    // Lacking neuro end
			"1.1:E{x=1.5;y=0.0}",    // Lacking param end
			"1.1:E[2]",    // Invalid neuron connection key
			"1.1:E[-2]",    // Invalid neuron connection key
			"1.1:E[;;3]",    // Invalid neuron connection key
			"1.E",			// No genotype param separator
			"1.E",			// No genotype param separator
			"EE",			// No genotype params
			"abc:E"		// Invalid genotype params
	};
	int errorIndexes[] = {
			5, 5, 5, 6,
			6, 8, 8, 7, 7,
			6, 6, 14, 1, 1,
			1, 1, 1, 1, 1
	};
	for (int i = 0; i < int(sizeof(invalidGenotypes) / sizeof(invalidGenotypes[0])); i++)
	{
		MultiMap map;
		ensure(operators.checkValidity(invalidGenotypes[i].c_str(), "") == errorIndexes[i]);
		SString genes = converter.convert(invalidGenotypes[i], &map, false);
		ensure(genes == "");
	}
}

void testRearrangeInputs()
{
	const int size = 6;
	string before = "1.1:E[T]bE[N'2'3]cRbC[T;G'1'2]bE[N'1'2'3;T]{x=3.0;y=3.0;z=3.0}";
	SHIFT shift[size]{
			SHIFT::RIGHT,
			SHIFT::RIGHT,
			SHIFT::RIGHT,
			SHIFT::LEFT,
			SHIFT::LEFT,
			SHIFT::LEFT,
	};
	int neuronNumber[size]{
			0,    // First
			2,   // Middle
			5,    // Last
			0,
			2,
			5
	};

	for (int i = 0; i < size; i++)
	{
		fS_Genotype geno(before);
		vector < fS_Neuron * > allNeurons = geno.getAllNeurons();
		fS_Neuron *neuron = allNeurons[neuronNumber[i]];

		geno.rearrangeNeuronConnections(neuron, shift[i]);

		cout<<geno.getGeno().c_str()<<endl;
	}

}

void testMutateSizeParam()
{
	GenoOper_fS operators;
	double minVolume = Model::getMinPart().volume;
	double maxVolume = Model::getMaxPart().volume;
	double minRadius = Model::getMinPart().scale.x;
	double maxRadius = Model::getMaxPart().scale.x;
	string test_cases[] = {
			"1.1:C{x=2.4}",
			"1.1:E{x=4.9}",
			"1.1:R{x=3.3}",
			"1.1:C{x=4.9;y=0.5}",
			"1.1:C{x=4.9;y=0.25;z=2.0}",
			"1.1:C{x=999.0;y=0.05;z=0.05}",
	};

	for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
	{
		for(int j=0; j < int(SIZE_PARAMS.size()); j++)
		{
			fS_Genotype geno(test_cases[i]);
			geno.getState(false);

			bool result = operators.mutateSizeParam(geno.startNode, SIZE_PARAMS[j], false);

			geno.getState(false);
			double volume = geno.startNode->calculateVolume();
			Pt3D size = geno.startNode->calculateSize();
			ensure(result);
			ensure(minVolume < volume && volume < maxVolume);
			ensure(minRadius < size.x && size.x < maxRadius);
			ensure(strcmp(geno.getGeno().c_str(), test_cases[i].c_str()) != 0);
		}
	}
}

int main(int argc, char *argv[])
{
	SString test_cases[] = {
			"1.1:E",
			"1.1:C",
			"1.1:R",
			"1.1:EEE",
			"1.1:E(E^E)",
			"1.1:E(E(E^E)^E^E(E^E)^E)",
			"1.1:EbE",
			"1.1:CcC",
			"1.1:ERbRcCRbCbE",
			"1.1:E",
			"1.1:FE",
			"1.1:fE",
			"1.1:FFE",
			"1.1:E{f=0.3}E{f=0.5}",
			"1.1:EE{ty=1.56}",
			"1.1:EE{tz=1.56}",
			"1.1:EE{tz=1.56}E{tx=1.56}E{ty=1.56}",
			"1.1:EE{tz=0.78}E{tx=0.78}E{ty=0.78}",
			"1.1:EE{tz=-1.56}E{tx=-1.56}E{ty=-1.56}",
			"1.1:EE{x=3.0}",
			"1.1:EE{x=3.0;y=3.0;z=3.0}",
			"1.1:SSE",
			"1.1:ssE",
			"1.1:SSSE",
			"1.1:ESSSE",
			"1.1:SSSEE",
			"1.1:IE",
			"1.1:iE",
			"1.1:IIE",
			"1.1:E{i=0.3}E{i=0.5}",
			"1.1:IIE{i=0.5}",
			"1.1:EE{ty=180.0;x=3.0}",
			"1.1:EE{ty=1.56;z=5.0}",
			"1.1:E{x=1.5;z=2.0}E{tz=1.56;x=2.0;y=5.0;z=3.0}",
			"1.1:E{y=3.0;z=4.0}E{y=4.0;z=5.0}",
			"1.1:E{y=399.0;z=599.0}E{y=799.0;z=999.0}",
			"1.1:EE{ty=0.78}",
			"1.1:EE{ty=1.56}",
			"1.1:EE{ry=1.56;z=2.0}",
			"1.1:E{ry=1.56;z=2.0}E{ry=1.56;z=2.0}",
			"1.1:EE{ry=0.78;z=2.0}",
			"1.1:EE{ry=0.52;z=2.0}",
			"1.1:E[N]",
			"1.1:E[N;N]",
			"1.1:E[N]E[N]",
			"1.1:E[T]",
			"1.1:E[N]E[N'0]",
			"1.1:E[Sin;T'0]",
			"1.1:E[Sin'2;T'0;T'0'1]",
			"1.1:E[Sin'2;T'0;T'0'1]{ry=1.56;z=2.0}E{ry=1.56;z=2.0}",
			"1.1:E[Sin'2:2.0;T'0:3.0;T'0:4.0'1:5.0]",
			"1.1:E[N]E[G'0]",
			"1.1:E[N]E[Rnd'0]",
			"1.1:E[Rnd]E[Rnd'0'1;Sin'0]",
			"1.1:E{s=1.5}",
			"1.1:SE{s=1.1;x=1.2;z=1.3}",
			"1.1:SE{s=0.9}E{s=1.1;x=1.2;z=1.3}",
			"1.1:ETTE{st=0.5}",
			"1.1:EttE{st=0.5}",
			"1.3:SE",
			"1.1:EC{ry=0.78}",
			"1.1:EC{ry=0.78;rx=0.78}",
			"1.1:C{ry=0.78;rx=0.78}C{ry=0.78;rx=0.78}",
			"1.1:E{rx=0.78}E{tz=1.57;rx=0.78}",
			"1.1:R{rx=0.78}R{tz=1.57;rx=0.78}",
			"1.1:C{rx=0.78}C{tz=1.57;rx=0.78}",
			"1.1:C{rx=0.78}R{tz=1.57;rx=0.78}",
			"1.1:C{rx=0.78}E{tz=1.57;rx=0.78}",
			"1.1:R{rx=0.78}E{tz=1.57;rx=0.78}",
	};


	int expectedPartCount[] = {
			1, 1, 1, 3, 3, 9, 2, 2, 7, 1,
			1, 1, 1, 2, 2, 2, 4, 4, 4, 2,
			2, 1, 1, 1, 2, 2, 1,
			1, 1, 2, 1, 2, 2, 2, 2, 2, 2,
			2, 2, 2, 2, 2, 1, 1, 2, 1, 2,
			1, 1, 2, 1, 2, 2, 2, 1, 1, 2,
			2, 2, 1, 2, 2, 2, 2, 2, 2, 2,
			2, 2};
	PreconfiguredGenetics genetics;


	for (int i = 0; i < int(sizeof(test_cases) / sizeof(test_cases[0])); i++)
	{
		testOneGenotype(test_cases[i], expectedPartCount[i]);
	}

	testAllPartSizesValid();
	testRearrangeInputs();
	validationTest();
	testCrossoverSimilarTrees();
	testRearrangeBeforeCrossover();
	testRearrangeAfterCrossover();
	testAddPart();
	testChangePartType();
	testUsePartType();
	testTurnWithRotation();
	testMutateSizeParam();

	cout << "FINISHED";
	return 0;
}