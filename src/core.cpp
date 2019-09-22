#include "core.h"

std::set<std::string> Core::getActions() const
{
    return std::set<std::string>({
        "install",
        "activate",
        "list",
        "help",
        "show",
        "@ps",
        "@og"
    });
}
bool Core::performAction(const std::string& action,const std::vector<std::string>& options)
{
    return false;
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
