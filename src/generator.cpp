#include "generator.h"

namespace ARCDOC
{
    Module* OutputGenerator::getModule() const
    {
        return m_;
    }
    void OutputGenerator::attachModule(Module& module)
    {
        m_ = &module;
    }
}
