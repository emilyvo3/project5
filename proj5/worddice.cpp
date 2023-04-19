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
        int visited=0;
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
        Graph(string dice_file); // constructor
        ~Graph(); // destructor
		Node* Get_Node(Node_Type t, string word, int i); // constructor
		Edge* Get_Edge(Node *from, Node *to, bool reverse_edge); // constructor
		Node* source;
		Node* sink;
        int BFS();
        int canISpell(); // determine whether a given word can be spelled using the dice faces by calling BFS()
        // int maxFlow();
        vector <int> spelling_ids; // optional- used to print; vector to store the IDs of the nodes used in the spelling of a word 
        void deleteHalfGraph(); // used in finding augumenting paths for max flow
        vector <Node*> Nodes;
		bool letterExist(Node* from, Node* to);
		vector<Node*> dice_nodes;
	    vector<Node*> words_nodes;
		int id, wordi;
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
/*	Graph g = Graph(input)
	stringstream ss(input);
	string token;
	while (getline(ss, token, '.')) {
		g.spelling_ids.push_back(stoi(token));
	}
	for (int i = 0; i < g.spelling_ids.size(); i++) { 
		cout << g.spelling_ids[i] << ",";
	}
	cout << " " << name << endl;*/
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

	if(!reverse_edge){
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
Graph::Graph(string dice_file)
{
	id = 0;
	dice_nodes.clear();
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
//	wordi = id;

	//making edges

    
//	source = Nodes[0];
//	sink = Nodes[(int)Nodes.size() - 1];

/*	bool res = true;
	do{
		if(BFS()){
			res = true;
		}

		else{
			res = false;
		}
			
		
	}while(res);*/
/*	for(int i = 0 ; i < (int)Nodes.size(); i++){
		Nodes[i]->Print();
//		cout<<endl;
	}
	cout<<endl;*/



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
		//if(Nodes[i]->visited != 0){
			Nodes[i]->visited = 0;
		//}
		//if(Nodes[i]->backedge != NULL){
            Nodes[i]->backedge = NULL;
        //}
	}

		frontier.push_back(Nodes[0]);
//		cout<<"Adding in Frontier: "<<Nodes[0]->name<<endl;
		Node* v;
		while(!frontier.empty()) {
			v = frontier[0];
			v->visited = 1;
	//		 cout<<"Currently on: "<<v->name<<endl;

			frontier.erase(frontier.begin());

	        // if node has already been visited, skip it
/*		    if(v->visited > 0) {
			    continue;
	        }*/
			//cout << "about to set as visited" << endl;
		    // process node v
//			v->visited = 1;
//			cout << "visited " << v->visited << endl;

	        // add the neighbors of v to the frontier
		    for(int i = 0; i < v->adj.size(); i++) {
				Node* u = v->adj[i]->to;
				if(v->adj[i]->original == 1 && u->visited == 0){
					frontier.push_back(u);
//					u->visited = 1;
//					cout<<"Adding in Frontier: "<<u->name<<" ";
	
					u->backedge = v->adj[i];
//					v = u;
					if(u->type == SINK){
//						cout<<"Would return 1 here"<<endl;
						return 1;
					}
				}	
	        }	
//			cout<<endl;
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
  //   }
		count++;
//		cout<<"In the while loop"<<endl;
        Node *current = sink; // start from sink
//		cout<<"set current node to sink"<<endl;
        while (current->type != SOURCE) { // until the source has been reached
//			cout<<"Getting back edge now"<<endl;
			
			Edge *bedge = current->backedge; // get backedge
            //if(){
//				cout<<"Changing back edge values"<<endl;
//				cout<<"Backedge of "<<current->name<<" is "<<current->backedge->to->name<<endl; 
				bedge->original = 0;
		        bedge->residual = 1;

//				cout<<"Setting current edge to "<<bedge->to->name<<endl;
  //              current = bedge->to; // move to the next node in the path

	//		}	
			
	//	else{
//				cout<<"Getting original edge"<<endl;
            
				Edge *edge = bedge->reverse; // get original edge
//				cout<<"CHanging original values"<<endl;
//				cout<<"reverse edge of Backedge "<<current->backedge->to->name<<" is "<<bedge->reverse->to->name<<endl;


				edge->original = 1;
		        edge->residual = 0;
			
//				cout<<"Setting current edge to "<<edge->to->name<<endl;
			    current = edge->to; // move to the next node in the path
				
		}
    }

    /* once BFS() can no longer find new paths, check all the word nodes to see 
    if residual = 1 got TO the sink. if ALL do, you can spell the word */
    for (int i = 0; i < (int)sink->adj.size(); i++)
    {
        //if (words_nodes[i]->visited != words_nodes[i]->visited)
		if (sink->adj[i]->residual == 0)
        {
			//cout<<"Reverse edge of sink is 0: "<<sink->adj[i]->residual<<endl;
			return 0;
        }
		else {
			continue;
		}
    }
    return 1; //If a word node has residual capacity of 1, it can be spelled */

/*		if(count == (int)words_nodes.size()){
			return 1;
		}
		else{
			return 0;
		}
    */
}

void Graph::deleteHalfGraph()
{
    //delete the edges first that connect to the word nodes and sink
	//delete the word nodes and sink node
	
	for(int i = 0; i < min_nodes; i++){
		if(Nodes[i]->type == DICE){
			for(int j = 0; j < (int)Nodes[i]->adj.size(); j++){
				if(Nodes[i]->adj[j]->to->type == WORD){
					delete Nodes[i]->adj[j];	
					Nodes[i]->adj.erase(Nodes[i]->adj.begin() + j);
				}
			}		
		}
	}
	
	for (int i = ((int)Nodes.size() - 1); i > (min_nodes - 1); i--)
    {
        for (int j = 0; j < (int)Nodes[i]->adj.size(); j++)
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
    
    //Graph *g = new Graph(dice_file, words_file);
	//cout<<"Before Graph constructor"<<endl;
	Graph g = Graph(dice_file);

	fin.open(words_file);
	g.words_nodes.clear();

	if (!fin.is_open())
	{
		cerr << "Error: Failed to open " << words_file << endl;
		//        return 1;
		exit(-1);
	}

	/*// normal and reverse edges from source to dice nodes
	for (int i = 0; i < (int)g.dice_nodes.size(); i++)
	{
		Edge *edge = g.Get_Edge(g.Nodes[0], g.dice_nodes[i], 0);
		Edge *redge = g.Get_Edge(g.dice_nodes[i], g.Nodes[0], 1);
		edge->reverse = redge;
		redge->reverse = edge;

		g.Nodes[0]->adj.push_back(edge);
		g.dice_nodes[i]->adj.push_back(redge);
	}*/

	// print the contents of the Words file

	//	wordi = id;
	//	while (fin >> input) {
	string input;
	while (fin >> input) { 
//		cout<<"hi"<<endl;
		g.words_nodes.clear();
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

	// making edges

	// normal and reverse edges from source to dice nodes
	/*	    for(int i = 0; i < (int)dice_nodes.size(); i++){
				Edge *edge = Get_Edge(Nodes[0], dice_nodes[i], 0);
				Edge *redge = Get_Edge(dice_nodes[i], Nodes[0], 1);
				edge->reverse = redge;
				redge->reverse = edge;

				Nodes[0]->adj.push_back(edge);
				dice_nodes[i]->adj.push_back(redge);

			}*/

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

			/*for(int i = 0 ; i < (int)g.Nodes.size(); i++){
				g.Nodes[i]->Print();
	//		    cout<<endl;
			}
			cout<<endl;*/
	// bool bfs;
	// bfs = BFS();

	// work on deletion before testing the edges
	/*	deleteHalfGraph();*/ // do this in main
	//    }

	// close the files
	
//		fin.close();
/*		for(int i = 0 ; i < (int)g.Nodes.size(); i++){
		g.Nodes[i]->Print();
		cout<<endl;
	}
	cout<<endl;*/
	// cout<<"after graph created but before calling canISpell()"<<endl;

	// testing
	
		g.source = g.Nodes[0];
		g.sink = g.Nodes[(int)g.Nodes.size() - 1];
		
/*		for(int i = 0 ; i < (int)g.Nodes.size(); i++){
	        g.Nodes[i]->Print();
		    cout<<endl;
	    }*/


		int can_spell1 = g.canISpell();
	//int can_spell2 = g.canISpell();
	//int can_spell3 = g.canISpell();*/
	//int result = g.canISpell();
    //cout << "result: " << result << endl;
        /* testing */
		    if (can_spell1 == 1) { // residual capacity of 1
	            cout << "Can spell " << input << endl;
	
		    }
			else {
				cout << "Cannot spell " << input << endl;
			}
		g.deleteHalfGraph();
		// Testing
        // Node node(DICE, "A");
        // std::cout << node << std::endl;
	}
	fin.close();
    return 0;
}


