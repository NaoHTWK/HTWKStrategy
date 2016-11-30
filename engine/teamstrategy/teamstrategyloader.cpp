#include "teamstrategyloader.h"

#define ROBOT_MAX_AMOUNT 6;

vector<string> TeamStrategyLoader::split(const string &s, char splitter) {
    vector<string> buff;
    stringstream ss(s);
    string item;
    while (getline(ss, item, splitter)) {
        buff.push_back(item);
    }
    return buff;
}

string TeamStrategyLoader::append_number(string string1,string string2, int num, char sep) {
    stringstream ss;
    ss << string1 << sep << num << string2;
    return ss.str();
}

vector<vector<Position> > TeamStrategyLoader::load(const string &path){
    string line;
    string property="",cmp;
    vector<string> buffer, bufferPos;
    vector<vector<Position> > tacticsList;
    stringstream strToNum;

    ifstream file (path.c_str());
    if (file.is_open()){
        for (int i=0;i<6;i++){
            vector<Position> positionSet;
            for (int j=0;j<=(i+1);j++){
                if (j==0){
                    cmp=append_number("GOALY","",(i+1));
                }else{
                    cmp=append_number("POSITION","",(i+1));
                    cmp=append_number(cmp,"",j);
                }
                //cout << cmp + "\n";
                while(property.compare(cmp) != 0){
                    getline(file,line);
                    buffer=split(line,'=');
                    property=buffer[0];
                }

                bufferPos=split(buffer[1],',');
                vector<float> pos(3);
                for (int k=0;k<3;k++){
                    //cout << bufferPos[k] << " -> ";
                    strToNum.str(string());
                    strToNum.clear();
                    strToNum << bufferPos[k];
                    strToNum >> pos[k];
                    //cout << pos[k] << "\n";
                }
                positionSet.push_back(Position(pos[0],pos[1],pos[2]));
            }
            tacticsList.push_back(positionSet);
        }
        file.close();
    }else{
        cout << "fail\n";
    }
    return tacticsList;
}

vector<vector<vector<Position> > > TeamStrategyLoader::getKickoffList(){
    return kickoffList;
}
vector<vector<vector<Position> > > TeamStrategyLoader::getOffensiveList(){
    return offensiveList;
}
vector<vector<vector<Position> > > TeamStrategyLoader::getDefensiveList(){
    return defensiveList;
}

/*
 * Problem!~Beispiel: 0-2 Kickoff-Strategy -> 0 oder 1 wird gelöscht. -> 1 oder 2 wird nicht mehr betrachtet!!
 */
TeamStrategyLoader::TeamStrategyLoader() {
    bool finish,isEmpty;
    int counter,failCounter; // failCounter allows deleted tactics
    string path;

    for (int modus=0;modus<3;modus++){
        finish=false; isEmpty=true;
        counter=0; failCounter=0;
        do{
            if (modus == 0){
                path=append_number("tactics/kickoff",".conf",counter);
            }else if (modus == 1){
                path=append_number("tactics/defensive",".conf",counter);
            }else{
                path=append_number("tactics/offensive",".conf",counter);
            }
            //cout << path << "\n";
            ifstream file (path);
            if (file.is_open()){
                file.close();
                if (modus == 0){
                    kickoffList.push_back(load(path));
                }else if (modus == 1){
                    defensiveList.push_back(load(path));
                }else{
                    offensiveList.push_back(load(path));
                }
                isEmpty=false;
            }else{
                failCounter++;
                if (failCounter == 5){
                    finish=true;
                }
            }
            counter++;
        }while(!finish);
        if (finish && isEmpty){
            cout << modus << ": no tactics found!\n";
            //default oder einfach nur fehler ausgabe?
        }
    }
}
