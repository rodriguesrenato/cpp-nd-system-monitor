#include <cctype>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

#include<iostream>
using std::cout;
// TODO: Return this process's ID
int Process::Pid()
{
    return 0;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization()
{
    // cout << "test: ";
    // float b = Process::a_ +0.2;
    // cout << Process::a_;
    // return b;
    return 0.0;
}

// void Process::CpuUtilization(int activeJiffies,int idleJiffies) { CpuUtilization_ =
// (float)activeJiffies/(float)idleJiffies;}

// TODO: Return the command that generated this process
string Process::Command()
{
    return string();
}

// TODO: Return this process's memory utilization
string Process::Ram()
{
    return string();
}

// TODO: Return the user (name) that generated this process
string Process::User()
{
    return string();
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime()
{
    return 0;
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const &a [[maybe_unused]]) const
{
    return true;
}