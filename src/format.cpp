#include <string>

#include "format.h"
using std::string;
using std::to_string;

// DONE: Complete this helper function
// INPUT: duration in seconds
// OUTPUT: time in hours:minute:seconds format
string Format::ElapsedTime(long seconds) {
  //  char ch_timeFormat[10];
    int i_hour = seconds / 3600;
    int residue = seconds % 3600;
    int i_min = residue / 60;
    int i_sec = residue % 60;
    return to_string(i_hour) + ":" + to_string(i_min) + ":" + to_string(i_sec);
;
}