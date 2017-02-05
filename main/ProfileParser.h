#include "includes\tinyxml2.h"

#pragma once
class ProfileParser
{

private:
	LPWSTR _xml;

public:
	ProfileParser(LPWSTR xml);
	~ProfileParser();

	bool Parse(char **pszNetworkKey);
};

