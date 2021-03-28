#include "processor.h"

#include <string>
#include <vector>

#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
    float cpuUtilization{0};

    // Get active and total jiffies values
    long activeJiffies = LinuxParser::ActiveJiffies();
    long totalJiffies = LinuxParser::Jiffies();

    // calculate cpu utilization only if active and total jiffies are valid
    if (totalJiffies > 0 && activeJiffies > 0) {
        // calculate difference from current to previous values
        long totalDiff = totalJiffies - Processor::totalJiffiesPrev_;
        long activeDiff = activeJiffies - Processor::activeJiffiesPrev_;
        
        if (totalDiff > 0) {
            // Update previous values stored in class
            Processor::totalJiffiesPrev_ = totalJiffies;
            Processor::activeJiffiesPrev_ = activeJiffies;

            // calculate cpu utilization
            cpuUtilization = (float)activeDiff / (float)totalDiff;
        }
    }
    // Return cpu utilization
    return cpuUtilization;
}