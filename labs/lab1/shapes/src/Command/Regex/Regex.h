#ifndef REGEX_H
#define REGEX_H

#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

namespace commands
{

inline std::vector<std::string> ParseRegex(const std::string& input, const std::regex& regexPattern)
{
	std::smatch matches;
	if (!std::regex_match(input, matches, regexPattern))
	{
		throw std::runtime_error("Invalid input format");
	}

	std::vector<std::string> result;
	for (size_t i = 1; i < matches.size(); ++i)
	{
		result.push_back(matches[i].str());
	}
	return result;
}

} // namespace commands

#endif /* REGEX_H */
