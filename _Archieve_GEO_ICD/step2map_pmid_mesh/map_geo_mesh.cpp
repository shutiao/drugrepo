#include <sstream>
#include <curl/curl.h>
#include <dirent.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

void read_file(istream & in, string geo, vector<string>  & geo_pubmed_unit);
void map_PMID_mesh(string pubmed_id, vector<string> & geo_pubmed_unit);
void write_data(ostream & out, vector<vector<string> > & geo_pubmed);
void print_vector(vector<vector<string> > & v);
vector<string> open(string path);
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
string httpget(string url);


int main(){
    ifstream fin;
    vector<vector<string> >geo_pubmed;
    vector<string> f = open(".");
    
    for ( int i = 0; i < f.size(); i++){
        string file_name = f[i];
        fin.open(file_name.c_str(), ios::in);
        
        if(!fin.is_open()){
            cerr<<"Unable to open file " << file_name << endl;
            exit(10);
        }
        else{
            cout << "Open file " << file_name << " successfuly!\n";
        }
        
        vector<string> *geo_pubmed_unit;
        geo_pubmed_unit = new vector<string>;
        read_file(fin, file_name, *geo_pubmed_unit);
        geo_pubmed.push_back(*geo_pubmed_unit);
        
        fin.close();
    }
    
    // print_vector int_vector(geo_pubmed);
    ofstream fout;
    fout.open("map_geo_pubmed_table", ios::out);
    if(!fout.is_open()){
        cerr << "Unable to open file map_geo_pubmed_table." << endl;
        exit(10);
    }
    write_data(fout, geo_pubmed);
    fout.close();
    return 0;
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

void read_file(istream & in, string geo, vector<string> & geo_pubmed_unit){
    geo_pubmed_unit.push_back(geo.substr(0, geo.find('_')) );
    
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
            //geo_pubmed_unit.push_back(pubmed_id);
            cout << "push a pubmed_id: " << pubmed_id << endl;
            // a function should be here.
            map_PMID_mesh(pubmed_id, geo_pubmed_unit);
        }
    }
}


void map_PMID_mesh(string pubmed_id, vector<string> & geo_pubmed_unit){
    string fullurl = "http://www.ncbi.nlm.nih.gov/pubmed?term=" + pubmed_id + "&report=medline";
    istringstream webpage (httpget(fullurl));
    string line;
    while ( getline(webpage, line) ){
        if ( line.substr(0,3) == "MH " ){
            string meshterm = line.substr(6, line.length() - 6);
            if ( meshterm[0] != '*'){
	      geo_pubmed_unit.push_back(meshterm.substr(0, meshterm.find('/')));
	    }
            cout << "push a meshterm: " << meshterm << endl;
        }
    }
    cout << endl;
}

void print_vector(vector<vector<string> >&v){
    for (int i = 0; i < v.size(); i++){
        for ( int j = 0; j < v[i].size(); j++ ){
            cout << v[i][j] << endl;
        }
        cout << endl;
    }
}

void write_data(ostream & out, vector<vector<string> > & geo_pubmed){
    for (int i = 0; i < geo_pubmed.size(); i++){
        //cout << i << " ";
        for ( int j = 1; j < geo_pubmed[i].size(); j++ ){
            //out << geo_pubmed[i][j] << '\t';
            out << geo_pubmed[i][0] << '\t';
            out << geo_pubmed[i][j] << endl;
        }
        //out << endl; //enable scrolloing
    }
}

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
