#pragma once

#include "refl_common.h"
#include <list>


namespace engine::drefl {
        class Numeric;
        class Enum;
        class Class;
        class Bool;

        class Type;
        std::list<const Type *> _typelist;

    class any {
        public:
            enum class StorageType {
                Copy, Steal, Ref, ConstRef, Empty
            };

            struct Operations {
                any (*Copy)(const any &) = {};
                any (*Steal)(any &) = {};
                void (*Release)(any &) = {};
            };

            void *payload {};
            Type *typeInfo;
            StorageType storage;
            Operations op;

            any() = default;

            ~any() {
                if (op.Release && (storage == StorageType::Copy || storage == StorageType::Steal)) {
                    op.Release(*this);
                }
            }

            any(const any &o) : typeInfo { o.typeInfo }, storage { o.storage }, op { o.op } {
                if (op.Copy) {
                    auto newAny = op.Copy(o);
                    payload = newAny.payload;
                    newAny.payload = nullptr;
                    newAny.op.Release = nullptr;

                } else {
                    storage = StorageType::Empty;
                    typeInfo = nullptr;
                }
            }

            any(any &&o) : typeInfo { std::move(o.typeInfo) }, storage { std::move(o.storage) }, op { std::move(o.op) } {
                if (op.Copy) {
                    auto newAny = op.Copy(o);
                    payload = newAny.payload;
                    newAny.payload = nullptr;
                    newAny.op.Release = nullptr;

                } else {
                    storage = StorageType::Empty;
                    typeInfo = nullptr;
                }
            }

            any &operator=(const any &o) {
                if (&o != this) {
                    typeInfo = o.typeInfo;
                    storage = o.storage;
                    op = o.op;
                    if (op.Copy) {
                        auto newAny = op.Copy(o);
                        payload = newAny.payload;
                        newAny.payload = nullptr;
                        newAny.op.Release = nullptr;
                    } else {
                        storage = StorageType::Empty;
                        typeInfo = nullptr;
                    }
                }
                return *this;
            }

            any &operator=(any &&o) {
                if (&o != this) {
                    typeInfo = std::move(o.typeInfo);
                    storage = std::move(o.storage);
                    op = std::move(o.op);
                    payload = std::move(o.payload);
                }
                return *this;
            }

            template<typename T>
            static T *TryCast(any &elem);
        };

        class Type {
        public:
            template<typename T>
            friend class EnumFactory;

            template<typename T>
            friend class ClassFactory;

            enum class TypeKind {
                Numeric, Enum, Class, Bool, Pointer, Void
            };

            Type(const std::string &name, TypeKind kind) : tpName(name), kind(kind) {}
            virtual ~Type() = default;

            auto &GetTypeName() const{
                return this->tpName;
            }

            auto &GetKind() const {
                return this->kind;
            }


            template<typename T>
            const T *TryCast() const {
                return const_cast<const T *>(dynamic_cast<T *>(const_cast<Type *>(this)));
            }

            const Numeric *AsNumeric() const {
                if (this->kind != TypeKind::Numeric) {
                    assert(false && "Invilid type cast");
                }
                return TryCast<Numeric>();
            }

            const Enum *AsEnum() const {
                if (this->kind != TypeKind::Enum) {
                    assert(false && "Invilid type cast");
                }
                return TryCast<Enum>();
            }

            const Class *AsClass() const {
                if (this->kind != TypeKind::Class) {
                    assert(false && "Invilid type cast");
                }
                return TryCast<Class>();
            }

        protected:
            std::string tpName;
            TypeKind kind;
        };


        class Numeric : public Type {
        public:
            enum class NumericKind {
                Char, Short, Int, Float, Double, Long, LongLong, Unknown
            };

            Numeric(NumericKind kind, bool _signed) : isSigned(_signed), nkind(kind),
            Type("Numeric", Type::TypeKind::Numeric)
            {}

            auto GetKind() {
                return this->nkind;
            }

            bool Signed() {
                return this->isSigned;
            }

            template<typename T>
            static Numeric Create() {
                return Numeric(Numeric::IdentifyKind<T>(), std::is_signed_v<T>);
            }

            void SetValue(double value, any &elem) {
                if (elem.typeInfo->GetKind() == Type::TypeKind::Numeric) {
                    switch (elem.typeInfo->AsNumeric()->nkind) {
                        case NumericKind::Char:
                            if (!elem.typeInfo->AsNumeric()->isSigned) {
                                *(unsigned char *) elem.payload = (unsigned char) value;
                                break;
                            }
                            *(char *) elem.payload = (char) value;
                            break;
                        case NumericKind::Short:
                            if (!elem.typeInfo->AsNumeric()->isSigned) {
                                *(unsigned short *) elem.payload = (unsigned short) value;
                                break;
                            }
                            *(short *) elem.payload = (short) value;
                            break;
                        case NumericKind::Int:
                            if (!elem.typeInfo->AsNumeric()->isSigned) {
                                *(unsigned int *) elem.payload = (unsigned int) value;
                                break;
                            }
                            *(int *) elem.payload = (int) value;
                            break;
                        case NumericKind::Float:
                            *(float *) elem.payload = value;
                            break;
                        case NumericKind::Double:
                            *(double *) elem.payload = value;
                            break;
                        case NumericKind::Long:
                            if (!elem.typeInfo->AsNumeric()->isSigned) {
                                *(unsigned *) elem.payload = (unsigned long) value;
                                break;
                            }
                            *(long *) elem.payload = (long) value;
                            break;
                        case NumericKind::LongLong:
                            if (!elem.typeInfo->AsNumeric()->isSigned) {
                                *(unsigned long long *) elem.payload = (unsigned long long) value;
                                break;
                            }
                            *(long long *) elem.payload = (long long) value;
                            break;
                        case NumericKind::Unknown:
                            assert(false && "Invilid operation on unknown type");
                    }
                } else {
                    assert(false && "Invilid operation on corrupted type");
                }
            }

            void SetValue(long long value, any &elem) {

            }

        private:
            bool isSigned;
            NumericKind nkind;

            static std::string GetKindName(NumericKind kind) {
                switch (kind) {
                    case NumericKind::Char:
                        return "Char";
                    case NumericKind::Int:
                        return "Int";
                    case NumericKind::Float:
                        return "Float";
                    case NumericKind::Double:
                        return "Double";
                    case NumericKind::Long:
                        return "Long";
                    case NumericKind::LongLong:
                        return "Int128";
                    case NumericKind::Short:
                        return "Int16";
                    case NumericKind::Unknown:
                        return "Unknown";
                    default:
                        assert(false);
                }
            }

            template<typename T>
            static NumericKind IdentifyKind() {
                if constexpr (std::is_same_v<T, char>) {
                    return NumericKind::Char;
                } else if constexpr (std::is_same_v<T, int>) {
                    return NumericKind::Int;
                } else if constexpr (std::is_same_v<T, float>) {
                    return NumericKind::Float;
                } else if constexpr (std::is_same_v<T, double>) {
                    return NumericKind::Double;
                } else if constexpr (std::is_same_v<T, long>) {
                    return NumericKind::Long;
                } else if constexpr (std::is_same_v<T, long long>) {
                    return NumericKind::LongLong;
                } else if constexpr (std::is_same_v<T, short>) {
                    return NumericKind::Short;
                } else {
                    return NumericKind::Unknown;
                }
            }
        };

        class Enum : public Type {
        public:
            struct EnumItem {
                using value_type = long;

                std::string name;
                value_type value;
            };

            Enum() : Type("Enum[Unknown]", Type::TypeKind::Enum) {}
            Enum(const std::string &enumName) : Type(enumName, Type::TypeKind::Enum) {}

            template<typename T>
            void Add(const std::string &name, T value) {
                items.push_back(EnumItem { name, static_cast<EnumItem::value_type>(value) });
            }

            auto GetItems() const {
                return this->items;
            }
        private:
            std::vector<EnumItem> items;
        };

        template<typename T>
        const Type *GetType();

        class Member {
        public:
            // If target is a field, args = { ptr to this }
            // Else if target is a method, args = { ptr to this, args... }
            virtual any Call(const std::vector<any> &args) = 0;
        };

    template<typename T>
      any MakeCopy(const T &);

    template<typename T>
    any MakeSteal(T &&);

    template<typename T>
    any MakeRef(T &);

    template<typename T>
    any MakeConstRef(const T &);

        template<typename Clazz, typename Tp>
        struct Field : public Member {
            std::string name;
            const Type *type;
            Tp Clazz::* ptr;

            Field() = default;
            Field(const std::string &name, const Type *tp, decltype(Field::ptr) p) {
                this->name = name;
                this->type = tp;
                this->ptr = p;
            }

            static Field *Create(const std::string &name, Tp Clazz::* ptr) {
                return new Field(name, GetType<Tp>(), ptr);
            }

            any Call(const std::vector<any> &args) override {
                assert(args.size() == 1 && args[0].typeInfo == GetType<Clazz>() && "Illegal call");
                Clazz *inst = (Clazz *) args[0].payload;
                auto v = inst->*ptr;
                return MakeCopy(v);
            }
        };

    template<typename T, bool IsConst = false>
    std::conditional_t<IsConst, const T &, T &> Unwrap(any &value) {
        assert(value.typeInfo == GetType<T>());
        if constexpr (IsConst) {
            if (value.storage != any::StorageType::Ref) {
                assert(false && "Unsupported reference object");
            }
            return *(const T *) value.payload;

        } else {
            return *(T *) value.payload;
        }
    }

        template<typename Clazz, typename Tp, size_t ...Index, typename ...Args>
        any InnerCall(Tp (Clazz::* ptr)(Args...), const std::vector<any> &params, std::index_sequence<Index...>) {
            if constexpr (std::is_same_v<Tp, void>) {
                (((Clazz *) params[0].payload)->*ptr)(Unwrap<Args>(std::remove_const_t<any &>(params[Index + 1]))...);
                return any();
            } else {
                return MakeCopy((((Clazz *) params[0].payload)->*ptr)(Unwrap<Args>(std::remove_const_t<any &>(params[Index + 1]))...));
            }
        }

        template<typename Clazz, typename Tp, typename Args, typename Func>
        struct Method : public Member {
            std::string name;
            const Type *ret;
            std::vector<const Type *> args;
            Func ptr;

        Method() = default;
        Method(const std::string &name, const Type *tp, const std::vector<const Type *> &args, decltype(Method::ptr) p) {
            this->name = name;
            this->ret = tp;
            this->ptr = p;
            this->args = args;
        }

        static Method *Create(const std::string &name, decltype(ptr) ptr) {
                return new Method (name, GetType<Tp>(),
                    CvtTypeList2Vector<Args>(std::make_index_sequence<std::tuple_size_v<Args>>()), ptr);
            }

            any Call(const std::vector<any> &args) override {
                assert(args.size() == this->args.size() + 1 && "Argument count mismatched");
                for (int i = 0; i < this->args.size(); i++) {
                    assert(args[i + 1].typeInfo == this->args[i] && std::format("Argument {} with mismatched type", i + 1).c_str());
                }

                return InnerCall(ptr, args, std::make_index_sequence<std::tuple_size_v<Args>>());
            }

        private:
            template<typename Params, size_t ...Index>
         static std::vector<const Type *> CvtTypeList2Vector(std::index_sequence<Index...>)  {
                return { GetType<std::tuple_element_t<Index, Params>>()...};
            }
        };

        class Class : public Type {
        public:
            Class() : Type("Class[Unknown]", Type::TypeKind::Class) {}
            Class(const std::string &clsName) : Type(clsName, Type::TypeKind::Class) {}

            void AddField(Member *f) {
                this->fields.push_back(f);
            }

            void AddMethod(Member *m) {
                this->methods.push_back(m);
            }

            auto &GetFields() const {
                return this->fields;
            }

            auto &GetMethods() const {
                return this->methods;
            }

        private:
            std::vector<Member *> fields;
            std::vector<Member *> methods;
        };

        class Pointer : public Type {
        public:
            Pointer(const Type *dest) :
                Type(std::format("{}*", dest->GetTypeName()), Type::TypeKind::Pointer),  dest(dest) {}
        private:
            const Type *dest;
        };

        class Bool : public Type {
        public:
            Bool() : Type("Bool", Type::TypeKind::Bool) {}
        };

        class Void : public Type {
        public:
            Void() : Type("Void", Type::TypeKind::Void) {}
        };


        template<typename T>
        class NumericFactory final {
        public:
            static NumericFactory &Instance() {
                static NumericFactory f { Numeric::Create<T>() };
                static bool isSaved = false;
                if (!isSaved) {
                    _typelist.push_back(&f.GetInfo());
                    isSaved = true;
                }
                return f;
            }

            const Numeric &GetInfo() const {
                return info;
            }

        private:
            explicit NumericFactory(Numeric &&info): info(std::move(info)) {}
            Numeric info;
        };

        template<typename T>
        class BoolFactory final {
        public:
            static BoolFactory &Instance() {
                static BoolFactory f;
                static bool isSaved = false;
                if (!isSaved) {
                    _typelist.push_back(&f.GetInfo());
                    isSaved = true;
                }
                return f;
            }

            auto &GetInfo() const {
                return this->info;
            }
        private:
            BoolFactory() = default;
            Bool info;
        };

    template<typename T>
        class VoidFactory final {
    public:
        static VoidFactory &Instance() {
            static VoidFactory f;
            static bool isSaved = false;
            if (!isSaved) {
                _typelist.push_back(&f.GetInfo());
                isSaved = true;
            }
            return f;
        }

        auto &GetInfo() const {
            return this->info;
        }
    private:
        VoidFactory() = default;
        Void info;
    };

        template<typename T>
        class EnumFactory final {
        public:
            static EnumFactory &Instance() {
                static EnumFactory f;
                static bool isSaved = false;
                if (!isSaved) {
                    _typelist.push_back(&f.GetInfo());
                    isSaved = true;
                }
                return f;
            }

            [[nodiscard]] const Enum &GetInfo() const {
                return info;
            }

            EnumFactory &Regist(const std::string &name) {
                info.tpName = name;
                return *this;
            }

            void UnRegist() {
                this->info = Enum {};
            }

            template<typename U>
            EnumFactory &Add(const std::string &name, U value) {
                info.Add(name, value);
                return *this;
            }

        private:
            Enum info;
        };

    template<typename T>
    class ClassFactory final {
    public:
        static ClassFactory &Instance() {
            static ClassFactory f;
            static bool isSaved = false;
            if (!isSaved) {
                _typelist.push_back(&f.GetInfo());
                isSaved = true;
            }
            return f;
        }

        auto &GetInfo() const {
            return this->info;
        }

        ClassFactory &Regist(const std::string &name) {
            info.tpName = name;
            return *this;
        }

        void UnRegist() {
            this->info = Class {};
        }

        template<typename U>
        ClassFactory &AddField(const std::string &name, U ptr) {
            info.AddField(Field<typename refl_common::variable_traits<U>::clazz, typename refl_common::variable_traits<U>::type>::Create(name, ptr));
            return *this;
        }

        template<typename U>
        ClassFactory &AddMethod(const std::string &name, U ptr) {
            info.AddMethod(Method<typename refl_common::function_traits<U>::clazz, typename refl_common::function_traits<U>::returnType, typename refl_common::function_traits<U>::args, typename refl_common::function_traits<U>::pointer>::Create(name, ptr));
            return *this;
        }
    private:
        ClassFactory() = default;
        Class info;
    };

        template<typename T>
        class Factory final {
        public:
            static auto &GetFactory() {
                using type = std::remove_reference_t<T>;
                if constexpr (std::is_fundamental_v<type> && !std::is_same_v<type, bool> && !std::is_void_v<type>) {
                    return NumericFactory<type>::Instance();
                } else if constexpr (std::is_enum_v<type>) {
                    return EnumFactory<type>::Instance();
                } else if constexpr (std::is_class_v<type>) {
                    return ClassFactory<type>::Instance();
                } else if constexpr (std::is_same_v<type, bool>) {
                    return BoolFactory<type>::Instance();
                } else if constexpr (std::is_void_v<type>) {
                    return VoidFactory<type>::Instance();
                } else {
                    assert(false && "Unregistered type");
                }
            }
        };

        template<typename T>
        auto &RegistReflection() {
            return Factory<T>::GetFactory();
        }

        template<typename T>
        const Type *GetType() {
            return &Factory<T>::GetFactory().GetInfo();
        }

        inline const Type *GetType(const std::string &tpName) {
            for (auto typeInfo : _typelist) {
                if (typeInfo->GetTypeName() == tpName) {
                    return typeInfo;
                }
            }
            return nullptr;
        }


    template<typename T>
    struct operation_traits {
        static any Copy(const any &a) {
            assert(a.typeInfo == GetType<T>());
            any ret;
            ret.payload = new T(*static_cast<T *>(a.payload));
            ret.typeInfo = a.typeInfo;
            ret.storage = any::StorageType::Copy;
            ret.op = a.op;
            return ret;
        }

        static any Steal(any &a) {
            assert(a.typeInfo == GetType<T>());
            any ret;
            ret.payload = new T(std::move(*static_cast<T *>(a.payload)));
            ret.typeInfo = a.typeInfo;
            ret.storage = any::StorageType::Copy;
            a.storage = any::StorageType::Steal;
            ret.op = a.op;
            return ret;
        }

        static void Release(any &a) {
            assert(a.typeInfo == GetType<T>());
            delete static_cast<T *>(a.payload);
            a.payload = nullptr;
            a.storage = any::StorageType::Empty;
            a.typeInfo = nullptr;
        }
    };

    template<typename T>
    T *any::TryCast(any &elem) {
        if (elem.typeInfo == GetType<T>()) {
            return (T *) (elem.payload);
        } else {
            return nullptr;
        }
    }

    template<typename T>
    any MakeCopy(const T &elem) {
        any ret;

        ret.payload = new T(elem);
        ret.typeInfo = const_cast<Type *>(GetType<T>());
        ret.storage = any::StorageType::Copy;

        if constexpr (std::is_copy_constructible_v<T>) {
            ret.op.Copy = operation_traits<T>::Copy;
        }
        if constexpr (std::is_move_constructible_v<T>) {
            ret.op.Steal = operation_traits<T>::Steal;
        }
        if constexpr (std::is_destructible_v<T>) {
            ret.op.Release = operation_traits<T>::Release;
        }

        return ret;
    }

    template<typename T>
    any MakeSteal(T &&elem) {
        any ret;
        ret.payload = new T { std::move(elem) };
        ret.typeInfo = const_cast<Type *>(GetType<T>());
        ret.storage = any::StorageType::Steal;
        if constexpr (std::is_copy_constructible_v<T>) {
            ret.op.Copy = operation_traits<T>::Copy;
        }
        if constexpr (std::is_move_constructible_v<T>) {
            ret.op.Steal = operation_traits<T>::Steal;
        }
        if constexpr (std::is_destructible_v<T>) {
            ret.op.Release = operation_traits<T>::Release;
        }

        return ret;
    }

    template<typename T>
    any MakeRef(T &elem) {
        any ret;
        ret.payload = &elem;
        ret.typeInfo = const_cast<Type *>(GetType<T>());
        ret.storage = any::StorageType::Ref;
        if constexpr (std::is_copy_constructible_v<T>) {
            ret.op.Copy = operation_traits<T>::Copy;
        }
        if constexpr (std::is_move_constructible_v<T>) {
            ret.op.Steal = operation_traits<T>::Steal;
        }
        if constexpr (std::is_destructible_v<T>) {
            ret.op.Release = operation_traits<T>::Release;
        }

        return ret;
    }

    template<typename T>
    any MakeConstRef(const T &elem) {
        any ret;
        ret.payload = &elem;
        ret.typeInfo = const_cast<Type *>(GetType<T>());
        ret.storage = any::StorageType::ConstRef;
        if constexpr (std::is_copy_constructible_v<T>) {
            ret.op.Copy = operation_traits<T>::Copy;
        }
        if constexpr (std::is_move_constructible_v<T>) {
            ret.op.Steal = operation_traits<T>::Steal;
        }
        if constexpr (std::is_destructible_v<T>) {
            ret.op.Release = operation_traits<T>::Release;
        }

        return ret;
    }

    }
