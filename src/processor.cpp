#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>
//#include <numeric>

using std::string;
//using std::accumulate;
using std::vector;

// DONE: Return the total CPU utilization
float Processor::Utilization() {
  // ref: https://www.geeksforgeeks.org/processing-strings-using-stdistringstream/
  string line, temp;
  int _iter;
  vector<int> v;//[LinuxParser::kGuestNice_];
  std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream lnstream(line);
    lnstream >> temp;
    for (int i = LinuxParser::kUser_; i <= LinuxParser::kGuestNice_; i++) {
      lnstream >>_iter; // stores the string as int in _iter
      v.push_back(_iter);
    } 

  }
  // Computes CPU utilization - idle + non-idle time
  float tot_time;//  = 30;//accumulate(v.begin(),v.end(), 0);
  for (auto i = v.begin(); i <= v.end(); i++)
    tot_time = tot_time + (*i);
  int idletime    = v[LinuxParser::kIdle_] + v[LinuxParser::kIOwait_];
  return (tot_time - idletime)/tot_time;
}