//
// Created by chenshouyang on 25-6-24.
//

#ifndef SINGLETON_H
#define SINGLETON_H
#include <iostream>
#include <memory>
#include <mutex>

inline std::mutex mtx; // Mutex for thread safety

template<typename T>
class singleton {
protected:
    singleton(const singleton &)=delete;
    singleton &operator=(const singleton &)=delete;
    singleton() = default;
    static std::shared_ptr<T> instance;
    
public:
    template<typename...Arg>
    static std::shared_ptr<T> getInstance(Arg...arg);

    void print_address() {
        std::cout << "Singleton instance address: " << std::addressof(instance) << std::endl;
    }

    virtual ~singleton() = default;
};

template<typename T>
std::shared_ptr<T> singleton<T>::instance = nullptr;

template <typename T>
template <typename... Arg>
std::shared_ptr<T> singleton<T>::getInstance(Arg...arg)
{
    std::unique_lock<std::mutex> lock(mtx);
    static std::once_flag flag;
    std::call_once(flag, [&] {
        if (instance == nullptr) {
            singleton<T>::instance = std::shared_ptr<T>(new T(std::forward<Arg>(arg)...));
        }
    });
    return singleton<T>::instance;
}

#endif //SINGLETON_H
