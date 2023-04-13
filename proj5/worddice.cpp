#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

enum Node_Type {
    SOURCE, SINK, DICE, WORD
};

class Node
{
    public:
		string name;
        Node(Node_Type t, string word); // constructor
        Node_Type type; // type of the node (source, sink, word or dice)
        vector <bool> letter; // length 26 with letters contained in word set to 1
        int visited;
		int id;//node id
        vector <class Edge*> adj; // adjacency list
//		friend ostream& operator<<(ostream& os, const Node& node);
        class Edge *backedge; // pointer to the reverse edge (used in augumenting paths for max flow)
};

class Edge
{
    public:
        Edge(Node *from, Node *to, int original); // constructor
        class Node *to; // pointer to the node at the end of the edge
        class Node *from; // pointer to the node at the start of the edge
        Edge *reverse; // pointer to the reverse edge (augumenting paths); keep track of edge going other way
        int original; // original capacity of the edge; original weight per edge
        int residual; // residual capacity of the edge (updated during max flow calculation);
                        // allows for updated weight during Edmonds-Karp
};

class Graph
{
    public:
        Graph(string dice_file, string words_file); // constructor
        ~Graph(); // destructor
        // void addNode(Node_Type type);
        // void addEdge(Node *from, Node to, int capacity);
        void BFS(Node* node);
        int canISpell(int n); // determine whether a given word can be spelled using the dice faces by calling BFS()
        // int maxFlow();
        vector <int> spelling_ids; // optional- used to print; vector to store the IDs of the nodes used in the spelling of a word 
    private:
        void deleteHalfGraph(); // used in finding augumenting paths for max flow
        vector <Node*> nodes;
        int min_nodes;
};

// ddefine constructor for Node class that set its type, visited flag, and backedge
Node::Node(Node_Type t, string word)
{
    type = t;
	name = word;
    visited = 0;
    backedge = NULL;
}


//Temporary code: DELETE LATER
/*ostream& operator<<(ostream& os, const Node& node) {
    os << node.name;
    return os;
}*/


// define constructor for Edge class that sets its from, to, original and residual capacity,
// and reverse edge
Edge::Edge(Node *from, Node *to)
{
	Edge *e = new Edge;
    e->from = from; // set the "from" node
    e->to = to; // set the "to" node
    e->residual = 0;
	e->original = 1;; // set the residual capacity to be equal to the original capacity
    e->reverse = NULL; // set the reverse edge to null

	return e;
}

// define constructor for Graph class that sets its minimum number of nodes
Graph::Graph(string dice_file, string words_file)
{
    
	vector<Nodes*> dice_nodes;
	vector<NOdes*> words_nodes;
	this->min_nodes = min_nodes;
//    	Graph g = new Graph();
	ifstream fin;

  /*  if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <Dice file> <Words file>\n";
        return 1;
    }

	string dice_file, words_file;
	dice_file = argv[1];
	words_file = argv[2];*/
    
	fin.open(dice_file);


    // check if the files are open
    if (!fin.is_open()) {
        cerr << "Error: Failed to open " << dice_file << endl;
//        return 1;
		exit(-1);
	}

    // print the contents of the Dice file
    Node* source = new Node(SOURCE, "Source");
	nodes.push_back(source);
    
	string input;
    while (fin >> input) {
		Node* source = new Node(DICE, input);
		dice_nodes.push_back(source);
		nodes.push_back(source);


    }


	fin.close();
  
	fin.open(words_file);
    if (!fin.is_open()) {
        cerr << "Error: Failed to open " << words_file << endl;
//        return 1;
		exit(-1);
    }

    // print the contents of the Words file
    
	
	while (fin >> input) {
		string charc;
		for(int i = 0; i < input.length(); i++){	
			charc = input[i];
			Node* source = new Node(WORD, charc);
			words_nodes.push_back(source);
			nodes.push_back(source);



		}
		Node* source = new Node(SINK, "Sink");
		nodes.push_back(source);


    }
    
    // close the files
    fin.close();

	Edge *edge, *redge;

	for(int i = 0; i < (int)dice_nodes.size(); i++){
		edge = Edge(nodes[0], dice_nodes[i]);
	}


}

Graph::~Graph()
{
    /* deallocate nodes and edges */
    for (int i = 0; i < nodes.size(); i++)
    {
        for (int j = 0; j < nodes[i]->adj.size(); j++)
        {
            delete nodes[i]->adj[j];
        }
        delete nodes[i];
    }
}

void Graph::BFS(Node* node)
{
    vector<Node*> frontier;
    frontier.push_back(node);

    while(!frontier.empty()) {
        Node* v = frontier[0];
        frontier.erase(frontier.begin());

        // if node has already been visited, skip it
        if(v->visited > 0) {
            continue;
        }

        // process node v
        v->visited = 1;

        // add the neighbors of v to the frontier
        for(int i = 0; i < v->adj.size(); i++) {
            Node* u = v->adj[i]->to;
            frontier.push_back(u);
        }
    }
}

int Graph::canISpell(int original)
{
//     while() { // while the paths are available
//        BFS();
        /* - once BFS() returns with a path found, follow the backedges 
        from the sink to the source, while changing the original/residual 
        on both "normal" and "reverse" edges so that original = 0 and residual = 1
        on normal, and original = 1 and residual = 0 on the reverse
        - that way in BFS you only follow paths where original = 1 and doing this, 
        you will have choose a path you have already been on */
  //   }

    /* - once BFS() can no longer find new paths, check all the word nodes to see 
    if residual = 1 got TO the sink. if ALL do, you can spell the word */
    /*if() {

        return 1; // If a word node has residual capacity of 1, it can be spelled
    }*/
    
    return 0; // If none of the word nodes can be spelled, the word cannot be spelled
}

void Graph::deleteHalfGraph()
{
    
}

int main(int argc, char *argv[])
{	
//	Graph g = new Graph();
	ifstream fin;

    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <Dice file> <Words file>\n";
        return 1;
    }

    //dice_file.open();
/*    ifstream dice_file(argv[1]);
    ifstream words_file(argv[2]);*/
	string dice_file, words_file;
	dice_file = argv[1];
	words_file = argv[2];
    
    Graph* g = new Graph(dice_file, words_file);
    

	//Testing
	//Node node(DICE, "A");
	//std::cout << node << std::endl;



    return 0;
}


