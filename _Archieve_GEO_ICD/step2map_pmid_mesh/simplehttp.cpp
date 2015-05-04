#include <iostream>
#include <curl/curl.h>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
void map_PMID_mesh(string pubmed_id, vector<string> & geo_pubmed_unit);
string httpget(string url);

int main(){
  string pubmed_id = "16893473";
  cout << "pubmed_id = " << pubmed_id << endl;
  vector<string> geo_pubmed_unit;
  map_PMID_mesh(pubmed_id, geo_pubmed_unit);
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
      
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

void map_PMID_mesh(string pubmed_id, vector<string> & geo_pubmed_unit){
  string fullurl = "http://www.ncbi.nlm.nih.gov/pubmed?term=" + pubmed_id + "&report=medline";
  istringstream webpage (httpget(fullurl));
  string line;
  while ( getline(webpage, line) ){
    if ( line.substr(0,3) == "MH " ){
      string meshterm = line.substr(6, line.length() - 6);
      geo_pubmed_unit.push_back(meshterm);
      cout << "push a meshterm: " << meshterm << endl;
    }
  }
  cout << endl;
}
