#include "format.h"

#include <sstream>
#include <string>
using std::string;

// Format uptime in seconds to a string of HH:MM:SS, 
// with hours limited to 4 digits
string Format::ElapsedTime(long uptime) {
  int seconds = uptime % 60;
  int minutes = uptime / 60 % 60;
  int hours = uptime / 60 / 60 % 60;

  // Max of 4 digits for hours
  char buffer[10];
  std::snprintf(buffer, sizeof(buffer), "%02u:%02u:%02u", hours, minutes,
                seconds);

  return buffer;
}