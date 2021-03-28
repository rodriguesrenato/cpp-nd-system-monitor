#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

#include <iostream>

// Return the system's CPU
Processor &System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process> &System::Processes() {
    // Get all system pids available now
    vector<int> pidsUpdated{LinuxParser::Pids()};

    // cached pids
    vector<int> pidsCached{};

    // loop through all cached process, from end to begin of the vector. Remove
    // processes that don't exist anymore
    for (int i = processes_.size() - 1; i >= 0; i--) {
        // If cached process pid is not in the updated pids, then remove it from
        // processes_ vector
        if (std::find(pidsUpdated.begin(), pidsUpdated.end(),
                      processes_[i].Pid()) == pidsUpdated.end()) {
            processes_.erase(processes_.begin() + i);
        }
        // otherwise add its pid to pidsCached vector
        else {
            pidsCached.push_back(processes_[i].Pid());
        }
    }

    // Add all updated processes that isnt in the processes_ vector
    for (int pid : pidsUpdated) {
        // if pid not found in pidsCached, then add a Process with this pid to
        // processes_ vector
        if (std::find(pidsCached.begin(), pidsCached.end(), pid) ==
            pidsCached.end()) {
            processes_.emplace_back(pid);
        }
    }

    // Update cpu utilization of all process in processes_
    for (Process &process : processes_) {
        process.CpuUtilization(LinuxParser::ActiveJiffies(process.Pid()),
                               LinuxParser::Jiffies());
    }

    // Sort processes by cpu utilization
    std::sort(processes_.begin(), processes_.end(), std::greater<Process>());

    return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long System::UpTime() { return LinuxParser::UpTime(); }