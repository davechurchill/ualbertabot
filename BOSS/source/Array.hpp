#pragma once
#include "BOSSAssert.h"
#include "Common.h"
#include <algorithm>

namespace BOSS
{
template <class T,size_t max_capacity>
class Vec
{
    size_t	_size;
    size_t  _capacity;
    T		_arr[max_capacity];

public:

    Vec<T,max_capacity>()
        : _size(0)
        ,_capacity(max_capacity)
    {
		BOSS_ASSERT(max_capacity>0, "Vec initializing with capacity = 0");
    }

    Vec<T,max_capacity>(const size_t & size)
        : _size(size)
        ,_capacity(max_capacity)
    {
        BOSS_ASSERT(size <= max_capacity,"Vec initializing with size > capacity, Size = %d, Capacity = %d",size,_capacity);
    }

    Vec<T,max_capacity>(const size_t & size,const T & val)
        : _size(size)
        ,_capacity(max_capacity)
    {
        BOSS_ASSERT(size <= max_capacity,"Vec initializing with size > capacity, Size = %d, Capacity = %d",size,_capacity);
        fill(val);
    }
    
    void resize(const size_t & size)
    {
        BOSS_ASSERT(size <= max_capacity,"Vec resizing with size > capacity, Size = %d, Cpacity = %d",size,_capacity);
        _size = size;
    }

    T & get(const size_t & index)
    {
        BOSS_ASSERT(index < size(),"Vec out of bounds exception, Size = %d, Index = %d",size(),index);
        return _arr[index];
    }

    const T & get(const size_t & index) const
    {
        BOSS_ASSERT(index < size(),"Vec out of bounds exception, Size = %d, Index = %d",size(),index);
        return _arr[index];
    }

    T & operator [] (const size_t & index)
    {
        return get(index);
    }

    const T & operator [] (const size_t & index) const
    {
        return get(index);
    }

    const bool contains(const T & e) const
    {
        for (size_t i(0); i<size(); ++i)
        {
            if (get(i) == e)
            {
                return true;
            }
        }

        return false;
    }

    void addSorted(const T & e)
    {
        size_t index(0);
        while (_arr[index] < e)
        {
            ++index;
        }
        
        addAtIndex(e, index);
    }

    void addAtIndex(const T & e, const size_t & index)
    {
        BOSS_ASSERT(index <= size(), "Can't add at index: Index = %d, Size = %d", index, size());
        BOSS_ASSERT(_size < capacity(),"Array over capacity: Size = %d",capacity());

        copyShiftRight(index);
        _arr[index] = e;
    }

    void copyShiftRight(const size_t & index)
    {
        BOSS_ASSERT(_size < capacity(),"Array over capacity: Size = %d",capacity());
        for (size_t i(_size + 1); i > index; --i)
        {
            _arr[i] = _arr[i-1];
        }

        _size++;
    }

    void copyShiftLeft(const size_t & index)
    {
        BOSS_ASSERT(size() > 0, "Can't shift left when empty");
        for (size_t i(index); i < size()-1; ++i)
        {
            _arr[i] = _arr[i+1];
        }

        _size--;
    }
    
    const size_t & capacity() const
    {
        return _capacity;
    }

    void push_back(const T & e)
    {
        BOSS_ASSERT(_size < capacity(),"Array over capacity: Capacity = %d, Size = %d",capacity(), _size);
        _arr[_size] = e;
        ++_size;
    }

    const T & back() const
    {
        BOSS_ASSERT(!empty(),"Vector back() with empty array");

        return get(_size);
    }

    void remove(const size_t & index)
    {
        remove_by_swap(index);
    }

    void removeByShift(const size_t & index)
    {
        copyShiftLeft(index);
    }

    void remove_by_swap(const size_t & index)
    {
        BOSS_ASSERT(index < size(),"Vector out of bounds exception, Size = %d, Index = %d",size(),index);

        std::swap(_arr[index],_arr[size()-1]);
        pop_back();
    }

    void pop_back()
    {
        BOSS_ASSERT(!empty(),"Vector pop_back() with empty array");

        _size--;
    }

    void sort()
    {
        std::sort(_arr, _arr + _size);
    }

    const bool empty() const
    {
        return size() == 0;
    }

    void clear()
    {
        _size = 0;
    }

    const size_t & size() const
    {
        return _size;
    }

    inline void fill(const T & val)
    {
        std::fill(_arr,_arr + _size,val);
    }

    class iterator
    {
        T * _ptr;
    public:
        iterator(T * ptr) : _ptr(ptr) { }
        iterator    operator ++ ()                      { iterator i = *this; _ptr++; return i; }
        iterator    operator ++ (int junk)              { _ptr++; return *this; }
        T &         operator *  ()                      { return *_ptr; }
        T *         operator -> ()                      { return _ptr; }
        bool        operator == (const iterator & rhs)  { return _ptr == rhs._ptr; }
        bool        operator != (const iterator & rhs)  { return _ptr != rhs._ptr; }
    };
 
    class const_iterator
    {
        T * _ptr;
    public:
        const_iterator(T * ptr) : _ptr(ptr) { }
        const_iterator  operator ++ ()                              { const_iterator i = *this; _ptr++; return i; }
        const_iterator  operator ++ (int junk)                      { _ptr++; return *this; }
        const T &       operator *  ()                              { return *_ptr; }
        const T *       operator -> ()                              { return _ptr; }
        bool            operator == (const const_iterator & rhs)    { return _ptr == rhs._ptr; }
        bool            operator != (const const_iterator & rhs)    { return _ptr != rhs._ptr; }
    };
    
    iterator begin()
    {
        return iterator(_arr);
    }

    iterator end()
    {
        return iterator(_arr + size());
    }

};

}