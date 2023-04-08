#include <iostream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;

 // define an enumeration to represent the different types of nodes in the graph
typedef enum
{
SOURCE, 
SINK, 
DICE, 
WORD
} Node_Type;

class Node
{
    public:
        Node(Node_Type t); // constructor
        Node_Type type; // type of the node
        vector <bool> letter;
        int visited;
        vector <class Edge*> adj;
        class Edge *backedge; // pointer to the reverse edge (used in augumenting paths for max flow)
};

class Edge
{
    public:
        Edge(Node *from, Node *to, int original); // constructor
        class Node *to; // pointer to the node at the end of the edge
        class Node *from; // pointer to the node at the start of the edge
        Edge *reverse; // pointer to the reverse edge (augumenting paths)
        int original; // original capacity of the edge
        int residual; // residual capacity of the edge (updated during max flow calculation)
};

class Graph
{
    public:
        Graph(int min_nodes); // constructor
        ~Graph(); // destructor
        // void addNode(Node_Type type);
        // void addEdge(Node *from, Node to, int capacity);
        void BFS(Node* node);
        int canISpell(); // determine whether a given word can be spelled using the dice faces by calling BFS()
        // int maxFlow();
        vector <int> spelling_ids; // optional- used to print; vector to store the IDs of the nodes used in the spelling of a word 
    private:
        void deleteHalfGraph(); // used in finding augumenting paths for max flow
        vector <Node*> nodes;
        int min_nodes;
};

Node::Node(Node_Type t)
{
    type = t;
    visited = 0;
    backedge = NULL;
}

Edge::Edge(Node *from, Node *to, int original)
{

}

Graph::Graph(int min_nodes)
{

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

int Graph::canISpell()
{

}

void Graph::deleteHalfGraph()
{
    
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <Dice file> <Words file>\n";
        return 1;
    }
    
    ifstream dice_file(argv[1]);
    ifstream words_file(argv[2]);
    
    // check if the files are open
    if (!dice_file.is_open()) {
        cerr << "Error: Failed to open " << argv[1] << endl;
        return 1;
    }
    if (!words_file.is_open()) {
        cerr << "Error: Failed to open " << argv[2] << endl;
        return 1;
    }
    
    // print the contents of the Dice file
    //cout << "Contents of " << argv[1] << ":" << std::endl;
    string line;
    while (getline(dice_file, line)) {
        //cout << line << endl;
    }
    //cout << endl;
    
    // print the contents of the Words file
    //cout << "Contents of " << argv[2] << ":" << endl;
    while (getline(words_file, line)) {
        //cout << line << endl;
    }
    //cout << endl;
    
    // close the files
    dice_file.close();
    words_file.close();
    
    return 0;
}


