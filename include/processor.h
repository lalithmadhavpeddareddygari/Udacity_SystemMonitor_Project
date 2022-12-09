#ifndef PROCESSOR_H
#define PROCESSOR_H
#include<vector>
#include<string>

class Processor {
 public:
  float Utilization();
  
    

  
 private:
 float utilization;
 std::vector<std::string> utils= {"0","0","0","0","0","0","0","0","0","0"};
 void updateUtils(std::vector<std::string> v);

};
#endif
