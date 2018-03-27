#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iterator>
#include <iomanip>

#include "StarMap.h"
#include "cli.h"

using namespace std;

void startCli(StarMap &map) {
  cout << map.getNumberOfSystems() << " surveyed systems found over "
       << map.getNumberOfSectors() << " sectors." << endl;

  vector<Resource> memory;

  while (true) {
    cout << endl << "-> ";
    string command;
    getline(cin, command);
    cout << endl;

    istringstream iss(command);
    vector<string> result{
      istream_iterator<string>(iss), {}
    };

    for (unsigned int i = 0; i < result.size(); i++) {
      for (unsigned int j = 0; j < result.at(i).length(); j++)
        result.at(i)[j] = tolower(result.at(i)[j]);
    }

    if (result.at(0) == "find") {
      if (result.size() == 2) {
        if (result.at(1).find("ringworld") != string::npos) {
          findRingworld(map);
        } else {
          cout << "Invalid parameter '" << result.at(1) << "'" << endl;
        }
      } else if (result.size() == 3) {
        if (result.at(1) == "best") {
          if (result.at(2) == "all") {
            findAllBest(memory, map);
          } else {
            findBestResource(memory, result.at(2), map);
          }
        } else {
          cout << "Invalid parameters for command 'find'" << endl;
        }
      } else {
        cout << "Invalid parameters for command 'find'" << endl;
      }
    } else if (result.at(0) == "memory") {
      if (result.size() == 1) {
        vector<string> parameters = {"Galaxy","Sector","System","Planet","Zone","Name","Quality","Abundance","Selection"};
        vector<vector<string>> res = generateResourceTable(memory.size(), parameters, memory, map);
        displayTable(res, 2);
      } else if (result.size() == 2) {
        if (result.at(1) == "clear") {
          memory.clear();
          cout << "Memory Cleared." << endl;
        }
      }
    } else if (result.at(0) == "exit") {
      return;
    } else if (result.at(0) == "help") {
      if (result.size() == 2) {
        help(result.at(1));
      } else if (result.size() == 1) {
        help("&^none");
      } else {
        cout << "Invalid parameters." << endl;
      }
    } else {
      cout << "Invalid command." << endl;
    }
  }
}

void help(string command) {
  if (command == "&^none") {
    cout << "Available commands: " << endl << endl
         << "find       Finds objects and resources with quality constraints." << endl
         << "exit       Ends the program." << endl << endl
         << "Type help <command> for detail." << endl;
  } else if (command == "find") {
    cout << "find <best, 'ringworld'> <resource, all>" << endl << endl
         << "Examples:" << endl
         << "'find best coal' lists the highest quality coal in the map." << endl
         << "'find best all' lists all of the best resources in the map." << endl
         << "'find ringworld' lists all of the ringworlds in the map." << endl;
  } else if (command == "exit") {
    cout << "Exits the program." << endl;
  } else if (command == "help") {
    cout << "Calm down, we've called the police." << endl;
  } else {
    cout << "No such command." << endl;
  }
}
void detail(Resource resource, StarMap &map) {

}

void findRingworld(StarMap &map) {
  vector<ringworld> bestResults;
  bestResults = map.getRingworlds();
  vector<vector<string>> dispText;
  vector<string> temp1 = {"Galaxy","Sector","System", "Name", "Arcs"};
  dispText.push_back(temp1);
  for (unsigned int i = 1; i < bestResults.size() + 1; i++) {
    vector<string> temp;
    for (unsigned int j = 0; j < dispText.at(0).size(); j++) {
      temp.push_back(map.getRingworldParameter(bestResults.at(i - 1), dispText.at(0).at(j)));
    }
    dispText.push_back(temp);
  }
  if (dispText.size() == 1) {
    cout << "No ringworlds found." << endl;
    return;
  }
  displayTable(dispText, 3);
}

void findBestResource(vector<Resource> &memory, string resource, StarMap &map) {
  vector<Resource> resources = map.findBestResource(resource);
  for (unsigned int i = 0; i < resources.size(); i++) {
    bool found = false;
    if (memory.size() > 0 ) {
      for (unsigned int j = 0; j < memory.size(); j++) {
        found = resources.at(i).identifier == memory.at(j).identifier;
        if (found) {
          resources.at(i).selection = memory.at(j).selection;
          break;
        }
      }
    }
    if (!found) {
      resources.at(i).selection = memory.size() + 1;
      memory.push_back(resources.at(i));
    }
  }

  vector<string> parameters = {"Galaxy","Sector","System", "Planet", "Zone", "Quality", "Abundance", "Selection"};
  vector<vector<string>> dispText = generateResourceTable(memory.size(), parameters, resources, map);
  if (dispText.size() == 1) {
    cout << "Resource not found." << endl;
    return;
  }
  displayTable(dispText, 3);
}

void findAllBest(vector<Resource> &memory, StarMap &map) {
  vector<Resource> resources = map.getAllBestResources();
  for (unsigned int i = 0; i < resources.size(); i++) {
    bool found = false;
    if (memory.size() > 0 ) {
      for (unsigned int j = 0; j < memory.size(); j++) {
        found = resources.at(i).identifier == memory.at(j).identifier;
        if (found) {
          resources.at(i).selection = memory.at(j).selection;
          break;
        }
      }
    }
    if (!found) {
      resources.at(i).selection = memory.size() + 1;
      memory.push_back(resources.at(i));
    }
  }

  vector<string> parameters = {"Name", "Quality", "Selection"};
  vector<vector<string>> dispText = generateResourceTable(memory.size(), parameters, resources, map);
  if (dispText.size() == 1) {
    cout << "No resources found." << endl;
    return;
  }
  displayTable(dispText, 3);
}

vector<vector<string>> generateResourceTable(int currentMemory, vector<string> parameters, vector<Resource> bestResults, StarMap &map) {
  vector<vector<string>> dispText;
  dispText.push_back(parameters);
  for (unsigned int i = 1; i < bestResults.size() + 1; i++) {
    vector<string> temp;
    for (unsigned int j = 0; j < dispText.at(0).size(); j++) {
      if ((parameters.back() == "Selection") && j == (dispText.at(0).size() - 1)) {
        string selection = "[" + to_string(bestResults.at(i - 1).selection) + "]";
        temp.push_back(selection);
        currentMemory++;
      } else {
        temp.push_back(map.getResourceParameter(bestResults.at(i - 1), dispText.at(0).at(j)));
      }
    }
    dispText.push_back(temp);
  }

  return dispText;
}

void displayTable(vector<vector<string>> dispText, int separation) {
  vector<int> maxWidth(dispText.at(0).size());
  for (unsigned int i = 0; i < dispText.at(0).size(); i++) {
    for (unsigned int j = 0; j < dispText.size(); j++) {
      if (dispText.at(j).at(i).length() > maxWidth.at(i))
        maxWidth.at(i) = dispText.at(j).at(i).length();
    }
  }

  cout << left;

  for (unsigned int i = 0; i < dispText.size(); i++) {
    for (unsigned int j = 0; j < dispText.at(0).size(); j++) {
      cout << setw(maxWidth.at(j) + separation) << dispText.at(i).at(j);
    }
    cout << endl;
  }
}
