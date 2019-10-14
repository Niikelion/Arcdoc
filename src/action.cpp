#include "action.h"

namespace ARCDOC
{
    std::map<std::string,Action> ActionProvider::getActions() const
    {
        return actions;
    }

    void ActionHandler::getActionPack(ActionHandler::ActionPack& p)
    {
        std::string l;
        std::getline(*input,l);
        p.args.clear();
        p.cmd.clear();
        unsigned la = 0;
        unsigned i = 0;
        for (; i<l.size(); ++i)
        {
            if (l[i] == ' ')
            {
                if (la == 0)
                {
                    p.cmd = l.substr(la,i-la);
                }
                else
                {
                    p.args.emplace_back(l.substr(la,i-la));
                }
                ++i;
                la = i;
            }
        }
        if (i == l.size())
        {
            if (la == 0)
            {
                p.cmd = l;
            }
            else
            {
                p.args.emplace_back(l.substr(la,i-la));
            }
        }
    }
    void ActionHandler::attachProvider(ActionProvider* p)
    {
        provider = p;
        if (p != nullptr)
            actions = p->getActions();
        else
            actions.clear();
    }

    void ActionHandler::loop()
    {
        ActionPack ap;
        if (pName.size() != 0)
        {
            (*output) << pName << " ";
        }
        getActionPack(ap);
        while (ap.cmd != "exit")
        {
            auto it = actions.find(ap.cmd);
            if (it != actions.end())
            {
                if (!it -> second(ap.args))
                {
                    throw std::logic_error("Action failed.");
                }
            }
            else
            {
                throw std::logic_error("Unknown action.");
            }
            if (pName.size() != 0)
            {
                (*output) << pName << " ";
            }
            getActionPack(ap);
        }
    }
}
