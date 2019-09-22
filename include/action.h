#ifndef ACTION_H_INCLUDED
#define ACTION_H_INCLUDED

#include <set>
#include <vector>
#include <string>

namespace ARCDOC
{
    class ActionProvider
    {
    public:
        virtual std::set<std::string> getActions() const = 0;
        virtual bool performAction(const std::string& action,const std::vector<std::string>& options) = 0;
    };
}

#endif // ACTION_H_INCLUDED
