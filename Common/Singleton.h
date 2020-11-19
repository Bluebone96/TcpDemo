#ifndef _SINGLETON_H_
#define _SINGLETON_H_

template<typename T>
class Singleton {
public:
    static T& GetInstance()
    {
        static T instance;
        return instance;
    }

    Singleton(const T&) = delete;
    Singleton(T&&) = delete;
    const T& operator=(const T&) = delete;
protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};

#endif
