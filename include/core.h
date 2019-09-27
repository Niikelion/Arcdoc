#ifndef CORE_H
#define CORE_H

#include <action.h>
#include <inter.h>

class Core: public ARCDOC::ActionProvider
{
private:
    Parser* activatedParser;
    std::map<std::string,std::unique_ptr<Parser>> parsers;

    Generator* activatedGenerator;
    std::map<std::string,std::unique_ptr<Generator>> generators;
public:
    std::vector<std::string> parsersList() const;
    std::vector<std::string> generatorsList() const;

    bool loadLang(const std::string& name,const std::string& file);
    bool loadGenerator(const std::string& name,const std::string& file);

    Parser* getParser(std::string const& name) const;
    Parser* getActiveParser() const;

    Generator* getGenerator(std::string const& name) const;
    Generator* getActiveGenerator() const;

    bool activateParser(std::string const& parser);
    bool activateGenerator(std::string const& generator);

    Core(): activatedParser(nullptr),activatedGenerator(nullptr)
    {
        registerAction("install",installCmd);
        registerAction("show",showCmd);
        registerAction("activate",activateCmd);
        registerAction("list",listCmd);
        registerAction("parse",parseCmd);
        registerAction("generate",generateCmd);
        registerAction("selfcheck",selfcheckCmd);
        /*
        @ps
        @og
        */
    }

    bool installCmd(const std::vector<std::string>& args);
    bool showCmd(const std::vector<std::string>& args);
    bool activateCmd(const std::vector<std::string>& args);
    bool listCmd(const std::vector<std::string>& args);
    bool parseCmd(const std::vector<std::string>& args);
    bool generateCmd(const std::vector<std::string>& args);
    bool selfcheckCmd(const std::vector<std::string>& args);
};

#endif // CORE_H
