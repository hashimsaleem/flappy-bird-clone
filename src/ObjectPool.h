#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

#include <vector>
#include <functional>

/**
 * Object pool for types without default constructors.
 * Uses a factory function to create objects.
 * Slots are pre-allocated with placeholders; factory is called on acquire.
 */
template<typename T>
class ObjectPool {
public:
    using Index = size_t;
    static constexpr Index INVALID = static_cast<Index>(-1);
    
    /// Create pool with factory function for new objects
    ObjectPool(std::function<T()> factory) : factory(std::move(factory)) {}
    
    /// Acquire a slot. If none available, creates new via factory.
    Index acquire() {
        for (size_t i = 0; i < valid.size(); ++i) {
            if (!valid[i]) {
                valid[i] = true;
                return i;
            }
        }
        valid.push_back(true);
        items.push_back(factory());
        return items.size() - 1;
    }
    
    /// Release a slot back to the pool
    void release(Index idx) {
        if (idx < valid.size()) valid[idx] = false;
    }
    
    /// Get reference to item at index (assumes it exists and is valid)
    T& operator[](Index idx) { return items[idx]; }
    const T& operator[](Index idx) const { return items[idx]; }
    
    /// Get all active indices (for iteration)
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
    std::vector<T> items;
    std::vector<bool> valid;
};

#endif // OBJECTPOOL_H
