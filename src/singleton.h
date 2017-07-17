//
// @ Create by CaiZhili on 2017/7/17
// @ Bref
//
#ifndef NF_TEST_SINGLETON_H
#define NF_TEST_SIGNLETON_H

namespace nf {

template <typename T>
class singleton {
public:
    static T& get_mutable_instance() {
        return get_instance();
    }

    static const T& get_const_instance() {
        return get_instance();
    }

private:
    singleton(const singleton&);
    singleton& operator=(const singleton&);

private:
    static T& instance;
    static void use(T const *Ta) {  }
    static T& get_instance() {
        static T t;
        use(&t);
        return t;
    };
};

template <typename T>
T& singleton<T>::instance = get_instance();

} //nf

#endif //NF_TEST_SINGLETON_H
