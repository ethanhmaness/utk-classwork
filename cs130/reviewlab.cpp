/*
Ethan Maness
1/28/21
Engine for basic Multi-User Dungeon games using specific text formatting
*/

#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>

using namespace std;

class Room {
    public:
        string name;
        string desc;
        int exits[4]; //Format as [n, e, s, w] with -1 as no exit
};

int main(int argc, char** argv) {

    ifstream fileIn; //input file stream
    string strTemp;
    int roomCount = 0;

    //check cmd arg count
    if(argc != 2){
        cout << "Usage: ./reviewlab <rooms_file>" << endl;
        return 1;
    }

    //get filename
    string filename;
    filename = argv[1];

    //open file
    fileIn.open(filename);

    //check file open
    if(!fileIn.is_open()){
        cout << "Could not open file." << endl;
        return 1;
    }

    //count the tilde lines to determine roomcount
    while(!fileIn.eof()){
        getline(fileIn, strTemp);
        if(strTemp == "~") roomCount++;
    }
    fileIn.close();
    roomCount /= 3;

    //allocate memory for rooms array
    Room *rooms = new Room[roomCount];

    //read room info from file
    fileIn.open(filename);

    //process all the rooms
    for(int i = 0; i < roomCount; i++){
        
        //start with empty strings
        rooms[i].name = "";
        rooms[i].desc = "";
        for(int& e: rooms[i].exits){
            e = -1;
        }

        //append all lines before tilde to name
        while(!fileIn.eof()){
            getline(fileIn, strTemp);
            if(strTemp != "~") rooms[i].name += strTemp + "\n";
            else break;
        }

        //append all lines after first tilde to desc
        while(!fileIn.eof()){
            getline(fileIn, strTemp);
            if(strTemp != "~") rooms[i].desc += strTemp + "\n";
            else break;
        }

        //set exit indexes
        while(!fileIn.eof()){
            getline(fileIn, strTemp);
            if(strTemp != "~"){
                //read dir and index, add into exits array accordingly
                char dir = strTemp.front();
                int index = stoi(strTemp.substr(2));
                switch(dir){
                    case 'n':
                        rooms[i].exits[0] = index;
                        break;
                    case 'e':
                        rooms[i].exits[1] = index;
                        break;
                    case 's':
                        rooms[i].exits[2] = index;
                        break;
                    case 'w':
                        rooms[i].exits[3] = index;
                        break;
                }
            } else break;

        }

    }

    //close ifstream
    fileIn.close();

    //print instructions
    cout << "q - Quit (closes the program) \nl - Look (looks at the room the player is in) \nn, e, s, w - Moves the user in the given cardinal direction (north, east, south, or west).\n";

    //start in room zero
    int curRoom = 0;

    char action;

    //game loop
    while(action != 'q'){
        cin >> action;
        switch(action){
            //q - quit
            case 'q':
                return 1;
                break; //won't actually run
            //l - list room info
            case 'l':
                cout << rooms[curRoom].name;
                cout << rooms[curRoom].desc;
                cout << "\n";
                cout << "Exits: ";
                if(rooms[curRoom].exits[0] != -1) cout << "n ";
                if(rooms[curRoom].exits[1] != -1) cout << "e ";
                if(rooms[curRoom].exits[2] != -1) cout << "s ";
                if(rooms[curRoom].exits[3] != -1) cout << "w ";
                cout << "\n";
                break;
            //n - go north
            case 'n':
                if(rooms[curRoom].exits[0] != -1){
                    cout << "You moved NORTH.\n";
                    curRoom = rooms[curRoom].exits[0];
                } else cout << "You can't go NORTH!\n";
                break;
            //e - go east
            case 'e':
                if(rooms[curRoom].exits[1] != -1){
                    cout << "You moved EAST.\n";
                    curRoom = rooms[curRoom].exits[1];
                } else cout << "You can't go EAST!\n";
                break;
            //s - go south
            case 's':
                if(rooms[curRoom].exits[2] != -1){
                    cout << "You moved SOUTH.\n";
                    curRoom = rooms[curRoom].exits[2];
                } else cout << "You can't go SOUTH!\n";
                break;
            //w - go west
            case 'w':
                if(rooms[curRoom].exits[3] != -1){
                    cout << "You moved WEST.\n";
                    curRoom = rooms[curRoom].exits[3];
                } else cout << "You can't go WEST!\n";
                break;
            //anything else - error, rerun loop
            default:
                cout << "Invalid command.";
        }
    }
}