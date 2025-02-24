#include "dlist.hpp"

using namespace std;

//returns the next linked node
Dnode* Dnode::Next(){
	return flink;
}

//returns the previous linked node
Dnode* Dnode::Prev(){
	return blink;
}

//constructs new Dlist, with size 0 and the sentinel pointing to itself
Dlist::Dlist(){
	sentinel = new Dnode();
	sentinel->flink = sentinel;
	sentinel->blink = sentinel;
	size = 0;
}

//copy constructor, initializes and then utilizes assignment overload
Dlist::Dlist(const Dlist &d){
	sentinel = new Dnode();
	sentinel->flink = sentinel;
	sentinel->blink = sentinel;
	size = 0;
	//assignment overload to copy all values into new nodes
	*this = d;
}

//clears the current list, then copies each dnode from the given dlist to this list
Dlist& Dlist::operator= (const Dlist &d){
	//clears all nodes
	Clear();
	//go to first node
	Dnode* n = d.Begin();
	//while not at sentinel node
	while(n != d.End()){
		//push node into this list
		Push_Back(n->s);
		//go to next node
		n = n->flink;
	}
	return *this;
}

//destroys ALL nodes, INCLUDING the sentinel
Dlist::~Dlist(){
	Clear();
	delete sentinel;
}

//removes all non-trivial nodes from the Dlist
void Dlist::Clear(){
	while(!Empty()) Pop_Front();
}

//returns if dlist is empty (other than sentinel)
bool Dlist::Empty() const{
	return size == 0;
}

//just returns size field
size_t Dlist::Size() const{
	return size;
}

//creates new node storing the given string at the front of the Dlist, and arranges the list's nodes' pointers appropriately
void Dlist::Push_Front(const string &s){
	Insert_After(s, sentinel);
}

//creates new node storing the given string at the back of the Dlist, and arranges the list's nodes' pointers appropriately
void Dlist::Push_Back(const string &s){
	Insert_Before(s, sentinel);
}

//de-links the first Dnode from the Dlist and deletes its memory, returning the stored value
string Dlist::Pop_Front(){
	string s = sentinel->flink->s;
	Erase(sentinel->flink);
	return s;
}

//de-links the last Dnode from the Dlist and deletes its memory, returning the stored value
string Dlist::Pop_Back(){
	string s = sentinel->blink->s;
	Erase(sentinel->blink);
	return s;
}

//returns the first dnode in the list
Dnode* Dlist::Begin() const{
	return sentinel->flink;
}

//returns "one after" the last node in the list
Dnode* Dlist::End() const{
	return sentinel;
}

//returns the last node in the list
Dnode* Dlist::Rbegin() const{
	return sentinel->blink;
}

//returns "one before" the first node of the list
Dnode* Dlist::Rend() const{
	return sentinel;
}

//creates new node storing the given string before the given node in Dlist, and arranges the list's nodes' pointers appropriately
void Dlist::Insert_Before(const string &s, Dnode *n){
	//l = node that was before n
	Dnode* l = n->blink;
	//m = node that will now be before n
	Dnode* m = new Dnode();
	//set the string of m to the given string
	m->s = s;
	
	//change each pointer appropriately so the list now goes L M N, instead of L N
	l->flink = m;
	n->blink = m;
	m->blink = l;
	m->flink = n;

	//increment size
	size++;
}

//creates new node storing the given string after the given node in Dlist, and arranges the list's nodes' pointers appropriately
void Dlist::Insert_After(const string &s, Dnode *n){
	Insert_Before(s, n->flink);
}

//de-links the given Dnode from the Dlist and deletes its memory
void Dlist::Erase(Dnode *n){
	if(n == sentinel) return;
	
	//m = node before n
	Dnode* m = n->blink;
	//o = node after n
	Dnode* o = n->flink;
	
	//change m and o's links to go to each other (instead of n)
	m->flink = o;
	o->blink = m;

	//free up n's memory
	delete n;
	//decrement size
	size--;
}
