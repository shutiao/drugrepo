#include <sstream>
#include <curl/curl.h>
#include <dirent.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

void read_file(string file_name, ostream & out, vector<string>  & gse_pubmed_unit);
void map_PMID_mesh(string pubmed_id, vector<string> & gse_pubmed_unit, ostream & out);
vector<string> open(string path);
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
string httpget(string url);


int main(){
    // output to a file
    ofstream fout;
    fout.open("map_gse_mesh_table", ios::out);
    if(!fout.is_open()){
        cerr << "Unable to open file map_gse_pubmed_table." << endl;
        exit(10);
    }
    // a 2D vector stores all the GSE Follow-Up data for further processing
    vector<vector<string> >gse_pubmed;
    // a 1D vector stores all the file name in this specific directory.
    vector<string> f = open(".");
    // read each file in the specific directory.
    for ( int i = 0; i < f.size(); i++){
        // create a dynamic sub vector holding the current GSE Data
        vector<string> *gse_pubmed_unit;
        gse_pubmed_unit = new vector<string>;
        read_file(f[i], fout, *gse_pubmed_unit);
        gse_pubmed.push_back(*gse_pubmed_unit);
        delete gse_pubmed_unit;
    }
    fout.close();
    return 0;
}



void read_file(string file_name, ostream & out, vector<string> & gse_pubmed_unit){
  // truncate the file name to fetch the prefix GSE number
  gse_pubmed_unit.push_back(file_name.substr(0, file_name.find('_')) );
  //out << gse_pubmed_unit[0];
  cout << "read the file: " << gse_pubmed_unit[0] << endl;
  ifstream fin;
  fin.open(file_name.c_str(), ios::in);
  while( !fin.fail() ){
    string line;
    getline(fin,line);
    //int found_dt = line.find("!Series_pubmed_id");
    if ( line.substr(0, 17) == "!Series_pubmed_id" ){
      //if( found_dt != string::npos ){
      string pub_line = line;
      int index = 0;
      for (int i = 0; i < pub_line.length(); i++){
	if ( pub_line[i] == '\t' ){
	  index = i;
	}
      }
      // Parse the PubMed ID
      string pubmed_id = pub_line.substr(index + 2, pub_line.length() - index -3);
      cout << "push a pubmed_id: " << pubmed_id << endl;
      // Mapping from PMID to MESH
      map_PMID_mesh(pubmed_id, gse_pubmed_unit, out);
    }
  }
  fin.close();
}


void map_PMID_mesh(string pubmed_id, vector<string> & gse_pubmed_unit, ostream & out){
    string fullurl = "http://www.ncbi.nlm.nih.gov/pubmed?term=" + pubmed_id + "&report=medline";
    istringstream webpage (httpget(fullurl));
    string line;
    while ( getline(webpage, line) ){
      if ( line.substr(0,3) == "MH " ){
	string meshterm = line.substr(6, line.length() - 6);
	if ( meshterm[0] != '*'){
	  string exactMeshTerm = meshterm.substr(0, meshterm.find('/'));
	  gse_pubmed_unit.push_back(exactMeshTerm);
	  out << gse_pubmed_unit[0] << '\t';
	  out << exactMeshTerm << endl;
	  cout << "push a meshterm: " << meshterm << endl;
	}
      }
    }
    cout << endl;
}

/*
 // open file log
if(!fin.is_open()){
    cerr<<"Unable to open file " << file_name << endl;
    exit(10);
    }
    else{
        cout << "Open file " << file_name << " successfuly!\n";
    }
    
*/


// open the file under the directory
vector<string> open(string path = "."){
    DIR* dir;
    dirent* pdir;
    vector<string> files;
    
    dir = opendir(path.c_str());
    while ((pdir = readdir(dir))){
        string filename = pdir->d_name;
        //if ( filename.substr(0,3) == "GSE" && filename.substr(filename.length()-4,3) == "txt"){
        if ( filename[0] != '.' and filename[0] == 'G'){
            files.push_back(pdir->d_name);
            cout << "push a filename: " << filename << endl;
        }
    }
    return files;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

string httpget(string url){
    CURL *curl;
    CURLcode res;
    string readBuffer;
    
    curl = curl_easy_init();
    
    if (curl){
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return readBuffer;
}
