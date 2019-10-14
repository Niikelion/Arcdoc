#include <nullscript/nullscript.h>
#include <nullscript/rules.h>
#include <module.h>
#include <console.h>
#include <colors.h>

using namespace NULLSCR;

enum Ids
{
    None = 0,
    Semicolon,
    Colon,
    Period,
    DoubleColon,
    Keyword,
    Keyword_Namespace,
    Keyword_Class,
    Keyword_Struct,
    Keyword_Template,
    Separator,
    Operator,
    Modifier,
    CurlyScope,
    RoundScope,
    SquareScope,
    PointyScope,
    String,
    Char,
    Comment,
    Number,
    Cas,
    Vla,
    Template,

    Namespace,
    Class,
    Type,
    Variable,
    Function,
    Alias
};

enum ScopeTypes
{
    Curly = 0,
    Round,
    Square,
    Pointy
};

enum VLA
{
    Private,
    Protected,
    Public
};

char const* VlaToStr(VLA c)
{
    switch (c)
    {
    case Private:
        return "private";
    case Protected:
        return "protected";
    case Public:
        return "public";
    }
    return "";
}

class EmptyToken: public TokenBase<EmptyToken>
{
    virtual char const* getName() const noexcept override
    {
        return "empty";
    }

    using TokenBase::TokenBase;
};

class NamespaceToken: public ScopeToken
{
public:
    virtual char const* getName() const noexcept override
    {
        return "namespace";
    }
    virtual std::type_index getType() const noexcept override
    {
        return typeid(NamespaceToken);
    }
    std::string name;
    NamespaceToken(const std::string& n): ScopeToken(0),name(n) { setPos(0); };
    NamespaceToken(const NamespaceToken& t): ScopeToken(t) {};
    NamespaceToken(NamespaceToken&&) noexcept = default;
};

class ClassToken: public ScopeToken
{
public:
    std::string name,templateP;
    VLA level;
    std::vector<std::tuple<VLA,std::string,std::string>> parents;

    virtual char const* getName() const noexcept override
    {
        return "class";
    }
    virtual std::type_index getType() const noexcept override
    {
        return typeid(ClassToken);
    }

    ClassToken(const std::string& n): ScopeToken(0),name(n) { setPos(0); };
    ClassToken(const ClassToken& t): ScopeToken(t) {};
    ClassToken(ClassToken&&) noexcept = default;
};

class AliasToken: public ScopeToken
{
public:
    virtual char const* getName() const noexcept override
    {
        return "alias";
    }
    virtual std::type_index getType() const noexcept override
    {
        return typeid(AliasToken);
    }
    std::string name;
    AliasToken(const std::string& n): ScopeToken(0),name(n) { setPos(0); };
    AliasToken(const AliasToken& t): ScopeToken(t) {};
    AliasToken(AliasToken&&) noexcept = default;
};

class VariableToken: public TokenBase<VariableToken>
{
public:
    virtual char const* getName() const noexcept override
    {
        return "variable";
    }

    std::string name;
    ARCDOC::TypeInfo type;

    VariableToken(const std::string& n): name(n) { setPos(0); };
    VariableToken(const VariableToken&) = default;
    VariableToken(VariableToken &&) noexcept = default;
};

class FunctionToken: public TokenBase<FunctionToken>
{
public:
    virtual char const* getName() const noexcept override
    {
        return "function";
    }

    std::string name;
    std::vector<std::string> modifiers;
    ARCDOC::TypeInfo type;
    std::vector<std::pair<ARCDOC::TypeInfo,std::string>> args;

    FunctionToken(const std::string& n): name(n) { setPos(0); };
    FunctionToken(const FunctionToken&) = default;
    FunctionToken(FunctionToken &&) noexcept = default;
};

class CasToken: public TokenBase<FunctionToken>
{
public:
    virtual char const* getName() const noexcept override
    {
        return "cas";
    }

    VLA level;

    CasToken(VLA c): level(c) { setPos(0); };
    CasToken(const CasToken&) = default;
    CasToken(CasToken &&) noexcept = default;
};

std::string stringifyTokens(const std::vector<TokenEntity>& s)
{
    const char scopes[4][2] = {
        {'{','}'},
        {'(',')'},
        {'[',']'},
        {'<','>'}
    };
    std::string ret;
    for (const auto& i:s)
    {
        if (i.token -> getType() == typeid(ScopeToken))
        {
            ScopeToken& st = i.token -> forceAs<ScopeToken>();
            ret += scopes[st.type][0];
            ret += stringifyTokens(st.tokens);
            ret += scopes[st.type][1];
        }
        else if (i.type == Ids::Semicolon)
        {
            ret += ";";
        }
        else if (i.type == Ids::Colon)
        {
            ret += ":";
        }
        else if (i.type == Ids::Comment)
        {
            StringToken* st = i.token -> as<StringToken>();
            if (st != nullptr)
            {
                if (ret.size())
                    ret += ' ';
                ret += st -> str;
            }
        }
    }
    return ret;
}

std::unique_ptr<Token> creator(const std::string& source,unsigned id)
{
    switch (id)
    {
    case Ids::CurlyScope:
    case Ids::RoundScope:
    case Ids::SquareScope:
    case Ids::PointyScope:
        {
            return std::unique_ptr<Token>(new ScopeToken(0));
            break;
        }
    case Ids::Semicolon:
    case Ids::Colon:
    case Ids::Keyword_Template:
    case Ids::DoubleColon:
    case Ids::Period:
        {
            return std::unique_ptr<Token>(new EmptyToken());
            break;
        }
    case Ids::Comment:
        {
            if (source.size() >= 2)
            {
                if (source[1] == '*')
                {
                    return std::unique_ptr<Token>(new StringToken(0,source.substr(2,source.size()-4)));
                }
                return std::unique_ptr<Token>(new StringToken(0,source.substr(2,source.size()-3)));
            }
            break;
        }
    case Ids::Vla:
        {
            VLA c = VLA::Public;
            if (source == "protected")
                c = VLA::Protected;
            else if (source == "private")
                c = VLA::Private;
            return std::unique_ptr<Token>(new CasToken(c));
        }
    }
    return std::unique_ptr<Token>(new StringToken(0,source));
}

void templateBracketsInCurlyFunc(std::vector<TokenEntity>& tokens)
{
    std::vector<unsigned> scopes;
    int offset = 0;

    while (static_cast<unsigned>(offset) < tokens.size())
    {
        if (tokens[offset].type == Ids::Operator)
        {
            std::string op = tokens[offset].token->forceAs<StringToken>().str;
            if (op == "<")
            {
                scopes.push_back(offset);
            }
            else if (op == ">")
            {
                if (scopes.size())
                {
                    auto beg = tokens.begin()+scopes.back();
                    auto en = tokens.begin()+offset;
                    ScopeToken *sc = new ScopeToken(beg->token->getPos());

                    sc->tokens.resize(en - beg - 1);
                    std::move(beg+1,en,sc->tokens.begin());
                    //sc->tokens.insert(sc->tokens.begin(),beg+1,en);
                    offset -= (en - beg);
                    tokens.erase(beg,en+1);
                    tokens.emplace(tokens.begin()+scopes.back(),std::unique_ptr<Token>(sc),Ids::PointyScope);
                    scopes.pop_back();
                }
                else
                {
                    //error
                }
            }
            else if (op == ">>")
            {
                if (scopes.size() > 1)
                {
                    unsigned b = scopes.back();
                    auto beg = tokens.begin()+b;
                    auto en = tokens.begin()+offset;
                    ScopeToken *sc = new ScopeToken(beg->token->getPos());

                    sc->tokens.resize(en - beg - 1);
                    std::move(beg+1,en,sc->tokens.begin());
                    //sc->tokens.insert(sc->tokens.begin(),beg+1,en);
                    offset -= (en - beg) + 1;
                    tokens.erase(beg,en+1);

                    scopes.pop_back();

                    beg = tokens.begin() + scopes.back();
                    en = tokens.begin() + b - 1;

                    ScopeToken *sc2 = new ScopeToken(beg->token->getPos());

                    sc2->tokens.resize(en - beg - 1);
                    std::move(beg+1,en,sc2->tokens.begin());
                    //sc2->tokens.insert(sc2->tokens.begin(),beg+1,en);

                    sc2->tokens.emplace_back(std::unique_ptr<Token>(sc),Ids::PointyScope);
                    offset -= (en - beg) - 1;
                    tokens.erase(beg,en);
                    tokens.emplace(tokens.begin()+scopes.back(),std::unique_ptr<Token>(sc2),Ids::PointyScope);
                    scopes.pop_back();
                }
                else
                {
                    //error
                }
            }
        }
        ++offset;
    }
}

void templateBracketsFunc(std::vector<TokenEntity>& tokens)
{
    std::vector<unsigned> scopes;

    int offset = 0;
    while (static_cast<unsigned>(offset) < tokens.size())
    {
        if (tokens[offset].type == Ids::RoundScope)
        {
            if (scopes.size())
            {
                //error
            }
            templateBracketsInCurlyFunc(tokens[offset].token->forceAs<ScopeToken>().tokens);

            ++offset;
            while (static_cast<unsigned>(offset) < tokens.size() && tokens[offset].type == Ids::Modifier) ++offset;
        }
        else if (tokens[offset].type == Ids::CurlyScope)
        {
            if (scopes.size())
            {
                //error
            }
            templateBracketsFunc(tokens[offset].token->forceAs<ScopeToken>().tokens);
        }
        else
        {
            if (tokens[offset].type == Ids::Operator)
            {
                std::string op = tokens[offset].token->forceAs<StringToken>().str;
                if (op == "<")
                {
                    scopes.push_back(offset);
                }
                else if (op == ">")
                {
                    if (scopes.size())
                    {
                        auto beg = tokens.begin()+scopes.back();
                        auto en = tokens.begin()+offset;
                        ScopeToken *sc = new ScopeToken(beg->token->getPos());

                        sc->tokens.resize((en - beg) - 1);
                        std::move(beg+1,en,sc->tokens.begin());
                        //sc->tokens.insert(sc->tokens.begin(),beg+1,en);

                        offset -= (en - beg);
                        tokens.erase(beg,en+1);
                        tokens.emplace(tokens.begin()+scopes.back(),std::unique_ptr<Token>(sc),Ids::PointyScope);
                        scopes.pop_back();
                    }
                    else
                    {
                        //error
                    }
                }
                else if (op == ">>")
                {
                    if (scopes.size() > 1)
                    {
                        unsigned b = scopes.back();
                        auto beg = tokens.begin()+b;
                        auto en = tokens.begin()+offset;
                        ScopeToken *sc = new ScopeToken(beg->token->getPos());

                        sc->tokens.resize(en - beg - 1);
                        std::move(beg+1,en,sc->tokens.begin());
                        //sc->tokens.insert(sc->tokens.begin(),beg+1,en);
                        offset -= (en - beg) +1;
                        tokens.erase(beg,en+1);

                        scopes.pop_back();

                        beg = tokens.begin() + scopes.back();
                        en = tokens.begin() + b - 1;

                        ScopeToken *sc2 = new ScopeToken(beg->token->getPos());

                        sc2->tokens.resize(en - beg - 1);
                        std::move(beg+1,en,sc2->tokens.begin());
                        //sc2->tokens.insert(sc2->tokens.begin(),beg+1,en);

                        sc2->tokens.emplace_back(std::unique_ptr<Token>(sc),Ids::PointyScope);

                        offset -= (en - beg) - 1;
                        tokens.erase(beg,en);
                        tokens.emplace(tokens.begin()+scopes.back(),std::unique_ptr<Token>(sc2),Ids::PointyScope);
                        scopes.pop_back();
                    }
                    else
                    {
                        //error
                    }
                }
            }
        }
        ++offset;
    }
}

void setupSlt(Stage* slt)
{
    LexicalRule* lrs = new LexicalRule();

    lrs -> addParsePoint(std::regex("\\s"),Ids::Separator,LexicalRule::States::forget,false);

    lrs -> addParsePoint(";",Ids::Semicolon,LexicalRule::Modes::String,LexicalRule::States::insert,false);

    lrs -> addParsePoint(":",Ids::Colon,LexicalRule::Modes::String,LexicalRule::States::insert,false);

    const char* keywords[] = {
        "alignas",
        "alignof",
        "auto",
        "concept",
        "const",
        "consteval",
        "constexpr",
        "decltype",
        "default",
        "delete",
        "enum",
        "explicit",
        "extern",
        "friend",
        "inline",
        "mutable",
        "noexcept",
        "operator",
        "register",
        "requires",
        "short",
        "signed",
        "static",
        "throw",
        "typedef",
        "typename",
        "union",
        "using",
        "unsigned",
        "void",
        "volatile"
    };

    const char* wrd_operators[] = {
        "and",
        "and_eq",
        "bitand",
        "bitor",
        "comlp",
        "not",
        "not_eq",
        "or",
        "or_eq",
        "xor",
        "xor_eq"
    };

    const char* operators[] = {
        "&&",
        "&=",
        "&",
        "|",
        "~",
        "!",
        "!=",
        "||",
        "|=",
        "^",
        "^=",
        ".",
        "->",
        "?",
        "%",
        "+",
        "+=",
        "-",
        "-=",
        "/",
        "/=",
        "*",
        "*=",
        "=",
        "==",
        "<",
        "<=",
        ">",
        ">="
    };

    for (auto i: keywords)
        lrs -> addParsePoint(i,Ids::Keyword,LexicalRule::Modes::Keyword,LexicalRule::States::insert,false);
    lrs -> addParsePoint("namespace",Ids::Keyword_Namespace,LexicalRule::Modes::Keyword,LexicalRule::States::insert,false);
    lrs -> addParsePoint("template",Ids::Keyword_Template,LexicalRule::Modes::Keyword,LexicalRule::States::insert,false);

    lrs -> addParsePoint("class",Ids::Keyword_Class,LexicalRule::Modes::Keyword,LexicalRule::States::insert,false);
    lrs -> addParsePoint("struct",Ids::Keyword_Struct,LexicalRule::Modes::Keyword,LexicalRule::States::insert,false);


    lrs -> addParsePoint("private",Ids::Vla,LexicalRule::Modes::Keyword,LexicalRule::States::insert,false);
    lrs -> addParsePoint("protected",Ids::Vla,LexicalRule::Modes::Keyword,LexicalRule::States::insert,false);
    lrs -> addParsePoint("public",Ids::Vla,LexicalRule::Modes::Keyword,LexicalRule::States::insert,false);

    for (auto i: wrd_operators)
        lrs -> addParsePoint(i,Ids::Operator,LexicalRule::Modes::Keyword,LexicalRule::States::insert,false);
    for (auto i: operators)
        lrs -> addParsePoint(i,Ids::Operator,LexicalRule::Modes::String,LexicalRule::States::insert,false);

    lrs -> addParsePoint("::",Ids::DoubleColon,LexicalRule::Modes::String,LexicalRule::States::insert,false);
    lrs -> addParsePoint(",",Ids::Period,LexicalRule::Modes::String,LexicalRule::States::insert,false);

    //scopes
    lrs -> addParsePoint("{",Ids::CurlyScope,LexicalRule::Modes::String,LexicalRule::States::push,true);
    lrs -> addParsePoint("}",Ids::CurlyScope,LexicalRule::Modes::String,LexicalRule::States::pop);

    lrs -> addParsePoint("[",Ids::SquareScope,LexicalRule::Modes::String,LexicalRule::States::push,true);
    lrs -> addParsePoint("]",Ids::SquareScope,LexicalRule::Modes::String,LexicalRule::States::pop);

    lrs -> addParsePoint("(",Ids::RoundScope,LexicalRule::Modes::String,LexicalRule::States::push,true);
    lrs -> addParsePoint(")",Ids::RoundScope,LexicalRule::Modes::String,LexicalRule::States::pop);

    lrs -> addParsePoint(std::regex("\\d+(?:\\.\\d*(?:(f)|(d)|(u?[l]{0,2}))?)?"),Ids::Number,LexicalRule::States::insert,false);
    lrs -> addParsePoint(std::regex("\\w\\d+(?:\\.\\d*(?:(f)|(d)|(u?[l]{0,2}))?)?"),Ids::Number,LexicalRule::States::ignore,false);
    lrs -> addParsePoint(std::regex("\\.\\d*(?:(f)|(d)|(u?[l]{0,2}))?"),Ids::Number,LexicalRule::States::insert,false);

    lrs -> addParsePoint("\'",Ids::Char,LexicalRule::Modes::String,LexicalRule::States::toggle);
    lrs -> addParsePoint("\"",Ids::String,LexicalRule::Modes::String,LexicalRule::States::toggle);

    //comments
    lrs -> addParsePoint("//",Ids::Comment,LexicalRule::Modes::String,LexicalRule::States::insert,false);
    lrs -> addParsePoint("\\\n",Ids::Comment,LexicalRule::Modes::String,LexicalRule::States::forget,false);
    lrs -> addParsePoint("/*",Ids::Comment,LexicalRule::Modes::String,LexicalRule::States::push,false);
    lrs -> addParsePoint("*/",Ids::Comment,LexicalRule::Modes::String,LexicalRule::States::pop,false);

    lrs -> setTokenCreator(creator);

    slt -> rules.emplace_back(lrs);

    ComplexRule* bmf = new ComplexRule(templateBracketsFunc);
    bmf -> deep = false;

    slt -> rules.emplace_back(bmf);
}

void preprocessorFunc(std::vector<TokenEntity>& source)
{
    //
}

void setupPrp(Stage* prp)
{
    ComplexRule* pr = new ComplexRule(preprocessorFunc,true);
    prp -> rules.emplace_back(pr);
}

std::unique_ptr<Token> preMergerFunc(unsigned b,unsigned e,unsigned t,const std::vector<TokenEntity>& source)
{
    switch (t)
    {
    case Ids::Template:
        {
            ScopeToken* tm = new ScopeToken(0);
            tm->tokens = std::move(source[b+1].token->forceAs<ScopeToken>().tokens);
            return std::unique_ptr<Token>(tm);
        }
    case Ids::Cas:
        {
            CasToken* ct = new CasToken(source.at(b).token->forceAs<CasToken>().level);
            return std::unique_ptr<Token>(ct);
        }
    }
    return std::unique_ptr<Token>();
}

std::unique_ptr<Token> mergerFunc(unsigned b,unsigned e,unsigned t,const std::vector<TokenEntity>& source)
{
    switch (t)
    {
    case Ids::Namespace:
        {
            NamespaceToken* ns = new NamespaceToken(source[b+1].token->forceAs<StringToken>().str);
            ns -> tokens = std::move(source[b+2].token->forceAs<ScopeToken>().tokens);
            return std::unique_ptr<Token>(ns);
            break;
        }
    case Ids::Class:
        {
            unsigned nameOffset = b+1;
            while (nameOffset+1 < e && source[nameOffset].type != 0) ++nameOffset;

            ClassToken* cls = new ClassToken(source[nameOffset].token->forceAs<StringToken>().str);

            unsigned i=nameOffset+1;

            while (i<e && source[i].type != Ids::CurlyScope) ++i;

            if (i != e)
            {
                cls -> tokens = std::move(source[i].token->forceAs<ScopeToken>().tokens);
            }
            return std::unique_ptr<Token>(cls);
            break;
        }
    }
    return std::unique_ptr<Token>();
}

void setupMrg(Stage* mrg)
{
    LayeredMergingRule* preLmr = new LayeredMergingRule();

    preLmr -> deep = true;
    preLmr -> setTokenMerger(preMergerFunc);
    preLmr -> layers.emplace_back();

    preLmr -> layers[0].addTypePath({Ids::Keyword_Template,Ids::PointyScope},Ids::Template);
    preLmr -> layers[0].addTypePath({Ids::Vla,Ids::Colon},Ids::Cas);

    LayeredMergingRule* lmr = new LayeredMergingRule();
    lmr -> deep = true;
    lmr -> setTokenMerger(mergerFunc);
    lmr -> layers.emplace_back();

    MergingLayer& lr1 = lmr -> layers[0];

    lr1.addTypePath({Ids::Keyword_Namespace,Ids::None,Ids::CurlyScope},Ids::Namespace);

    MergingLayer::TypesTrieNode* classEntry = lr1.addTypePath({Ids::Keyword_Class},-1);

    lr1.connectTypePath({Ids::Keyword_Struct},classEntry);
    lr1.connectTypePath({Ids::Template,Ids::Keyword_Class},classEntry);
    lr1.connectTypePath({Ids::Template,Ids::Keyword_Struct},classEntry);

    classEntry = lr1.appendTypePath(classEntry,{Ids::None},-1);

    MergingLayer::TypesTrieNode* classExit = lr1.appendTypePath(classEntry,{Ids::Semicolon},Ids::Class);
    MergingLayer::TypesTrieNode* classBeforeExit = lr1.appendTypePath(classEntry,{Ids::CurlyScope},-1);

    lr1.connectTypePath(classBeforeExit,{Ids::Semicolon},classExit);

    MergingLayer::TypesTrieNode* classParentsEntry = lr1.appendTypePath(classEntry,{Ids::Colon},-1);
    MergingLayer::TypesTrieNode* classParentsExit = lr1.appendTypePath(classParentsEntry,{Ids::Vla,Ids::None},-1);

    lr1.connectTypePath(classParentsEntry,{Ids::Vla,Ids::DoubleColon,Ids::None},classParentsExit);

    lr1.connectTypePath(classParentsExit,{Ids::Period},classParentsEntry);
    lr1.connectTypePath(classParentsExit,{Ids::CurlyScope},classBeforeExit);

    MergingLayer::TypesTrieNode* classParentsNamespace = lr1.appendTypePath(classParentsExit,{Ids::DoubleColon},-1);

    lr1.connectTypePath(classParentsNamespace,{Ids::None},classParentsExit);
    lr1.connectTypePath(classParentsExit,{Ids::PointyScope,Ids::DoubleColon},classParentsNamespace);
    lr1.connectTypePath(classParentsExit,{Ids::PointyScope,Ids::CurlyScope},classBeforeExit);

    mrg -> rules.emplace_back(preLmr);
    mrg -> rules.emplace_back(lmr);
}

ARCDOC::Namespace* createNamespaceFt(NamespaceToken& nt,const std::string& filename)
{
    ARCDOC::Namespace* n = new ARCDOC::Namespace(nt.name);
    n->origins.emplace(filename,nt.getPos());
    return n;
}

ARCDOC::Structure* createStructureFt(ClassToken& ct,const std::string& filename)
{
    ARCDOC::Structure* n = new ARCDOC::Structure(ct.name);
    n->origins.emplace(filename,ct.getPos());
    for (const auto& parent: ct.parents)
    {
        n -> parents.emplace_back(std::string(VlaToStr(std::get<0>(parent)))+" "+std::get<1>(parent),std::get<2>(parent));
    }
    return n;
}

ARCDOC::Function* createFunctionFt(FunctionToken& ft,const std::string& filename)//
{
    ARCDOC::Function* n = new ARCDOC::Function(ft.name);
    n->origins.emplace(filename,ft.getPos());

    n -> type = ft.type;
    n -> modifiers = ft.modifiers;
    n -> args = ft.args;
    return n;
}

ARCDOC::Variable* createVariableFt(VariableToken& vt,const std::string& filename)//
{
    ARCDOC::Variable* n = new ARCDOC::Variable(vt.name);
    n->origins.emplace(filename,vt.getPos());

    n -> type = vt.type;
    return n;
}

ARCDOC::Alias* createAliasFt(AliasToken& at,const std::string& filename)
{
    ARCDOC::Alias* n = new ARCDOC::Alias(at.name);
    n->origins.emplace(filename,at.getPos());

    n -> target = stringifyTokens(at.tokens);
    return n;
}

ARCDOC::Member* findToMerge(ARCDOC::Member* t,ARCDOC::Namespace& target)
{
    for (auto i=target.members.begin(); i != target.members.end(); ++i)
    {
        if (i->get()->isSame(*t))
        {
            return i->get();
        }
    }
    return nullptr;
}

ARCDOC::Member* findToMerge(ARCDOC::Member* t,ARCDOC::Structure& target)
{
    return nullptr;
}

void recursiveClassExtract(const std::vector<TokenEntity>& source, ARCDOC::Structure& target,VLA def, std::set<ARCDOC::Member*>& nm,std::map<std::string,ARCDOC::Member*>& sm, const std::string& filename)
{
    VLA level = def;
    for (const auto& i:source)
    {
        if (i.type == Ids::Class)
        {
            ClassToken& ct = i.token -> forceAs<ClassToken>();
            ARCDOC::Structure* n = createStructureFt(ct,filename);

            n->parent = &target;

            auto it = findToMerge(n,target);
            if (it == nullptr)
            {
                target.members[VlaToStr(level)].emplace_back(n);
                nm.emplace(n);
                sm.emplace("class",n);
                recursiveClassExtract(ct.tokens,*n,ct.level,nm,sm,filename);
            }
            else
            {
                it->mergeWith(std::move(*n));
                recursiveClassExtract(ct.tokens,*reinterpret_cast<ARCDOC::Structure*>(it),ct.level,nm,sm,filename);
                nm.emplace(it);
                sm.emplace("class",it);
                delete n;
            }
        }
        /*else if (i.type == Ids::Function)
        {
            FunctionToken& ft = i.token -> forceAs<FunctionToken>();
            ARCDOC::Function* n = createFunctionFt(ft,filename);
            target.members[VlaToStr(level)].emplace_back(n);
        }
        else if (i.type == Ids::Variable)
        {
            VariableToken& vt = i.token -> forceAs<VariableToken>();
            ARCDOC::Variable* n = createVariableFt(vt,filename);
            target.members[VlaToStr(level)].emplace_back(n);
        }
        else if (i.type == Ids::Alias)
        {
            AliasToken& at = i.token -> forceAs<AliasToken>();
            ARCDOC::Alias* n = createAliasFt(at,filename);
            target.members[VlaToStr(level)].emplace_back(n);
        }*/
        else if (i.type == Ids::Cas)
        {
            level = i.token -> forceAs<CasToken>().level;
        }
    }
}

void recursiveNamespaceSearch(const std::vector<TokenEntity>& source, ARCDOC::Namespace& target,std::set<ARCDOC::Member*>& nm,std::map<std::string,ARCDOC::Member*>& sm,const std::string& filename)
{
    for (const auto& i:source)
    {
        if (i.type == Ids::Namespace)
        {
            NamespaceToken& nt = i.token -> forceAs<NamespaceToken>();
            ARCDOC::Namespace* n = createNamespaceFt(nt,filename);

            n->parent = &target;

            auto it = findToMerge(n,target);
            if (it == nullptr)
            {
                target.members.emplace_back(n);
                nm.insert(n);
                sm.emplace("namespace",n);
                recursiveNamespaceSearch(nt.tokens,*n,nm,sm,filename);
            }
            else
            {
                it->mergeWith(std::move(*n));
                recursiveNamespaceSearch(nt.tokens,*reinterpret_cast<ARCDOC::Namespace*>(it),nm,sm,filename);
                nm.insert(it);
                sm.emplace("namespace",it);

                delete n;
            }
        }
        else if (i.type == Ids::Class)
        {
            ClassToken& ct = i.token -> forceAs<ClassToken>();
            ARCDOC::Structure* n = createStructureFt(ct,filename);

            n->parent = &target;

            auto it = findToMerge(n,target);

            if (it == nullptr)
            {
                target.members.emplace_back(n);
                nm.insert(n);
                sm.emplace("class",n);
                recursiveClassExtract(ct.tokens,*n,ct.level,nm,sm,filename);
            }
            else
            {
                it->mergeWith(std::move(*n));
                recursiveClassExtract(ct.tokens,*reinterpret_cast<ARCDOC::Structure*>(it),ct.level,nm,sm,filename);
                nm.insert(it);
                sm.emplace("class",it);

                delete n;
            }
        }
        /*else if (i.type == Ids::Function)
        {
            FunctionToken& ft = i.token -> forceAs<FunctionToken>();
            ARCDOC::Function* n = createFunctionFt(ft,filename);
            target.members.emplace_back(n);
        }
        else if (i.type == Ids::Alias)
        {
            AliasToken& at = i.token -> forceAs<AliasToken>();
            ARCDOC::Alias* n = createAliasFt(at,filename);
            target.members.emplace_back(n);
        }*/
    }
}

void tokenizerSetup(Tokenizer& tk)
{
    tk.addStage("splitting");
    tk.addStage("preprocessing");
    tk.addStage("merging");

    Stage* slt_stage = tk.findStage("splitting");
    Stage* prp_stage = tk.findStage("preprocessing");
    Stage* mrg_stage = tk.findStage("merging");

    setupSlt(slt_stage);
    setupPrp(prp_stage);
    setupMrg(mrg_stage);
}


void extractor(const std::vector<TokenEntity>& source,ARCDOC::Namespace& ns,std::set<ARCDOC::Member*>& nm,std::map<std::string,ARCDOC::Member*>& sm,const std::string& filename)
{
    //NULLSCR::printTokens(source,std::cerr,true);
    recursiveNamespaceSearch(source,ns,nm,sm,filename);
}

class CppModule: public ARCDOC::Module
{
private:
    void recursiveTreeExport(const ARCDOC::Member& m,ParseLib::XML::Tag* ret)
    {
        ParseLib::XML::Tag* parents = new ParseLib::XML::Tag();
        ret->children.emplace_back(parents);
        parents->name = "parents";
        ARCDOC::Member* parent = m.parent;
        while (parent != nullptr && parent -> parent != nullptr)
        {
            ParseLib::XML::Tag* tmp_p = new ParseLib::XML::Tag();
            tmp_p->name = "name";
            tmp_p->children.emplace_back(new ParseLib::XML::TextNode(parent->name));
            parents->children.emplace(parents->children.begin(),tmp_p);
            parent = parent -> parent;
        }
        if (m.getType() == typeid(ARCDOC::Namespace))
        {
            ret->attributes.emplace("type",ParseLib::XML::Value("namespace"));

            ParseLib::XML::Tag* tmp = new ParseLib::XML::Tag();
            ret->children.emplace_back(tmp);

            tmp->name="list";

            for(const auto& i: reinterpret_cast<const ARCDOC::Namespace&>(m).members)
            {
                std::unique_ptr<ParseLib::XML::Tag> tmp2(new ParseLib::XML::Tag());
                tmp2->name="item";
                if (i->name.size())
                    tmp2->attributes.emplace("name",ParseLib::XML::Value(i->name));

                recursiveTreeExport(*i,tmp2.get());

                tmp->children.emplace_back(std::move(tmp2));
            }
        }
        else if (m.getType() == typeid(ARCDOC::Structure))
        {
            ret->attributes.emplace("type",ParseLib::XML::Value("structure"));
        }
    }
protected:
    virtual void extractSymbols(const std::vector<NULLSCR::TokenEntity>& tokens) override
    {
        extractor(tokens,globalNamespace,newMembers,members,currentFile);
    }
    virtual void initTokenizer() override
    {
        tokenizerSetup(tokenizer);
    }
public:
    ARCDOC::Member* selectedSymbol;

    virtual std::set<std::string> getAvailableItems() override
    {
        return std::set<std::string>({
            "member_tree"
        });
    }
    virtual std::shared_ptr<ParseLib::XML::Tag> getItem(const std::string& name) override
    {
        std::shared_ptr<ParseLib::XML::Tag> ret(new ParseLib::XML::Tag());
        ParseLib::XML::Tag* tag = ret.get();
        tag->name = "container";
        if (name == "member_tree")
        {
            recursiveTreeExport(globalNamespace,tag);
        }
        return std::move(ret);
    }

    CppModule(): selectedSymbol(nullptr)
    {
        registerAction("select",selectElementCmd);
        registerAction("visit",visitElementCmd);
    }
    ARCDOC::Member* step(ARCDOC::Member* target,const std::string& name)
    {
        if (target->getType() == typeid(ARCDOC::Namespace))
        {
            ARCDOC::Namespace* ns = reinterpret_cast<ARCDOC::Namespace*>(target);
            for (const auto& i: ns->members)
            {
                if (i->name == name)
                {
                    return i.get();
                }
            }
        }
        else if (target->getType() == typeid(ARCDOC::Structure))
        {
            ARCDOC::Structure* st = reinterpret_cast<ARCDOC::Structure*>(target);
            for (const auto& i: st->members)
            {
                for (const auto& j: i.second)
                {
                    if (j->name == name)
                    {
                        return j.get();
                    }
                }
            }
        }
        return nullptr;
    }

    ARCDOC::Member* findMember(const std::string& p)
    {
        ARCDOC::Member* c = &globalNamespace;
        unsigned pos = 0,pos2 = 0;
        pos = p.find("::",0);
        while (pos != std::string::npos)
        {
            if (pos != pos2)
            {
                c = step(c,p.substr(pos2,pos - pos2));
                if (c == nullptr)
                    return nullptr;
            }
            pos2 = pos+2;
            pos = p.find("::",pos+2);
        }
        if (pos2 != p.size())
        {
            c = step(c,p.substr(pos2,p.size() - pos2));
        }
        return c;
    }

    bool selectElementCmd(const std::vector<std::string>& args)//TODO: handle last element, error messages
    {
        ConsoleHandler ch(args,
        {
            std::make_pair("v",0)
        });
        if (ch.values.size() != 1)
        {
            std::cout << CmdColors::yellow << "Expected one argument.\n" << CmdColors::none;
            return false;
        }
        ARCDOC::Member* c = findMember(ch.values.front());

        if (c == nullptr)
        {
            std::cout << CmdColors::yellow << "Element not found.\n" << CmdColors::none;
            return false;
        }
        selectedSymbol = c;
        if (ch.hasFlag("v") && selectedSymbol != nullptr)
            newMembers.erase(selectedSymbol);
        return true;
    }
    bool visitElementCmd(const std::vector<std::string>& args)
    {
        ConsoleHandler ch(args,{});
        if (ch.values.size() == 0)
        {
            if (selectedSymbol == nullptr)
                return false;
            newMembers.erase(selectedSymbol);
        }
        else
        {
            for (const auto& i:ch.values)
            {
                ARCDOC::Member* c = findMember(i);
                if (c != nullptr)
                    newMembers.erase(c);
                else
                    std::cout << CmdColors::red << i << " was not found.\n" << CmdColors::none;
            }
        }
        return true;
    }
};

extern "C" __declspec(dllexport) __cdecl ARCDOC::Module* moduleFactory()
{
    return new CppModule();
}

extern "C" __declspec(dllexport) __cdecl void moduleDeleter(ARCDOC::Module* m)
{
    delete m;
}
