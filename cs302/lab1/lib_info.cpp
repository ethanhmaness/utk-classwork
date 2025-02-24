#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>

/*
    lib_info.cpp
    Ethan Maness
    Feb 6, 2022
    
    This programs reads in formatted lines of song info from a given file and prints to stdout a sorted list of all artists, albums, and songs with their associated info
*/

using namespace std;

class Song {
  public:
    string title;
    int time;
    int track;
};

class Album {
  public:
    map <int, Song *> songs;
    string name;
    int time;
};

class Artist {
  public:
    map <string, Album *> albums;
    string name;
    int time;
    int nsongs;
};

int main(int argc, char** argv){
	
    //Exit if improper call
    if(argc != 2){
        cout << "Usage: lib_info filename" << endl;
        return 0;
    }
    
    
/*
    Initialization
*/
    
    //initialization vars
    ifstream fs;
    char* fname = argv[1];
    
    //open file or exit if unable
    fs.open(fname);
    if(!(fs.is_open())){
    	cout << "Unable to open file" << endl;
    	return 0;
    }
    
/*
    Reading Data
*/
    
    //data structure vars
    map <string, Artist*> artists;
    map <string, Artist*>::iterator artit;
    map <string, Album *>::iterator albit;
    map <int, Song *>::iterator songit;
    
    //reading vars
    string line;
    vector<string> linedata;
    int linenum;
    int min;
    int sec;
    int track;
    Artist* artist;
    Album* album;
    Song* song;
    
    //processing line-by-line
    while(getline(fs,line)){
    
        linenum++;
    
        //read the line into linedata as separated strings
        linedata.clear();
        while(line.find(' ') != string::npos){
            linedata.push_back(line.substr(0,line.find(' ')));
            line = line.substr(line.find(' ') + 1);
        }
        linedata.push_back(line);
        
        //remove all underscores (replace with spaces)
        for(size_t i = 0; i < linedata.size(); i++){
            replace(linedata.at(i).begin(), linedata.at(i).end(), '_', ' ');
        }
        
        //if incorrect num of strings, data is malformed
        if(linedata.size() != 6){
            cout << "Data formatted incorrectly on line " << linenum << endl;
            return 0;
        }
        
        /*  
            linedata format:
            0 - Title
            1 - Time (mm:ss)
            2 - Artist
            3 - Album
            4 - Genre
            5 - Track #
        */
        
        //If Artist doesn't exist, create and insert into artists map
        if(artists.find(linedata.at(2)) == artists.end()){
            artist = new Artist();
            artist->name = linedata.at(2);
            artists.insert(pair<string, Artist*>(linedata.at(2), artist));
        }
    
        artist = artists.at(linedata.at(2)); 
           
        //If Album doesn't exist, create and insert into Artist->albums
        if(artist->albums.find(linedata.at(3)) == artist->albums.end()){
            album = new Album();
            album->name = linedata.at(3);
            artist->albums.insert(pair<string, Album*>(linedata.at(3), album));
        }
        
        album = artist->albums.at(linedata.at(3));
        istringstream(linedata.at(5)) >> track;
        
        //If Song doesn't exist, create and insert into Album->songs
        if(album->songs.find(track) == album->songs.end()){
            song = new Song();
            song->title = linedata.at(0);
            song->track = track;
            
            //parse time format into integer num of seconds
            istringstream(linedata.at(1).substr(0, linedata.at(1).find(':'))) >> min;
            istringstream(linedata.at(1).substr(linedata.at(1).find(':') + 1)) >> sec;
            song->time = min * 60 + sec;
            
            album->songs.insert(pair<int, Song*>(track, song));
        }
        
        song = album->songs.at(track);
        
        //increase aggregated data accordingly
        album->time += song->time;
        artist->time += song->time;
        artist->nsongs++;
        
    }
    
/*    
    Printing data
*/

    //for every artist
    for(artit = artists.begin(); artit != artists.end(); artit++){
    
        //print the artist info
        artist = (*artit).second;
        cout << artist->name << ": " << artist->nsongs << ", " << artist->time / 60 << ":" << setfill('0') << setw(2) << artist->time % 60 << endl;
        
        //for each of the artists albums
        for(albit = artist->albums.begin(); albit != artist->albums.end(); albit++){
            
            //print the album info
            album = (*albit).second;
            cout << "        " << album->name << ": " << album->songs.size() << ", " << album->time / 60 << ":" << setfill('0') << setw(2) << album->time % 60 << endl;
            
            //for each of the albums songs
            for(songit = album->songs.begin(); songit != album->songs.end(); songit++){
            
                //print the song info
                song = (*songit).second;
                cout << "                " << song->track << ". " << song->title << ": " << song->time / 60 << ":" << setfill('0') << setw(2) << song->time % 60 << endl;
                
            }
            
        }
        
    }

/*
    Clearing memory
*/

    for(artit = artists.begin(); artit != artists.end(); artit++){
        artist = (*artit).second;
        for(albit = artist->albums.begin(); albit != artist->albums.end(); albit++){
            album = (*albit).second;
            for(songit = album->songs.begin(); songit != album->songs.end(); songit++){
                delete (*songit).second;
            }
            delete album;
        }
        delete artist;
    }

	return 0;
}
