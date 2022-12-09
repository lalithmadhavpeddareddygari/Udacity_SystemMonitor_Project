#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include "linux_parser.h"
#include<iostream>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid): pid_(pid){};
// To: Return this process's ID
int Process::Pid() const { 
    return pid_;
    }

// To Return this process's CPU utilization
float Process::CpuUtilization() const { 
 return LinuxParser::CpuUtilization(Pid());
 }

// To Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

// To Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// To Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// To Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// To Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    
    return this->CpuUtilization()<a.CpuUtilization(); }
