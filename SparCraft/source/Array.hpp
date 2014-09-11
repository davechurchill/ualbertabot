#pragma once

namespace SparCraft
{
template <class T, size_t elem>
class Array
{
	size_t	_size;
	size_t  _capacity;
	T		_arr[elem];

public:

	Array<T, elem> ()
		: _size(0)
		, _capacity(elem)
	{
	}

	Array<T, elem> (const T & fill)
		: _size(0)
		, _capacity(elem)
	{
		std::fill(_arr, _arr + elem, fill);
	}

	T & get(const size_t & index)
	{
		return _arr[index];
	}

	const T & get(const size_t & index) const
	{
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
		for (size_t i(0); i<capacity(); ++i)
		{
			if (get(i) == e)
			{
				return true;
			}
		}

		return false;
	}

	const size_t capacity() const
	{
		return _capacity;
	}

	const bool containsSize(const T & e) const
	{
		for (size_t i(0); i<_size; ++i)
		{
			if (get(i) == e)
			{
				return true;
			}
		}

		return false;
	}

	void inc()
	{
		_size++;
	}		

	void add(const T & e)
	{
		assert(_size < (capacity()-1));
		get(_size) = e;
		_size++;
	}

	void addUnique(const T & e)
	{
		if (!contains(e))
		{
			add(e);
		}
	}

	const T & back() const
	{
		assert(_size > 0);
		return get(_size);
	}

	void clear()
	{
		_size = 0;
	}

	const size_t & size() const
	{
		return _size;
	}

	void fill(const T & e)
	{
		std::fill(_arr, _arr + elem, e);
	}
};

template <class T, size_t rows, size_t cols>
class Array2D
{
	size_t					_rows;
	size_t					_cols;

	Array< Array<T, cols>, rows>	_arr;

public:

	Array2D<T, rows, cols>()
		: _rows(rows)
		, _cols(cols)
	{
	}

	Array2D<T, rows, cols> & operator = (const Array2D<T, rows, cols> & rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		for (size_t r(0); r<_rows; ++r)
		{
			_arr[r] = rhs._arr[r];
		}

		return *this;
	}

	Array2D<T, rows, cols> (const Array2D<T, rows, cols> & rhs)
		: _rows(rhs._rows)
		, _cols(rhs._cols)
		, _arr(rhs._arr)
	{
		
	}

	const size_t getRows() const
	{
		return _arr.capacity();
	}
	
	Array<T, cols> & operator [] (const size_t & index)
	{
		assert(index < _rows);
		return _arr[index];
	}
	
	const Array<T, cols> & operator [] (const size_t & index) const
	{
		assert(index < _rows);
		return _arr[index];
	}

	void fill(const T & e)
	{
		for (size_t i(0); i<_rows; ++i)
		{
			_arr[i].fill(e);
		}
	}

};
}