#include<string>

namespace TEST
{
    class A
    {
    public:
        std::string name;
    };
}

namespace TEST
{
    class B
    {
    private:
        struct C
        {
        protected:
            double v;
        };
    public:
        int pos;
    };
}
