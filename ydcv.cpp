#include <iostream>
#include <sstream>
#include <curlcpp/curl_easy.h>
#include <rapidjson/reader.h>
#include <rapidjson/document.h>

using std::string;
using std::stringstream;
using curl::curl_easy;
using namespace curl;
using namespace rapidjson;
using namespace std;

// API KEY from ydcv
#define API "YouDaoCV"
#define API_KEY "659600698"

#define YD_BASE_URL "http://fanyi.youdao.com"
#define YD_API_URL  (YD_BASE_URL "/openapi.do?keyfrom=" API "&key=" API_KEY "&type=data&doctype=json&version=1.1&q=")

void print_explanation(Document& doc)
{
	int has_result = 0;
	cout << doc["query"].GetString();
	if (doc.HasMember("basic")) {
		has_result = 1;
		Value &basic_dic = doc["basic"];
		if (basic_dic.HasMember("uk-phonetic") || basic_dic.HasMember("us-phonetic"))
			cout << " UK: [" << basic_dic["uk-phonetic"].GetString() <<
					"], US: [" << basic_dic["us-phonetic"].GetString() << "]" << endl;
		else if (basic_dic.HasMember("phonetic"))
			cout << " [" << basic_dic["phonetic"].GetString() << "]" << endl;
		else
			cout << endl;

		if (basic_dic.HasMember("explains")) {
			cout << "   Word Explanation:" << endl;
			Value &explains = basic_dic["explains"];
			for (auto itr = explains.Begin(); itr != explains.End(); ++itr)
				cout << "     * " << itr->GetString() << endl;
		} else
			cout << endl;
	} else if (doc.HasMember("translation")) {
		has_result = 1;
		cout << endl;
		cout << "  Translation:" << endl;
		Value &trans = doc["translation"];
		for (auto itr = trans.Begin(); itr != trans.End(); ++itr)
			cout << "     * " << itr->GetString() << endl;
	} else {
		cout << endl;
	}

	if (doc.HasMember("web")) {
		has_result = 1;
		cout << endl;
		cout << "   Web Reference:" << endl;
		Value &web_dic = doc["web"];
		for (auto dic = web_dic.Begin(); dic != web_dic.End(); ++dic) {
			cout << "     * " << (*dic)["key"].GetString() << endl;
			cout << "      ";
			Value &values = (*dic)["value"];
			for (auto itr = values.Begin(); itr != values.End(); ++itr)
				cout << " " << itr->GetString() << ";";
			cout << endl;
		}
	}

	if (has_result == 0)
		cout << " -- No result for this query." << endl;

	cout << endl;
}

int main(int argc, char **argv)
{
	stringstream res;
	curl_writer writer(res);
	curl_easy easy(writer);

	string url = string(YD_API_URL) + argv[1];

	easy.add(curl_pair<CURLoption, string>(CURLOPT_URL, url));
	easy.add(curl_pair<CURLoption, long>(CURLOPT_FOLLOWLOCATION, 1L));

	try {
		Reader reader;
		Document doc;

		easy.perform();

		string str(res.str());

		doc.Parse(str.c_str());

		print_explanation(doc);

	} catch (curl_easy_exception error) {
		// If you want to get the entire error stack we can do:
		vector<pair<string,string>> errors = error.get_traceback();
		// Otherwise we could print the stack like this:
		error.print_traceback();
		// Note that the printing the stack will erase it
	}

	return 0;
}
