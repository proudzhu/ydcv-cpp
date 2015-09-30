#include <iostream>
#include <sstream>
#include <curlcpp/curl_easy.h>
#include <rapidjson/reader.h>
#include <rapidjson/document.h>

/* boost */
#include <boost/program_options.hpp>

/* readline */
#include "readline.hpp"

#include "cpplog.hpp"

using std::string;
using std::stringstream;
using curl::curl_easy;
using namespace curl;
using namespace rapidjson;
using namespace std;
using namespace CPPLOG;

namespace po = boost::program_options;

// API KEY from ydcv
#define API "YouDaoCV"
#define API_KEY "659600698"
#define API_VERSION "1.2"

#define YD_BASE_URL "http://fanyi.youdao.com"
#define YD_API_URL  (YD_BASE_URL "/openapi.do?keyfrom=" API "&key=" API_KEY "&type=data&doctype=json&version=" API_VERSION "&q=")

/* runtime configuration */
static struct {
	int logmask;
	bool out_full;
	int color;
	bool selection;
	bool speech;

	vector<string> words;
} cfg;

static cpplog mylog;

void print_explanation(Document& doc);

void query(string &word) {
	stringstream res;
	curl_writer writer(res);
	curl_easy easy(writer);

	string url = string(YD_API_URL) + word;

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
}

void print_explanation(Document& doc)
{
	int has_result = 0;
	ILOG(mylog) << doc["query"].GetString();
	if (doc.HasMember("basic")) {
		has_result = 1;
		Value &basic_dic = doc["basic"];
		if (basic_dic.HasMember("uk-phonetic") || basic_dic.HasMember("us-phonetic"))
			ILOG(mylog) << " UK: [" << basic_dic["uk-phonetic"].GetString() <<
					"], US: [" << basic_dic["us-phonetic"].GetString() << "]" << endl;
		else if (basic_dic.HasMember("phonetic"))
			ILOG(mylog) << " [" << basic_dic["phonetic"].GetString() << "]" << endl;
		else
			ILOG(mylog) << endl;

		if (cfg.speech) {
			if (basic_dic.HasMember("uk-speech") && basic_dic.HasMember("us-speech")) {
				ILOG(mylog) << "  Text to Speech:" << endl;
				ILOG(mylog) << "    * UK: " << basic_dic["uk-speech"].GetString() << endl;
				ILOG(mylog) << "    * US: " << basic_dic["us-speech"].GetString() << endl;
			}
			else if (basic_dic.HasMember("speech"))
				ILOG(mylog) << "    * " << basic_dic["speech"].GetString() << endl;
			ILOG(mylog) << endl;
		}

		if (basic_dic.HasMember("explains")) {
			ILOG(mylog) << "  Word Explanation:" << endl;
			Value &explains = basic_dic["explains"];
			for (auto itr = explains.Begin(); itr != explains.End(); ++itr)
				ILOG(mylog) << "     * " << itr->GetString() << endl;
		} else
			ILOG(mylog) << endl;
	} else if (doc.HasMember("translation")) {
		has_result = 1;
		ILOG(mylog) << endl;
		ILOG(mylog) << "  Translation:" << endl;
		Value &trans = doc["translation"];
		for (auto itr = trans.Begin(); itr != trans.End(); ++itr)
			ILOG(mylog) << "     * " << itr->GetString() << endl;
	} else {
		ILOG(mylog) << endl;
	}

	if (doc.HasMember("web")) {
		has_result = 1;
		ILOG(mylog) << endl;
		ILOG(mylog) << "  Web Reference:" << endl;
		Value &web_dic = doc["web"];
		for (auto dic = web_dic.Begin(); dic != web_dic.End(); ++dic) {
			ILOG(mylog) << "     * " << (*dic)["key"].GetString() << endl;
			ILOG(mylog) << "      ";
			Value &values = (*dic)["value"];
			for (auto itr = values.Begin(); itr != values.End(); ++itr)
				ILOG(mylog) << " " << itr->GetString() << ";";
			ILOG(mylog) << endl;
		}
	}

	if (has_result == 0)
		ILOG(mylog) << " -- No result for this query." << endl;

	ILOG(mylog) << endl;
}

int parse_options(int argc, char **argv)
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "show this help message and exit")
		("full,f", "print full web reference, only the first 3 results\n"
				 "will be printed without this flag.")
		("simple,s", "only show explainations. argument \"-f\" will not take\n"
				   "effect.")
		("speech,S", "print URL to speech audio.")
		("selection,x", "show explaination of current selection.")
		("color", po::value<string>(), "{always,auto,never}"
										"colorize the output. Default to 'auto' or can be\n"
										"'never' or 'always'.")
		("debug", "print debug info.")
		("verbose", "print verbose info.")
		("words", po::value<vector<string>>()->multitoken(), "words to lookup, or quoted sentences to translate.")
		;

	po::positional_options_description pos_desc;
	pos_desc.add("words", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
			  options(desc).positional(pos_desc).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
		cout << desc << "\n";
		return 1;
	}

	if (vm.count("simple"))
		cfg.out_full = 0;
	else if (vm.count("full"))
		cfg.out_full = 1;

	if (vm.count("speech"))
		cfg.speech = 1;

	if (vm.count("selection")) {
		cfg.selection = 1;
		cout << "Not implemeted yet." << endl;
	}

	if (vm.count("color")) {
		string color = vm["color"].as<string>();
		if (color == string("always"))
			cfg.color = 1;
		else if (color == string("never"))
			cfg.color = 0;
		else
			cout << "color only accept {always,auto,never}" << endl;
	}

	if (vm.count("debug"))
		cfg.logmask |= LOG_DEBUG;

	if (vm.count("verbose"))
		cfg.logmask |= LOG_DEBUG | LOG_VERBOSE;

	if (vm.count("words")) {
		const vector<string> &v = vm["words"].as<vector<string>>();
		// to_cout(v);
		copy(v.begin(), v.end(), back_insert_iterator<vector<string>>(cfg.words));
	}

	return 0;
}

int main(int argc, char **argv)
{
	int ret = 0;

	/* initialize config */
	cfg.logmask = LOG_ERROR|LOG_WARN|LOG_INFO;
	cfg.out_full = 1;
	cfg.color = 0;
	cfg.selection = 0;
	cfg.speech = 0;

	ret = parse_options(argc, argv);
	if (ret)
	{
		return ret;
	}

	mylog.SetLogMask(cfg.logmask);

	for (auto word : cfg.words) {
		query(word);
	}

	if (cfg.words.size() == 0) {
		while (1) {
			yarw rl(string("> "));
			string word = rl.getLine();
			rl.addHistory(word);
			if (word.empty()) {
				cout << "\nBye\n";
				break;
			} else {
				query(word);
			}
		}
	}

	return 0;
}
