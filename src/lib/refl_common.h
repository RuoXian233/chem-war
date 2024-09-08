#pragma once
#include <type_traits>
#include <tuple>


namespace engine {
    namespace refl_common {
        namespace details {
            // variable_traits
            template<typename T>
            struct variable_type {
                using type = T;
            };

            template<typename Class, typename T>
            struct variable_type<T Class::*> {
                using type = T;
            };

            // function_traits
            template<typename Func>
            struct basic_function_traits;

            template<typename Ret, typename ...Args>
            struct basic_function_traits<Ret(Args...)> {
                using args = std::tuple<Args...>;
                using returnType = Ret;
            };
        }
    
        template<typename T>
        using variable_type_t = typename details::variable_type<T>::type;

        namespace internal {
            template<typename T>
            struct basic_variable_traits {
                using type = variable_type_t<T>;
                static constexpr bool isMember = std::is_member_pointer_v<T>;
            };
        }

        template<typename T>
        struct variable_traits;

        template<typename T>
        struct variable_traits<T *> : internal::basic_variable_traits<T> {
            using pointerType = T *;
        };

        template<typename Class, typename T>
        struct variable_traits<T Class::*> : internal::basic_variable_traits<T Class::*> {
            using pointerType = T Class::*;
            using clazz = Class;
        };

        template<typename Func>
        struct function_traits;

        template<typename Ret, typename ...Args>
        struct function_traits<Ret(Args...)> : details::basic_function_traits<Ret(Args...)> {
            using type = Ret(Args...);
            using argsWithClass = std::tuple<Args...>;
            using pointer = Ret(*)(Args...);
            static constexpr bool isMember = false;
            static constexpr bool isConst = false;
        };

        template<typename Ret, typename Class, typename ...Args>
        struct function_traits<Ret(Class::*)(Args...)> 
            : details::basic_function_traits<Ret(Args...)> {
            using type = Ret (Class::*)(Args...);
            using clazz = Class;
            // `this` argument
            using argsWithClass = std::tuple<Class *, Args...>;
            using pointer = Ret (Class::*)(Args...);
            static constexpr bool isMember = true;
            static constexpr bool isConst = false;
        };

        template<typename Ret, typename Class, typename ...Args>
        struct function_traits<Ret(Class::*)(Args...) const> 
            : details::basic_function_traits<Ret(Args...)> {
            using type = Ret (Class::*)(Args...) const;
            // `this` argument
            using argsWithClass = std::tuple<Class *, Args...>;
            using pointer = Ret (Class::*)(Args...) const;
            static constexpr bool isMember = true;
            static constexpr bool isConst = true;
        };
    }
}
