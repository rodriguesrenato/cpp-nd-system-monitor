#include "process.h"

#include <cctype>
#include <string>

#include "linux_parser.h"

using std::string;

// Return this process's ID
int Process::Pid() const { return Process::pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const { return Process::cpuUtilization_; }

// Calculate this process's CPU utilization
void Process::CpuUtilization(long activeJiffies, long totalJiffies) {
    float cpuUtilization{0};

    // Only update cached values and calculate cpu utilization if both total and
    // active Jiffies passed are valid
    if (totalJiffies > 0 && activeJiffies > 0) {
        // calculate the difference from current to previous value
        long activeDiff = activeJiffies - Process::activeJiffiesPrev_;
        long totalDiff = totalJiffies - Process::totalJiffiesPrev_;

        // Update previous values stored in class
        Process::activeJiffiesPrev_ = activeJiffies;
        Process::totalJiffiesPrev_ = totalJiffies;

        cpuUtilization = (float)activeDiff / (float)totalDiff;
        // calculate cpu utilization and update cpuUtilization_
    }
    Process::cpuUtilization_ = cpuUtilization;
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Process::pid_); }

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Process::pid_); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Process::pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Process::pid_); }

// Operator "less than" is overloaded to compare cpuUtilization_ value
bool Process::operator>(Process const &a) const {
    return Process::cpuUtilization_ > a.cpuUtilization_;
}