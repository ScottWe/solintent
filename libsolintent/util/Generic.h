/**
 * @author Arthur Scott Wesley <aswesley@uwaterloo.ca>
 * @date 2019
 * Generic tools for general-purpose applications.
 */

#pragma once

#include <type_traits>

namespace dev
{
namespace solintent
{

/**
 * Provides an interface through which a variable may be set, and then reverted
 * once this object has gone out of scope.
 */
template<typename T>
class ScopedSet
{
public:
    static_assert(!std::is_const_v<T>, "T must not be const to permit sets.");
    static_assert(std::is_copy_assignable_v<T>, "T must be assignable to set.");

    /**
     * Sets a value for the duration of this object's lifespan.
     * 
     * _var: the variable to set
     * _val: the value to use
     */
    ScopedSet(T & _var, T _val)
        : M_ORIG(_var), m_var(_var)
    {
        m_var = _val;
    }

    T const& old()
    {
        return M_ORIG;
    }
    
    ~ScopedSet()
    {
        m_var = M_ORIG;
    }

protected:
    // Blocks heap allocation.
    static void * operator new(std::size_t) {}
    static void * operator new[](std::size_t) {}

private:
    // The original value of m_var.
    T const M_ORIG;
    // A reference to the wrapped data.
    T & m_var;
};

}
}
