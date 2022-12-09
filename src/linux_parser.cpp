#include <dirent.h>
#include <unistd.h>
#include <cstdio>
#include <sstream>
#include <string>
#include<iostream>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// To truncated the decimals of the RAM for aesthetic appearance of the monitor
std::string reduce(float val, int d) {
    std::ostringstream s;
    s.precision(d);
    s << val;
    return s.str();
}
// To read data from the filesystem for OS
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
  return value;
}

// To read data from the filesystem for Kernel
string LinuxParser::Kernel() {
  string os, kernel, version;
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

// To Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line;
  string key;
  string value;
  float memTotal;
  float memFree;
  float memUtil;
  std::ifstream filestream(kProcDirectory+kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          memTotal=std::stof(value);
        }
        else if (key == "MemFree")
        {
          memFree = std::stof(value);
        }
        
      }
    }
  }
  memUtil = (memTotal - memFree)/memTotal;
  return memUtil;
   }

// To Read and return the system uptime
long LinuxParser::UpTime() { 
  string uptime;
  string line;
  string idletime;
  std::ifstream stream(kProcDirectory+kUptimeFilename);

  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream>>uptime>>idletime;
  }
  
  return std::stol(uptime); }
// https://knowledge.udacity.com/questions/129844
// To Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  
  return (LinuxParser::UpTime() / ((sysconf(_SC_CLK_TCK)))); }

// To Read and return the number of active jiffies for a PID

long LinuxParser::ActiveJiffies(int pid) { 
  long activeJiffies=0;
  string line;
  vector<string> values;
  string value;
  
  std::ifstream stream(kProcDirectory+std::to_string(pid)+kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
      std::istringstream linestream(line);
      while (linestream >> value) {
        values.emplace_back(value);
      }
  }
  
  
activeJiffies= std::stol(values[13])+std::stoi(values[14])+std::stoi(values[15])+std::stoi(values[16]);
return activeJiffies;}

// To Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long activeJiffs;
  string value;
  string line;
  string key;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream>>key;
    while(linestream>>value){
      activeJiffs+=std::stol(value);
    }

  }  
  return activeJiffs/(sysconf(_SC_CLK_TCK)); }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  long idleJiffs;
  string value;
  vector<string> values;
  string line;
  string key;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream>>key;
    while(linestream>>value){
      values.emplace_back(value);
    }
  idleJiffs=std::stol(values[5])+std::stol(values[6]);
  }  
  return idleJiffs; }


// To Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string key;
  string value;
  string line;
  vector<string> cpuUtils;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);

    linestream>>key;
    while(linestream){
      linestream>>value;
      cpuUtils.emplace_back(value);
      
    }
  }


  
  return cpuUtils; }

// To read and return CPU utilization of a pid 
 float LinuxParser::CpuUtilization(int pid){
 // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  float cpuUtil;
  long total_time = LinuxParser::ActiveJiffies(pid);
  float seconds = LinuxParser::UpTime()- LinuxParser::UpTime(pid);
  cpuUtil = ((total_time*100.0)/seconds);
return cpuUtil;
 }

// To Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  int TP;
  string key;
  string value;
  string line;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          TP=std::stoi(value);
          break;
        }
        
      }
    }
  }
  
  
  return TP; }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  int RP;
  string key;
  string value;
  string line;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          RP=std::stoi(value);
          break;
        }
        
      }
    }
  }
  
  return RP; }

// To Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
string cmd;
std::ifstream stream(kProcDirectory+std::to_string(pid)+kCmdlineFilename);
if(stream.is_open()){
  std::getline(stream,cmd);
}

return cmd; }

// To Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  
  string ram;
  string key;
  string line;
  string value;
  float val = 0.0f;
  
  std::ifstream stream(kProcDirectory+std::to_string(pid)+kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          val= std::stol(value);
          val/=1000;
          ram=reduce(val, 4);
          break;
        }
        
      }
    }
  
  }
  
  return ram;}

// To Read and return the user ID associated with a process

string LinuxParser::Uid(int pid) { 
  string uid;
  string key;
  string line;
  string value;
  
  std::ifstream stream(kProcDirectory+std::to_string(pid)+kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") {
          uid = value;
          break;
        }
        
      }
    }
  
  }

  return uid; }

// To Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string userName;
  string key;
  string line;
  string x;
  string value;
  string uID = LinuxParser::Uid(pid); 
  
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >>x>> value) {
        if (value == uID) {
          userName = key;
          break;
        }
        
      }
    }
  
  }

  return userName; }

// To Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  long upTime;
  string line;
  string value;
  int counter=0;
  std::ifstream stream(kProcDirectory+std::to_string(pid)+kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), '(', ' ');
      std::replace(line.begin(), line.end(), ')', ' ');
      std::istringstream linestream(line);
      while (linestream >> value) {
        if (counter==21) {
          upTime = std::stol(value)/sysconf(_SC_CLK_TCK);
          break;
        }
        counter++;
      }
    }
  

  }

return upTime; }

