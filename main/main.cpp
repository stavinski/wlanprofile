// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ProfileParser.h"

#pragma comment(lib, "wlanapi.lib") 

int main(int argc, WCHAR **argvv)
{

	HANDLE hClient = NULL;
	DWORD dwMaxClient = 2;       
	DWORD dwCurVersion = 0;
	DWORD dwResult = 0;
	DWORD dwRetVal = 0;
	ProfileParser *profileParser = NULL;

	PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
	PWLAN_INTERFACE_INFO pIfInfo = NULL;
	
	PWLAN_PROFILE_INFO_LIST pProfileList = NULL;
	PWLAN_PROFILE_INFO pProfile = NULL;

	dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
	if (dwResult != ERROR_SUCCESS) {
		wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
		// FormatMessage can be used to find out why the function failed
		return 1;
	}

	dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
	if (dwResult != ERROR_SUCCESS) {
		wprintf(L"WlanEnumInterfaces failed with error: %u\n", dwResult);
		// FormatMessage can be used to find out why the function failed
		return 1;
	}
	else
	{
		int iRet;
		WCHAR GuidString[40] = { 0 };

		wprintf(L"Current Index: %lu\n", pIfList->dwIndex);
		for (DWORD i = 0; i < (int)pIfList->dwNumberOfItems; i++) {
			pIfInfo = (WLAN_INTERFACE_INFO *)&pIfList->InterfaceInfo[i];
			iRet = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString, 39);
			wprintf(L"Interface: %s\n", pIfInfo->strInterfaceDescription);

			dwResult = WlanGetProfileList(hClient,
				&pIfInfo->InterfaceGuid,
				NULL,
				&pProfileList);

			if (dwResult != ERROR_SUCCESS) {
				wprintf(L"WlanGetProfileList failed with error: %u\n",
					dwResult);
				dwRetVal = 1;
				// You can use FormatMessage to find out why the function failed
			}
			else {
				wprintf(L"WLAN_PROFILE_INFO_LIST for this interface\n");

				wprintf(L"  Num Entries: %lu\n\n", pProfileList->dwNumberOfItems);

				for (DWORD j = 0; j < pProfileList->dwNumberOfItems; j++) {
					pProfile =
						(WLAN_PROFILE_INFO *)& pProfileList->ProfileInfo[j];

					wprintf(L"  Profile Name[%u]:  %ws\n", j, pProfile->strProfileName);

					wprintf(L"  Flags[%u]:\t    0x%x", j, pProfile->dwFlags);
					if (pProfile->dwFlags & WLAN_PROFILE_GROUP_POLICY)
						wprintf(L"   Group Policy");
					if (pProfile->dwFlags & WLAN_PROFILE_USER)
						wprintf(L"   Per User Profile");
					
					wprintf(L"\n");

					LPWSTR pstrProfileXml = NULL;
					DWORD dwFlags = WLAN_PROFILE_GET_PLAINTEXT_KEY; // makes sure xml includes plaintext network key
					DWORD dwGrantedAccess = NULL;

					dwResult = WlanGetProfile(hClient, &pIfInfo->InterfaceGuid, pProfile->strProfileName, NULL, &pstrProfileXml, &dwFlags, &dwGrantedAccess);
					
					if (dwResult != ERROR_SUCCESS)
					{
						wprintf(L"WlanGetProfile failed with error: %u\n",
							dwResult);
						dwRetVal = 1;
					}
					else
					{

						wprintf(L"  Profile Name:  %ws\n", pProfile->strProfileName);
						
						//wprintf(L"  Profile XML string:\n");
						//wprintf(L"%ws\n\n", pstrProfileXml);

						profileParser = new ProfileParser(pstrProfileXml);
						char *key;
						bool success = profileParser->Parse(&key);

						if (success)
						{
							wprintf(L"  Network Key:  %hs\n", key);
						}
						else
						{
							wprintf(L"  [network key not found]\n");
						}

						free(key);
												
						wprintf(L"  dwFlags:\t    0x%x", dwFlags);
						if (dwFlags & WLAN_PROFILE_GET_PLAINTEXT_KEY)
						    wprintf(L"   Get Plain Text Key");

						if (dwFlags & WLAN_PROFILE_GROUP_POLICY)
							wprintf(L"  Group Policy");

						if (dwFlags & WLAN_PROFILE_USER)
							wprintf(L"  Per User Profile");

						wprintf(L"\n");

						wprintf(L"  dwGrantedAccess:  0x%x", dwGrantedAccess);
						if (dwGrantedAccess & WLAN_READ_ACCESS)
							wprintf(L"  Read access");
						if (dwGrantedAccess & WLAN_EXECUTE_ACCESS)
							wprintf(L"  Execute access");
						if (dwGrantedAccess & WLAN_WRITE_ACCESS)
							wprintf(L"  Write access");
						wprintf(L"\n");

						wprintf(L"\n");

					}
				}
			}

		}
	}

	printf("Press any key to continue...");
	getchar();

	if (profileParser != NULL)
	{
		delete profileParser;
		profileParser = NULL;
	}

	if (pIfList != NULL) {
		WlanFreeMemory(pIfList);
		pIfList = NULL;
	}

	if (pProfileList != NULL)
	{
		WlanFreeMemory(pProfileList);
		pProfileList = NULL;
	}

	if (hClient != NULL)
	{
		WlanCloseHandle(hClient, NULL);
	}

    return 0;
}

