#pragma once

#include <string>

// Returns the current local time, second precision, formatted as
// "YYYY-MM-DD HH:MM:SS" (e.g. "1900-08-21 01:23:34").
std::string getCurrentTimestamp();

// Returns the current local time, second precision, formatted as
// "YYYY-MM-DD-HH-MM-SS" (e.g. "1900-08-21-01-23-34"). Used as the
// timestamp component of generated order numbers.
std::string getCurrentTimestampDashed();
