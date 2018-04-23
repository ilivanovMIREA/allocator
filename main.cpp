#include <cstdlib>
#include <iostream>
#include <map>

//аллокатор
template <class T>
class customAllocator
{
public:

	using value_type = T;
	using const_pointer = const T*;
	using pointer = T * ;
	using const_reference = const T&;
	using reference = T & ;

	template <typename U>
	struct rebind
	{
		using other = customAllocator<U>;
	};

	template <class U>
	customAllocator(const customAllocator<U> &) :
		size(0),
		head(nullptr),
		next(nullptr)
	{}

	explicit customAllocator(size_t size_ = 10):size(size_)
	{
		head = reinterpret_cast<pointer>(malloc(size * sizeof(T)));
		next = head;
	}

	customAllocator(const customAllocator& other):
		size(other.size),
		head(other.head),
		next(other.next)
	{}

	void operator=(customAllocator &&) = delete;
	void operator=(const customAllocator &) = delete;

	~customAllocator()
	{
		if (head)
			free(head);
	}

	T* allocate(size_t n)
	{
		pointer ptr = next;
		next += n;
		return ptr;
	}

	void deallocate(pointer ptr, size_t)
	{}

	template<typename U, typename... Args>
	void construct(U *p, Args&&... args)
	{
		new((void *)p) U(std::forward<Args>(args)...);
	}

	void destroy(pointer p)
	{
		p->~T();
	}

private:
	size_t size;
	pointer head;
	pointer next;
};

template<typename T>
inline bool operator ==(const customAllocator<T> &left, const customAllocator<T> &right) {
		return false;
}

template<typename T>
inline bool operator !=(const customAllocator<T> &left, const customAllocator<T> &right) {
		return true;
}

template<class T>
struct ListNode
{
		ListNode(const T val = 0): value(val), next(nullptr) {};
		T value;
		ListNode * next;

};


//список
template<class T, class Allocator = std::allocator<ListNode<T>>>
class MyList
{
public:
		using NodeType = ListNode<T>;
		using iterator = NodeType*;

public:
	MyList() : 
	  length(0), 
	  head(nullptr), 
	  tail(nullptr) 
	 {}

	~MyList()
	{
		iterator curPtr = head;
		while(curPtr)
		{
			iterator ptrToDelete = curPtr;
			curPtr = curPtr->next;
			allocator.destroy(ptrToDelete);
			allocator.deallocate(ptrToDelete, 1);

		}
		 
	}
	
	
	void push_back(const T &value)
	{
		auto ptr = allocator.allocate(1);
		if (!ptr)
			return;

		allocator.construct(ptr, value);

		if (length)
		{
			tail->next = ptr;
			tail = ptr;

		}
		else
		{
			head = tail = ptr;
		}
		length++;
	}

	iterator begin() 
	{ 
		return head;
	}

	iterator end()
	{
		return tail;
	}

private:
	size_t length;
	NodeType *head;
	NodeType *tail;
	Allocator allocator;
	
};


//факториал
int fact(int n)
{
	if(n == 0 || n == 1)
		return 1;
	else
		return n*(fact(n-1));	
}

int main()
{
	
	//Map
	{
		std::map<int, int> m;
		for (int i = 1; i < 10; ++i)
			m[i] = fact(i);			
	}
	
	//My map allocator
	{
		std::map<int, int, std::less<int>, customAllocator<std::pair<int, int>>> my_al_m;
		for (int i = 1; i < 10; ++i)
			my_al_m[i] = fact(i);
	}
	
	//list
	{
		MyList<int> lst;
		for (int i = 0; i < 10; ++i)
			lst.push_back(i);

		
	}

	//my allocator list 
	{
		MyList<int, customAllocator<ListNode<int>>> lst;
		for (int i = 0; i < 10; ++i)
			lst.push_back(i);

	}
	
	return 0;
}