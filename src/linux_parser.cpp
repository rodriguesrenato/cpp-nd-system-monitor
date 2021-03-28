#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <sstream>
#include <string>
#include <vector>

using std::stol;
using std::string;
using std::to_string;
using std::vector;

// Read and return Operating System name (pretty) from system files
string LinuxParser::OperatingSystem() {
    string line;
    string key;
    string value{};
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
    filestream.close();
    return value;
}

// Read and return Kernel information from /proc/version
string LinuxParser::Kernel() {
    string kernel{};
    string line;
    string os;
    string version;
    std::ifstream filestream(kProcDirectory + kVersionFilename);

    if (filestream.is_open()) {
        std::getline(filestream, line);
        std::istringstream linestream(line);
        linestream >> os >> version >> kernel;
    }
    filestream.close();
    return kernel;
}

// Read and return a vector of available pids from /proc folder
vector<int> LinuxParser::Pids() {
    vector<int> pids;
    DIR *directory = opendir(kProcDirectory.c_str());
    struct dirent *file;
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
    string line;
    string key;
    string value;
    long mem_total{0};
    long mem_free{0};
    float mem_utilization{0.0};

    // Read line by line of /proc/meminfo file
    std::ifstream filestream(kProcDirectory + kMeminfoFilename);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            linestream >> key >> value;

            // if key is found, convert it's value to int and store in the
            // respective int variable if any error ocurred during string to int
            // conversion, it will break filestream scan and return
            // mem_utilization 0.
            if (key == "MemTotal:") {
                try {
                    mem_total = stol(value);
                } catch (...) {
                    break;
                }
            }
            if (key == "MemFree:") {
                try {
                    mem_free = stol(value);
                } catch (...) {
                    break;
                }
            }

            // When both MemTotal and MemFree were found and were greater than
            // zero, calculate memory utilization percentage, otherwise
            // mem_utilization will be 0 as already initialized.

            if (mem_total > 0 && mem_free > 0) {
                mem_utilization = 1.0 - ((float)mem_free / (float)mem_total);
                break;
            }
        }
    }
    filestream.close();
    return mem_utilization;
}

// Read and return the system uptime from /proc/uptime.
// If a conversion error happens, just return uptime 0
long LinuxParser::UpTime() {
    string line;
    string uptimeStr;
    string idletimeStr;
    long uptime{0};

    std::ifstream filestream(kProcDirectory + kUptimeFilename);
    if (filestream.is_open()) {
        std::getline(filestream, line);
        std::istringstream linestream(line);
        linestream >> uptimeStr >> idletimeStr;
        try {
            uptime = stoi(uptimeStr);
        } catch (...) {
            return uptime;
        }
    }
    filestream.close();
    return uptime;
}

// Read and return the number of jiffies for the system.
// If a conversion error happens, just return totalJiffies 0
long LinuxParser::Jiffies() {
    vector<string> jiffiesVec = LinuxParser::CpuUtilization();
    long totalJiffies{0};

    // Sum up all cpu jiffies values
    for (string jiffies : jiffiesVec) {
        try {
            totalJiffies += stol(jiffies);
        } catch (...) {
            return 0;
        }
    }
    return totalJiffies;
}

// Read and return the number of active jiffies for a PID.
// If a conversion error happens, just return actveJiffies 0
long LinuxParser::ActiveJiffies(int pid) {
    string line;
    string key;
    string value;
    long activeJiffies{0};

    // Read /proc/[PID]/stat file and parse values
    std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                             kStatFilename);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);

            // Read only
            for (int i = 1; i < 18; i++) {
                linestream >> value;

                // Sum utime, stime, cutime and cstime (14th to 17th value)
                if (i >= 14 && i <= 17) {
                    try {
                        activeJiffies += stol(value);
                    } catch (...) {
                        return 0;
                    }
                }
            }
        }
    }
    filestream.close();
    return activeJiffies;
}

// Read and return the number of active jiffies for the system.
// If a conversion error happens, just return actveJiffies 0
long LinuxParser::ActiveJiffies() {
    long activeJiffies{0};

    // Get the vector of cpu jiffies
    vector<string> jiffiesVec = LinuxParser::CpuUtilization();

    // Sum up all cpu jiffies values that are not Idle or IOwait.
    for (unsigned int i = 0; i < jiffiesVec.size(); i++) {
        if (i != LinuxParser::CPUStates::kIdle_ &&
            i != LinuxParser::CPUStates::kIOwait_) {
            try {
                activeJiffies += stol(jiffiesVec[i]);
            } catch (...) {
                return 0;
            }
        }
    }
    return activeJiffies;
}

// Read and return the number of idle jiffies for the system.
// If a conversion error happens, just return actveJiffies 0
long LinuxParser::IdleJiffies() {
    long idleJiffies{0};

    // Get the vector of cpu jiffies
    vector<string> jiffiesVec = LinuxParser::CpuUtilization();

    // Sum Idle and IOwait cpu jiffies values
    for (unsigned int i = 0; i < jiffiesVec.size(); i++) {
        if (i == LinuxParser::CPUStates::kIdle_ ||
            i == LinuxParser::CPUStates::kIOwait_) {
            try {
                idleJiffies += stol(jiffiesVec[i]);
            } catch (...) {
                return 0;
            }
        }
    }
    return idleJiffies;
}

// Read and return CPU utilization from /proc/stat
vector<string> LinuxParser::CpuUtilization() {
    string line;
    string key;
    string value;
    vector<string> jiffiesVec{};

    // Read /proc/stat file and get cpu line
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);

            // Read the first value in the line, if it is equal to cpu, then
            // push all following values to the vector and break the filestream
            // loop
            linestream >> key;
            if (key == "cpu") {
                while (linestream >> value) {
                    jiffiesVec.push_back(value);
                }
                break;
            }
        }
    }
    filestream.close();
    return jiffiesVec;
}

// Read and return the total number of processes from /proc/stat
int LinuxParser::TotalProcesses() {
    string line;
    string key;
    string totalProcess{};

    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            linestream >> key;

            // Check if the first value of the line is processes, then assign
            // the next value to totalProcess.
            if (key == "processes") {
                linestream >> totalProcess;
                break;
            }
        }
    }
    filestream.close();

    // If a conversion error happens, just return 0.
    try {
        return stoi(totalProcess);
    } catch (...) {
        return 0;
    }
}

// Read and return the number of running processes from /proc/stat
int LinuxParser::RunningProcesses() {
    string line;
    string key;
    string procsRunning{};

    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            linestream >> key;

            // Check if the first value of the line is procs_running, then
            // assign the next value to totalProcess.
            if (key == "procs_running") {
                linestream >> procsRunning;
                break;
            }
        }
    }
    filestream.close();

    // If a conversion error happens, just return 0.
    try {
        return stoi(procsRunning);
    } catch (...) {
        return 0;
    }
}

// Read and return the command associated with a process from
// /proc/[pid]/cmdline file
string LinuxParser::Command(int pid) {
    string cmdline{};

    // Read and return the whole line from filestream
    std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                             kCmdlineFilename);
    if (filestream.is_open()) {
        std::getline(filestream, cmdline);
    }
    filestream.close();
    return cmdline;
}

// Read and return the memory used by a process from /proc/[pid]/status
string LinuxParser::Ram(int pid) {
    string line;
    string key;
    string value;
    string processRam{};

    // Read line by line of the filestream and look for "VmSize" param
    std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                             kStatusFilename);

    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            linestream >> key >> value;

            // if VmSize key is found, convert value to MB. If conversion fail,
            // return a empty string
            if (key == "VmSize:") {
                try {
                    processRam = to_string(stol(value) / 1000);
                } catch (...) {
                    return string();
                }
                break;
            }
        }
    }
    filestream.close();
    return processRam;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
    string line;
    string key;
    string value;
    string userUID{};

    // Read line by line of /proc/[pid]/status file
    std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                             kStatusFilename);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            linestream >> key >> value;

            // if Uid key is found, stop search and return value
            if (key == "Uid:") {
                userUID = value;
                break;
            }
        }
    }
    filestream.close();
    return userUID;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
    string line;
    string key;
    string value;
    string userUID;
    string username{};
    string encryptedPassword;

    // get the Uid of this process
    userUID = LinuxParser::Uid(pid);

    // find the correspondent Username in the /etc/passwd file
    std::ifstream filestream(kPasswordPath);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            //  prepare line for istringstream operations
            std::replace(line.begin(), line.end(), ':', ' ');

            std::istringstream linestream(line);

            linestream >> value >> encryptedPassword >> key;

            // if Uid is found, stop search and return value
            if (key == userUID) {
                username = value;
                break;
            }
        }
    }
    filestream.close();
    return username;
}

// Read and return the uptime of a process from /proc/[pid]/stat file
long LinuxParser::UpTime(int pid) {
    string line;
    string value;
    long processUpTime{0};

    // get system uptime
    long systemUpTime{LinuxParser::UpTime()};

    // Read /proc/[PID]/stat file and get it's 22th value (starttime)
    std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                             kStatFilename);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);

            for (int i = 0; i < 22; i++) {
                linestream >> value;
                if (i == 21) {
                    // Convert process starttime to long, divide it by system
                    // configuration _SC_CLK_TCK to get starttime in seconds and
                    // then subtract system uptime by the process start time. In
                    // case string to long conversion fail, return 0
                    try {
                        processUpTime =
                            systemUpTime - (stol(value) / sysconf(_SC_CLK_TCK));
                    } catch (...) {
                        return 0;
                    }
                }
            }
        }
    }
    filestream.close();
    return processUpTime;
}