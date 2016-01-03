/*  TITLE
	DEDUCTIVE FAULT SIMULATOR
*/

/*  PERSONAL DETAILS
	Krishna Prasad Suresh
	MS in ECE at Georgia Institute of Technology
	Specialization: VLSI Systems and Logic Design
	skprasad.22@gmail.com
	+1 (470) 263-0867
*/

/*  DESCRIPTION
	This Deductive Fault Simulator can be used for 2 purposes:
	1. To analyse the circuit by checking the faults detected for individual test vectors
		Gives a list of faults detected and the fault coverage for the given inputs.
	2. To find out the number of test vectors required to achieve a given fault coverage

	For every location, both s-a-0 and s-s-1 faults are considered. The fault locations can be chosen in 5 different ways:
	1. At Primary inputs.
	2. At Primary outputs.
	3. At Gate outputs.
	4. At Primary inputs and Primary outputs.
	5. At Primary inputs, Primary outputs and Gate outputs.
*/

/*  STEPS
	1. Put the text file containing the circuit in the same folder as that of the code.
	2. Enter the filename without .txt.
	3. Proceed further based on the options you select.
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

struct Gates
{
	string	gateType;
	int		gateNumber;
	int		gateInput1, gateInput2;
	int		gateOutput;
	int		gateFlag;
}G;

struct Nodes
{
	int				nodeValue;
	int				nodeFlag;
	int				SAvalue0;
	int				SAvalue1;
	vector <int>	nodeErrorList;
	vector <Gates>	listofGates;
}N;

ofstream outputFile;

int no_spaces1 = 0;
int faultCount = 0;										// Number of faults

float expFaultCoverage = 0;								// The needed fault coverage

string inputValues = "";								// Input test vector when manually entering the test case

vector <Gates> Gate;
vector <Nodes> Node;
vector <Gates> Temp;
vector <int>   PINodes;									// List of primary input nodes
vector <int>   PONodes;									// List of primary output nodes
vector <int>   GONodes;									// List of gate output nodes
vector <int>   faultList;								// List of faults

class DeductiveFM
{
public:
	DeductiveFM();
	~DeductiveFM();

	// Reads the circuit file
	int readCircuitFile(int choice)
	{
		int no_spaces	= 0;												// Number of spaces in each line
		int lineCount	= 0;												// Number of the line being read
		int i			= 0;												// Control variable

		string fileName	   = "";											// Name of the file containing the interconnection
		string line		   = "";											// Each line in the file

		// Reading the file containing the interconnection
		cout << endl << "Enter the filename here: ";
		cin  >> fileName;

		fileName += ".txt";

		ifstream inputFile(fileName);										// Opens the file containing the interconnection

		// Displaying all the contents of the file
	    if(inputFile.is_open())
		{
			while(getline(inputFile, line))
				cout << endl << line;
			inputFile.close();
		}
		else
			return 0;

		inputFile.open(fileName);

		// Tokenizing and storing all the contents of the file
		if (inputFile.is_open())
		{
			while (getline(inputFile, line))
			{
				no_spaces = std::count(line.begin(), line.end(), ' ');

				string *subString = new string[no_spaces + 1];							// Sub string in each line
				istringstream iss(line);

				i = 0;

				// Tokenizing each line into sub strings
				while (iss && i <= no_spaces)
				{
					string sub;
					iss >> sub;
					subString[i] = sub;
					i++;
				}

				G.gateType		= subString[0];							// Name of the gate
				G.gateNumber	= lineCount;							// Number of the gate

				// Reading all lines except inputs and outputs
				if (G.gateType != "INPUT" && G.gateType != "OUTPUT")
				{
					G.gateInput1 = atoi(subString[1].c_str());							// Input 1 of the gate

					// Total number of input nodes till gate input 1
					while (Node.size() <= G.gateInput1)
						Node.push_back(N);

					Node.at(G.gateInput1).listofGates.push_back(G);						// List of gates at each node using gate input 1

					// Reading all lines except INV and BUF
					if (G.gateType != "INV" && G.gateType != "BUF")
					{
						G.gateInput2 = atoi(subString[2].c_str());						// Input 2 of the gate
						G.gateOutput = atoi(subString[3].c_str());						// Output of the double-input gate

						GONodes.push_back(G.gateOutput);

						// Total number of input nodes till gate input 2
						while (Node.size() <= G.gateInput2)
							Node.push_back(N);

						Node.at(G.gateInput2).listofGates.push_back(G);					// List of gates at each node using gate input 2
					}
					else
					{
						G.gateOutput = atoi(subString[2].c_str());						// Output of the single-input gate

						GONodes.push_back(G.gateOutput);
					}						

					Gate.push_back(G);
				}
				else
				{
					// Reading the INPUTS line
					if (G.gateType == "INPUT")
					{
						no_spaces1 = no_spaces;
						cout << endl << endl;

						// Choice 1: Individual Analysis
						if (choice == 1)
						{
							cout << "Input Vector (" << (no_spaces - 2) << " inputs): ";
							cin  >> inputValues;

							if (inputValues.length() != (no_spaces - 2))
								return 2;

							// Assigning values to Input nodes
							for (int i = 1; i < (no_spaces - 1); i++)
							{
								Node.at(atoi(subString[i].c_str())).nodeValue = (int) (inputValues[i-1] - 48);
								Node.at(atoi(subString[i].c_str())).nodeFlag = 1;			                        // Flag indicating that the node has been assigned a value

								PINodes.push_back(atoi(subString[i].c_str()));										// List of PI nodes
							}
						}
						// Choice 2: Fault Coverage
						else if (choice == 2)
						{
							// Expected fault coverage
							cout << "What is the expected fault coverage?" << endl;
							cin  >> expFaultCoverage;

							if (expFaultCoverage >= 100)
								return 3;

							// Generating a random input
							for (int i = 1; i < (no_spaces - 1); i++)
								inputValues += ((rand() % 2) + 48);

							// Assigning values to Input nodes
							for (int i = 1; i < (no_spaces - 1); i++)
							{
								Node.at(atoi(subString[i].c_str())).nodeValue = (int) (inputValues[i-1] - 48);
								Node.at(atoi(subString[i].c_str())).nodeFlag = 1;			                        // Flag indicating that the node has been assigned a value

								PINodes.push_back(atoi(subString[i].c_str()));										// List of PI nodes
							}
						}
						else
						{
							cout << endl << "Incorrect choice!";
							return 0;
						}
					}

					// Reading the OUTPUTS line
					if (G.gateType == "OUTPUT")
					{
						for (int i = 1; i < (no_spaces - 1) ; i++)
							PONodes.push_back(atoi(subString[i].c_str()));			// List of PO nodes

						for (int i = 0; i < PONodes.size(); i++)
							while (Node.size() <= PONodes.at(i))
								Node.push_back(N);
						break;
					}
				}

				no_spaces = 0;
				lineCount++;
			}

	        inputFile.close();							// Close the file containing the interconnection
		}

		return 1;
	}

	// Creates the fault list
	int SAFaultList(int choice)
	{
		int errorNodeValue = 0;												// l in l s-a-v

		// PIs
		if (choice == 1)
		{
			for (int i = 0; i < PINodes.size(); i++)
				assignFaults(PINodes.at(i));
			faultCount = 2 * PINodes.size();
			return 1;
		}
		// POs
		else if (choice == 2)
		{
			for (int i = 0; i < PONodes.size(); i++)
				assignFaults(PONodes.at(i));
			faultCount = 2 * PONodes.size();
			return 1;
		}
		// GOs
		else if (choice == 3)
		{
			for (int i = 0; i < GONodes.size(); i++)
				assignFaults(GONodes.at(i));
			faultCount = 2 * GONodes.size();
			return 1;
		}
		// PIs and POs
		else if (choice == 4)
		{
			for (int i = 0; i < PINodes.size(); i++)
				assignFaults(PINodes.at(i));
			for (int i = 0; i < PONodes.size(); i++)
				assignFaults(PONodes.at(i));
			faultCount = 2 * (PINodes.size() + PONodes.size());
			return 1;
		}
		// PIs, POs and GOs
		else if (choice == 5)
		{
			for (int i = 0; i < PINodes.size(); i++)
				assignFaults(PINodes.at(i));
			for (int i = 0; i < PONodes.size(); i++)
				assignFaults(PONodes.at(i));
			for (int i = 0; i < GONodes.size(); i++)
				assignFaults(GONodes.at(i));
			faultCount = 2 * (PINodes.size() + GONodes.size());
			return 1;
		}
		else
			return 0;
	}

	// Assigns the l s-a-vs
	void assignFaults(int errorNodeValue)
	{
		Node.at(errorNodeValue).SAvalue0 = errorNodeValue * 10 + 0;

		if (Node.at(errorNodeValue).nodeFlag == 1)																// Checking if the node has a value
			if (errorNodeValue * 10 + Node.at(errorNodeValue).nodeValue != Node.at(errorNodeValue).SAvalue0)	// Checking if the node value and the stuck at value are the same
				Node.at(errorNodeValue).nodeErrorList.push_back(Node.at(errorNodeValue).SAvalue0);				// Changing the node value to the stuck at value

		Node.at(errorNodeValue).SAvalue1 = errorNodeValue * 10 + 1;

		if (Node.at(errorNodeValue).nodeFlag == 1)																// Checking if the node has a value
			if (errorNodeValue * 10 + Node.at(errorNodeValue).nodeValue != Node.at(errorNodeValue).SAvalue1)	// Checking if the node value and the stuck at value are the same
				Node.at(errorNodeValue).nodeErrorList.push_back(Node.at(errorNodeValue).SAvalue1);				// Changing the node value to the stuck at value
	}

	// Creates the output file for case 1
	int createOutputFile()
	{
		char ans = 'y';

		int inputVectorCount = 1;
		int coverage		 = 0;

		outputFile.open("Outputs.txt");                         // Creating the output file

		while (ans == 'y')
		{
			outputFile << endl << "The input vector is : " << inputValues << endl;

			displayOutputs();

			sort(faultList.begin(), faultList.end());
			faultList.erase(unique(faultList.begin(), faultList.end()), faultList.end());

			cout << endl << endl << "The total number of inputs applied are : " << inputVectorCount;
			outputFile << endl << endl << "The total number of inputs applied are : " << inputVectorCount;

			cout << endl << "The total number of faults detected are " << faultList.size() << " out of "<< faultCount;
			outputFile << endl << "The total number of faults detected are " << faultList.size()<< " out of "<< faultCount;

			coverage = (faultList.size() * 100) / faultCount;

			cout << endl << "The fault coverage is : " << coverage;
			outputFile << endl << "The fault coverage is : " << coverage;

			cout << endl << "The final fault list is : ";
			outputFile << endl << "The final fault list is : ";

			for (int i = 0 ; i < faultList.size() ; i++)
			{
				cout << faultList.at(i)/10 << " s-a " << faultList.at(i)%10 << " , ";
				cout << "\b\b ";
				outputFile << faultList.at(i)<< ",";
				outputFile << "\b\b ";
			}

			cout << endl << endl << "Do you wish to enter more input vectors (y/n): ";
			cin >> ans;

			if (ans != 'y' && ans != 'n')
				return 2;

			if (ans == 'y')
			{
				for (int i = 0; i < Node.size(); i++)
				{
					Node.at(i).nodeFlag = 0;
					Node.at(i).nodeErrorList.clear();
				}

				for (int i = 0; i < Gate.size(); i++)
					Gate.at(i).gateFlag = 0;

				cout << endl << "Input Vector (" << (no_spaces1 - 2) << " inputs): ";
				cin  >> inputValues;

				if (inputValues.length() != (no_spaces1 - 2))
					return 0;

				inputVectorCount++;

				for (int i = 0; i < (no_spaces1 - 2); i++)
				{
					Node.at(PINodes.at(i)).nodeValue = (int)(inputValues[i] - 48);
					Node.at(PINodes.at(i)).nodeFlag = 1;

					if (PINodes.at(i) * 10 + Node.at(PINodes.at(i)).nodeValue != Node.at(PINodes.at(i)).SAvalue0)
						Node.at(PINodes.at(i)).nodeErrorList.push_back(Node.at(PINodes.at(i)).SAvalue0);

					if (PINodes.at(i) * 10 + Node.at(PINodes.at(i)).nodeValue != Node.at(PINodes.at(i)).SAvalue1)
						Node.at(PINodes.at(i)).nodeErrorList.push_back(Node.at(PINodes.at(i)).SAvalue1);
				}
			}
		}

		outputFile.close();
		return 1;
	}

	// Creates the output file for case 2
	void createOutputFile_FC()
	{
		int inputVectorCount = 1;
		int coverage		 = 0;

		outputFile.open("Outputs.txt");                         // Creating the output file

		while (coverage < expFaultCoverage)
		{
			calculateOutputs();

			sort(faultList.begin(), faultList.end());
			faultList.erase(unique(faultList.begin(), faultList.end()), faultList.end());

			coverage = (faultList.size() * 100) / faultCount;

			if (coverage < expFaultCoverage)
			{
				for (int i = 0; i < Node.size(); i++)
				{
					Node.at(i).nodeFlag = 0;
					Node.at(i).nodeErrorList.clear();
				}

				for (int i = 0; i < Gate.size(); i++)
					Gate.at(i).gateFlag = 0;

				inputVectorCount++;

				inputValues = "";

				// Generating a random input
				for (int i = 1; i < (no_spaces1 - 1); i++)
					inputValues += ((rand() % 2) + 48);

				for (int i = 0; i < (no_spaces1 - 2); i++)
				{
					Node.at(PINodes.at(i)).nodeValue = (int)(inputValues[i] - 48);
					Node.at(PINodes.at(i)).nodeFlag = 1;

					if (PINodes.at(i) * 10 + Node.at(PINodes.at(i)).nodeValue != Node.at(PINodes.at(i)).SAvalue0)
						Node.at(PINodes.at(i)).nodeErrorList.push_back(Node.at(PINodes.at(i)).SAvalue0);

					if (PINodes.at(i) * 10 + Node.at(PINodes.at(i)).nodeValue != Node.at(PINodes.at(i)).SAvalue1)
						Node.at(PINodes.at(i)).nodeErrorList.push_back(Node.at(PINodes.at(i)).SAvalue1);
				}
			}
		}

		cout << endl << "Fault Coverage = " << coverage;
		outputFile << endl << "Fault Coverage = " << coverage;
		cout << endl << "No. of inputs applied = " << inputVectorCount;
		outputFile << endl << "No. of inputs applied = " << inputVectorCount;
	}

	// Displays the final outputs for Case 1
	void displayOutputs()
	{
		while (checkOutputValue(PONodes) != 1)
			for (int i = 0 ; i < Gate.size() ; i++)
				if (Gate.at(i).gateFlag != 1)
					checkOutput(i);

		for (int i = 0; i < PONodes.size(); i++)
		{
			if (Node.at(PONodes.at(i)).nodeErrorList.size())
			{
				cout << endl << "The faults propagated at the node " << PONodes.at(i) << " are: ";
				outputFile << endl << "The faults propagated at the node " << PONodes.at(i) << " are: ";
			}

			for (int j = 0; j < Node.at(PONodes.at(i)).nodeErrorList.size(); j++)
			{
				cout << Node.at(PONodes.at(i)).nodeErrorList.at(j)/10 << " s-a " << Node.at(PONodes.at(i)).nodeErrorList.at(j)%10 << " , ";
				outputFile << Node.at(PONodes.at(i)).nodeErrorList.at(j)/10 << " s-a " << Node.at(PONodes.at(i)).nodeErrorList.at(j)%10 << " , ";

				faultList.push_back(Node.at(PONodes.at(i)).nodeErrorList.at(j));
			}

			if (Node.at(PONodes.at(i)).nodeErrorList.size())
			{
				cout << "\b\b ";
				outputFile << "b\b ";
			}
		}
	}

	// Calculate outputs for Case 2
	void calculateOutputs()
	{
		while (checkOutputValue(PONodes) != 1)
			for (int i = 0 ; i < Gate.size() ; i++)
				if (Gate.at(i).gateFlag != 1)
					checkOutput(i);

		for (int i = 0; i < PONodes.size(); i++)
			for (int j = 0; j < Node.at(PONodes.at(i)).nodeErrorList.size(); j++)
				faultList.push_back(Node.at(PONodes.at(i)).nodeErrorList.at(j));
	}

	// Checks if the POs have values assigned
	int checkOutputValue(vector <int> outputNode)
	{
		int a = 0;

		for (int i = 0; i < outputNode.size(); i++)
		{
			a = outputNode.at(i);
			if (Node.at(a).nodeFlag != 1)
				return 0;
		}

		return 1;
	}

	// Evaluates every gate
	void checkOutput(int a)
	{
		if (Gate.at(a).gateType == "INV" || Gate.at(a).gateType == "BUF")
		{
			if (Node.at(Gate.at(a).gateInput1).nodeFlag == 1 && Gate.at(a).gateFlag != 1)
			{
				Gate.at(a).gateFlag = 1;

				if (Gate.at(a).gateType == "INV")
				{
					Node.at(Gate.at(a).gateOutput).nodeValue = !(Node.at(Gate.at(a).gateInput1).nodeValue);
					Node.at(Gate.at(a).gateOutput).nodeFlag = 1;

					Node.at(Gate.at(a).gateOutput).nodeErrorList.reserve(Node.at(Gate.at(a).gateOutput).nodeErrorList.size() + Node.at(Gate.at(a).gateInput1).nodeErrorList.size() + 1);
					Node.at(Gate.at(a).gateOutput).nodeErrorList.insert(Node.at(Gate.at(a).gateOutput).nodeErrorList.end(),Node.at(Gate.at(a).gateInput1).nodeErrorList.begin(),Node.at(Gate.at(a).gateInput1).nodeErrorList.end());

					if (Node.at(Gate.at(a).gateOutput).SAvalue0 > 0)
						if ((Gate.at(a).gateOutput * 10 + Node.at(Gate.at(a).gateOutput).nodeValue) != Node.at(Gate.at(a).gateOutput).SAvalue0)
							Node.at(Gate.at(a).gateOutput).nodeErrorList.push_back(Node.at(Gate.at(a).gateOutput).SAvalue0);

					if (Node.at(Gate.at(a).gateOutput).SAvalue1 > 0)
						if (Gate.at(a).gateOutput * 10 + Node.at(Gate.at(a).gateOutput).nodeValue != Node.at(Gate.at(a).gateOutput).SAvalue1)
							Node.at(Gate.at(a).gateOutput).nodeErrorList.push_back(Node.at(Gate.at(a).gateOutput).SAvalue1);
				}

				else
				{
					Node.at(Gate.at(a).gateOutput).nodeValue = Node.at(Gate.at(a).gateInput1).nodeValue;
					Node.at(Gate.at(a).gateOutput).nodeFlag = 1;

					Node.at(Gate.at(a).gateOutput).nodeErrorList.reserve(Node.at(Gate.at(a).gateOutput).nodeErrorList.size() + Node.at(Gate.at(a).gateInput1).nodeErrorList.size() + 1);
					Node.at(Gate.at(a).gateOutput).nodeErrorList.insert(Node.at(Gate.at(a).gateOutput).nodeErrorList.end(),Node.at(Gate.at(a).gateInput1).nodeErrorList.begin(),Node.at(Gate.at(a).gateInput1).nodeErrorList.end());

					if (Node.at(Gate.at(a).gateOutput).SAvalue0 > 0)
						if (Gate.at(a).gateOutput * 10 + Node.at(Gate.at(a).gateOutput).nodeValue != Node.at(Gate.at(a).gateOutput).SAvalue0)
							Node.at(Gate.at(a).gateOutput).nodeErrorList.push_back(Node.at(Gate.at(a).gateOutput).SAvalue0);

					if (Node.at(Gate.at(a).gateOutput).SAvalue1 > 0)
						if (Gate.at(a).gateOutput * 10 + Node.at(Gate.at(a).gateOutput).nodeValue != Node.at(Gate.at(a).gateOutput).SAvalue1)
							Node.at(Gate.at(a).gateOutput).nodeErrorList.push_back(Node.at(Gate.at(a).gateOutput).SAvalue1);
				}

				checkNode(Gate.at(a).gateOutput);
			}
		}

		else
		{
			if (Node.at(Gate.at(a).gateInput1).nodeFlag == 1 && Node.at(Gate.at(a).gateInput2).nodeFlag == 1 && Gate.at(a).gateFlag != 1)
			{
				Gate.at(a).gateFlag = 1;

				if (Gate.at(a).gateType == "AND")
				{
					Node.at(Gate.at(a).gateOutput).nodeValue = Node.at(Gate.at(a).gateInput1).nodeValue & Node.at(Gate.at(a).gateInput2).nodeValue;
					Node.at(Gate.at(a).gateOutput).nodeFlag = 1;

					errorList(a, 0);								// Output node of gate, controlling variable of Gate
				}

				else if (Gate.at(a).gateType == "NAND")
				{
					Node.at(Gate.at(a).gateOutput).nodeValue = !(Node.at(Gate.at(a).gateInput1).nodeValue & Node.at(Gate.at(a).gateInput2).nodeValue);
					Node.at(Gate.at(a).gateOutput).nodeFlag = 1;

					errorList(a, 0);								// Output node of gate, controlling variable of Gate
				}

				else if (Gate.at(a).gateType == "OR")
				{
					Node.at(Gate.at(a).gateOutput).nodeValue = Node.at(Gate.at(a).gateInput1).nodeValue | Node.at(Gate.at(a).gateInput2).nodeValue;
					Node.at(Gate.at(a).gateOutput).nodeFlag = 1;

					errorList(a, 1);								// Output node of gate, controlling variable of Gate
				}

				else if (Gate.at(a).gateType == "NOR")
				{
					Node.at(Gate.at(a).gateOutput).nodeValue = !(Node.at(Gate.at(a).gateInput1).nodeValue | Node.at(Gate.at(a).gateInput2).nodeValue);
					Node.at(Gate.at(a).gateOutput).nodeFlag = 1;

					errorList(a, 1);								// Output node of gate, controlling variable of Gate
				}

				checkNode(Gate.at(a).gateOutput);
			}
		}
	}

	// Propagates the values at every node
	void checkNode(int a)
	{
		for (int i = 0 ; i < Node.at(a).listofGates.size() ; i++)
		{
			Temp = Node.at(a).listofGates;
			if (Gate.at(Temp.at(i).gateNumber).gateFlag != 1)
				checkOutput(Temp.at(i).gateNumber);
		}
	}

	// Propagates faults from gate inputs to outputs based on the controlling values
	void errorList(int a, int c)
	{
		if (Node.at(Gate.at(a).gateInput1).nodeValue != c && Node.at(Gate.at(a).gateInput2).nodeValue != c)
		{
			Node.at(Gate.at(a).gateOutput).nodeErrorList.reserve(Node.at(Gate.at(a).gateOutput).nodeErrorList.size() + Node.at(Gate.at(a).gateInput1).nodeErrorList.size() + Node.at(Gate.at(a).gateInput2).nodeErrorList.size() + 1);
			Node.at(Gate.at(a).gateOutput).nodeErrorList.insert(Node.at(Gate.at(a).gateOutput).nodeErrorList.end(),Node.at(Gate.at(a).gateInput1).nodeErrorList.begin(),Node.at(Gate.at(a).gateInput1).nodeErrorList.end());
			Node.at(Gate.at(a).gateOutput).nodeErrorList.insert(Node.at(Gate.at(a).gateOutput).nodeErrorList.end(),Node.at(Gate.at(a).gateInput2).nodeErrorList.begin(),Node.at(Gate.at(a).gateInput2).nodeErrorList.end());

			sort(Node.at(Gate.at(a).gateOutput).nodeErrorList.begin(), Node.at(Gate.at(a).gateOutput).nodeErrorList.end());
			Node.at(Gate.at(a).gateOutput).nodeErrorList.erase(unique(Node.at(Gate.at(a).gateOutput).nodeErrorList.begin(), Node.at(Gate.at(a).gateOutput).nodeErrorList.end()), Node.at(Gate.at(a).gateOutput).nodeErrorList.end());

			if (Node.at(Gate.at(a).gateOutput).SAvalue0 > 0)
				if (Gate.at(a).gateOutput * 10 + Node.at(Gate.at(a).gateOutput).nodeValue != Node.at(Gate.at(a).gateOutput).SAvalue0)
					Node.at(Gate.at(a).gateOutput).nodeErrorList.push_back(Node.at(Gate.at(a).gateOutput).SAvalue0);

			if (Node.at(Gate.at(a).gateOutput).SAvalue1 > 0)
				if (Gate.at(a).gateOutput * 10 + Node.at(Gate.at(a).gateOutput).nodeValue != Node.at(Gate.at(a).gateOutput).SAvalue1)
					Node.at(Gate.at(a).gateOutput).nodeErrorList.push_back(Node.at(Gate.at(a).gateOutput).SAvalue1);
		}

		else if (Node.at(Gate.at(a).gateInput1).nodeValue == c && Node.at(Gate.at(a).gateInput2).nodeValue != c)
		{
			Node.at(Gate.at(a).gateOutput).nodeErrorList.reserve(Node.at(Gate.at(a).gateOutput).nodeErrorList.size() + Node.at(Gate.at(a).gateInput1).nodeErrorList.size() + 1);
			Node.at(Gate.at(a).gateOutput).nodeErrorList.insert(Node.at(Gate.at(a).gateOutput).nodeErrorList.end(),Node.at(Gate.at(a).gateInput1).nodeErrorList.begin(),Node.at(Gate.at(a).gateInput1).nodeErrorList.end());

			for (int i = 0; i < Node.at(Gate.at(a).gateInput2).nodeErrorList.size(); i++)
				Node.at(Gate.at(a).gateOutput).nodeErrorList.erase(std::remove(Node.at(Gate.at(a).gateOutput).nodeErrorList.begin(), Node.at(Gate.at(a).gateOutput).nodeErrorList.end(), Node.at(Gate.at(a).gateInput2).nodeErrorList.at(i)), Node.at(Gate.at(a).gateOutput).nodeErrorList.end());

			if (Node.at(Gate.at(a).gateOutput).SAvalue0 > 0)
				if (Gate.at(a).gateOutput * 10 + Node.at(Gate.at(a).gateOutput).nodeValue != Node.at(Gate.at(a).gateOutput).SAvalue0)
					Node.at(Gate.at(a).gateOutput).nodeErrorList.push_back(Node.at(Gate.at(a).gateOutput).SAvalue0);

			if (Node.at(Gate.at(a).gateOutput).SAvalue1 > 0)
				if (Gate.at(a).gateOutput * 10 + Node.at(Gate.at(a).gateOutput).nodeValue != Node.at(Gate.at(a).gateOutput).SAvalue1)
					Node.at(Gate.at(a).gateOutput).nodeErrorList.push_back(Node.at(Gate.at(a).gateOutput).SAvalue1);
		}

		else if (Node.at(Gate.at(a).gateInput1).nodeValue != c && Node.at(Gate.at(a).gateInput2).nodeValue == c)
		{
			Node.at(Gate.at(a).gateOutput).nodeErrorList.reserve(Node.at(Gate.at(a).gateOutput).nodeErrorList.size() + Node.at(Gate.at(a).gateInput2).nodeErrorList.size() + 1);
			Node.at(Gate.at(a).gateOutput).nodeErrorList.insert(Node.at(Gate.at(a).gateOutput).nodeErrorList.end(),Node.at(Gate.at(a).gateInput2).nodeErrorList.begin(),Node.at(Gate.at(a).gateInput2).nodeErrorList.end());

			for (int i = 0; i < Node.at(Gate.at(a).gateInput1).nodeErrorList.size(); i++)
				Node.at(Gate.at(a).gateOutput).nodeErrorList.erase(std::remove(Node.at(Gate.at(a).gateOutput).nodeErrorList.begin(), Node.at(Gate.at(a).gateOutput).nodeErrorList.end(), Node.at(Gate.at(a).gateInput1).nodeErrorList.at(i)), Node.at(Gate.at(a).gateOutput).nodeErrorList.end());

			if (Node.at(Gate.at(a).gateOutput).SAvalue0 > 0)
				if (Gate.at(a).gateOutput * 10 + Node.at(Gate.at(a).gateOutput).nodeValue != Node.at(Gate.at(a).gateOutput).SAvalue0)
					Node.at(Gate.at(a).gateOutput).nodeErrorList.push_back(Node.at(Gate.at(a).gateOutput).SAvalue0);

			if (Node.at(Gate.at(a).gateOutput).SAvalue1 > 0)
				if (Gate.at(a).gateOutput * 10 + Node.at(Gate.at(a).gateOutput).nodeValue != Node.at(Gate.at(a).gateOutput).SAvalue1)
					Node.at(Gate.at(a).gateOutput).nodeErrorList.push_back(Node.at(Gate.at(a).gateOutput).SAvalue1);
		}

		else
		{
			Node.at(Gate.at(a).gateOutput).nodeErrorList.reserve(Node.at(Gate.at(a).gateOutput).nodeErrorList.size() + Node.at(Gate.at(a).gateInput2).nodeErrorList.size() + 1);

			for (int i = 0; i < Node.at(Gate.at(a).gateInput2).nodeErrorList.size(); i++)
				for (int j = 0; j < Node.at(Gate.at(a).gateInput1).nodeErrorList.size(); j++)
					if (Node.at(Gate.at(a).gateInput2).nodeErrorList.at(i) == Node.at(Gate.at(a).gateInput1).nodeErrorList.at(j))
						Node.at(Gate.at(a).gateOutput).nodeErrorList.push_back(Node.at(Gate.at(a).gateInput1).nodeErrorList.at(j));

			if (Node.at(Gate.at(a).gateOutput).SAvalue0 > 0)
				if (Gate.at(a).gateOutput * 10 + Node.at(Gate.at(a).gateOutput).nodeValue != Node.at(Gate.at(a).gateOutput).SAvalue0)
					Node.at(Gate.at(a).gateOutput).nodeErrorList.push_back(Node.at(Gate.at(a).gateOutput).SAvalue0);

			if (Node.at(Gate.at(a).gateOutput).SAvalue1 > 0)
				if (Gate.at(a).gateOutput * 10 + Node.at(Gate.at(a).gateOutput).nodeValue != Node.at(Gate.at(a).gateOutput).SAvalue1)
					Node.at(Gate.at(a).gateOutput).nodeErrorList.push_back(Node.at(Gate.at(a).gateOutput).SAvalue1);
		}
	}

private:

}DFM_Func;

DeductiveFM::DeductiveFM()
{
}

DeductiveFM::~DeductiveFM()
{
}

int main()
{
	int flag1 = 1, flag2 = 1, flag3 = 1;
	int choice1 = 0, choice2 = 0;

	cout << endl << "What do you wish to do?";
	cout << endl << "1. Analyse by giving individual test vectors";
	cout << endl << "2. Obtain a specific fault coverage (in %)";
	cout << endl << endl << "Choose one of the options (1 or 2): ";
	cin  >> choice1;

	if (choice1 != 1 && choice1 != 2)
	{
		cout << endl << "Incorrect choice!";
		cout << endl << endl;
		return 0;
	}

	flag1 = DFM_Func.readCircuitFile(choice1);
	if (flag1 == 0)
	{
		cout << endl << "File not found!";
		cout << endl << endl;
		return 0;
	}
	else if (flag1 == 2)
	{
		cout << endl << "Incorrect no. of inputs";
		cout << endl << endl;
		return 0;
	}
	else if (flag1 == 3)
	{
		cout << endl << "Fault coverage cannot be greater than or equal to 100%";
		cout << endl << endl;
		return 0;
	}

	cout << endl << "At which nodes do you want the faults to be considered?";
	cout << endl << "1. Only Primary Inputs (PI)";
	cout << endl << "2. Only Primary Outputs (PO)";
	cout << endl << "3. Only Gate Outputs (GO)";
	cout << endl << "4. Both PIs and POs";
	cout << endl << "5. All three, PIs, POs and GOs";
	cout << endl << endl << "Choose one of the options (1/2/3/4/5): ";
	cin  >> choice2;

	// Assigning faults
	flag2 = DFM_Func.SAFaultList(choice2);

	if (flag2 == 0)
	{
		cout << endl << "Incorrect choice!";
		cout << endl << endl;
		return 0;
	}

	switch (choice1)
	{
	case 1:
		flag3 = DFM_Func.createOutputFile();
		if (flag3 == 0)
			cout << endl << "Incorrect no. of inputs";
		else if (flag3 == 2)
			cout << endl << "Incorrect choice";
		break;
	case 2:
		DFM_Func.createOutputFile_FC();
		break;
	default:
		cout << endl << "Incorrect choice!";
		break;
	}

	cout << endl << endl;

	return 0;
}
