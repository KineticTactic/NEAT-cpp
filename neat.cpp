#include <iostream>
#include <vector>
#include <cmath>
#include <random>

std::random_device rd; ///TODO: Initialize only once.
std::mt19937 mt(rd());
std::uniform_real_distribution<double> dist100(0.0, 100.0);
std::uniform_real_distribution<double> dist1(-1.0, 1.0);

enum NodeType { input, hidden, output, bias };

float activation(double z) {
	return 1 / (1 + exp(-z));
}

struct Node {
	NodeType type;
	double value;
	bool calculated;
};

struct Connection {
	int in
	int out;
	double weight;
	bool enabled;
	int innov;
	d
};

struct Genome {
	std::vector<Connection> connections;
	std::vector<Node> nodes;

	int addNode(NodeType type, double value = 0) {
		nodes.push_back({ type, value, false });
		return nodes.size() - 1;
	}

	int addConnection(int in, int out, double weight) {
		connections.push_back({in, out, weight, true, 1});
		return connections.size() - 1;
	}

	void value(int id) {
		if(nodes[id].type == NodeType::input || nodes[id].type == NodeType::bias) return;
		if(nodes[id].calculated == true) return;

		double sum = 0;
		for(auto it = connections.begin(); it != connections.end(); it++) {
			if(it-> enabled && it->out == id) {
				value(it->in);
				sum += it->weight * nodes[it->in].value;
			}
		}

		nodes[id].value = activation(sum);
		nodes[id].calculated = true;
	}

	void calculate() {
		for(int i = 0; i < nodes.size(); i++) {
			nodes[i].calculated = false;
		}

		for(int i = 0; i < nodes.size(); i++) {
			if (nodes[i].type == NodeType::output) {
				value(i);
			}
		}
	}

	void mutateWeights() {
		//std::cout << "Mutating weights" << std::endl;
		for (int i = 0; i < connections.size(); i++) {
			connections[i].weight += dist1(mt);
		}
	}

	void addRandomConnection() {
		//std::cout << "Adding Random Connection" << std::endl;
		
		///TODO: Optimise
		std::vector<int> inIndices;
		for(int i = 0; i < nodes.size(); i++) {
			if (nodes[i].type != NodeType::output) inIndices.push_back(i);
		}

		std::uniform_int_distribution<std::size_t> randDist1(0, inIndices.size() - 1);
		int in = inIndices[randDist1(mt)];

		std::vector<int>outIndices;
		for(int i = 0; i < nodes.size(); i++) {
			if(nodes[i].type != NodeType::input && i != in && nodes[i].type != NodeType::bias) outIndices.push_back(i);
		}

		std::uniform_int_distribution<std::size_t> randDist2(0, outIndices.size() - 1);
		int out = outIndices[randDist2(mt)];

		//std::cout << "Adding random connection from " << in << " to " << out << "\n";

		addConnection(in, out, dist1(mt));
	}

	void addRandomNode() {
		//std::cout << "Adding Random Node" << std::endl;

		std::uniform_int_distribution<std::size_t> randDist(0, connections.size() - 1);
		int index = randDist(mt);

		connections[index].enabled = false;
		int nodeIndex = addNode(NodeType::hidden);
		addConnection(connections[index].in, nodeIndex, 1.0);
		addConnection(nodeIndex, connections[index].out, connections[index].weight);

		//std::cout << "Added node between " << connections[index].in << " and " << connections[index].out << std::endl;
	}

	void mutate() {
		double random = dist100(mt);

		if(random < 50.0) {
			mutateWeights();
		} else if (random < 75.0) {
			addRandomConnection();
		} else {
			addRandomNode();
		}
	}

	void printNodes() {
		for(Node n : nodes) {
			std::cout << "Node: " << (n.type == 0 ? "INPUT" : (n.type == 1 ? "HIDDEN" : (n.type == 2 ? "OUTPUT" : "BIAS") )) << ", Value: " << n.value << std::endl;
		}
	}

	void printConnections() {
		for(Connection c : connections) {
			std::cout << "Connection: " << c.weight << ", from " << c.in << " to " << c.out << std::endl;
		}
	}

	void print() {
		printNodes();
		printConnections();
	}
};

double evaluateXOR(Genome& genome) {
	double fitness = 0.0;

	std::vector<std::tuple<double, double, double>> xorCases = {
		{0.0, 0.0, 0.0},
		{0.0, 1.0, 1.0},
		{1.0, 0.0, 1.0},
		{1.0, 1.0, 0.0}
	};

	for(const auto& [in1, in2, expected] : xorCases) {
		genome.nodes[1].value = in1;
		genome.nodes[2].value = in2;
		genome.calculate();
		double out = genome.nodes[genome.nodes.size() - 1].value;
		double error = expected - out;
		fitness += 1.0 - (error * error);
	}
	return fitness;
}

int main() {
	std::vector<Genome> genomes(100);

	double maxFitness = 0.0;
	Genome fittest;
	
	for(Genome genome : genomes) {
		genome.addNode(NodeType::bias, 1.0); // bias
		genome.addNode(NodeType::input, 0.5);
		genome.addNode(NodeType::input, 0.3);
		genome.addNode(NodeType::hidden);
		genome.addNode(NodeType::output);

		genome.addConnection(0, 4, dist1(mt) * 5);
		genome.addConnection(1, 3, dist1(mt) * 5);
		genome.addConnection(2, 3, dist1(mt) * 5);
		genome.addConnection(3, 4, dist1(mt) * 5);

		genome.calculate();

//		genome.printNodes();
//		genome.printConnections();
//
		double fitness = evaluateXOR(genome); 
		if(maxFitness < fitness ) {
			maxFitness = fitness;
			fittest = genome;
		}
//		std::cout << "Fitness: " << fitness << std::endl;
	}

	std::cout << std::endl;
	std::cout << evaluateXOR(fittest) << std::endl;
	

	for(int i = 0; i < 10; i++) {
		for (int i = 0; i < genomes.size(); i++) {
			genomes[i] = fittest;
			if(i != 0) genomes[i].mutate();
			double fitness = evaluateXOR(genomes[i]); 
			if(maxFitness < fitness ) {
				maxFitness = fitness;
				fittest = genomes[i];
			}
//			std::cout << "Fitness: " << fitness << std::endl;
		}
		std::cout << "Max Fitness: " << maxFitness << ", nodes: " << fittest.nodes.size() << std::endl;
	}


	return 0;
}
