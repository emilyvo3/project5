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
        Node_Type type; // type of the node (source, sink, word or dice)
        vector <bool> letter; // length 26 with letters contained in word set to 1
        int visited=0;
		int id;
        vector <class Edge*> adj; // adjacency list
		void Print();
        class Edge *backedge; // pointer to the reverse edge (used in augumenting paths for max flow)
};

class Edge
{
    public:
        class Node *to; // pointer to the node at the end of the edge
        class Node *from; // pointer to the node at the start of the edge
        Edge *reverse; // pointer to the reverse edge (augumenting paths); keep track of edge going other way
        int original; // original capacity of the edge; original weight per edge
        int residual; // residual capacity of the edge (updated during max flow calculation);
		int index; //helsp with adj vector in Nodes class
};

class Graph
{
    public:
        Graph(string dice_file); // constructor
        ~Graph(); // destructor
		Node* Get_Node(Node_Type t, string word, int i); // constructor
		Edge* Get_Edge(Node *from, Node *to, bool reverse_edge); // constructor
		Node* source;
		Node* sink;
        int BFS();
        int canISpell(); // determine whether a given word can be spelled using the dice faces by calling BFS()
        vector <int> spelling_ids; // optional- used to print; vector to store the IDs of the nodes used in the spelling of a word 
        void deleteHalfGraph(); // used in finding augumenting paths for max flow
        vector <Node*> Nodes;
		bool letterExist(Node* from, Node* to);
		vector<Node*> dice_nodes;
	    vector<Node*> words_nodes;
		int id, wordi;
		int min_nodes;
		int findDice(int wordid);
		void Print(string input);
};

// ddefine constructor for Node class that set its type, visited flag, and backedge
Node* Graph::Get_Node(Node_Type t, string word, int i)
{
	Node *n;
	n = new Node;
    n->type = t;
	n->name = word;
    n->visited = 0;
	n->id = i;
	n->letter = vector <bool> (26, false);
    n->backedge = NULL;
	return n;    
}

void Graph::Print(string input){
	for(int i = 0; i < (int) spelling_ids.size() - 1; i++){
		cout<<spelling_ids[i]<<",";
	}
	cout<<spelling_ids[(int) spelling_ids.size() - 1]<<": "<<input<<endl;
}

void Node::Print(){
	printf("Node %i: %s Edges to ", id, name.c_str());
	for(int i = 0; i < (int) adj.size(); i++){
		printf("%i ", adj[i]->to->id);
	}
}

Edge* Graph::Get_Edge(Node *from, Node *to, bool reverse_edge)
{
	Edge *e = new Edge;
    e->from = from; // set the "from" node
    e->to = to; // set the "to" node

	if(!reverse_edge){
	    e->residual = 0;
		e->original = 1; // set the residual capacity to be equal to the original capacity
	}
	else{
		e->residual = 0;
        e->original = 0; 
	}
	return e;
}

bool Graph::letterExist(Node* from, Node* to){
	int ind = from->name[0];
	if(to->letter[ind - 65]){
		return true;
	}
	else{
		return false;
	}
}

// define constructor for Graph class that sets its minimum number of nodes
Graph::Graph(string dice_file)
{
	id = 0;
	dice_nodes.clear();
	ifstream fin;
	fin.open(dice_file);

    // check if the files are open
    if (!fin.is_open()) {
        cerr << "Error: Failed to open " << dice_file << endl;
		exit(-1);
	}

    // print the contents of the Dice file
    Node *source = Get_Node(SOURCE, "Source", id);
	Nodes.push_back(source);
    id++;

	string input;
    while (fin >> input) {
		source = Get_Node(DICE, input, id);
		id++;
		dice_nodes.push_back(source);
		Nodes.push_back(source);
		
		for(int i = 0; i < (int)input.length(); i++){
			source->letter[(int)input.at(i) - 65] = true;
		}

    }
	
	// normal and reverse edges from source to dice nodes
    for (int i = 0; i < (int)dice_nodes.size(); i++)
    {
        Edge *edge = Get_Edge(Nodes[0], dice_nodes[i], 0);
        Edge *redge = Get_Edge(dice_nodes[i], Nodes[0], 1);
        edge->reverse = redge;
        redge->reverse = edge;

        Nodes[0]->adj.push_back(edge);
        dice_nodes[i]->adj.push_back(redge);
    }

	fin.close();
	min_nodes = id;
}

Graph::~Graph()
{
    /* deallocate nodes and edges */
    for (int i = 0; i < Nodes.size(); i++)
    {
        for (int j = 0; j < Nodes[i]->adj.size(); j++)
        {
            delete Nodes[i]->adj[j];
        }
        delete Nodes[i];
    }
}

int Graph::BFS()
{
    vector<Node*> frontier;
	for(int i = 0; i < (int)Nodes.size(); i++){
			Nodes[i]->visited = 0;
            Nodes[i]->backedge = NULL;
	}

		frontier.push_back(Nodes[0]);
//		cout<<"Adding in Frontier: "<<Nodes[0]->name<<endl;
		Node* v;
		while(!frontier.empty()) {
			v = frontier[0];
			v->visited = 1;

			frontier.erase(frontier.begin());

	        // add the neighbors of v to the frontier
		    for(int i = 0; i < v->adj.size(); i++) {
				Node* u = v->adj[i]->to;
				if(v->adj[i]->original == 1 && u->visited == 0){
					frontier.push_back(u);
					u->backedge = v->adj[i];
					if(u->type == SINK){
						return 1;
					}
				}	
	        }	
		}
	return 0;
}

int Graph::canISpell()
{
	int count = 0;
	while(BFS()) { // while the paths are available
        /* 
        - once BFS() returns with a path found, follow the backedges 
        from the sink to the source, while changing the original/residual 
        on both "normal" and "reverse" edges so that original = 0 and residual = 1
        on normal, and original = 1 and residual = 0 on the reverse
        - that way in BFS you only follow paths where original = 1 and doing this, 
        you will have choose a path you have already been on */
        Node *current = sink; // start from sink
        while (current->type != SOURCE) { // until the source has been reached
			Edge *bedge = current->backedge; // get backedge
				bedge->original = 0;
		        bedge->residual = 1;
				Edge *edge = bedge->reverse; // get original edge
				edge->original = 1;
		        edge->residual = 0;
			    current = edge->to; // move to the next node in the path
				
		}
    }
	
    /* once BFS() can no longer find new paths, check all the word nodes to see 
    if residual = 1 got TO the sink. if ALL do, you can spell the word */
    for (int i = 0; i < (int)words_nodes.size(); i++)
    {
		for(int j = 0; j < (int)words_nodes[i]->adj.size(); j++){
			if(words_nodes[i]->adj[j]->to->type == SINK){
				if(words_nodes[i]->adj[j]->residual == 0){
					return 0;
				}
			}
		}
    }

	//we'll be here if the word can be spelt
	for(int i = 0; i < (int)words_nodes.size(); i++){
		for(int j = 0; j < (int)words_nodes[i]->adj.size(); j++){
			if(words_nodes[i]->adj[j]->to->type == DICE){
				if(words_nodes[i]->adj[j]->original == 1){
					int id = words_nodes[i]->adj[j]->to->id;
					int diceid = findDice(id);
					spelling_ids.push_back(diceid);
				
				}
			}
		}
	}

    return 1; //If a word node has residual capacity of 1, it can be spelled */
}

int Graph::findDice(int wordid){
	for(int i = 0; i < (int)dice_nodes.size(); i++){
		if(dice_nodes[i]->id == wordid){
			return i;
		}
	}
	return 0;
}

void Graph::deleteHalfGraph()
{
    //delete the edges first that connect to the word nodes and sink
	//delete the word nodes and sink node
	for(int i = 0; i < min_nodes; i++){
		if(Nodes[i]->type == DICE){
			for(int j = (int)Nodes[i]->adj.size() - 1; j > 0; j--){
				if(Nodes[i]->adj[j]->to->type == WORD ){
					delete Nodes[i]->adj[j];	
					Nodes[i]->adj.erase(Nodes[i]->adj.begin() + j);
				}
			}		
		}
	}
	
	for (int i = ((int)Nodes.size() - 1); i > (min_nodes - 1); i--)
    {
        for (int j = (int)Nodes[i]->adj.size() - 1; j > 0; j--)
        {
            delete Nodes[i]->adj[j];
			Nodes[i]->adj.erase(Nodes[i]->adj.begin() + j);

        }
        delete Nodes[i];
    	Nodes.erase(Nodes.begin() + i);
    }

	words_nodes.clear();
}

int main(int argc, char *argv[])
{	
	ifstream fin;

    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <Dice file> <Words file>\n";
        return 1;
    }

	string dice_file, words_file;
	dice_file = argv[1];
	words_file = argv[2];

	fin.open(words_file);

	if (!fin.is_open())
	{
		cerr << "Error: Failed to open " << words_file << endl;
		//        return 1;
		exit(-1);
	}
	//make an array of string that holds the words from Words1.txt and individually give the while loop the string
	string input;// in1, in2;
	while (fin >> input) { 
		Graph g = Graph(dice_file);
	
		string charc;
		g.wordi = g.min_nodes;

		for (int i = 0; i < (int)input.length(); i++)
		{
			charc = input[i];
			Node* source = g.Get_Node(WORD, charc, g.wordi);
			g.wordi++;
			g.words_nodes.push_back(source);
			g.Nodes.push_back(source);
		}
		Node* source = g.Get_Node(SINK, "Sink", g.wordi);
		g.Nodes.push_back(source);

	// making edges between Dice nodes and Word nodes
		for (int i = 0; i < (int)g.words_nodes.size(); i++)
		{
			for (int j = 0; j < (int)g.dice_nodes.size(); j++)
			{
				if (g.letterExist(g.words_nodes[i], g.dice_nodes[j]))
				{
					Edge *edge = g.Get_Edge(g.dice_nodes[j], g.words_nodes[i], 0);
					Edge *redge = g.Get_Edge(g.words_nodes[i], g.dice_nodes[j], 1);
					edge->reverse = redge;
					redge->reverse = edge;
	
					g.dice_nodes[j]->adj.push_back(edge);
					g.words_nodes[i]->adj.push_back(redge);
				}
			}
		}	
	
	// normal and reverse edges from words nodes to sink
		for (int i = 0; i < (int)g.words_nodes.size(); i++)
		{
			Edge *edge = g.Get_Edge(g.words_nodes[i], g.Nodes[(int)g.Nodes.size() - 1], 0);
			Edge *redge = g.Get_Edge(g.Nodes[(int)g.Nodes.size() - 1], g.words_nodes[i], 1);

			edge->reverse = redge;
			redge->reverse = edge;

			g.words_nodes[i]->adj.push_back(edge);
			g.Nodes[(int)g.Nodes.size() - 1]->adj.push_back(redge);
		}

		g.source = g.Nodes[0];
		g.sink = g.Nodes[(int)g.Nodes.size() - 1];

		int can_spell = g.canISpell();
        /* testing */
		    if (can_spell == 1) { // residual capacity of 1
				g.Print(input);
		    }
			else {
				cout << "Cannot spell " << input << endl;
			}
		g.deleteHalfGraph();
	}
	fin.close();
    return 0;
}


