#ifndef TOKENS_H_INCLUDED
#define TOKENS_H_INCLUDED

#include <nullscript/tokens.h>
#include <string>
#include <set>
#include <typeindex>
#include <memory>
#include <map>

namespace ARCDOC
{
    class VariableToken: public NULLSCR::TokenBase<VariableToken>
    {
    public:
        std::string name,type;
        std::vector<std::string> qualifiers;
    };

    class FunctionToken: public NULLSCR::TokenBase<FunctionToken>
    {
    public:
        NULLSCR::ScopeToken args;
        std::string name,type;
        std::vector<std::string> modifiers;
    };

    class ClassToken: public NULLSCR::TokenBase<ClassToken>
    {
    public:
        NULLSCR::ScopeToken members;
        std::vector<std::pair<std::string,std::string>> parents;
    };

    struct MemberOrigin
    {
        std::string filename;
        unsigned pos;

        bool operator < (const MemberOrigin& t) const noexcept
        {
            if (filename.size() == t.filename.size())
            {
                for (unsigned i=0; i<filename.size(); ++i)
                {
                    if (filename[i] != t.filename[i])
                        return filename[i] < t.filename[i];
                }
                return pos < t.pos;
            }
            return filename.size() < t.filename.size();
        }

        bool operator == (const MemberOrigin& t) const noexcept
        {
            if (pos == t.pos)
            {
                return filename == t.filename;
            }
            return false;
        }

        MemberOrigin():pos(0) {}
        MemberOrigin(unsigned p):pos(p) {}
        MemberOrigin(const std::string& f):filename(f),pos(0) {}
        MemberOrigin(const std::string& f,unsigned p):filename(f),pos(p) {}
        MemberOrigin(const MemberOrigin&) = default;
    };

    struct Member
    {
        std::string name,description,path;
        std::set<MemberOrigin> origins;
        std::vector<Member*> parents;

        virtual std::type_index getType() const = 0;
        virtual bool isSame(const Member& t) const
        {
            if (getType() == t.getType() && name == t.name && parents.size() == t.parents.size())
            {
                for (unsigned i=0; i<parents.size(); ++i)
                    if (parents[i] != t.parents[i])
                        return false;
                return true;
            }
            return false;
        }
        virtual void mergeWith(Member&& t)
        {
            for (auto& i:t.origins)
            {
                origins.emplace(i);
            }
        }
        virtual std::unique_ptr<Member> clone() const = 0;

        Member(const std::string& n): name(n) {};
        virtual ~Member() = default;
    };

    template<typename T> class MemberBase: public Member
    {
    public:
        virtual std::unique_ptr<Member> clone() const override
        {
            return std::unique_ptr<T>(new T(reinterpret_cast<const T&>(*this)));
        }
        virtual std::type_index getType() const override
        {
            return typeid(T);
        }
        MemberBase(const std::string& n):Member(n) {};
    };

    class Alias: public MemberBase<Alias>
    {
    public:
        std::string target;
        using MemberBase::MemberBase;
    };

    class TypeInfo
    {
    public:
        std::string typeName;
        std::vector<std::string> qualifiers;
    };

    class Variable: public MemberBase<Variable>
    {
    public:
        TypeInfo type;
        using MemberBase::MemberBase;
    };

    class Function: public MemberBase<Function>
    {
    public:
        TypeInfo type;
        std::vector<std::string> modifiers;
        std::vector<std::pair<TypeInfo,std::string>> args;
        using MemberBase::MemberBase;
    };

    class Structure: public MemberBase<Structure>
    {
    public:
        std::vector<std::pair<std::string,std::string>> parents;
        std::map<std::string,std::vector<std::unique_ptr<Member>>> members; //visibility-member list

        Structure(const std::string& n): MemberBase(n) {};
        Structure(const Structure& t): MemberBase(t)
        {
            parents = t.parents;
            for (const auto& i:t.members)
            {
                std::vector<std::unique_ptr<Member>> cmp;
                for (const auto& j:i.second)
                    cmp.emplace_back(j -> clone());
                members.emplace(i.first,std::move(cmp));
            }
        }
    };

    class Namespace: public MemberBase<Namespace>
    {
    public:
        std::vector<std::unique_ptr<Member>> members;

        virtual void mergeWith(Member&& t) override
        {
            if (t.getType() == getType())
            {
                Member::mergeWith(std::move(t));
                for (auto& i:reinterpret_cast<Namespace&&>(t).members)
                {
                    bool n = true;
                    for (auto& j:members)
                    {
                        if (j->isSame(*i))
                        {
                            n=false;
                            j->mergeWith(std::move(*i));
                            break;
                        }
                    }
                    if (n)
                        members.emplace_back(std::move(i));
                }
            }
        }

        Namespace(): MemberBase("") {};

        Namespace(const std::string& name): MemberBase(name) {};
        Namespace(const Namespace& t):MemberBase(t.name)
        {
            members.reserve(t.members.size());
            for (const auto& i:t.members)
            {
                members.emplace_back(i->clone());
            }
        }

        template<typename T>std::vector<T*> get() const
        {
            std::vector<T*> ret;
            for(auto& i:members)
            {
                if (i->getType() == typeid(T))
                    ret.push_back(static_cast<T*>(i.get()));
            }
            return std::move(ret);
        }

        Namespace(Namespace&&) noexcept = default;
    };
}
#endif // TOKENS_H_INCLUDED
