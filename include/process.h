#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
   public:
    Process(int const pid) : pid_(pid) {}
    int Pid() const;
    std::string User();
    std::string Command();
    float CpuUtilization() const;
    void CpuUtilization(long activeJiffies, long totalJiffies);
    std::string Ram();
    long int UpTime();
    bool operator>(Process const &a) const;

   private:
    int pid_;
    float cpuUtilization_{0.0};
    long totalJiffiesPrev_{0};
    long activeJiffiesPrev_{0};
};

#endif