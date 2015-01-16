#include <iterator>
#include <memory>

template <typename range_type>
class range : public std::iterator<std::bidirectional_iterator_tag, range_type>
{
public:
	
	range(range_type f, range_type l) : first_(f), last_(l), curr_(f)
	{
	}

	range begin()
	{
		return{ first_, last_, first_ };
	}

	range end()
	{
		return{ first_, last_, last_ };
	}

	bool operator==(const range& rhs) const
	{
		return (rhs.first_ == first_) && (rhs.last_ == last_) && (rhs.curr_ == curr_);
	}

	bool operator!=(const range& rhs) const
	{
		return !(*this == rhs);
	}

	range_type operator*() const
	{
		return curr_;
	}

	range& operator++() 
	{
		++curr_;
		return *this;
	}

	range operator++(int)
	{
		range tmp(*this);
		operator++();
		return tmp;
	}

	range& operator--()
	{
		--curr_;
		return *this;
	}

	range operator--(int)
	{
		range tmp(*this);
		operator--();
		return tmp;
	}

private:
	range(range_type f, range_type l, range_type c) : curr_(c), first_(f), last_(l)
	{
	}

	range_type first_;
	range_type last_;
	range_type curr_;
};

template<typename range_type, typename IV>
range<range_type> range_from_to(IV first, range_type last)
{
	return{ first + (0u*last), last };
}

template <typename range_type>
range<range_type> count_until(range_type last)
{
	return{ 0u, last };
}