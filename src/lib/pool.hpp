#pragma once
#include <vector>
#include <functional>


namespace engine {

    template<typename T>
    struct ObjectPool final {
        explicit ObjectPool(long initialSize = 64) : ObjectPool(nullptr, initialSize) {}

        explicit ObjectPool(std::function<void(T *)> deallocator, long initialSize = 64) {
            this->size = initialSize;
            this->deallocator = deallocator;
            for (long i = 0; i < initialSize; i++) {
                container.push_back(CreateObject());
            }
        }

        T *GetObject() {
            if (container.empty()) {
                return CreateObject();
            } else {
                T *obj = container.back();
                container.pop_back();
                size--;
                return obj;
            }
        }

        void ReleaseObject(T *t) {
            container.push_back(t);
            size++;
        }

        void DestroyAll() {
            for (T *obj : container) {
                if (!this->deallocator) {
                    delete obj;
                } else {
                    this->deallocator(obj);
                }
            }

            container.clear();
            size = 0;
        }

        inline long GetSize() const {
            return this->size;
        }


        virtual ~ObjectPool() {
            this->DestroyAll();
        }

    private:
        std::vector<T *> container;
        long size;
        std::function<void(T *)> deallocator;

        T *CreateObject() {
            return new T;
        }
    };
}
