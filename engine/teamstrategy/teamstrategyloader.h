#ifndef TEAMSTRATEGYLOADER_H_
#define TEAMSTRATEGYLOADER_H_
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <position.h>

using namespace std;

class TeamStrategyLoader {
public:
	TeamStrategyLoader();
	vector<vector<Position> > load(const string &path);
	vector<string> split(const string &s, char splitter);
	string append_number(string string1,string string2, int num, char sep = '_');
	vector<vector<vector<Position> > > getKickoffList();
	vector<vector<vector<Position> > > getOffensiveList();
	vector<vector<vector<Position> > > getDefensiveList();

private:
	vector < vector< vector<Position> > > kickoffList;
	vector < vector< vector<Position> > > defensiveList;
	vector < vector< vector<Position> > > offensiveList;
};

#endif /* TEAMSTRATEGYLOADER_H_ */
