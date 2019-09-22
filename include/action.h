#ifndef ACTION_H_INCLUDED
#define ACTION_H_INCLUDED

#include <set>
#include <vector>
#include <string>
#include <iostream>

namespace ARCDOC
{
    class ActionProvider
    {
    public:
        virtual std::set<std::string> getActions() const = 0;
        virtual bool performAction(const std::string& action,const std::vector<std::string>& options) = 0;
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
        std::set<std::string> actions;

        void getActionPack(ActionPack& p);
    public:
        ActionHandler(std::ostream& out = std::cout,std::istream& in = std::cin): output(&out), input(&in), provider(nullptr) {}

        void attachProvider(ActionProvider* provider);
        void loop();
    };
}

#endif // ACTION_H_INCLUDED
