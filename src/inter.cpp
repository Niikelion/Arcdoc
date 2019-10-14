#include "inter.h"
#include <fstream>

#include <iostream>

using namespace NULLSCR;
using namespace ARCDOC;
using namespace std;

Namespace& Parser::getGlobalNamespace()
{
    if (module == nullptr)
        throw std::logic_error("No module attached.");
    return module->globalNamespace;
}

const Namespace& Parser::getGlobalNamespace() const
{
    if (module == nullptr)
        throw std::logic_error("No module attached.");
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
            module->parseString(source);
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
