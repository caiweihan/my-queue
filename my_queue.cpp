namespace my
{
	namespace detail
	{
		template<typename T, typename Alloc = std::allocator<T> >
		class circular_array
		{
			Alloc alloc;
			
		public:
			circular_array(int n):
				begin(alloc.allocate(n+1)),
				end(begin + n+1),
				l(begin),
				r(begin)
			{}
			
			~circular_array()
			{ alloc.deallocate(begin, end - begin); }
			
			void push_back(T x)
			{
				alloc.construct(r, x);
				r = next(r);
			}
			
			void pop_front()
			{
				alloc.destroy(l);
				l = next(l);
			}
			
			T& front()
			{ return *l; }
			
			T& back()
			{ return *(r == begin ? end-1 : r-1); }
			
			size_t size()
			{ return l <= r ? r-l : end-l + r-begin; }
			
			bool empty()
			{ return l == r; }
			
			bool full()
			{ return l == next(r); }
			
			size_t max_size()
			{ return end - begin - 1; }
			
			template<typename F>
			void for_each(F f)
			{
				if(l <= r) {
					for(auto it = l; it != r; it++)
						f(*it);
				}
				
				else {
					for(auto it = l; it != end; it++)
						f(*it);
					
					for(auto it = begin; it != r; it++)
						f(*it);
				}
			}
			
		private:
			T *begin;
			T *end;
			T *l;
			T *r;
			
			T *next(T *p)
			{ return p+1 == end ? begin : p+1; }
		};
		
		template<typename T, typename Alloc = std::allocator<T> >
		class list
		{
		public:
			list():
				sz(0),
				last(v.before_begin())
			{}
			
			void emplace_back(size_t x)
			{
				v.emplace_after(last, x);
				sz++;
				last++;
			}
			
			void pop_front()
			{
				v.pop_front();
				sz--;
			}
			
			template<typename F>
			void for_each(F f)
			{
				for(const auto& x : v)
					x.for_each(f);
			}
			
			void clear()
			{
				v.clear();
				sz = 0;
				last = v.before_begin();
			}
			
			T& front()
			{ return v.front(); }
			
			T& back()
			{ return *last; }
			
			size_t size()
			{ return sz; }
			
			bool empty()
			{ return sz == 0; }
			
		private:
			std::forward_list<T, Alloc> v;
			size_t sz;
			typename std::forward_list<T, Alloc>::iterator last;
		};
		
		constexpr size_t cmax(size_t a, size_t b)
		{ return a < b ? b : a; }
	};
	
	template<typename T, typename Alloc = std::allocator<T> >
	class queue
	{
		static constexpr size_t init_size = detail::cmax(512 / sizeof(T), 8);
		static constexpr size_t growth_factor = 2;
		
	public:
		queue()
		{ v.emplace_back(init_size); }
		
		void push(const T& x)
		{
			if(v.back().full())
				v.emplace_back(v.back().size() * growth_factor);
			
			v.back().push_back(x);
		}
		
		void pop()
		{
			v.front().pop_front();
			
			if(v.front().empty() && v.size() != 1)
				v.pop_front();
		}
		
		void reset()
		{
			if(v.front().max_size() != init_size) {
				auto temp = std::move(v);
				v.clear();
				v.emplace_back(init_size);
				
				temp.for_each([&](const T& x) { push(x); });
			}
		}
		
		void clear_memory()
		{ v.clear(); }
		
		T& front()
		{ return v.front().front(); }
		
		T& back()
		{ return v.back().back(); }
		
		bool empty()
		{ return v.size() == 1 && v.front().empty(); }
		
		size_t size()
		{
			if(v.size() == 1)
				return v.front().size();
			else
				return v.front().size() + v.back().size()
					 + v.front().max_size() * 2 * ((1<<(v.size() - 2)) - 1);
		}
		
	private:
		detail::list< detail::circular_array<T, Alloc> > v;
	};
};
