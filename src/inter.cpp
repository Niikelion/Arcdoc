#include "inter.h"
#include <fstream>

#include <iostream>

using namespace NULLSCR;
using namespace ARCDOC;
using namespace std;

Namespace& Parser::getGlobalNamespace()
{
    if (module == nullptr)
        throw std::logic_error("No module attached");
    return module->globalNamespace;
}

const Namespace& Parser::getGlobalNamespace() const
{
    if (module == nullptr)
        throw std::logic_error("No module attached");
    return module->globalNamespace;
}

bool Parser::load(const string& path)
{
    if (!sl.load(path))
    {
        return false;
    }

    Module*(__cdecl *factory)() = sl.getFunction<Module*>("moduleFactory");
    deleter = sl.getFunction<void,Module*>("moduleDeleter");
    if (!factory || !deleter)
    {
        return false;
    }
    module = factory();
    if (module != nullptr)
    {
        module -> init();
        return true;
    }
    return false;
}

bool Parser::parseString(const string& source)
{
    if (module != nullptr)
    {
        try
        {
            //module->globalNamespace.members.clear();
            module->parseString(source);
            //std::cout << module->getItem("member_tree")->toString() << std::endl;
        }
        catch(std::exception e)
        {
            return false;
        }
        return true;
    }
    return false;
}

bool Parser::parseFile(const string& filename)
{
    std::ifstream t(filename);
    if (t.is_open() && module != nullptr)
    {
        std::string str((   std::istreambuf_iterator<char>(t)),
                            std::istreambuf_iterator<char>());
        module->setCurrentFile(filename);
        return parseString(str);
    }
    return false;
}

bool Parser::parseProject(const string& filename)
{
    return false;
}

map<string,Action> Parser::getActions() const
{
    if (module != nullptr)
        return module->getActions();
    return map<string,Action>();
}

Parser::~Parser()
{
    if (module != nullptr)
    {
        deleter(module);
    }
    sl.free();
}

bool Generator::load(const std::string& path)
{
    if (!sl.load(path))
    {
        return false;
    }

    OutputGenerator*(__cdecl *factory)() = sl.getFunction<OutputGenerator*>("generatorFactory");
    deleter = sl.getFunction<void,OutputGenerator*>("generatorDeleter");
    if (!factory || !deleter)
    {
        return false;
    }
    generator = factory();
    if (generator != nullptr)
    {
        generator -> init();
        return true;
    }
    return false;
}

void Generator::attach(Parser* p)
{
    if (generator != nullptr)
    {
        generator->attachModule(p->getModule());
    }
}

void Generator::generateTo(const std::string& path,const std::string& name) const
{
    generator->generateOutput(path,name,ParseLib::JSON::Object());
}

map<string,Action> Generator::getActions() const
{
    if (generator != nullptr)
        return generator->getActions();
    return map<string,Action>();
}

Generator::~Generator()
{
    if (generator != nullptr)
    {
        deleter(generator);
    }
    sl.free();
}


ConsoleHandler::ConsoleHandler(unsigned argc,const char* argv[],const std::map<std::string,int>& fd)
{
    flagsD = fd;
    std::string tmp;

    for (unsigned i = 0; i < argc; ++i)
    {
        tmp = argv[i];
        if (argv[i][0] == '-')
        {
            tmp.erase(0,1);
            auto it = flagsD.find(tmp);
            if (it != flagsD.end()) //found flag
            {
                if (it->second == -1) //capture all inputs till next flag
                {
                    auto fg = flags.find(tmp);
                    if (fg == flags.end())
                        fg = flags.emplace(tmp,std::vector<std::string>()).first;
                    for (unsigned j = i+1; j<argc; ++j)
                    {
                        if (argv[j][0] == '-')
                        {
                            break;
                        }
                        else
                        {
                            fg -> second.emplace_back(argv[j]);
                        }
                        i = j;
                    }
                }
                else //capture specific amount
                {
                    std::vector<std::string> tmpv;
                    if (i + it->second < argc)
                    {
                        bool err = false;
                        for (unsigned j = 1; j <= static_cast<unsigned>(it -> second); ++j)
                        {
                            if (argv[i+j][0] == '-')
                            {
                                err = true;
                            }
                        }
                        if (!err) //emplace inputs and push flag
                        {
                            auto fg = flags.find(tmp);
                            if (fg == flags.end())
                                fg = flags.emplace(tmp,std::vector<std::string>()).first;
                            for (unsigned j = 1; j <= static_cast<unsigned>(it -> second); ++j)
                            {
                                fg -> second.emplace_back(argv[i+j]);
                            }
                            i += it -> second;
                        }
                    }
                }

            }
        }
        else
        {
            values.emplace_back(tmp);
        }
    }
}

ConsoleHandler::ConsoleHandler(const std::vector<std::string>& args,const std::map<std::string,int>& fd)
{
    flagsD = fd;
    std::string tmp;

    for (unsigned i = 0; i < args.size(); ++i)
    {
        tmp = args[i];
        if (args[i][0] == '-')
        {
            tmp.erase(0,1);
            auto it = flagsD.find(tmp);
            if (it != flagsD.end()) //found flag
            {
                if (it->second == -1) //capture all inputs till next flag
                {
                    auto fg = flags.find(tmp);
                    if (fg == flags.end())
                        fg = flags.emplace(tmp,std::vector<std::string>()).first;
                    for (unsigned j = i+1; j<args.size(); ++j)
                    {
                        if (args[j][0] == '-')
                        {
                            break;
                        }
                        else
                        {
                            fg -> second.emplace_back(args[j]);
                        }
                        i = j;
                    }
                }
                else //capture specific amount
                {
                    std::vector<std::string> tmpv;
                    if (i + it->second < args.size())
                    {
                        bool err = false;
                        for (unsigned j = 1; j <= static_cast<unsigned>(it -> second); ++j)
                        {
                            if (args[i+j][0] == '-')
                            {
                                err = true;
                            }
                        }
                        if (!err) //emplace inputs and push flag
                        {
                            auto fg = flags.find(tmp);
                            if (fg == flags.end())
                                fg = flags.emplace(tmp,std::vector<std::string>()).first;
                            for (unsigned j = 1; j <= static_cast<unsigned>(it -> second); ++j)
                            {
                                fg -> second.emplace_back(args[i+j]);
                            }
                            i += it -> second;
                        }
                    }
                }

            }
        }
        else
        {
            values.emplace_back(tmp);
        }
    }
}

bool ConsoleHandler::hasFlag(const std::string& flag) const
{
    auto it = flags.find(flag);
    return it != flags.end();
}

std::vector<std::string> ConsoleHandler::getFlag(std::string const& flag) const
{
    auto it = flags.find(flag);
    if (it != flags.end())
        return it -> second;
    return std::vector<std::string>();
}
