#include "core.h"

using namespace std;

bool Core::loadLang(const string& name,const string& path)
{
    unique_ptr<Parser> p(new Parser());
    if (p -> load("langs/"+path))
    {
        parsers.emplace(name,std::move(p));
        return true;
    }
    return false;
}

bool Core::activateParser(string const& parser)
{
    auto it = parsers.find(parser);
    if (it == parsers.end())
        return false;
    activatedParser = it -> second.get();
    if (getActiveGenerator() != nullptr)
        getActiveGenerator()->attach(getActiveParser());
    return true;
}

Parser* Core::getParser(string const& name) const
{
    auto it = parsers.find(name);
    if (it == parsers.end())
        return nullptr;
    return it -> second.get();
}

Parser* Core::getActiveParser() const
{
    return activatedParser;
}

bool Core::loadGenerator(const string& name,const string& path)
{
    unique_ptr<Generator> g(new Generator());
    if (g -> load("formats/"+path))
    {
        generators.emplace(name,std::move(g));
        return true;
    }
    return false;
}

bool Core::activateGenerator(string const& generator)
{
    auto it = generators.find(generator);
    if (it == generators.end())
        return false;
    activatedGenerator = it -> second.get();
    if (getActiveParser() != nullptr)
        getActiveGenerator()->attach(getActiveParser());
    return true;
}

Generator* Core::getGenerator(string const& name) const
{
    auto it = generators.find(name);
    if (it == generators.end())
        return nullptr;
    return it -> second.get();
}

Generator* Core::getActiveGenerator() const
{
    return activatedGenerator;
}

vector<string> Core::parsersList() const
{
    vector<string> ret;
    ret.reserve(parsers.size());
    for (const auto& i:parsers)
        ret.emplace_back(i.first);
    return move(ret);
}

vector<string> Core::generatorsList() const
{
    vector<string> ret;
    ret.reserve(generators.size());
    for (const auto& i:generators)
        ret.emplace_back(i.first);
    return move(ret);
}

bool Core::installCmd(const vector<string>& args)
{
    cout << CmdColors::yellow << "Not implemented yet.\n" << CmdColors::none;
    return false;
}

bool Core::showCmd(const vector<string>& args)
{
    if (args.size() == 1)
    {
        vector<string> tmp;
        if (args[0] == "languages")
        {
            tmp = parsersList();
            for (const auto& i:tmp)
                cout << i << "\n";
        }
        else if (args[0] == "formats")
        {
            tmp = generatorsList();
            for (const auto& i:tmp)
                cout << i << "\n";
        }
        else
        {
            cout << CmdColors::yellow << "Unknown argument.\n" << CmdColors::none;
            return false;
        }
        return true;
    }
    if (args.size() == 0)
    {
        cout << "Available arguments: languages, formats.\n";
        return true;
    }
    cout << CmdColors::yellow << "Too many arguments.\n" << CmdColors::none;
    return false;
}

bool Core::activateCmd(const vector<string>& args)
{
    ConsoleHandler ch(args,
    {
        make_pair("l",0),
        make_pair("of",0)
    });
    bool l = ch.hasFlag("l");
    bool of = ch.hasFlag("of");
    if ((l && of) || ch.values.size() != 1)
    {
        cout << CmdColors::yellow;
        if (ch.values.size() == 0)
            cout << "Module not specified.\n";
        else if (ch.values.size() > 1)
            cout << "Too much modules specified.\n";
        else
            cout << "Can't activate both language and format modules.\n";
        cout << CmdColors::none;
        return false;
    }
    else if (l)
    {
        return activateParser(ch.values.front());
    }
    else if (of)
    {
        return activateGenerator(ch.values.front());
    }
    else
    {
        cout << CmdColors::yellow << "No target specified.\n" << CmdColors::none;
    }
    return false;
}

bool Core::listCmd(const vector<string>& args)
{
    ConsoleHandler ch(args,
    {
        make_pair("r",0)
    });
    //bool recursive = ch.hasFlag("r");
    if (ch.values.size() != 1)
    {
        if (ch.values.size() == 0)
        {
            cout << "Available arguments: updated.\n";
            return true;
        }
        cout << CmdColors::yellow << "Too many arguments.\n" << CmdColors::none;
        return false;
    }
    if (ch.values.front() == "updated")
    {
        if (getActiveParser() != nullptr)
        {
            for (const auto& i:getActiveParser()->getModule()->newMembers)
            {
                for (auto origin = i->origins.cbegin(); origin != i->origins.cend(); origin++)
                {
                    auto tmp = origin;
                    tmp++;
                    cout << origin->filename << "(" << origin->pos << ")" << (((tmp)==i->origins.cend())?": ":",\n");
                }
                ARCDOC::Member* parent = i->parent;
                std::string parents;
                const ARCDOC::Member* globals = &(getActiveParser()->getModule()->globalNamespace);
                while (parent != nullptr && parent != globals)
                {
                    parents = parent -> name + "::" + parents;
                    parent = parent -> parent;
                }
                cout << parents << i->name << "\n";
            }
        }
        else
        {
            cout << CmdColors::yellow << "Language not specified.\n" << CmdColors::none;
            return false;
        }
    }
    else
    {
        cout << CmdColors::yellow << "Unknown argument.\n" << CmdColors::none;
        return false;
    }
    return true;
}

bool Core::parseCmd(const vector<string>& args)
{
    ConsoleHandler ch(args,
    {
        make_pair("p",0)
    });
    bool isProject = ch.hasFlag("p");
    if (ch.values.size() >= 1)
    {
        if (getActiveParser() != nullptr)
        {
            for (const auto& i:ch.values)
            {
                if (isProject)
                    return getActiveParser()->parseProject(i);
                else
                    return getActiveParser()->parseFile(i);
            }
        }
        cout << CmdColors::red << "Inactive language module.\n" << CmdColors::none;
        return false;
    }
    else if (ch.values.size() == 0)
    {
        cout << CmdColors::yellow << "File(s) not specified.\n" << CmdColors::none;
    }
    return false;
}

bool Core::generateCmd(const vector<string>& args)
{
    ConsoleHandler ch(args,
    {
        make_pair("of",1),
        make_pair("n",1)
    });
    if (ch.values.size() == 1)
    {
        if (ch.hasFlag("of"))
        {
            //more module forkery
        }

        if (getActiveGenerator() != nullptr)
        {
            string name="project";
            if (ch.hasFlag("n"))
                name = ch.getFlag("n").front();
            getActiveGenerator()->generateTo(ch.values.front(),name);
            return true;
        }
        cout << CmdColors::red << "Inactive format module.\n" << CmdColors::none;
        return false;
    }
    cout << CmdColors::yellow;
    if (ch.values.size() == 0)
    {
        cout << "output path not specified.\n";
    }
    else
    {
        cout << "Too many arguments.\n";
    }
    cout << CmdColors::none;
    return false;
}

bool Core::selfcheckCmd(const vector<string>& args)
{
    if (getActiveParser() == nullptr)
    {
        cout << CmdColors::yellow << "Inactive language module.\n";
    }
    else if (getActiveParser()->getModule() == nullptr)
    {
        cout << CmdColors::red << "Corrupted language module.\n";
    }
    if (getActiveGenerator() == nullptr)
    {
        cout << CmdColors::yellow << "Inactive format module.\n";
    }
    else if (getActiveGenerator()->getModule() == nullptr)
    {
        cout << CmdColors::red << "Corrupted format module.\n";
    }
    else if (getActiveGenerator()->getModule()->getModule() == nullptr)
    {
        cout << CmdColors::red << "Desynchronized format module.\n";
    }
    cout << CmdColors::none;
    return true;
}

bool Core::parserCmd(const vector<string>& args)
{
    Parser* p = getActiveParser();
    if (p == nullptr || args.size() == 0)
        return false;
    map<string,ARCDOC::Action> acts = p->getActions();
    auto it = acts.find(args[0]);
    if (it == acts.end())
        return false;
    vector<string> args2;
    args2.reserve(args.size()-1);
    for (unsigned i=1; i<args.size(); ++i)
    {
        args2.emplace_back(args[i]);
    }
    return it->second(args2);
}

bool Core::generatorCmd(const vector<string>& args)
{
    Generator* g = getActiveGenerator();
    if (g == nullptr || args.size() == 0)
        return false;
    map<string,ARCDOC::Action> acts = g->getActions();
    auto it = acts.find(args[0]);
    if (it == acts.end())
        return false;
    vector<string> args2;
    args2.reserve(args.size()-1);
    for (unsigned i=1; i<args.size(); ++i)
    {
        args2.emplace_back(args[i]);
    }
    return it->second(args2);
}
