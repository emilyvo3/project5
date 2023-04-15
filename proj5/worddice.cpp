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
//        Node(Node_Type t, string word); // constructor
        Node_Type type; // type of the node (source, sink, word or dice)
        vector <bool> letter; // length 26 with letters contained in word set to 1
        int visited;
		int id;
        vector <class Edge*> adj; // adjacency list
		void Print();
//		friend ostream& operator<<(ostream& os, const Node& node);
        class Edge *backedge; // pointer to the reverse edge (used in augumenting paths for max flow)
};

class Edge
{
    public:
//        Edge(Node *from, Node *to, int original); // constructor
        class Node *to; // pointer to the node at the end of the edge
        class Node *from; // pointer to the node at the start of the edge
        Edge *reverse; // pointer to the reverse edge (augumenting paths); keep track of edge going other way
        int original; // original capacity of the edge; original weight per edge
        int residual; // residual capacity of the edge (updated during max flow calculation);

		// allows for updated weight during Edmonds-Karp
		int index; //helsp with adj vector in Nodes class
};

class Graph
{
    public:
        Graph(string dice_file, string words_file); // constructor
        ~Graph(); // destructor
		Node* Get_Node(Node_Type t, string word, int i); // constructor
		Edge* Get_Edge(Node *from, Node *to, bool reverse_edge); // constructor

        int BFS(Node* node);
        int canISpell(int n); // determine whether a given word can be spelled using the dice faces by calling BFS()
        // int maxFlow();
        vector <int> spelling_ids; // optional- used to print; vector to store the IDs of the nodes used in the spelling of a word 
        void deleteHalfGraph(); // used in finding augumenting paths for max flow
        vector <Node*> Nodes;
		bool letterExist(Node* from, Node* to);
		vector<Node*> dice_nodes;
	    vector<Node*> words_nodes;

		int min_nodes;
};

// ddefine constructor for Node class that set its type, visited flag, and backedge
Node* Graph::Get_Node(Node_Type t, string word, int i)
{
	Node *n;


	n = new Node;
    n->type = t;
	n->name = word;
//	Nodes.push_back(n);
    n->visited = 0;
	n->id = i;
	n->letter = vector <bool> (26, false);
    n->backedge = NULL;
	return n;    
}

void Node::Print(){
	printf("Node %i: %s Edges to ", id, name.c_str());
	for(int i = 0; i < (int) adj.size(); i++){
		printf("%i ", adj[i]->to->id);
	}
	cout<<endl;
}

//Temporary code: DELETE LATER
/*ostreaif(reverse_edge){
        e->residual = 0;
        e->original = 1; // set the residual capacity to be equal to the original capacity
    }
    else{
    //    e->reverse = NULL; // set the reverse edge to null
        e->residual = 0;
        e->original = 0;
    }
m& operator<<(ostream& os, const Node& node) {
    os << node.name;
    return os;
}*/



Edge* Graph::Get_Edge(Node *from, Node *to, bool reverse_edge)
{
	Edge *e = new Edge;
    e->from = from; // set the "from" node
    e->to = to; // set the "to" node

	if(reverse_edge){
	    e->residual = 0;
		e->original = 1; // set the residual capacity to be equal to the original capacity
	}
	else{
	//    e->reverse = NULL; // set the reverse edge to null
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
Graph::Graph(string dice_file, string words_file)
{
    
	int id = 0;
	int wordi;
/*	vector<Nodes*> dice_nodes;
	vector<Nodes*> words_nodes;*/
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


	fin.close();
  
	min_nodes = id;


	fin.open(words_file);
    if (!fin.is_open()) {
        cerr << "Error: Failed to open " << words_file << endl;
//        return 1;
		exit(-1);
    }

    // print the contents of the Words file
    
	wordi = id;
//	while (fin >> input) {
		fin>>input;
		string charc;
		for(int i = 0; i < input.length(); i++){	
			charc = input[i];
			source = Get_Node(WORD, charc, wordi);
			wordi++;
			words_nodes.push_back(source);
			Nodes.push_back(source);



		}
		source = Get_Node(SINK, "Sink", wordi);
		Nodes.push_back(source);
		
		//making edges

		//normal and reverse edges from source to dice nodes
	    for(int i = 0; i < (int)dice_nodes.size(); i++){
		    Edge *edge = Get_Edge(Nodes[0], dice_nodes[i], 0);
			Edge *redge = Get_Edge(dice_nodes[i], Nodes[0], 1);
			edge->reverse = redge;
			redge->reverse = edge;

			Nodes[0]->adj.push_back(edge);
			dice_nodes[i]->adj.push_back(redge);

	    }


		//making edges between Dice nodes and Word nodes
		//still incomplete: change the from and to while calling edges
		for(int i = 0; i < (int)words_nodes.size(); i++){
			for(int j = 0; j < (int)dice_nodes.size(); j++){
				if(letterExist(words_nodes[i], dice_nodes[j])){
					Edge *edge = Get_Edge(dice_nodes[j], words_nodes[i], 0);
			        Edge *redge = Get_Edge(words_nodes[i], dice_nodes[j], 1);
			        edge->reverse = redge;
                    redge->reverse = edge;

                    dice_nodes[j]->adj.push_back(edge);
                    words_nodes[i]->adj.push_back(redge);
				}
			}	
		}

		//normal and reverse edges from words nodes to sink
        for(int i = 0; i < (int)words_nodes.size(); i++){
            Edge *edge = Get_Edge(words_nodes[i], Nodes[(int)Nodes.size() - 1], 0);
			Edge *redge = Get_Edge(Nodes[(int)Nodes.size() - 1], words_nodes[i], 1);
			
			edge->reverse = redge;
            redge->reverse = edge;

			words_nodes[i]->adj.push_back(edge);
			Nodes[(int)Nodes.size() - 1]->adj.push_back(redge);

        }
		
		//work on deletion before testing the edges

  //  }
    
    // close the files
    fin.close();
	
	for(int i = 0 ; i < (int)Nodes.size(); i++){
		Nodes[i]->Print();
	}


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

int Graph::BFS(Node* node)
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

	return 1;
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
    //delete the edges first that connect to the word nodes and sink
	//delete the word nodes and sink node
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
    
    Graph *g = new Graph(dice_file, words_file);
    

	//Testing
	//Node node(DICE, "A");
	//std::cout << node << std::endl;



    return 0;
}


