#include <iostream>
#include <vector>
#include <cmath>

enum NodeType { input, hidden, output };

float activation(float z) {
	return 1 / (1 + exp(-z));
}

struct Node {
	NodeType type;
	float value;
	bool calculated;
};

struct Connection {
	int in;
	int out;
	float weight;
	bool enabled;
	int innov;
};

struct Genome {
	std::vector<Connection> connections;
	std::vector<Node> nodes;

	void addNode(NodeType type, float value = 0) {
		nodes.push_back({ type, value, false });
	}

	void addConnection(int in, int out, float weight) {
		connections.push_back({in, out, weight, true, 1});
	}

	void value(int id) {
		if(nodes[id].type == NodeType::input) return;
		if(nodes[id].calculated == true) return;

		float sum = 0;
		for(auto it = connections.begin(); it != connections.end(); it++) {
			if(it->out == id) {
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

	void printNodes() {
		for(Node n : nodes) {
			std::cout << "Node: " << (n.type == 0 ? "INPUT" : (n.type == 1 ? "HIDDEN" : "OUTPUT")) << ", Value: " << n.value << std::endl;
		}
	}

	void printConnections() {
		for(Connection c : connections) {
			std::cout << "Connection: " << c.weight << ", from " << c.in << " to " << c.out << std::endl;
		}
	}
};

int main() {
	std::cout << "Hello NEAT!" << std::endl;

	Genome genome;
	genome.addNode(NodeType::input, 0.5);
	genome.addNode(NodeType::input, 0.3);
	genome.addNode(NodeType::hidden);
	genome.addNode(NodeType::output);

	genome.addConnection(0, 2, 0.9);
	genome.addConnection(1, 2, 0.8);
	genome.addConnection(2, 3, 0.5);

	genome.calculate();

	genome.printNodes();
	genome.printConnections();

	return 0;
}
