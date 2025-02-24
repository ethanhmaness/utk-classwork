#include <string>
#include <cstdio>
#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <queue>

/*
  worddice.cpp
  Ethan Maness
  April 24, 2022
  
  Reads in two filenames -- one containing a list of sides of letter dice and the other containing a list of words -- and determines which words can be spelled from some combination of the dice (and what that combination is).
  And it does it fast using a bunch of OOP stuff that Plank always whines about, just because I can.
*/

using namespace std;

typedef vector<bool> Die;

//Returns vector with all strings extracted from the given file
vector<string> StringVectorFromFile(char* filename){
  
  ifstream ifs;
  string temp;
  vector<string> vec;
  
  ifs.open(filename);
  if(!(ifs.is_open())){
    cerr << "Cannot open " << filename << endl;
    return vec;
  }
  while(ifs >> temp){
    vec.push_back(temp);
  }
  return vec;
  
}

//The graph used to solve the problem. The node values are stored here, since the nodes can have values of different data types or no data at all
class WordDiceGraph{

  public:
  
    class Node;
    class Edge;
  
    //Each dice has a size-256 bool vector indicating which chars the dice has
    vector< Die > dice;
    
    //Vector of words
    vector<string> words;
    size_t max_word_size;
    size_t current_word_index;
    
    //Vector of nodes, including source and sink
    vector<Node*> nodes;
    
    //important node indices
    size_t source_index;
    size_t sink_index;
    size_t dice_start_index;
    size_t dice_end_index;
    size_t word_start_index;
    size_t word_end_index;
    
    //constructor
    WordDiceGraph(vector< Die > dice, vector<string> wordstrings);
    
    //solver methods
    void SetWord(size_t word_index);
    vector<size_t> Spell();
    vector< pair<size_t, char> > Restricted_BFS(const vector< vector<char> > &availableEdges);
    
    //utility methods
    static vector< Die > DiceFromStrings(const vector<string> &dicestrings);
    static Die DieFromString(const string &s);
    string NodeVal(size_t node_index, size_t word_index) const;
    void Print();

};

//Nodes of the WordDiceGraph (note -- the actual value of the node isn't stored here, but in the graph)
class WordDiceGraph::Node {
  public:
    /*
      's' - source
      't' - sink
      'd' - dice
      'w' - word
    */
    char type;
    
    //For dice or word nodes, indicates the index of the node's value in diceflags or words (not used for source or sink nodes)
    size_t index;
    
    //Index of the node in WordDiceGraph.nodes
    size_t node_index;
    
    //Outgoing edges from this node
    vector<Edge*> edges;
    vector<Edge*> reverse_edges;
};

//Edges of the WordDiceGraph
class WordDiceGraph::Edge{
  public:
    Node* from;
    Node* to;
    size_t edge_index;
    Edge(Node* from, Node* to,size_t edge_index);
    Edge* ReverseEdge();
};

//Simply constructs an edge
WordDiceGraph::Edge::Edge(Node* from, Node* to, size_t edge_index) : from(from), to(to), edge_index(edge_index) {}

//Finds an edge that points opposite to this edge, if it exists
WordDiceGraph::Edge* WordDiceGraph::Edge::ReverseEdge(){
  
  for(size_t e = 0; e < to->edges.size(); e++){
    if(to->edges[e]->to == from) return to->edges[e];
  }
  
  for(size_t e = 0; e < to->reverse_edges.size(); e++){
    if(to->reverse_edges[e]->to == from) return to->reverse_edges[e];
  }
  
  return NULL;
  
}

//Returns a vector of 256 bools indicating which char values the string parameter contains
Die WordDiceGraph::DieFromString(const string &s){

  Die rv(256, false);
  
  for(size_t i = 0; i < s.size(); i++){
    rv[s[i]] = true;
  }
  
  return rv;

}

//Creates a vector of dice from a vector of strings each indicating sides of a character dice
vector< Die > WordDiceGraph::DiceFromStrings(const vector<string> &dicestrings){
  
  vector< Die > rv;
  
  //Populate with dice
  for(size_t i = 0; i < dicestrings.size(); i++){
    rv.push_back(DieFromString(dicestrings[i]));
  }

  return rv;

}

//Returns a string representing the node at node[node_index], given that the current word being analyzed is words[word_index]
string WordDiceGraph::NodeVal(size_t node_index, size_t word_index) const{

  char type = nodes[node_index]->type;
  if(type == 's') return "SOURCE";
  if(type == 't') return "SINK";
  if(type == 'd'){
    string rv;
    Die die = dice[node_index - dice_start_index];
    for(size_t i = 0; i < die.size(); i++){
      if(die[i]) rv.push_back(i);
    }
    return rv;
  }
  if(type == 'w') return (node_index - word_start_index < words[word_index].size()) ? string(1, words[word_index][node_index - word_start_index]) : "-";
  
  return "";

}

//Prints the graph on stdout
void WordDiceGraph::Print(){

  for(size_t i = 0; i < nodes.size(); i++){
    cout << "Node " << i << ": " << NodeVal(i, current_word_index) << " Edges to ";
    for(size_t e = 0; e < nodes[i]->edges.size(); e++){
      cout << nodes[i]->edges[e]->to->node_index << " ";
    }
    cout << endl;
  }
  
}

//Sets the edges from dice nodes to word nodes based on the word at the given index in words
void WordDiceGraph::SetWord(size_t word_index){

  Edge* edge;
  
  //destroy existing edges of dice nodes
  for(size_t d = dice_start_index; d <= dice_end_index; d++){
    for(size_t e = 0; e < nodes[d]->edges.size(); e++){
      delete nodes[d]->edges[e];
    }
    nodes[d]->edges.clear();
  }
  
  //destroy existing reverse edges of word nodes
  for(size_t w = word_start_index; w <= word_end_index; w++){
    for(size_t e = 0; e < nodes[w]->reverse_edges.size(); e++){
      delete nodes[w]->reverse_edges[e];
    }
    nodes[w]->reverse_edges.clear();
  }

  //for each matching of a die to a letter in the word
  for(size_t d = dice_start_index; d <= dice_end_index; d++){
    for(size_t w = word_start_index; w <= word_end_index; w++){
    
      //if the die has the character of the word node, create the edges
      if(dice[d - dice_start_index][NodeVal(w, word_index)[0]]){
        edge = new Edge(nodes[d], nodes[w], nodes[d]->edges.size());
        nodes[d]->edges.push_back(edge);
        edge = new Edge(nodes[w], nodes[d], nodes[w]->reverse_edges.size());
        nodes[w]->reverse_edges.push_back(edge);
      }
    
    }
  }
  
  current_word_index = word_index;

}

/*
  Determines if the current word is spellable using the Edmonds-Karp algorithm
  If it can be spelled, returns a vector of the indices of the dice used to spell it in order
  Otherwise, returns an empty vector
*/
vector<size_t> WordDiceGraph::Spell(){

  vector< pair<size_t, char> > path;
  vector<size_t> dice_order(words[current_word_index].size());
  Node* node;
  Edge* edge;
  size_t max_flow = 0;

  /*
    Indicate which edges are present in their respective graphs
    [n][e] indicates if node n's edge at index e is present in the graph
    'f' indicates forward edge, 'r' indicates reverse edge, 'n' indicates no edge
  */
  vector< vector<char> > flowEdges; //flow graph
  vector< vector<char> > resEdges; //residual graph
  
  //populate edge vecs
  for(size_t n = 0; n < nodes.size(); n++){
    flowEdges.push_back(vector<char>());
    resEdges.push_back(vector<char>());
    for(size_t e = 0; e < nodes[n]->edges.size(); e++){
      flowEdges[n].push_back('n'); //flow graph starts empty
      resEdges[n].push_back('f'); //residual graph starts as copy of original
    }
  }
  
  //Edmonds-Karp algorithm
  while(1){
    //list of shortest path of edges to follow from source to sink in reverse order
    path = Restricted_BFS(resEdges);
    if(path.size() == 0) break;
    max_flow++;
    node = nodes[source_index];
    //Update flow and residual graphs
    for(size_t i = path.size() - 1; true; i--){
      //if edge is forward edge
			if(path[i].second == 'f'){		 
		    flowEdges[node->node_index][path[i].first] = 'f';
		    resEdges[node->node_index][path[i].first] = 'r';
		    node = node->edges[path[i].first]->to;
        if(i == 0) break;
      //if edge is reverse edge
      } else {
        flowEdges[node->reverse_edges[path[i].first]->to->node_index][node->reverse_edges[path[i].first]->ReverseEdge()->edge_index] = 'n';
        resEdges[node->reverse_edges[path[i].first]->to->node_index][node->reverse_edges[path[i].first]->ReverseEdge()->edge_index] = 'f';
        node = node->reverse_edges[path[i].first]->to;
      }
		}
  }
  
  //If the word cannot be spelled, return empty vector
  if(max_flow != words[current_word_index].size()) return vector<size_t>();
  
  //Determine dice order from flow graph
  for(size_t d = dice_start_index; d <= dice_end_index; d++){
    node = nodes[d];
    for(size_t e = 0; e < node->edges.size(); e++){
      edge = node->edges[e];
      if(edge->to->type == 'w' && flowEdges[d][e] == 'f') dice_order[edge->to->node_index - word_start_index] = d - dice_start_index;
    }
  }

  return dice_order;

}

//Returns a vector of edge indices to follow from source to sink with the shortest path given the available edges (in reverse order)
vector< pair<size_t, char> > WordDiceGraph::Restricted_BFS(const vector< vector<char> > &availableEdges){

  Node* node;
  Edge* edge;
  queue<Node*> Q;
  vector<int> distances(nodes.size(), -1);
  vector<Edge*> backlinks(nodes.size(), NULL);
  
  //Breadth-First Search starting at source
  distances[source_index] = 0;
  Q.push(nodes[source_index]);
  //Stop when sink index is found, or when the algorithm finishes (which means no path exists)
  while(distances[sink_index] == -1 && Q.size() != 0){
    
    node = Q.front();
    Q.pop();
    //Forward edges
    for(size_t e = 0; e < node->edges.size(); e++){
      edge = node->edges[e];
      //only check edges that are specified by available edges
      if(distances[edge->to->node_index] == -1 && availableEdges[node->node_index][e] == 'f'){
        distances[edge->to->node_index] = distances[node->node_index] + 1;
        backlinks[edge->to->node_index] = edge;
        Q.push(edge->to);
      }
    }
    //Reverse edges
    for(size_t e = 0; e < node->reverse_edges.size(); e++){
      edge = node->reverse_edges[e];
      //only check edges that are specified by available edges
      if(distances[edge->to->node_index] == -1 && availableEdges[edge->to->node_index][edge->ReverseEdge()->edge_index] == 'r'){
        distances[edge->to->node_index] = distances[node->node_index] + 1;
        backlinks[edge->to->node_index] = edge;
        Q.push(edge->to);
      }
    }
  }
  
  //If sink_index is unreachable, return empty vector
  if(distances[sink_index] == -1) return vector< pair<size_t,char> >();
  
  //Otherwise, follow the backlinks to construct the path in reverse order
  vector< pair<size_t, char> > path; //each pair is an edge_index and whether it is forward or reverse ('f' or 'r')
  node = nodes[sink_index];
  while(node != nodes[source_index]){
    edge = backlinks[node->node_index];
    node = edge->from;
    path.push_back(make_pair(edge->edge_index, (edge->from->edges[edge->edge_index] == edge) ? 'f' : 'r'));
  }
  
  return path;

}

//Constructs a WordDiceGraph given vectors of dice and words
WordDiceGraph::WordDiceGraph(vector<Die> dice, vector<string> words) : dice(dice), words(words) {

  Node* node;
  Edge* edge;
  
  max_word_size = 0;
  for(size_t i = 0; i < words.size(); i++){
    if(words[i].size() > max_word_size) max_word_size = words[i].size();
  }
  
  //create source node
  source_index = nodes.size();
  node = new Node();
  node->type = 's';
  node->node_index = source_index;
  nodes.push_back(node);
  
  //create sink node
  sink_index = nodes.size();
  node = new Node();
  node->type = 't';
  node->node_index = sink_index;
  nodes.push_back(node);
  
  //create dice nodes
  dice_start_index = nodes.size();
  for(size_t i = 0; i < dice.size(); i++){
    node = new Node();
    node->type = 'd';
    node->index = i;
    node->node_index = dice_start_index + i;
    nodes.push_back(node);
  }
  dice_end_index = nodes.size() - 1;
  
  //create word nodes
  word_start_index = nodes.size();
  for(size_t i = 0; i < max_word_size; i++){
    node = new Node();
    node->type = 'w';
    node->index = i;
    node->node_index = word_start_index + i;
    nodes.push_back(node);
  }
  word_end_index = nodes.size() - 1;
  
  //build edges from source
  for(size_t i = dice_start_index; i <= dice_end_index; i++){
    edge = new Edge(nodes[source_index], nodes[i], nodes[source_index]->edges.size());
    nodes[source_index]->edges.push_back(edge);
    edge = new Edge(nodes[i], nodes[source_index], nodes[i]->reverse_edges.size());
    nodes[i]->reverse_edges.push_back(edge);
  }
  
  //build edges to sink
  for(size_t i = word_start_index; i <= word_end_index; i++){
    edge = new Edge(nodes[i], nodes[sink_index], nodes[i]->edges.size());
    nodes[i]->edges.push_back(edge);
    edge = new Edge(nodes[sink_index], nodes[i], nodes[sink_index]->reverse_edges.size());
    nodes[sink_index]->reverse_edges.push_back(edge);
  }

}

int main(int argc, char** argv){

/*
  Check arguments
*/

  if(argc != 3){
    cerr << "Usage: " << argv[0] << " dice-file word-list-file" << endl;
    exit(1);
  }

/*
  Constructing the graph from file data
*/

  //strings read directly from the files
  vector<string> dicestrings = StringVectorFromFile(argv[1]);
  vector<string> wordstrings = StringVectorFromFile(argv[2]);
  
  //End program if unable to read any strings from files
  if(dicestrings.size() == 0 || wordstrings.size() == 0) exit(1);

  WordDiceGraph G(WordDiceGraph::DiceFromStrings(dicestrings), wordstrings);
  
/*
  Determining which words can be spelled, and which dice to use in which order to spell them
*/
	
	vector<size_t> dice_order;
  
  //Check every word
  for(size_t w = 0; w < G.words.size(); w++){
    //Create the graph for the given word
    G.SetWord(w);
    //Get the order of dice to use to spell the word (if possible)
    dice_order = G.Spell();
    //If dice_order is empty, it is not possible
    if(dice_order.size() == 0) printf("Cannot spell %s\n", G.words[w].c_str());
    //Otherwise print the order
    else{
      for(size_t i = 0; i < dice_order.size(); i++){
        printf("%lu", dice_order[i]);
        if(i < dice_order.size() - 1) printf(",");
      }
      printf(": %s\n", G.words[w].c_str());
    }
  }

}