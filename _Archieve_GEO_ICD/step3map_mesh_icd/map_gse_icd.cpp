#include <sstream>
#include <curl/curl.h>
#include <dirent.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

string getExactMeshTerm(string meshSentence);
string getCUI(string meshTerm);
string fetchICD(string CUI);
void read_file(istream & in, ostream & out, string geo, vector<string>  & geo_pubmed_unit);
void map_PMID_mesh(string pubmed_id, vector<string> & geo_pubmed_unit, ostream & out);
void write_data(ostream & out, vector<vector<string> > & geo_pubmed);
void print_vector(vector<vector<string> > & v);
vector<string> open(string path);
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
string httpget(string url);


int main(){
    ifstream fin;
    vector<vector<string> >geo_pubmed;
    vector<string> f = open(".");
    //vector<string> f = open("/home/hxu/geo/__no_directories");
    //vector<string> f = open("/home/hxu/geo/Map_GEO_PubMed");
    //vector<string> f = open("/home/hxu/geo/test");
    
    ofstream fout;
    fout.open("Map_GSE_PMID_MESH_ICD_Table", ios::out);
    if(!fout.is_open()){
        cerr << "Unable to open file map_geo_pubmed_table." << endl;
        exit(10);
    }
    
    // recursively open files in s specific directory, by default current folder.
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
        // allocate a new vector to restore info associated with a geo
        vector<string> *geo_pubmed_unit;
        geo_pubmed_unit = new vector<string>;
        (*geo_pubmed_unit).push_back(file_name);
        //fout << "GSE " << '\t' << file_name << endl;
	fout << file_name.substr(0 ,file_name.find('_')) << '\t';
	read_file(fin, fout, file_name, *geo_pubmed_unit);
        geo_pubmed.push_back(*geo_pubmed_unit);
        fin.close();
        fout << endl << endl;
    }
    fout.close();
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
        //int found = getMesh.find(meshTerm);
        //if ( found != string::npos ){
        if ( meshTerm == getMesh.substr(0, meshTerm.length()) ){
	  //cout << "Find MESH term match to th equery: " << endl << line << endl;
	  CUI = line.substr(0, line.find('\t'));
	  //cout << "getCUI is " << CUI << endl;
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

// read file and map from geo# to PMID
void read_file(istream & in, ostream & out, string geo, vector<string> & geo_pubmed_unit){
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
            //out << '\t' << "PMID" << '\t' << pubmed_id << endl;
            map_PMID_mesh(pubmed_id, geo_pubmed_unit, out);
        }
    }
}

// mapping from a PMID to mesh terms
void map_PMID_mesh(string pubmed_id, vector<string> & geo_pubmed_unit, ostream & out){
    string fullurl = "http://www.ncbi.nlm.nih.gov/pubmed?term=" + pubmed_id + "&report=medline";
    istringstream webpage (httpget(fullurl));
    string line;
    while ( getline(webpage, line) ){
        if ( line.substr(0,3) == "MH " ){
            string meshSentence = line.substr(6, line.length() - 6);
            // Processing MeshSentence
	    string query = getExactMeshTerm(meshSentence);
	    cout << "search mesh term " << query << endl;
	    string CUI = getCUI(query);
	    string ICD9CM = fetchICD(CUI);
	    
	    if(ICD9CM != "NA"){
	      geo_pubmed_unit.push_back(meshSentence);
	      geo_pubmed_unit.push_back(ICD9CM);
	      //out << '\t' << '\t' << "ICD9CM" << '\t' << ICD9CM << '\t' << "MESH" << '\t' << meshSentence << endl;
	      out << '\t' << CUI << '\t' << ICD9CM << '\t' << meshSentence << endl;
	      cout << "push a meshterm: " << meshSentence << endl;
	    }
	   	    	    
      	    
        }
    }
    cout << endl;
}

// print 2D vector on screen.
void print_vector(vector<vector<string> >&v){
    for (int i = 0; i < v.size(); i++){
        for ( int j = 0; j < v[i].size(); j++ ){
            cout << v[i][j] << endl;
        }
        cout << endl;
    }
}

// write 2D vector into a file.
void write_data(ostream & out, vector<vector<string> > & geo_pubmed){
    for (int i = 0; i < geo_pubmed.size(); i++){
        for ( int j = 0; j < geo_pubmed[i].size(); j++ ){
            out << geo_pubmed[i][j] << '\t';
        }
        out << endl;
    }
}

// fetch file names in a directory and store in a vector.
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

// service function to httpget
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
