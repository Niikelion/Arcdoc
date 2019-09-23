#ifndef INTER_H
#define INTER_H

#include <nullscript/nullscript.h>
#include <tokens.h>
#include <nlib/slib/slib.h>

#include <module.h>
#include <generator.h>
#include <action.h>

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

    bool parseString(const std::string& source);
    bool parseFile(const std::string& filename);

    bool parseProject(const std::string& filename); ///TODO: store files to load, all configuration etc.

    template<typename T> std::vector<T*> get() const
    {
        return std::move(getGlobalNamespace().get<T>());
    }

    ARCDOC::Module* getModule() { return module; };
    const ARCDOC::Module* getModule() const {return module; };

    std::map<std::string,ARCDOC::Action> getActions() const;

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

    std::map<std::string,ARCDOC::Action> getActions() const;

    ~Generator();
};

class ConsoleHandler
{
    std::map<std::string,int> flagsD;
public:
    std::map<std::string,std::vector<std::string> >flags;
    std::vector<std::string> values;
    ConsoleHandler(unsigned argc,const char* argv[],const std::map<std::string,int>& fd);
    ConsoleHandler(const std::vector<std::string>& args,const std::map<std::string,int>& fd);

    bool hasFlag(const std::string& flag) const;
    std::vector<std::string> getFlag(std::string const& flag) const;
};

#endif // INTER_H
