#include "game.h"
#include "lib/srefl.h"
#include "lib/drefl.h"



// template<>
// struct chem_war::srefl::Reflected<chem_war::Vec2> {
//     static constexpr auto functions = std::make_tuple(
//         chem_war::srefl::field_traits { &chem_war::Vec2::CreateRect },
//         chem_war::srefl::field_traits { &chem_war::Vec2::ToString }
//     );

//     static constexpr auto fields = std::make_tuple(
//         chem_war::srefl::field_traits { &chem_war::Vec2::x },
//         chem_war::srefl::field_traits { &chem_war::Vec2::y }
//     );
// };

struct A{
    int v;
    int r;
};

SREFL_BEGIN_CLASS(A)
    SREFL_FIELDS(
        SREFL_FIELD(&A::v),
        SREFL_FIELD(&A::r)
    )
SREFL_END_CLASS()

SREFL_BEGIN_CLASS(chem_war::Vec2)
    SREFL_SUPER_CLASSES(
        SREFL_SUPER_CLASS(A)
    )
    SREFL_FUNCTIONS(
        SREFL_FUNC(&chem_war::Vec2::ToString)
    )
    SREFL_FIELDS(
        SREFL_FIELD(&chem_war::Vec2::x),
        SREFL_FIELD(&chem_war::Vec2::y)
    )
SREFL_END_CLASS()


template<typename T>
struct is_integral_v_ {
    static constexpr bool value = std::is_integral_v<T>;
};

template<typename T>
struct integral_to_float {
    using type = std::conditional_t<std::is_integral_v<T>, float, T>;
};

template<typename T>
struct remove_char_t {
    static constexpr bool value = !std::is_same_v<T, char>;
};


enum class E {
    v1 = 1,
    v2 = 2
};


#undef main
int main(int argc, char *argv[]) {
    constexpr auto info = chem_war::srefl::Reflect<A>();
    auto i = A { 2 };
    // C++17 or later
    chem_war::srefl::ParseTuple(info.fields, &i, [](auto, auto) {}, std::make_index_sequence<std::tuple_size_v<decltype(info.fields)>>());

    using tp = chem_war::srefl::type_list<int, char, float>;
    using first_t = chem_war::srefl::head<tp>;
    using tail_t = chem_war::srefl::tail<tp>;
    using r_t = chem_war::srefl::nth<tp, 2>;
    constexpr auto v = chem_war::srefl::count<tp, is_integral_v_>;
    using new_l = chem_war::srefl::map<tp, integral_to_float>;
    using j = chem_war::srefl::init<tp>;
    using fl = chem_war::srefl::filter<tp, remove_char_t>;

    chem_war::drefl::RegistVariable<E>().Register("E").Add("v1", E::v1).Add("v2", E::v2);
    chem_war::drefl::RegistVariable<chem_war::Vec2>().Register("chem_war::Vec2")
                .AddField<decltype(&chem_war::Vec2::x)>("x")
                .AddField<decltype(&chem_war::Vec2::y)>("y")
                .AddMethod<decltype(&chem_war::Vec2::ToString)>("ToString");

    auto t = chem_war::drefl::GetType("E");
    std::cout << t->AsEnum()->GetItems()[0].value << std::endl;

    std::cout << chem_war::drefl::GetType("chem_war::Vec2")->AsClass()->GetName() << std::endl;

    // chem_war::Game::Prepare(argc, argv);
    // chem_war::Game::Run();
    // chem_war::Game::Quit();
    return 0;
}
