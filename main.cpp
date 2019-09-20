#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <exception>
#include <algorithm>
#include <functional>

#include <inter.h>

using namespace std;

void showParsers(Core& core)
{
    std::vector<std::string> ps = core.parsersList();
    cout << "\navailable modules:\n";
    for (const auto& i:ps)
    {
        cout << "-" << i << "\n";
    }
}

void listNamespaces(const ARCDOC::Namespace& ns,bool recursive)
{
    for(const auto& i: ns.members)
    {
        if (i->getType() == typeid(ARCDOC::Namespace))
        {
            cout << i->name << endl;
            if (recursive)
                listNamespaces(*reinterpret_cast<const ARCDOC::Namespace*>(i.get()),true);
        }
    }
}

void listNamespaces(Core& core,bool recursive)
{
    listNamespaces(core.getActiveParser()->getGlobalNamespace(),recursive);
}

void listClasses(const ARCDOC::Structure& st)
{
    for(const auto& i: st.members)
    {
        for(const auto& j:i.second)
        {
            if (j->getType() == typeid(ARCDOC::Structure))
            {
                cout << j->name << endl;
                listClasses(*reinterpret_cast<const ARCDOC::Structure*>(j.get()));
            }
        }
    }
}

void listClasses(const ARCDOC::Namespace& ns,bool recursive)
{
    for(const auto& i: ns.members)
    {
        if (i->getType() == typeid(ARCDOC::Namespace))
        {
            if (recursive)
                listClasses(*reinterpret_cast<const ARCDOC::Namespace*>(i.get()),true);
        }
        else if (i->getType() == typeid(ARCDOC::Structure))
        {
            cout << i->name << endl;
            if (recursive)
                listClasses(*reinterpret_cast<const ARCDOC::Structure*>(i.get()));
        }
    }
}

void listClasses(Core& core,bool recursive)
{
    listClasses(core.getActiveParser()->getGlobalNamespace(),recursive);
}

void activateCmd(Core& core,const std::vector<std::string>& args)
{
    if (args.size())
    {
        if (!core.activateParser(args[0]))
        {
            cout << "invalid module\n";
        }
    }
    else
    {
        cout << "module not specified\n";
    }
}

void parseCmd(Core& core,const std::vector<std::string>& args)
{
    if (args.size())
    {
        core.getActiveParser() -> parseFile(args[0]);
    }
}

void showCmd(Core& core,const std::vector<std::string>& args)
{
    if (args.size())
    {
        if (args[0] == "modules")
        {
            showParsers(core);
        }
    }
    else
    {
        cout << "\navailable arguments:\n";
        cout << "modules (shows installed modules)\n";
    }
}

void listNew(Core& core)
{
    for (const auto& i:core.getActiveParser()->getModule()->newMembers)
    {
        cout << i->filename << "(" << i->pos << "): " << i->name << "\n";
    }
}

void listCmd(Core& core,const std::vector<std::string>& args)
{
    if (args.size())
    {
        bool recursive = false;
        if (args.size() >= 2 && args[1] == "recursive")
            recursive = true;
        if (args[0] == "namespaces")
        {
            listNamespaces(core,recursive);
        }
        else if (args[0] == "classes")
        {
            listClasses(core,recursive);
        }
        else if (args[0] == "new")
        {
            listNew(core);
        }
    }
    else
    {
        cout << "\navailable arguments:\n";
        cout << "namespaces\n";
        cout << "classes\n";
        cout << "new\n";
    }
}

void helpCmd(Core& core,const std::vector<std::string>& args);

void generateCmd(Core& core,const std::vector<std::string>& args)
{
    if (args.size() > 1)
    {
        core.getActiveGenerator()->generateTo(args[0],args[1]);
    }
    else
    {
        cout << "path to output and project name needed\n";
    }
}

void split(const std::string& in,std::vector<std::string>& ret)
{
    ret.clear();
    bool str = false;
    unsigned s = 0;
    for (unsigned i=0; i<in.size(); ++i)
    {
        if (in[i] == ' ' && !str)
        {
            ret.emplace_back(in.substr(s,i-s));
            s = i+1;
        }
        else if (in[i] == '"')
        {
            str = !str;
        }
    }
    if (s < in.size())
    {
        ret.emplace_back(in.substr(s));
    }
}

map<string,pair<function<void(Core&,const vector<string>&)>,string>> funcs = {
        make_pair("show",       make_pair(showCmd,"use to view configuration and loaded modules")),
        make_pair("help",       make_pair(helpCmd,"")),
        make_pair("activate",   make_pair(activateCmd,"activates parsing module")),
        make_pair("parse",      make_pair(parseCmd,"parses specified file")),
        make_pair("list",       make_pair(listCmd,"gets list of found entities")),
        make_pair("generate",   make_pair(generateCmd,"generated output files of documentation"))
};

void helpCmd(Core& core,const std::vector<std::string>& args)
{
    if (args.size())
    {
        //
    }
    else
    {
        cout << "\navailable commands:\n";
        cout << "-exit\n";
        for (const auto& i:funcs)
        {
            cout << "-" << i.first;
            if (i.second.second.size())
            {
                cout << " (" << i.second.second << ")";
            }
            cout << "\n";
        }
    }
}


int main(int argc,const char* argv[])
{
    bool interactive = false;
    ifstream inputfile;
    try
    {
        ConsoleHandler ch(argc,argv,
        {
            make_pair("ps",1),
            make_pair("i",0),
            make_pair("f",1),
            make_pair("og",1)
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

        if (ch.hasFlag("og"))
        {
            if (!core.activateGenerator(ch.getFlag("og")[0]))
            {
                throw logic_error("invalid format");
            }
        }
        if (ch.hasFlag("ps"))
        {
            if (!core.activateParser(ch.getFlag("ps")[0]))
            {
                throw logic_error("invalid module");
            }
        }

        if (core.getActiveGenerator() == nullptr)
            throw logic_error("output format not specified");
        if (core.getActiveParser() == nullptr)
            throw logic_error("parsing module not specified");

        if (ch.hasFlag("i"))
        {
            interactive = true;
        }
        else if (ch.hasFlag("f"))
        {
            inputfile.open(ch.getFlag("f")[0]);
        }
        istream* input = nullptr;

        if (interactive)
            input = &cin;
        else if (inputfile.is_open())
            input = &inputfile;

        if (input != nullptr)
        {
            string line;
            vector<string> args;
            while ((!interactive || cout << ">") && getline(*input,line) && line != "exit")
            {
                split(line,args);
                if (args.size())
                {
                    line = args[0];
                    args.erase(args.begin());
                    auto it = funcs.find(line);
                    if (it != funcs.end())
                    {
                        it -> second.first(core,args);
                    }
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
