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
        return true;
    }
    if (args.size() == 0)
    {
        cout << "available arguments: languages, formats\n";
        return true;
    }
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
        //err
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
            cout << "available arguments: updated\n";
            return true;
        }
        //err
        return false;
    }
    if (ch.values.front() == "updated")
    {
        if (getActiveParser() != nullptr)
        {
            for (const auto& i:getActiveParser()->getModule()->newMembers)
            {
                cout << i->filename << "(" << i->pos << "): " << i->name << "\n";
            }
        }
        else
        {
            //error
            return false;
        }
    }
    else
    {
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

    if (ch.values.size() == 1)
    {
        if (getActiveParser() != nullptr)
        {
            if (isProject)
                return getActiveParser()->parseProject(ch.values.front());
            else
                return getActiveParser()->parseFile(ch.values.front());
        }
        //error
        return false;
    }
    //err
    return false;
}

bool Core::generateCmd(const vector<string>& args)
{
    ConsoleHandler ch(args,
    {
        make_pair("og",1),
        make_pair("n",1)
    });
    if (ch.values.size() == 1)
    {
        if (ch.hasFlag("og"))
        {
            //more module forkery
        }

        string name="project";
        if (ch.hasFlag("n"))
            name = ch.getFlag("n").front();

        getActiveGenerator()->generateTo(ch.values.front(),name);
        return true;
    }
    //error
    return false;
}
