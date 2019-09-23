#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <nullscript/nullscript.h>
#include <tokens.h>
#include <action.h>

#include <memory>
#include <vector>
#include <string>
#include <set>

#include <parselib/XML/xml.hpp>

namespace ARCDOC
{
    class Module: public ActionProvider
    {
    protected:
        NULLSCR::Tokenizer tokenizer;
        std::string currentFile;

        virtual void extractSymbols(const std::vector<NULLSCR::TokenEntity>&) = 0;
        virtual void initTokenizer() = 0;
    public:
        std::set<Member*> newMembers;
        ARCDOC::Namespace globalNamespace;

        void init();
        void setCurrentFile(const std::string& source);

        void parseString(const std::string& source);

        virtual std::set<std::string> getAvailableItems() = 0;
        virtual std::shared_ptr<ParseLib::XML::Tag> getItem(const std::string& name) = 0;

        virtual ~Module() = default;
    };
}

#endif // EXTRACTOR_H
