#include "stdafx.h"

#include "ProfileParser.h"

ProfileParser::ProfileParser(LPWSTR xml)
{
	_xml = xml;
}

ProfileParser::~ProfileParser()
{
	delete _xml;
}

bool ProfileParser::Parse(char **pszNetworkKey)
{
	tinyxml2::XMLDocument *xmlDOM = new tinyxml2::XMLDocument();
	
	size_t outputSize = wcslen(_xml) + 1;
	char *xml = new char[outputSize];
	size_t converted = 0;
	wcstombs_s(&converted, xml, outputSize, _xml, wcslen(_xml));
	tinyxml2::XMLError res = xmlDOM->Parse(xml, converted);

	if (res != tinyxml2::XMLError::XML_SUCCESS)
	{
		return false;
	}
	
	tinyxml2::XMLElement *sharedKeyElm = xmlDOM->RootElement()
		->FirstChildElement("MSM")
		->FirstChildElement("security")
		->FirstChildElement("sharedKey");

	if (sharedKeyElm == NULL)
		return false;

	tinyxml2::XMLElement *keyElm = sharedKeyElm->FirstChildElement("keyMaterial");
	
	if (keyElm == NULL)
		return false;
	
	const char* elmText = keyElm->GetText();
	size_t len = strlen(elmText) + 1;
	*pszNetworkKey = new char[len];
	strcpy_s(*pszNetworkKey, len, elmText);
	
	return true;
}
