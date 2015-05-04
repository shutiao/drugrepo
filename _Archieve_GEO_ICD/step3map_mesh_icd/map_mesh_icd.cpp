#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

string getExactMeshTerm(string meshSentence);
string getCUI(string meshTerm);
string fetchICD(string CUI);

int main(){
  ifstream fin;
  string meshSentence = "Animals";
  //string meshSentence = "Keratosis, Actinic/*genetics/metabolism/pathology";
  string query = getExactMeshTerm(meshSentence);
  cout << "getExactMeshTerm: " << query << endl;
  string CUI = getCUI(query);
  cout << "CUI is " << CUI << endl;
  string ICD9CM = fetchICD(CUI);
  fin.close();
  return 0;
}

string fetchICD(string CUI){
  string ICD9CM = "NA";
  ifstream fin;
  fin.open("mrconso.txt", ios::in);
  string line;
  getline(fin, line);
  while( !fin.eof() ){
    getline(fin, line);
    if (line.substr(0, 8) == CUI){
      if (line.substr(9,3) == "ICD") {
	ICD9CM = "";
	for ( int i = 16; i < line.length(); i++){
	  if(line[i] == '\t'){
	    break;
	  }
	  else{
	    ICD9CM = ICD9CM + line[i];
	    cout << "ICD9CM is updated to " << ICD9CM << endl;
	  }
	}
	cout << "find ICD9CM:" << ICD9CM << endl;
	break;
      }
    }
  }
  fin.close();
  return ICD9CM;
}

// match mesh
string getCUI(string meshTerm){
    string CUI;
    ifstream fin;
    fin.open("mrconso.txt", ios::in);
    string line;
    getline(fin, line);
    while( !fin.eof() ){
        string getMesh;
        getline(fin, line);
        if ( line[21] == '\"' ) {
            getMesh = line.substr(22);
        }
        else{
            getMesh = line.substr(21);
        }
	// XiuZheng
        //int found = getMesh.find(meshTerm);
        //if ( found != string::npos ){
        if( meshTerm == getMesh.substr(0, meshTerm.length())){    
	    cout << "Find MESH term match to th equery: " << endl << line << endl;
	    CUI = line.substr(0, line.find('\t'));
	    cout << "getCUI is " << CUI << endl;
            break;
        }
    }
    fin.close();
    return CUI;
}
  

string getExactMeshTerm(string meshSentence){
  int index = meshSentence.length();;
  for ( int i = 0; i < meshSentence.length(); i++){
    if ( meshSentence[i] == '/' ){
      index = i;
      break;
    }
  }
  return (meshSentence.substr(0,index));
}
