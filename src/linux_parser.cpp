#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>
#include "linux_parser.h"

using std::stof;
using std::stoi;
using std::stol;
using std::string;
using std::to_string;
using std::vector;
using std::accumulate;
// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return "-1";
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float memTotal = 0.0;
  float memFree = 0.0;
  string line, kb;
  string key;
  string value;

  // read file /proc/meminfo and look for MemTotal and MemFree https://access.redhat.com/solutions/406773
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value >> kb) {
        // search for key memTotal
        if (key == "MemTotal:") {
          memTotal = stof(value);
        }
        // search for key memFree
        else if (key == "MemFree:") {
          memFree = stof(value);
          break;
        }
      }
    }
  }

  try{
    return ((memTotal - memFree) / memTotal);
  }catch(...){
    return 1;
  }
  
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  // https://docs.fedoraproject.org/en-US/Fedora/18/html/System_Administrators_Guide/s2-proc-uptime.html
  long uptime, idletime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream lnstream(line);
    lnstream >> uptime >> idletime;
  }
  return uptime;
}

// DONE: Read and return the number of jiffies for the system
// INPUT: Process ID 
// OUTPUT: Returns the timing parametes of pid in Hz
long LinuxParser::ActiveJiffies(int pid) {
  //   https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat
  // https://man7.org/linux/man-pages/man5/proc.5.html
  string line;
  string var;
  string time; 
  vector<long int> v = {0};
  string utime, stime, cutime, cstime; 
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream lntream(line);
      for(int i = 1; i < 14; i++) {
        lntream >> var; 
      }
      for(int j = 1; j<=4; j++)
      {
        lntream >>time;
        v.push_back(stol(time));
      }

      //return accumulate(v.begin(),v.end(), 0);

        }
  }
 return accumulate(v.begin(),v.end(), 0);
}

// DONE: Returns the tot # of processes
int LinuxParser::TotalProcesses() {
  string key, line;
  int val;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream lntream(line);
      while (lntream >> key) { 
        if (key == "processes") {
          lntream >> val;
        }
      }
    }
  }
  return val;
}

// DONE: Returns the # of running processes
int LinuxParser::RunningProcesses() {
  string key, line;
  int val = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream lntream(line);
      while (lntream >> key) { //""
        if (key == "procs_running") {
          lntream >> val;
        }
      }
    }
  }
  return val;
}

// DONE: Returns the command associated with a process
string LinuxParser::Command(int pid) {
  string ln;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, ln);
  }
  return ln;
}

// DONE: Returns the memory used by a process
string LinuxParser::Ram(int pid) {
  string key, line;
int val = 1;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream lntream(line);
      while (lntream >> key) {
        if (key == "VmSize:") {
          lntream >> val;
          val = val / 1024;
        }
      }
    }
  }
   return to_string (val);;
}

// DONE: return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string key, _uid{"000"}, line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream lntream(line);
      while (lntream >> key ) {
        if (key == "Uid:") 
        {
          lntream >> _uid;
          break;
        }
      }
    }
  }
  return _uid;
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  // https://www.hackingarticles.in/editing-etc-passwd-file-for-privilege-escalation/ https://linuxize.com/post/etc-passwd-file/
  string line, usr, pwd, _uid;
  string _puid{LinuxParser::Uid(pid)};
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lnstream(line);
      while (lnstream >> usr >> pwd >> _uid) {
        if (_uid == _puid) 
          break;
      }
    }
  }
   return usr;
}

// DONE: Return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, var;
  long int starttime{0};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream lnstream(line);
      for(int i = 1; i < 22; i++) {
        lnstream >> var; 
      }
      lnstream >> starttime;
      
    }
  }
return starttime / sysconf(_SC_CLK_TCK); // https://man7.org/linux/man-pages/man5/proc.5.html /proc/[pid]/stat
}