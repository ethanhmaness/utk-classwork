/*
    Ethan Maness
    COSC 461 Intro to Compilers
    Programming Assignment 1 - nfa2dfa
*/

#include <cstdio>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cctype>
#include <stack>
#include <map>
#include <iomanip>

using namespace std;

/* Converts an integer to a string */
string IntToString(int i)
{
    stringstream ss;
    ss << i;
    return ss.str();
}

/* Splits a string into a vector of no-whitespace strings using stringstreams */
vector<string> SplitLine(string str)
{
    istringstream ss(str);
    vector<string> rv;
    string word;
    while (ss >> word)
    {
        rv.push_back(word);
    }
    return rv;
}

/* Splits a string into a vector of strings at any character listed in delimiters */
vector<string> SplitString(string str, string delimiters)
{
    vector<string> rv;
    string tmp = "";
    for (int i = 0; i < str.size(); i++)
    {
        if (delimiters.find(str[i]) == string::npos)
            tmp += str[i];
        else
        {
            if (tmp.size() > 0)
                rv.push_back(tmp);
            tmp = "";
        }
    }
    if (tmp != "") rv.push_back(tmp);
    return rv;
}

/* Reads a string in the format {a,b,c} into a set of integers */
set<int> ParseIntSet(string str)
{
    if (str[0] != '{' || str[str.size() - 1] != '}')
        throw invalid_argument("Invalid set format.");

    set<int> rv;
    vector<string> split = SplitString(str.substr(1, str.size() - 2), ",");

    for (int i = 0; i < split.size(); i++)
        rv.insert(atoi(split[i].c_str()));

    return rv;
}

/* Prints a set of ints in the {a,b,c} format */
string SetToString(set<int> s)
{
    string str;
    str += "{";
    for(set<int>::iterator sit = s.begin(); sit != s.end(); sit++)
    {
        if (sit != s.begin()) str += ",";
        if (*sit != 0) str += IntToString(*sit);
    }
    str += "}";
    return str;
}

/* Removes whitespace from the beginning and end of a string */
string Strip(string str)
{
    int i = 0;
    while (isspace(str[0]))
        str = str.substr(1);

    while (isspace(str[str.size() - 1]))
        str = str.substr(0, str.size() - 1);

    return str;
}

/* Returns all elements in full_set that aren't in subset */
set<int> SubsetComplement(set<int> full_set, set<int> subset)
{
    set<int> rv;
    for(set<int>::iterator sit = full_set.begin(); sit != full_set.end(); sit++)
    {
        if(subset.find(*sit) == subset.end()) rv.insert(*sit);
    }
    return rv;
}

/* Returns a set of keys in the map */
set<int> KeySet(map<int, set<int> > m)
{
    set<int> rv;
    for(map<int, set<int> >::iterator mit = m.begin(); mit != m.end(); mit++)
    {
        rv.insert(mit->first);
    }
    return rv;
}

/* Checks if sets contain the same values */
bool SetsEqual(set<int> a, set<int> b)
{
    for(set<int>::iterator sit = a.begin(); sit != a.end(); sit++)
    {
        if (b.find(*sit) == b.end()) return false;
    }
    for(set<int>::iterator sit = b.begin(); sit != b.end(); sit++)
    {
        if (a.find(*sit) == a.end()) return false;
    }
    return true;
}

/* Returns the key of a given set in a map of (int, set) pairs */
int KeyOfValue(map<int, set<int> > m, set<int> v)
{
    for (map<int, set<int> >::iterator mit = m.begin(); mit != m.end(); mit++)
    {
        if(SetsEqual(mit->second, v))
        {
            printf("Index %d is %s\n", mit->first, SetToString(v).c_str());
            return mit->first;
        }
    }
    return -1;
}

/* Returns a set of all keys where the associated set intersects set n at all */
set<int> KeysWithValueIntersecting(map<int, set<int> > m, set<int> n)
{
    set<int> rv;
    for (map<int, set<int> >::iterator mit = m.begin(); mit != m.end(); mit++)
    {
        for (set<int>::iterator sit = n.begin(); sit != n.end(); sit++)
        {
            if (mit->second.find(*sit) != mit->second.end()) rv.insert(mit->first);
        }
    }
    return rv;
}

/*
    Represents a deterministic finite automaton by a transition table.
    Symbols are stored as ints and labeled with strings.
    State 0 is a "dead" state.
*/
struct DFA
{
    /* Index of initial state. */
    const int init_state;

    /* Symbol labels stored by index. */
    const vector<string>* symbols;

    /* Indices of accepting states. */
    const set<int>* accept_states;

    /*
    Represents the transition function of this DFA instance.
    transition_table[A][B] is the state reached from state A by a transition on symbol B.
    */
    const vector< vector<int> >* transition_table;

    /* Constructs a DFA */
    DFA(int init_state, const vector<string>* symbols, const set<int>* accept_states, const vector< vector<int> >* transition_table)
        : init_state(init_state), symbols(symbols), accept_states(accept_states), transition_table(transition_table)
    {
        if (transition_table->size() <= init_state)
            throw invalid_argument("init_state must be smaller than transition_table->size()");

        for (set<int>::const_iterator sit = accept_states->begin(); sit != accept_states->end(); sit++)
            if (*sit - 1 >= transition_table->size())
                throw invalid_argument("accept_states may only contain values smaller than transition_table->size()");

        for (int i = 0; i < transition_table->size(); i++)
        {
            if ((*transition_table)[i].size() != (*symbols).size())
                throw invalid_argument(string("State ").append(IntToString(i)).append("'s outgoing edge count does not match the symbol count."));

            for (int j = 0; j < (*transition_table)[i].size(); j++)
                if ((*transition_table)[i][j] >= (*transition_table).size())
                    throw invalid_argument(string("State ").append(IntToString(i)).append("'s transition on ").append((*symbols)[j]).append(" points to an undefined state '").append(IntToString((*transition_table)[i][j])).append("'."));
        }
    }
};

/*
    Represents a non-deterministic finite automaton by a transition table.
    Symbols are stored as ints and labeled with strings. The last symbol indicates epsilon transitions.
*/
struct NFA
{
    /* Index of initial state. */
    const int init_state;

    /* Symbol labels stored by index. */
    const vector<string>* symbols;

    /* Indices of accepting states. */
    const set<int>* accept_states;

    /*
    Represents the transition function of this NFA instance.
    transition_table[A][B] is the set of states reached from state A by a transition on symbol B.
    */
    const vector< vector< set<int> > > * transition_table;

    /* Set of states reachable from the from set with a transition on symbol */
    set<int> Move(set<int> from, int symbol)
    {
        set<int> rv;
        for (set<int>::iterator sit = from.begin(); sit != from.end(); sit++)
            for (set<int>::iterator sit2 = (*transition_table)[*sit][symbol].begin(); sit2 != (*transition_table)[*sit][symbol].end(); sit2++)
                rv.insert(*sit2);

        return rv;
    }

    /* Set of states reachable from the from set through transitions on symbol */
    set<int> Closure(set<int> from, int symbol)
    {
        int t;
        stack<int> stack;
        set<int> rv = from;
        for(set<int>::iterator sit = from.begin(); sit != from.end(); sit++)
            stack.push(*sit);
        
        while(!stack.empty())
        {
            t = stack.top();
            stack.pop();
            for(set<int>::iterator sit = (*transition_table)[t][symbol].begin(); sit != (*transition_table)[t][symbol].end(); sit++)
            {
                if (rv.find(*sit) == rv.end())
                {
                    rv.insert(*sit);
                    stack.push(*sit);
                }
            }
        }
        return rv;
    }

    /* Converts the NFA to a DFA */
    DFA ToDFA()
    {
        vector< vector< int > > Dtran;
        vector< int > dummy_state_transition_vector;
        map<int, set< int > > Dstates;
        set<int> marked;
        set<int> unmarked;
        set<int> U;
        set<int> T;
        int key;

        set<int> init_state_set;
        init_state_set.insert(init_state);
        Dstates.insert(make_pair(1, Closure(init_state_set, Epsilon())));
        dummy_state_transition_vector.resize(symbols->size()-1);
        Dtran.resize(Dstates.size()+1, dummy_state_transition_vector);

        while(!(unmarked = SubsetComplement(KeySet(Dstates), marked)).empty())
        {
            T = Dstates.at(*(unmarked.begin()));
            marked.insert(*(unmarked.begin()));
            printf("new DFA state: %d    -->  %s\n", *(unmarked.begin()), SetToString(Dstates[*(unmarked.begin())]).c_str());

            for(int i = 0; i <= symbols->size() - 2; i++)
            {
                U = Closure(Move(T, i), Epsilon());
                if (!U.empty())
                {
                    key = KeyOfValue(Dstates, U);
                    if (key == -1)
                    {
                        Dstates.insert(make_pair(Dstates.size() + 1, U));
                        Dtran.resize(Dstates.size()+1, dummy_state_transition_vector);
                        key = Dstates.size();
                    }
                    Dtran[*(unmarked.begin())][i] = key;
                }
            }
        }

        vector<string> *symbols_noepsilon = new vector<string>(*symbols);
        symbols_noepsilon->resize(symbols->size() - 1);

        return DFA(1, symbols_noepsilon, new set<int>(KeysWithValueIntersecting(Dstates, *accept_states)), new vector< vector< int > >(Dtran));
    }

    /* The last symbol represents epsilon */
    int Epsilon(){ return symbols->size() - 1; }

    /* Constructs an NFA */
    NFA(int init_state, const vector<string>* symbols, const set<int>* accept_states, const vector< vector< set<int> > >* transition_table)
        : init_state(init_state), symbols(symbols), accept_states(accept_states), transition_table(transition_table)
    {
        if (transition_table->size() <= init_state)
            throw invalid_argument("init_state must be smaller than transition_table->size()");

        for (set<int>::const_iterator sit = accept_states->begin(); sit != accept_states->end(); sit++)
            if (*sit - 1 >= transition_table->size())
                throw invalid_argument("accept_states may only contain values smaller than transition_table->size()");
        
        for (int i = 0; i < transition_table->size(); i++)
            for (int j = 0; j < (*transition_table)[i].size(); j++)
                for (set<int>::const_iterator sit = (*transition_table)[i][j].begin(); sit != (*transition_table)[i][j].end(); sit++)
                    if (*sit - 1 >= (*transition_table).size())
                        throw invalid_argument(string("State ").append(IntToString(i)).append("'s transitions on ").append((*symbols)[j]).append(" include an undefined state '").append(IntToString(*sit)).append("'."));
    }
};

int main()
{
    /* String values for reading input */
    const string init_state_str = "Initial State:";
    const string accept_states_str = "Final States:";
    const string num_states_str = "Total States:";
    const string state_row_str = "State";

    string line;
    int init_state, num_states;
    set<int>* accept_states = new set<int>();
    vector<string>* symbols = new vector<string>(), linesplit;
    vector< vector< set<int> > >* transition_table = new vector< vector< set<int> > >();
    vector< set<int> > state_transitions;

    /* Reading input */
    printf("reading NFA ... ");

    getline(cin, line);
    if (line.find(init_state_str) == 0)
        init_state = atoi(line.substr(init_state_str.size()).c_str());
    else
    {
        printf("Bad input\n");
        return 1;
    }

    getline(cin, line);
    if (line.find(accept_states_str) == 0)
        *accept_states = ParseIntSet(Strip(line.substr(accept_states_str.size())));
    else
    {
        printf("Bad input\n");
        return 1;
    }

    getline(cin, line);
    if (line.find(num_states_str) == 0)
        num_states = atoi(line.substr(num_states_str.size()).c_str());
    else
    {
        printf("Bad input\n");
        return 1;
    }

    getline(cin, line);
    if (line.find(state_row_str) == 0)
        *symbols = SplitString(line.substr(state_row_str.size()), " ");

    // empty vector at state 0 (dead node)
    transition_table->push_back(state_transitions);

    /* Reading in transition table */
    for (int i = 1; i <= num_states; i++)
    {
        state_transitions.clear();
        getline(cin, line);
        linesplit = SplitString(line, " ");
        if (atoi(linesplit[0].c_str()) != i)
        {
            printf("Bad input\n");
            return 1;
        }

        for (int j = 0; j < symbols->size(); j++)
        {
            state_transitions.push_back(ParseIntSet(linesplit[j + 1]));
        }

        transition_table->push_back(state_transitions);
    }

    NFA nfa(init_state, symbols, accept_states, transition_table);

    printf("done\n");
    printf("\n");

    /* Create DFA */
    printf("creating corresponding DFA ... \n");
    DFA dfa = nfa.ToDFA();
    printf("done\n");
    printf("\n");

    /* Print out DFA data */

    printf("final DFA:\n");
    printf("Initial State:  %d\n", dfa.init_state);
    printf("Final States:   %s\n", SetToString(*(dfa.accept_states)).c_str());
    printf("Total States:   %d\n", dfa.transition_table->size());

    cout << left << setw(17) << "State";
    for(int i = 0; i < dfa.symbols->size(); i++)
    {
        cout << setw(17) << symbols->at(i);
    }
    cout << endl;
    for(int i = 1; i < dfa.transition_table->size(); i++)
    {
        cout << setw(17) << i;
        for(int j = 0; j < dfa.symbols->size(); j++)
        {
            set<int> tmp;
            tmp.insert((*dfa.transition_table)[i][j]);
            cout << setw(17) << SetToString(tmp);
        }
        cout << endl;
    }
    return 0;
}