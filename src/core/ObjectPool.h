#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

#include <vector>
#include <functional>

template<typename T>
class ObjectPool {
public:
    using Index = size_t;
    static constexpr Index INVALID = static_cast<Index>(-1);

    ObjectPool(std::function<T()> factory,
               std::function<void(T&)> cleanup = nullptr)
        : factory(std::move(factory)), cleanup(std::move(cleanup)) {}

    Index acquire() {
        for (size_t i = 0; i < valid.size(); ++i) {
            if (!valid[i]) {
                valid[i] = true;
                return i;
            }
        }
        valid.push_back(true);
        items.push_back(std::move(factory()));
        return items.size() - 1;
    }

    void release(Index idx) {
        if (idx < valid.size() && valid[idx]) {
            if (cleanup) cleanup(items[idx]);
            valid[idx] = false;
        }
    }

    T& operator[](Index idx) { return items[idx]; }
    const T& operator[](Index idx) const { return items[idx]; }

    std::vector<Index> getActiveIndices() const {
        std::vector<Index> result;
        for (size_t i = 0; i < valid.size(); ++i) {
            if (valid[i]) result.push_back(i);
        }
        return result;
    }

    size_t size() const { return items.size(); }

private:
    std::function<T()> factory;
    std::function<void(T&)> cleanup;
    std::vector<T> items;
    std::vector<bool> valid;
};

#endif // OBJECTPOOL_H
