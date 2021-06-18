
#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// Define custom constructor
Process::Process(int pid)
    : pid_(pid),
      command_(LinuxParser::Command(pid)),
      user_(LinuxParser::User(pid)),
      ram_(LinuxParser::Ram(pid)),
      puptime_(LinuxParser::UpTime(pid)){
CpuUtilization();
      }

// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return process's CPU utilization
float Process::CpuUtilization() {
  float tot_time      = LinuxParser::ActiveJiffies(Pid());  
  float uptime        = LinuxParser::UpTime();                 
  float active_dur    = uptime - (Process::UpTime() / sysconf(_SC_CLK_TCK));  
  cpu_utilization      = (tot_time / sysconf(_SC_CLK_TCK)) / active_dur; 
  return cpu_utilization;
}

// DONE: Return the command that generated this process
string Process::Command() { return command_; }

// DONE: Return this process's memory utilization
string Process::Ram() { return ram_; }

// DONE: Return the user (name) that generated this process
string Process::User() { return user_; }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return puptime_; }

bool Process::operator<(Process const& a) const {
  return (this->cpu_utilization > a.cpu_utilization);
}