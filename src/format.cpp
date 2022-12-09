#include <string>

#include "format.h"

using std::string;

// Function that converts seconds to HH:MM::SS format
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
long hr = seconds/3600;
seconds = seconds%3600;
long min = seconds/60;
seconds = seconds%60;
long s = seconds;
string temp = std::to_string(hr)+":"+std::to_string(min)+":"+std::to_string(s);
return temp; }
