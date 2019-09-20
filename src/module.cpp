#include "module.h"

namespace ARCDOC
{
    void Module::init()
    {
        initTokenizer();
    }
    void Module::setCurrentFile(const std::string& source)
    {
        currentFile = source;
    }
    void Module::parseString(const std::string& source)
    {
        std::vector<NULLSCR::TokenEntity> tokens = std::move(tokenizer.tokenize(source));
        extractSymbols(tokens);
    }
}
