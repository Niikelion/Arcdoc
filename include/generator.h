#ifndef GENERATOR_H
#define GENERATOR_H

#include <module.h>
#include <parselib/JSON/json.h>

namespace ARCDOC
{
    class OutputGenerator
    {
    private:
        Module* m_;
    public:
        Module* getModule() const;
        void attachModule(Module& module);
        virtual void generateOutput(const std::string& path,const std::string& name,const ParseLib::JSON::Value& config) = 0;
        virtual void init() {}

        virtual ~OutputGenerator() = default;
    };
}

#endif // GENERATOR_H
