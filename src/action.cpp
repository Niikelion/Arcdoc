#include "action.h"

namespace ARCDOC
{
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
                p.args.emplace_back(la,i-la);
            }
        }
    }
    void ActionHandler::attachProvider(ActionProvider* p)
    {
        provider = p;
        if (p != nullptr)
            actions = p->getActions();
    }

    void ActionHandler::loop()
    {
        ActionPack ap;
        getActionPack(ap);
        while (ap.cmd != "exit")
        {
            provider -> performAction(ap.cmd,ap.args);
            getActionPack(ap);
        }
    }
}
