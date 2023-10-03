#include <fstream>
#include <sstream>
#include "common.h"

std::optional<std::string> LoadTextFile(const std::string &filename)
{
	std::ifstream fin(filename);
	if (!fin.is_open())
	{
		SPDLOG_ERROR("failed to open fime: {}", filename);
		return {};
	}
	std::stringstream text;
	text << fin.rdbuf();
	return text.str();
}