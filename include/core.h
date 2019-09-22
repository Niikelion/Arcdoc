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

    virtual std::set<std::string> getActions()const override;
    virtual bool performAction(const std::string& action,const std::vector<std::string>& options) override;

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

    Core(): activatedParser(nullptr),activatedGenerator(nullptr) {};
};

#endif // CORE_H
