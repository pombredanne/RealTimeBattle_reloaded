#include <iostream>
#include <libintl.h>
#include <getopt.h>

//-----------------INI MACROS-------------------------------------------
#define _(str)		dgettext("RealTimeBattle_reloaded_bakend",str)
#define RTB_LOCALEDIR "" //TODO (read the gettext documentation...again!
//-----------------END MACROS-------------------------------------------

using namespace std;

//-----------------INI Function prototipes------------------------------
void print_help_message(void);
void parse_command_line(int argc, char **argv);
//-----------------END Function prototipes------------------------------

int main(int argc, char* argv[])
{
	bindtextdomain ("RealTimeBattle_reloaded_backend", RTB_LOCALEDIR);
	// allow german umlauts
	bind_textdomain_codeset ("RealTimeBattle_reloaded_backend", "UTF-8");
	textdomain ("RealTimeBattle_reloaded_backend");
	
	parse_command_line(argc, argv);
	
	return 0;
}

//-----------------INI Function declarations----------------------------

void print_help_message(void)
{
	cout << endl;
	cout << _(" Usage: RealTimeBattle [options] ") << endl << endl;
	cout << _(" Options:") << endl;
	cout << _("    --debug_level [0-5],         -D   sets the initial debug level. Implies -d") << endl;
	cout << _("    --option_file [file],        -o   selects option-file (default: $HOME/.rtbrc)")  << endl;
	cout << _("    --log_file [file],           -l   make log file (default: STDOUT), if 'file' is '-'\n"
		"                                      the log is sent to STDOUT") << endl;
	cout << _("    --help,                      -h   prints this message") << endl;
	cout << _("    --version,                   -v   prints the version number") << endl;
	cout << endl;
}

void parse_command_line(int argc, char **argv)
{
	int version_flag=false, help_flag=false;
	int sel_option_char;
	
	static struct option long_options[] =
	{
		//option, argument?, flag, value
		{"debug_level", 2, 0, 0},
		{"option_file", 2, 0, 0},
		{"log_file", 2, 0, 0},
		{"help", 0, &help_flag, true},
		{"version", 0, &version_flag, true}
	};
	
	for(;;)
	{
		int sel_option_large_index = 0;
		
		sel_option_char = getopt_long( argc, argv, "D:o:l:hv",
			long_options, &sel_option_large_index);
		
		// Detect the end of the options.
		if (sel_option_char == -1)
			break;
		
		//The user write some options in a large mode
		if (sel_option_char == 0) {
			switch (sel_option_large_index) {
			}
		}
	}
}

//-----------------END Function declarations----------------------------