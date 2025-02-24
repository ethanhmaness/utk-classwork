#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <list>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include "cs302-midi.h"

/*
  cs302-midi.cpp
  Ethan Maness
  Feb 19, 2022
  
  Defines methods of the CS302_Midi class which allow for converting of tracks between note and event storage formats.
*/

using namespace std;

void CS302_Midi::el_to_nd()
{

  nd = new NDMap;
  
  ND* note;
  Event* event;
  double currentTime = 0;
  
  vector<ND*> pitches(128,NULL);
  ND* pedal = NULL;
  
  list<Event*>::iterator elit;
  
  //for each event
  for(elit = el->begin(); elit != el->end(); elit++){
  
    event = *elit;
    currentTime += (double)event->time / 480;
    
    //ON = create new note, store it indexed on its pitch
    if(event->key == 'O'){
    
      note = new ND;
      
      note->key = 'N';
      note->start = currentTime;
      note->pitch = event->v1;
      note->volume = event->v2;
    
      pitches.at(note->pitch) = note;
    
    //OFF = retrieve note indexed at pitch, set its stoptime, push it into nd
    } else if (event->key == 'F'){
    
      note = pitches.at(event->v1);
      
      note->stop = currentTime;
      
      nd->insert(make_pair(currentTime, note));
      
      pitches.at(event->v1) = NULL;
    
    //DAMPER (handles UP and DOWN)
    } else {
    
      //UP = create new damper, store it
      if(event->v1 == 1){
      
        note = new ND;
      
        note->key = 'D';
        note->start = currentTime;
        
        pedal = note;
        
      //DOWN = retrieve damper, set stoptime, push into nd
      } else {
      
        note = pedal;
        
        note->stop = currentTime;
        
        nd->insert(make_pair(currentTime, note));
        
        pedal = NULL;
      
      }
    
    }
  
  }
  
  //Make sure to close off all notes and dampers if OFF or DAMPER UP events are erroneously not included
  for(size_t i = 0; i < 128; i++){
  
    if(pitches.at(i) != NULL){
    
      note = pitches.at(i);
      
      note->stop = currentTime;
      
      nd->insert(make_pair(currentTime, note));
      
      pitches.at(i) = NULL;
    
    }
  
  }
  
  if(pedal != NULL){
  
    note = pedal;
        
    note->stop = currentTime;
        
    nd->insert(make_pair(currentTime, note));
        
    pedal = NULL;
  
  }
  
}

void CS302_Midi::nd_to_el()
{

  el = new EventList;
  
  Event* event;
  ND* note;
  
  //format is (time, (event type, event)) where event type is 1-4. 1 = OFF, 2 = DAMPER UP, 3 = DAMPER DOWN, 4 = ON
  map<int, multimap<char, Event*> > eventTimes;
  int currentTime;
  
  multimap<char, Event*> typeAndEvent;
  pair<multimap<char, Event*>::iterator, multimap<char, Event*>::iterator> typeRange;
  multimap<char, Event*>::iterator tmit;
  map<int, multimap<char, Event*> >::iterator emit;
  multimap<double, ND*>::iterator mit;
  
  //for each note
  for(mit = nd->begin(); mit != nd->end(); mit++){
  
    note = mit->second;
    
    //create start event
    event = new Event;
    
    if(note->key == 'N'){
    
      event->key = 'O';
      event->time = rint(480 * note->start);
      event->v1 = note->pitch;
      event->v2 = note->volume;
    
    } else {
    
      event->key = 'D';
      event->time = rint(480 * note->start);
      event->v1 = 1;
    
    }
    
    //insert at appropriate time with event type marker for ordering
    eventTimes.insert(make_pair((int)(rint(480 * note->start)), multimap<char, Event*>()));
    eventTimes.at((int)rint(480 * note->start)).insert(make_pair((event->key == 'O') ? 4 : 3, event));
    
    //create end event
    event = new Event;
    
    if(note->key == 'N'){
    
      event->key = 'F';
      event->time = rint(480 * note->stop);
      event->v1 = note->pitch;
    
    } else {
    
      event->key = 'D';
      event->time = rint(480 * note->stop);
      event->v1 = 0;
    
    }
    
    //insert at appropriate time with event type marker for ordering
    eventTimes.insert(make_pair((int)rint(480 * note->stop), multimap<char, Event*>()));
    eventTimes.at((int)rint(480 * note->stop)).insert(make_pair((event->key == 'F') ? 1 : 2, event));
    
  }
  
  //NOTE: At this point, all event times are wrong.  They will be adjusted just before inserting to indicate delay rather than absolute time.
  
  currentTime = 0;
  
  //go through the list of event groups in chronological order
  for(emit = eventTimes.begin(); emit != eventTimes.end(); emit++){
  
    //at each time, insert in type order
    for(char i = 1; i <= 4; i++){
      
      //get iterators to first and last events of type i at the given time
      typeRange = emit->second.equal_range(i);
      
      //go through all events at given time
      for(tmit = typeRange.first; tmit != typeRange.second; tmit++){
      
        event = tmit->second;
        
        //determines how much (if any) delay to add to event after previous
        event->time -= currentTime;
        currentTime += event->time;
      
        //adds event to event list
        el->push_back(event);
      
      }
    
    }
  
  }
  
}
