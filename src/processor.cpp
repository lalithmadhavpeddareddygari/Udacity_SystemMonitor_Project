#include "processor.h"
#include "linux_parser.h"
using std::stol;

void Processor::updateUtils(std::vector<std::string> v){
        Processor::utils = v;
    }

// To Return the aggregate CPU utilization
float Processor::Utilization() { 
    //https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
    long prevIdle,idle,prevNonIdle,nonIdle,prevTotal,total,totalDiff,idleDiff;
    std::vector<std::string>currentUtils = LinuxParser::CpuUtilization();
   

    prevIdle = stol(utils[3])+stol(utils[4]);
    idle = stol(currentUtils[3])+stol(currentUtils[4]);
    prevNonIdle = stol(utils[0])+stol(utils[1])+stol(utils[2])+stol(utils[5])+stol(utils[6])+stol(utils[7]);
    nonIdle =   stol(currentUtils[0])+stol(currentUtils[1])+stol(currentUtils[2])+stol(currentUtils[5])+stol(currentUtils[6])+stol(currentUtils[7]);

    prevTotal = prevIdle+prevNonIdle;
    total = idle+nonIdle;
    // Difference in utilization : current time - previous time
    totalDiff = total-prevTotal;
    idleDiff = idle - prevIdle;
   
    Processor::utilization = (1.0*(totalDiff-idleDiff))/(totalDiff);
    updateUtils(currentUtils);
    return Processor::utilization; }



