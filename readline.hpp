#ifndef _READLINE_HPP
#define _READLINE_HPP

#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

class yarw
{
	public:
		yarw(string prom) : prompt(prom)
		{
			using_history();
		}

		~yarw()
		{ }

		string getLine()
		{
			char* line = readline(prompt.c_str());
			if (line == NULL)
				return string();

			string word(line);
			free(line);

			return word;
		}

		void addHistory(string str)
		{
			add_history(str.c_str());
		}

	private:
		string prompt;
};

#endif
