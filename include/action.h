#ifndef ACTION_H_INCLUDED
#define ACTION_H_INCLUDED

#include <set>
#include <string>

namespace ARCDOC
{
    class ActionProvider
    {
    public:
        virtual std::set<std::string> getActions() const = 0;
        virtual bool performAction(const std::string& action) = 0;
    };
}

#endif // ACTION_H_INCLUDED
