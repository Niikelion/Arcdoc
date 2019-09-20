#include "inter.h"
#include <fstream>

#include <iostream>

using namespace NULLSCR;
using namespace ARCDOC;

Namespace& Parser::getGlobalNamespace()
{
    if (module == nullptr)
        throw std::logic_error("no module attached");
    return module->globalNamespace;
}

const Namespace& Parser::getGlobalNamespace() const
{
    if (module == nullptr)
        throw std::logic_error("no module attached");
    return module->globalNamespace;
}

bool Parser::load(const std::string& path)
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

void Parser::parseString(const std::string& source)
{
    if (module != nullptr)
    {
        module->parseString(source);
        //std::cout << module->getItem("member_tree")->toString() << std::endl;
    }
}

void Parser::parseFile(const std::string& filename)
{
    std::ifstream t(filename);
    if (t.is_open() && module != nullptr)
    {
        std::string str((   std::istreambuf_iterator<char>(t)),
                            std::istreambuf_iterator<char>());
        module->setCurrentFile(filename);
        parseString(str);
    }
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
        generator->attachModule(*p->getModule());
    }
}

void Generator::generateTo(const std::string& path,const std::string& name) const
{
    generator->generateOutput(path,name,ParseLib::JSON::Object());
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

bool Core::loadLang(const std::string& name,const std::string& path)
{
    std::unique_ptr<Parser> p(new Parser());
    if (p -> load("langs/"+path))
    {
        parsers.emplace(name,std::move(p));
        return true;
    }
    return false;
}

bool Core::activateParser(std::string const& parser)
{
    auto it = parsers.find(parser);
    if (it == parsers.end())
        return false;
    activatedParser = it -> second.get();
    if (getActiveGenerator() != nullptr)
        getActiveGenerator()->attach(getActiveParser());
    return true;
}

Parser* Core::getParser(std::string const& name) const
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

bool Core::loadGenerator(const std::string& name,const std::string& path)
{
    std::unique_ptr<Generator> g(new Generator());
    if (g -> load("formats/"+path))
    {
        generators.emplace(name,std::move(g));
        return true;
    }
    return false;
}

bool Core::activateGenerator(std::string const& generator)
{
    auto it = generators.find(generator);
    if (it == generators.end())
        return false;
    activatedGenerator = it -> second.get();
    if (getActiveParser() != nullptr)
        getActiveGenerator()->attach(getActiveParser());
    return true;
}

Generator* Core::getGenerator(std::string const& name) const
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

std::vector<std::string> Core::parsersList() const
{
    std::vector<std::string> ret;
    ret.reserve(parsers.size());
    for (const auto& i:parsers)
        ret.emplace_back(i.first);
    return std::move(ret);
}

std::vector<std::string> Core::generatorsList() const
{
    std::vector<std::string> ret;
    ret.reserve(generators.size());
    for (const auto& i:generators)
        ret.emplace_back(i.first);
    return std::move(ret);
}

