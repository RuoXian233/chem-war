#pragma once
#include "refl_common.h"


#define SREFL_BEGIN_CLASS(X) \
template<> struct chem_war::srefl::Reflected<X> { static constexpr auto className = #X;

#define SREFL_FUNCTIONS(...) \
static constexpr auto functions = std::make_tuple(__VA_ARGS__);

#define SREFL_FUNC(f) chem_war::srefl::field_traits { f, #f }

#define SREFL_FIELDS(...) \
static constexpr auto fields = std::make_tuple(__VA_ARGS__);

#define SREFL_FIELD(field) chem_war::srefl::field_traits { field, #field }

#define SREFL_SUPER_CLASSES(...) \
static constexpr auto superClasses = std::make_tuple(__VA_ARGS__);

#define SREFL_SUPER_CLASS(c) chem_war::srefl::Reflected<c> {}

#define SREFL_END_CLASS() };


namespace chem_war {
    namespace srefl {
        template<typename T>
        struct Reflected;

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

            constexpr size_t ParameterCount() const {
                return std::tuple_size_v<typename traits::args>;
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

            constexpr size_t ParameterCount() const {
                return std::tuple_size_v<typename traits::args>;
            }
        };

        template<typename T>
        struct field_traits : public basic_field_traits<T, is_function_v<T>> {
            T pointer;
            constexpr field_traits(T &&pointer, std::string_view name) : pointer { pointer }, name { name } {}
            std::string_view name;
        };

        template<typename T>
        constexpr auto Reflect() {
            return Reflected<T> {};
        }

        // C++20
        template<size_t Index, typename ...Args, typename Class>
        void VisitTuple(const std::tuple<Args...> &tuple, Class *instance, auto func) {
            using tupleType = std::tuple<Args...>;
            if constexpr (Index >= std::tuple_size_v<tupleType>) {
                return;
            } else {
                auto element = std::get<Index>(tuple);
                func(instance, element);
                VisitTuple<Index + 1>(tuple, instance, func);
            }
        }

        template<size_t ...Index, typename Class, typename Tuple, typename Function>
        void ParseTuple(Tuple t, Class *instance, Function &&f, std::index_sequence<Index...>) {
            (f(instance, std::get<Index>(t)), ...);
        }

        template<typename ...Args>
        struct type_list {
            static constexpr size_t size = sizeof...(Args);
        };

        namespace details {
            template<typename>
            struct head;

            template<typename T, typename ...Remains>
            struct head<type_list<T, Remains...>> {
                using type = T;
            };

            template<typename>
            struct tail;

            template<typename T, typename ...Remains>
            struct tail<type_list<T, Remains...>> {
                using type = type_list<Remains...>;
            };

            template<typename, size_t>
            struct nth;

            template<typename T, typename ...Remains>
            struct nth<type_list<T, Remains...>, 0> {
                using type = T;
            };

            template<typename T, typename ...Remains, size_t N>
            struct nth<type_list<T, Remains...>, N> {
                using type = typename nth<type_list<Remains...>, N - 1>::type;
            };

            template<typename, template<typename> typename, size_t N>
            struct count;

            template<typename T, typename ...Remains, template<typename> typename F>
            struct count<type_list<T, Remains...>, F, 0> {
                static constexpr int value = F<T>::value ? 1 : 0;
            };

            template<typename T, typename ...Remains, template<typename> typename F, size_t N>
            struct count<type_list<T, Remains...>, F, N> {
                static constexpr int value = (F<T>::value ? 1 : 0) + count<type_list<Remains...>, F, N - 1>::value;
            };

            template<typename, template<typename> typename>
            struct map;

            template<typename ...Args, template<typename> typename F>
            struct map<type_list<Args...>, F> {
                using type = type_list<typename F<Args>::type...>;
            };

            template<typename, typename>
            struct cons;

            template<typename ...Args, typename T>
            struct cons<T, type_list<Args...>> {
                using type = type_list<T, Args...>;
            };

            template<typename, typename>
            struct concat;

            template<typename ...Args1, typename ...Args2>
            struct concat<type_list<Args1...>, type_list<Args2...>> {
                using type = type_list<Args1..., Args2...>;
            };

            template<typename>
            struct init;

            template<typename T>
            struct init<type_list<T>> {
                using type = type_list<>;
            };

            template<typename T, typename ...Remains>
            struct init<type_list<T, Remains...>> {
                using type = typename cons<T, typename init<type_list<Remains...>>::type>::type;
            };

            template<typename, template<typename> typename>
            struct filter;

            template<template<typename> typename Predicate>
            struct filter<type_list<>, Predicate> {
                using type = type_list<>;
            };

            template<typename T, typename ...Remains, template<typename> typename Predicate>
            struct filter<type_list<T, Remains...>, Predicate> {
                using type = std::conditional_t<Predicate<T>::value, 
                    typename cons<T, typename filter<type_list<Remains...>, Predicate>::type>::type, 
                    typename filter<type_list<Remains...>, Predicate>::type>;
            };
        }

        template<typename TypeList>
        using head = typename srefl::details::head<TypeList>::type;
        template<typename TypeList>
        using tail = typename srefl::details::head<TypeList>::type;
        template<typename TypeList, size_t N>
        using nth = typename srefl::details::nth<TypeList, N>::type;

        template<typename TypeList, template<typename> typename F>
        constexpr int count = srefl::details::count<TypeList, F, TypeList::size - 1>::value; 

        template<typename TypeList, template<typename> typename F>
        using map = typename srefl::details::map<TypeList, F>::type; 

        template<typename TypeList, typename T>
        using cons = typename srefl::details::cons<T, TypeList>::type;

        template<typename TypeList1, typename TypeList2>
        using concat = typename srefl::details::concat<TypeList1, TypeList2>::type; 

        template<typename TypeList>
        using init = typename srefl::details::init<TypeList>::type;

        template<typename TypeList, template<typename> typename Predicate>
        using filter = typename srefl::details::filter<TypeList, Predicate>::type;
    }
}
