namespace ParseLib
{
    class SyntaxError: public std::exception
    {
    private:
        std::string msg_;
    public:
        SyntaxError(const std::string&);
        SyntaxError(const SyntaxError&) = default;
        const char* what() const noexcept;
    };
}
