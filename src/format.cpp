#include "format.h"

#include <sstream>
#include <string>

using std::string;

// Format upTime in seconds to a string of HH:MM:SS,
// with hours limited to 4 digits
string Format::ElapsedTime(long upTime) {
    int seconds = upTime % 60;
    int minutes = upTime / 60 % 60;
    int hours = upTime / 60 / 60 % 60;

    // Max of 4 digits for hours
    char buffer[10];

    // Format variables according to the format specified below,
    // minimum of 2 digits each and leading zeros.
    std::snprintf(buffer, sizeof(buffer), "%02u:%02u:%02u", hours, minutes,
                  seconds);

    return buffer;
}

// Add blank spaces if string size is less the specified length
// and if its greater, use substring from begin to get specified length
string Format::StrClean(string str, unsigned int length) {
    string formattedStr;

    if (str.length() > length) {
        formattedStr = str.substr(0, length - 1);
    } else {
        formattedStr = str;

        // Concatenate blank spaces till formattedStr get specified length
        for (unsigned int i = str.length(); i < length; i++) {
            formattedStr += " ";
        }
    }

    return formattedStr;
}