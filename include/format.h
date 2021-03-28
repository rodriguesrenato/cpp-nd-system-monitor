#ifndef FORMAT_H
#define FORMAT_H

#include <string>

// Helper functions to format data to show on ncursedisplay
namespace Format {
std::string ElapsedTime(long times);
std::string StrClean(std::string, unsigned int length);
};  // Namespace Format

#endif