#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <exception>
#include <algorithm>
#include <functional>

#include <core.h>
#include <conio.h>
#include <console.h>

using namespace std;

int main(int argc,const char* argv[])
{
    bool interactive = false;
    ifstream inputfile;
    try
    {
        ConsoleHandler ch(argc,argv,
        {
            make_pair("l",1),
            make_pair("i",0),
            make_pair("f",1),
            make_pair("of",1)
        });


        Core core;
        ifstream langs("langs.txt");
        if (langs.is_open())
        {
            string line;
            while (getline(langs,line))
            {
                core.loadLang(line,line+".dll");
            }
            langs.close();
        }
        ifstream formats("formats.txt");
        if (formats.is_open())
        {
            string line;
            while (getline(formats,line))
            {
                core.loadGenerator(line,line+".dll");
            }
            formats.close();
        }

        if (ch.hasFlag("of"))
        {
            if (!core.activateGenerator(ch.getFlag("of").front()))
            {
                throw logic_error("Invalid format.");
            }
        }
        if (ch.hasFlag("l"))
        {
            if (!core.activateParser(ch.getFlag("l").front()))
            {
                throw logic_error("Invalid module.");
            }
        }

        istream* input = nullptr;

        if (ch.hasFlag("i"))
        {
            interactive = true;
            input = &cin;
        }
        else if (ch.hasFlag("f"))
        {
            inputfile.open(ch.getFlag("f")[0]);
            if (inputfile.is_open())
                input = &inputfile;
        }

        if (input != nullptr)
        {
            ARCDOC::ActionHandler cmdHandler(cout,*input,interactive?"Arcdoc>":"");
            cmdHandler.attachProvider(&core);
            bool exit=false;
            while (!exit)
            {
                try
                {
                    cmdHandler.loop();
                    exit = true;
                }
                catch (logic_error& e)
                {
                    cout << CmdColors::red << e.what() << CmdColors::none << endl;
                }
            }
        }
        if (inputfile.is_open())
            inputfile.close();
    }
    catch(exception& err)
    {
        cout << err.what() << endl;
    }
    return 0;
}
