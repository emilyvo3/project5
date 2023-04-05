#include <iostream>
#include <vector>
using namespace std;

class Node
{
    public:
        Node(Node_Type t); // constructor
        Node_Type type; // type of the node
        vector <bool> letter;
        int visited;
        vector <Edge*> adj;
        Edge *backedge; // pointer to the reverse edge (used in augumenting paths for max flow)
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
        int BFS();
        int canISpell(); // determine whether a given word can be spelled using the dice faces
        // int maxFlow();
        vector <int> spelling_ids; // optional- used to print; vector to store the IDs of the nodes used in the spelling of a word 
    private:
        void deleteHalfGraph(); // used in finding augumenting paths for max flow
        vector <Node*> nodes;
        int min_nodes;
};
 // define an enumeration to represent the different types of nodes in the graph
typedef enum
{
SOURCE, 
SINK, 
DICE, 
WORD
} Node_Type;
