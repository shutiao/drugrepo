#include <dirent.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

void read_file(istream & in, string geo, vector<string>  & geo_pubmed_unit);
void write_data(ostream & out, vector<vector<string> > & geo_pubmed);
void print_vector(vector<vector<string> > & v);
vector<string> open(string path);

int main(){
  ifstream fin;
  vector<vector<string> >geo_pubmed;
  vector<string> f = open(".");
  //vector<string> f = open("/home/hxu/geo/__no_directories");
  //vector<string> f = open("/home/hxu/geo/Map_GEO_PubMed");
  //vector<string> f = open("/home/hxu/geo/test");
  
  for ( int i = 0; i < f.size(); i++){
    string file_name = f[i];
    fin.open(file_name.c_str(), ios::in);
    
    if(!fin.is_open()){
      cerr<<"Unable to open file " << file_name << endl;
      //exit(10);
    }
    else{
      cout << "Open file " << file_name << " successfuly!\n";
    }

    vector<string> geo_pubmed_unit;
    //vector<string> *geo_pubmed_unit;
    //geo_pubmed_unit = new vector<string>;
    read_file(fin, file_name, geo_pubmed_unit);
    geo_pubmed.push_back(geo_pubmed_unit);
    
    fin.close();
  }
  
  print_vector(geo_pubmed);
  ofstream fout;
  fout.open("map_geo_pubmed_table", ios::out);
  if(!fout.is_open()){
    cerr << "Unable to open file map_geo_pubmed_table." << endl;
    exit(10);
  }
  write_data(fout, geo_pubmed);
  return 0;
  }

 void read_file(istream & in, string geo, vector<string> & geo_pubmed_unit){
  geo_pubmed_unit.push_back(geo);

  while( !in.eof() ){
    string line;
    getline(in,line);
    
    int found_dt = line.find("!Series_pubmed_id");
    if( found_dt != string::npos ){
      string pub_line = line;
      int index = 0;
      for (int i = 0; i < pub_line.length(); i++){
	if ( pub_line[i] == '\t' ){
	  index = i;
	}
      }
      string pubmed_id = pub_line.substr(index + 2, pub_line.length() - index -3);
      geo_pubmed_unit.push_back(pubmed_id);
      cout << "push a pubmed_id: " << pubmed_id << endl;
    }
  }
}

void print_vector(vector<vector<string> >&v){
  for (int i = 0; i < v.size(); i++){
    //cout << i << " ";
    for ( int j = 0; j < v[i].size(); j++ ){
      cout << v[i][j] << '\t';
    }
    cout << endl;
  }
}

void write_data(ostream & out, vector<vector<string> > & geo_pubmed){
  for (int i = 0; i < geo_pubmed.size(); i++){
    //cout << i << " ";
    for ( int j = 0; j < geo_pubmed[i].size(); j++ ){
      out << geo_pubmed[i][j] << '\t';
    }
    out << endl;
  }
}

vector<string> open(string path = "."){
  DIR* dir;
  dirent* pdir;
  vector<string> files;

  dir = opendir(path.c_str());
  while (pdir = readdir(dir)){
    string filename = pdir->d_name;
    //if ( filename.substr(0,3) == "GSE" && filename.substr(filename.length()-4,3) == "txt"){
    if ( filename[0] != '.' and filename[0] == 'G'){
      files.push_back(pdir->d_name);
      cout << "push a filename: " << filename << endl;
      }
  }
  return files;
}
