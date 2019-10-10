#include <generator.h>

#include <fstream>
#include <iostream>

class DebugGenerator: public ARCDOC::OutputGenerator
{
public:
    virtual void generateOutput(const std::string& path,const std::string& name,const ParseLib::JSON::Value& config) override
    {
        std::ofstream out(path+"/"+name+".xml");
        if (out.is_open())
        {
            if (getModule() != nullptr)
            {
                std::set<std::string> items = getModule()->getAvailableItems();
                for (const auto& i:items)
                {
                    out << "<!--" << i << "-->\n" << getModule()->getItem(i)->toString();
                }
            }
            out.close();
        }
    }
    DebugGenerator(): ARCDOC::OutputGenerator() {}
};

extern "C" __declspec(dllexport) __cdecl ARCDOC::OutputGenerator* generatorFactory()
{
    return new DebugGenerator();
}

extern "C" __declspec(dllexport) __cdecl void generatorDeleter(ARCDOC::OutputGenerator* m)
{
    delete m;
}
