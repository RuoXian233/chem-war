#include "refl_common.h"
#include <string>
#include <vector>
#include <list>


namespace chem_war {
    namespace drefl {
        class Numeric;
        class Enum;
        class Class;
        
        class Type {
        public:
            enum class Kind {
                _Numeric,
                _Enum,
                _Class
            };

            template<typename T>
            friend class EnumFactory;

            template<typename T>
            friend class ClassFactory;

            Type(const std::string &name, Kind kind) : name(name), type(kind) {}
            virtual ~Type() = default;

            auto &GetName() const {
                return this->name;
            }

            const Numeric *AsNumeric() const {
                if (type == Kind::_Numeric) {
                    return reinterpret_cast<const Numeric *>(this);
                } else {
                    return nullptr;
                }
            }

            const Enum *AsEnum() const {
                if (type == Kind::_Enum) {
                    return reinterpret_cast<const Enum *>(this);
                } else {
                    return nullptr;
                }
            }

            const Class *AsClass() const {
                if (type == Kind::_Class) {
                    return reinterpret_cast<const Class *>(this);
                } else {
                    return nullptr;
                }
            }
        private:
            std::string name;
            Kind type;
        };
        std::list<const Type *> __typeList;


        class Numeric : public Type {
        public:
            enum class Kind {
                Int8, Int16, Int32, Int64, Int128, Float, Double, Invilid
            };

            Numeric(Kind kind, bool isSigned) : kind(kind), isSigned(isSigned), Type(GetKindName(kind), Type::Kind::_Numeric) {}
            auto GetKind() const {
                return this->kind; 
            }

            bool IsSigned() const {
                return this->isSigned;
            }

            template<typename T>
            static Numeric Create() {
                return Numeric { DetectKind<T>(), std::is_signed_v<T> };
            }

        private:
            bool isSigned;
            Kind kind;

            static std::string GetKindName(Kind kind) {
                switch (kind) {
                case Kind::Int8:
                    return "Int8";
                case Kind::Int16:
                    return "Int16";
                case Kind::Int32:
                    return "Int32";
                case Kind::Int64:
                    return "Int64";
                case Kind::Int128:
                    return "Int128";
                case Kind::Float:
                    return "Float";
                case Kind::Double:
                    return "Double";
                default:
                    return "Numeric[Unknown]";
                }
            }

            template<typename T>
            static Kind DetectKind() {
                if constexpr (std::is_same_v<T, char>) {
                    return Kind::Int8;
                } else if constexpr (std::is_same_v<T, short>) {
                    return Kind::Int16;
                } else if constexpr (std::is_same_v<T, int>) {
                    return Kind::Int32;
                } else if constexpr (std::is_same_v<T, long>) {
                    return Kind::Int64;
                } else if constexpr (std::is_same_v<T, long long>) {
                    return Kind::Int128;
                } else if constexpr (std::is_same_v<T, float>) {
                    return Kind::Float;
                } else if constexpr (std::is_same_v<T, double>) {
                    return Kind::Double;
                } else {
                    return Kind::Invilid;
                }
            }
        };
        

        class Enum : public Type {
        public:
            struct Item {
                using value_t = long;

                std::string name;
                value_t value;
            };

            Enum() : Type("Enum[Uninitalized]", Type::Kind::_Enum) {}
            Enum(const std::string &name) : Type(name, Type::Kind::_Enum) {}

            template<typename T>
            void AddItem(const std::string &name, T value) {
                this->items.emplace_back(Item { name, static_cast<typename Item::value_t>(value) });
            } 

            auto &GetItems() const {
                return this->items;
            }
        private:
            std::vector<Item> items;

        };

        struct MemberVariable {
            std::string name;
            const Type *tp;

            template<typename T>
            static MemberVariable Create(const std::string &name);
        };

        struct MemberFunction {
            std::string name;
            const Type *retTp;
            std::vector<const Type *> paramTp;

            template<typename T>
            static MemberFunction Create(const std::string &name);

        private:
            template<typename Params, size_t ...Index>
            static std::vector<const Type *> ConvertTypeList2Vector(std::index_sequence<Index...>);
        };

        class Class : public Type {
        public:
            Class() : Type("Class[Uninitalized]", Type::Kind::_Class) {}
            Class(const std::string &name) : Type(name, Type::Kind::_Class) {}
        
            void AddMember(MemberVariable &&member) {
                this->fields.emplace_back(member);
            }   

            void AddMember(MemberFunction &&member) {
                this->methods.emplace_back(member);
            }

            auto &GetFields() const {
                return this->fields;
            }

            auto &GetMethods() const {
                return this->methods;
            }
        
        private:
            std::vector<MemberVariable> fields;
            std::vector<MemberFunction> methods;
        };

        class Function : public Type {

        };

        
        template<typename T>
        class NumericFactory final {
        public:
            static NumericFactory &Instance() {
                static NumericFactory inst { Numeric::Create<T>() };
                static bool saved = false;
                if (!saved) {
                    __typeList.push_back(&inst.Info());
                    saved = true;
                }
                return inst;
            }

            auto &Info() const {
                return this->info;
            }
        private:
            NumericFactory(Numeric &&info) : info(std::move(info)) {}

            Numeric info;
        };

        template<typename T>
        class EnumFactory final {
        public:
            static EnumFactory &Instance() {
                static EnumFactory inst;
                static bool saved = false;
                if (!saved) {
                    __typeList.push_back(&inst.Info());
                    saved = true;
                }
                return inst;
            }

            EnumFactory &Register(const std::string &name) {
                info.name = name;
                return *this;
            }

            template<typename U>
            EnumFactory &Add(const std::string &name, U value) {
                info.AddItem(name, value);
                return *this;
            }

            void UnRegister() {
                info = Enum();
            }

            auto &Info() const {
                return this->info;
            }
        private:
            Enum info;
        };

        template<typename T>
        class ClassFactory final {
        public:
            static ClassFactory& Instance() {
                static ClassFactory instance;
                static bool saved = false;
                if (!saved) {
                    __typeList.push_back(&instance.Info());
                    saved = true;
                }
                return instance;
            }

            ClassFactory &Register(const std::string &name) {
                info.name = name;
                return *this;
            }

            template<typename U>
            ClassFactory &AddField(const std::string &name) {
                info.AddMember(MemberVariable::Create<U>(name));
                return *this;
            }

            template<typename U>
            ClassFactory &AddMethod(const std::string &name) {
                info.AddMember(MemberFunction::Create<U>(name));
                return *this;
            }

            void UnRegister() {
                info = Class();
            }

            auto &Info() const {
                return this->info;
            }
        private:
            Class info;
        };

        template<typename T>
        class Factory final {
        public:
            static auto &GetFactory() {
                using type = std::remove_reference_t<T>;

                if constexpr (std::is_fundamental_v<type>) {
                    return NumericFactory<type>::Instance();
                } else if constexpr (std::is_enum_v<type>) {
                    return EnumFactory<type>::Instance();
                } else if constexpr (std::is_class_v<type>) {
                    return ClassFactory<type>::Instance();
                } else {
                    assert(false && "Type not implemented");
                }
            }
        };

        template<typename T>
        auto &RegistVariable() {
            return Factory<T>::GetFactory();
        }

        template<typename T>
        const Type *GetType() {
            return &Factory<T>::GetFactory().Info();
        }

        const Type *GetType(const std::string &name) {
            for (auto typeInfo : __typeList) {
                if (typeInfo->GetName() == name) {
                    return typeInfo;
                }
            }
            return nullptr;
        }

        template<typename Params, size_t ...Index>
        std::vector<const Type *> MemberFunction::ConvertTypeList2Vector(std::index_sequence<Index...>) {
            return { GetType<std::tuple_element_t<Index, Params>>() ... };
        }

        template<typename T>
        MemberVariable MemberVariable::Create(const std::string &name) {
                using tp = typename refl_common::variable_traits<T>::type;
                return MemberVariable { name, GetType<tp>() };
        }

        template<typename T>
        MemberFunction MemberFunction::Create(const std::string &name) {
            using tp = refl_common::function_traits<T>;
            return MemberFunction { 
                name, GetType<typename tp::returnType>(), 
                MemberFunction::ConvertTypeList2Vector<typename tp::args>(std::make_index_sequence<std::tuple_size_v<typename tp::args>>()) 
            };
        }
    }
}
