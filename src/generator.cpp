#include "generator.h"

namespace ARCDOC
{
    Module* OutputGenerator::getModule()
    {
        return m_;
    }
    void OutputGenerator::attachModule(Module* module)
    {
        m_ = module;
    }
}
