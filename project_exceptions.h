#pragma once
#include <exception>
#include <string>

class BaseException : public std::exception
{
private:
    std::string m_source;
    std::string m_message;

public:
    BaseException(const std::string &source, const std::string &message) : m_source(source),
                                                                           m_message(message)
    {

    }

    ~BaseException() override = default;
};

class MapperException : public BaseException
{
private:

public:
    MapperException(const std::string &source, const std::string &message) :
                                                      BaseException(source, message)
    {

    }

    MapperException(const MapperException &) = delete;
    MapperException &operator=(MapperException &rhs) = delete;

    ~MapperException() override = default;
};

template <class ExceptionType>
void ThrowIfFalse(bool condition, const std::string &source, const std::string &message)
{
    if(!condition)
    {
        throw(ExceptionType(source, message));
    }
}