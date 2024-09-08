#include "game.h"
#include "lib/drefl.h"
#include "lib/srefl.h"


enum class TestEnum {
    A = 1, B = 3
};

struct X {
    int a;
    bool b;

    X() : X(0, false) {}
    X(int v, bool br) : a(v), b(br) {
        std::cout << "X constructed" << std::endl;
    }

    X(X &&x) {
        std::cout << "X moved" << std::endl;
        this->a = x.a;
        this->b = x.b;
    }

    X(const X &x) {
        std::cout << "X copied" << std::endl;
        this->a = x.a;
        this->b = x.b;
    }

    ~X() {
        std::cout << "X destructed" << std::endl;
    }

    TestEnum f() { std::cout << "X.f()" << std::endl; return TestEnum::B; }
    void g(int x, TestEnum y) { std::cout << "X.g()" << std::endl; }
};

#undef main
int main(int, char *[]) {
    // drefl::RegistReflection<TestEnum>()
    //     .Regist("TestEnum")
    //     .Add("A", TestEnum::A)
    //     .Add("B", TestEnum::B);

    drefl::RegistReflection<X>()
        .Regist("X")
        .AddField("a", &X::a)
        .AddField("b", &X::b)
        .AddMethod("f", &X::f)
        .AddMethod("g", &X::g);

    auto classInfo = drefl::GetType("X")->AsClass();
    drefl::any target = drefl::MakeSteal(X(114514, false));
    drefl::any newTarget = std::move(target);

    auto av = classInfo->GetFields()[0]->Call({ newTarget });
    auto ff = classInfo->GetMethods()[0]->Call({ newTarget });
    auto gg = classInfo->GetMethods()[1]->Call({ newTarget, drefl::MakeCopy(TestEnum::A) });
    auto rt = drefl::any::TryCast<TestEnum>(ff);
    std::cout << static_cast<int>(*rt) << std::endl;
    auto r = drefl::any::TryCast<int>(av);
    std::cout << *r << std::endl;

    // chem_war::Game::Prepare(argc, argv);
    // chem_war::Game::Run();
    // chem_war::Game::Quit();
    return 0;
}
