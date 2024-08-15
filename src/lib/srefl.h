#pragma once
#include "refl_common.h"


namespace chem_war {
    namespace srefl {
        template<typename T>
        struct TypeInfo;

        template<typename RetT, typename ...Params>
        auto function_pointer_type(RetT(*)(Params...)) -> RetT(*)(Params...);

        template<typename RetT, typename Class, typename ...Params>
        auto function_pointer_type(RetT(Class::*)(Params...)) -> RetT(Class::*)(Params...);

        template<typename RetT, typename Class, typename ...Params>
        auto function_pointer_type(RetT(Class::*)(Params...) const) -> RetT(Class::*)(Params...) const;

        template<auto F>
        using function_pointer_type_t = decltype(function_pointer_type(F));

        // template<auto F>
        // using function_traits_t = ::chem_war::refl_common::function_traits<function_pointer_type_t<F>>;
    
        template<typename T, bool isFunc>
        struct basic_field_traits;

        template<typename T>
        struct basic_field_traits<T, true> : public refl_common::function_traits<T> {
            using traits = refl_common::function_traits<T>;

            constexpr bool IsMember() const {
                return traits::isMember;
            }

            constexpr bool IsConst() const {
                return traits::isConst;
            }

            constexpr bool IsFunction() const {
                return true;
            }

            constexpr bool IsVariable() const {
                return false;
            }
        };

        template<typename T>
        struct is_function {
            static constexpr bool value = std::is_function_v<std::remove_pointer_t<T>> 
                || std::is_member_function_pointer_v<T>;
        };

        template<typename T>
        constexpr bool is_function_v = is_function<T>::value;

        template<typename T>
        struct basic_field_traits<T, false> : public refl_common::variable_traits<T> {
            using traits = refl_common::variable_traits<T>;

            constexpr bool IsMember() const {
                return traits::isMember;
            }

            constexpr bool IsConst() const {
                return traits::isConst;
            }

            constexpr bool IsFunction() const {
                return false;
            }

            constexpr bool IsVariable() const {
                return true;
            }
        };

        template<typename T>
        struct field_traits : basic_field_traits<T, is_function_v<T>> {
            T pointer;
            constexpr field_traits(T &&pointer) : pointer { pointer } {}
        };
    }
}
