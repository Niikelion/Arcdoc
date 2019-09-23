#ifndef ACTION_H_INCLUDED
#define ACTION_H_INCLUDED

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <functional>

namespace ARCDOC
{
    using Action = std::function<bool(const std::vector<std::string>&)>;
    class ActionProvider
    {
    private:
        std::map<std::string,Action> actions;
    protected:
        template<typename T>void registerAction(const std::string& actionName,bool (T::*method)(const std::vector<std::string>&))
        {
            actions.emplace(actionName,[this,method](const std::vector<std::string>& args)
            {
                return (reinterpret_cast<T*>(this)->*method)(args);
            });
        }
    public:
        std::map<std::string,Action> getActions() const;
    };

    class ActionHandler
    {
    private:
        struct ActionPack
        {
            std::string cmd;
            std::vector<std::string> args;
        };
        std::ostream* output;
        std::istream* input;

        ActionProvider* provider;
        std::map<std::string,Action> actions;

        void getActionPack(ActionPack& p);
        std::string pName;
    public:
        ActionHandler(std::ostream& out = std::cout,std::istream& in = std::cin,const std::string& p = std::string()): output(&out), input(&in), provider(nullptr), pName(p) {}

        void attachProvider(ActionProvider* provider);
        void loop();
    };
}

#endif // ACTION_H_INCLUDED
