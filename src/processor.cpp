#include "linux_parser.h"
#include "processor.h"
#include <string>
#include <vector>


// TODO: Return the aggregate CPU utilization
float Processor::Utilization()
{
    // Processor::cpuJiffs_ = LinuxParser::CpuUtilization();

    // get current jiffies values
    long activeJiffies = LinuxParser::ActiveJiffies();
    // long idleJiffies = LinuxParser::IdleJiffies();
    long totalJiffies = LinuxParser::Jiffies();

    // calculate difference from current to previous values
    long totalDiff = totalJiffies - Processor::totalJiffiesPrev_;
    long activeDiff = activeJiffies - Processor::activeJiffiesPrev_;

    // Update previous values stored in class
    Processor::totalJiffiesPrev_ = totalJiffies;
    Processor::activeJiffiesPrev_ = activeJiffies;

    // Return cpu utilization
    return (float)activeDiff/(float)totalDiff;
}