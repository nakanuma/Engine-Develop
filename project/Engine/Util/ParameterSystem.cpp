#include "ParameterSystem.h"

bool ParameterManager::SaveToFile(const std::string& filename)
{
	nlohmann::json j;

	for (auto& p : params)
	{
		p->Save(j);
	}

	std::ofstream ofs(filename);

	if (!ofs.is_open())
	{
		return false;
	}

	ofs << j.dump(4);

	return true;
}

bool ParameterManager::LoadFromFile(const std::string& filename)
{
	std::ifstream ifs(filename);

	if (!ifs.is_open())
	{
		return false;
	}

	nlohmann::json j;
	ifs >> j;

	for (auto& p : params)
	{
		p->Load(j);
	}

	return true;
}
