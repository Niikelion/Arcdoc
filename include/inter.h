#ifndef INTER_H
#define INTER_H

#include <nullscript/nullscript.h>
#include <tokens.h>
#include <nlib/slib/slib.h>

#include <module.h>
#include <generator.h>

#include <memory>
#include <string>

class Parser
{
private:
    Nlib::SharedLib sl;
    ARCDOC::Module* module;

    typedef void(__cdecl *deleter_t)(ARCDOC::Module*);
    deleter_t deleter;
public:
    ARCDOC::Namespace& getGlobalNamespace();
    const ARCDOC::Namespace& getGlobalNamespace() const;

    bool load(const std::string& path);

    void parseString(const std::string& source);
    void parseFile(const std::string& filename);

    void parseProject(); ///TODO: store files to load, all configuration etc.

    template<typename T> std::vector<T*> get() const
    {
        return std::move(getGlobalNamespace().get<T>());
    }

    ARCDOC::Module* getModule() { return module; };
    const ARCDOC::Module* getModule() const {return module; };

    Parser():sl(),module(nullptr){};
    ~Parser();
};

class Generator
{
private:
    Nlib::SharedLib sl;
    ARCDOC::OutputGenerator* generator;

    typedef void(__cdecl *deleter_t)(ARCDOC::OutputGenerator*);
    deleter_t deleter;
public:
    bool load(const std::string& path);
    void attach(Parser* p);

    void generateTo(const std::string& path,const std::string& name) const;

    ~Generator();
};

class ConsoleHandler
{
    std::map<std::string,int> flagsD;
public:
    std::map<std::string,std::vector<std::string> >flags;
    std::vector<std::string> values;
    ConsoleHandler(unsigned argc,const char* argv[],const std::map<std::string,int>& fd);

    bool hasFlag(const std::string& flag) const;
    std::vector<std::string> getFlag(std::string const& flag) const;
};

class Core
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

    Core(): activatedParser(nullptr),activatedGenerator(nullptr) {};
};

#endif // INTER_H
