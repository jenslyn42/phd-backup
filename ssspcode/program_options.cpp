#include <boost/program_options.hpp>
	

using namespace boost::program_options;

#include <iostream>
#include <fstream>
#include <iterator>
using namespace std;

// A helper function to simplify the main part.
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    copy(v.begin(), v.end(), ostream_iterator<T>(cout, " ")); 
    return os;
}


int main(int ac, char* av[])
{
    try {
        int opt;
    
        // Declare a group of options that will be 
        // allowed only on command line
        options_description generic("Generic options");
        generic.add_options()
		("version,v", "print version string")
		("help,h", "produce help message")
		("optimization", value<int>(&opt)->default_value(10), "optimization level")
		("testname,N", value<vector<string> >(), "Set the test name\nWill name the result file after the test")
		("mapfile,F", value<vector<string> >(), "set the map-data filename")
		("numqueries,Q", value< vector<string> >()->composing(), "Number of queries in test")
		("cachesize,C", value< vector<string> >()->composing(), "Size of cache in test")
		("queryrangestart,RS", value< int >()->default_value(0), "Start range for generating queries")
		("queryrangeend,RE", value< vector<string> >()->composing(), "End range for generating queries")
		("skeweddata,S", value< bool >()->default_value(false), "Skew the generated data using Gaussian destribution")
		("sigma,G", value< int >()->default_value(0), "Sigma value, used with Gaussian\nOnly used if skeweddata is set.")
		("optimalsubstructure,O", value< bool >()->default_value(true), "Use optimal substructure in tests\nCan only disable this in LRU and FIFO")
		("nodescore,NS", value< bool >()->default_value(false), "use node score in OSC")
		("hitscore,HS", value< bool >()->default_value(false), "use hit score in OSC")
            ;

        // Declare a group of options that will be 
        // allowed both on command line and in
        // config file
        options_description config("Configuration");
        config.add_options()
            ("optimization", value<int>(&opt)->default_value(10), 
                  "optimization level")
            ("include-path,I", 
                 value< vector<string> >()->composing(), 
                 "include path")
            ;

        // Hidden options, will be allowed both on command line and
        // in config file, but will not be shown to the user.
        options_description hidden("Hidden options");
        hidden.add_options()
            ("input-file", value< vector<string> >(), "input file")
	;
        
        options_description cmdline_options;
        cmdline_options.add(generic).add(config).add(hidden);

        options_description config_file_options;
        config_file_options.add(config).add(hidden);

        options_description visible("Allowed options");
        visible.add(generic).add(config);
        
        positional_options_description p;
        p.add("input-file", -1);
        
        variables_map vm;
        store(command_line_parser(ac, av).
              options(cmdline_options).positional(p).run(), vm);

        ifstream ifs("multiple_sources.cfg");
        store(parse_config_file(ifs, config_file_options), vm);
        notify(vm);
    
        if (vm.count("help")) {
            cout << visible << "\n";
            return 0;
        }

        if (vm.count("version")) {
            cout << "Multiple sources example, version 1.0\n";
            return 0;
        }

        if (vm.count("include-path"))
        {
            cout << "Include paths are: " 
                 << vm["include-path"].as< vector<string> >() << "\n";
        }

        if (vm.count("input-file"))
        {
            cout << "Input files are: " 
                 << vm["input-file"].as< vector<string> >() << "\n";
        }

        cout << "Optimization level is " << opt << "\n";                
    }
    catch(exception& e)
    {
        cout << e.what() << "\n";
        return 1;
    }    
    return 0;
}