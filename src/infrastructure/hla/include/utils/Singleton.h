#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <cassert>
#include <cstddef>
#include <memory>

namespace events {

    template <class T>
    class Singleton
    {

    public:
        virtual ~Singleton() = default;

        static T& getInstance()
        {
            static std::unique_ptr<T> theInstance{ new T() };
            return *theInstance;
        }

    protected:
        Singleton() = default;

    private:
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;
    };

}

#endif // SINGLETON_HPP