#ifndef GENERATOR_H
#define GENERATOR_H

#include <module.h>
#include <parselib/JSON/json.h>

namespace ARCDOC
{
    class OutputGenerator: public ActionProvider
    {
    protected:
        Module* m_;
    public:
        Module* getModule();
        void attachModule(Module* module);
        virtual void generateOutput(const std::string& path,const std::string& name,const ParseLib::JSON::Value& config) = 0;
        virtual void init() {}

        OutputGenerator(): m_(nullptr) {};

        virtual ~OutputGenerator() = default;
    };
}

#endif // GENERATOR_H
