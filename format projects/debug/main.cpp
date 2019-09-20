#include <generator.h>

#include <fstream>

class DebugGenerator: public ARCDOC::OutputGenerator
{
public:
    virtual void generateOutput(const std::string& path,const std::string& name,const ParseLib::JSON::Value& config) override
    {
        std::ofstream out(path+"/"+name+".txt");
        if (out.is_open())
        {
            std::set<std::string> items = getModule()->getAvailableItems();
            for (const auto& i:items)
            {
                out << "<!--" << i << "-->\n" << getModule()->getItem(i)->toString();
            }
            out.close();
        }
    }
};

extern "C" __declspec(dllexport) __cdecl ARCDOC::OutputGenerator* generatorFactory()
{
    return new DebugGenerator();
}

extern "C" __declspec(dllexport) __cdecl void generatorDeleter(ARCDOC::OutputGenerator* m)
{
    delete m;
}
