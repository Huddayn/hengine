/*
 * Copyright (C) 2016  Hedede <Haddayn@gmail.com>
 *
 * License LGPLv3 or later:
 * GNU Lesser GPL version 3 <http://gnu.org/licenses/lgpl-3.0.html>
 * This is free software: you are free to change and redistribute it.
 * There is NO WARRANTY, to the extent permitted by law.
 */
#ifndef aw_signals_connection_h
#define aw_signals_connection_h
#include <aw/types/mem_fn.h>
#include <aw/types/support/reinterpret.h>
#include <aw/utility/static_object.h>
#include <aw/utility/memory/pool.h>
namespace aw {
namespace signals {
inline namespace v1 {
namespace impl {
template<class threading_policy>
struct observer;

template<class threading_policy>
struct signal_impl;

template<class threading_policy>
struct signal_base;

template<class policy, class signature>
struct signal;

template<class policy>
struct connection {
	using signal_type   = signal_impl<policy>;
	using observer_type = observer<policy>;

	~connection();
	void disconnect();

	signal_base<policy>& source() const;

	observer_type& target() const
	{
		return *receiver;
	}

	void* operator new(size_t count);
	void operator delete(void* ptr);

private:
	friend class connection_access;
	friend class connection_invoker;

	/*!
	 * Constructs connection between signal and observer,
	 * which automatically registers itself.
	 * Should never be called manually.
	 */
	connection(signal_type& sig, observer_type& obs);

	template<typename T, typename...Args>
	connection(signal_type& sig, T& obj, mem_fn<void(T*,Args...)> fn)
		: connection(sig, obj)
	{
		invoker = (void*)Invoker<Args...>::template invoke<T>;
		storage = reinterpret_any<storage_type>(fn);
	}

	connection(signal_type& sig, connection const& other)
	        : connection(sig, *other.observer)
	{
		invoker = other.invoker;
		storage = other.storage;
	}

	using unknown_mem_fn = mem_fn<void(_unknown*)>;

	using storage_type = typename std::aligned_storage<
		sizeof(unknown_mem_fn),
		alignof(unknown_mem_fn)
	>::type;

	template<typename...Args>
	struct Invoker {
		using invoker_type = void(storage_type const&, observer<policy>*, Args...);

		static invoker_type* cast(void* invoker)
		{
			return reinterpret_cast<invoker_type*>(invoker);
		}

		template<class T> static
		void invoke(storage_type const& data, observer<policy>* obj, Args... args)
		{
			auto func = reinterpret_any<mem_fn<void(T*,Args...)>>(data);
			T* ptr = static_cast<T*>(obj);
			(ptr->*func)(args...);
		}
	};

	template<typename... Args>
	void invoke(Args&&...args)
	{
		auto invoker = Invoker<Args...>::cast(this->invoker);
		invoker(storage, receiver, std::forward<Args>(args)...);
	}

	signal_type* sender;
	observer_type* receiver;

	void* invoker;
	storage_type storage;
};

class connection_access {
	template<class policy>
	friend class signal_base;

	template<class policy, class signature>
	friend class signal;

	template<class policy> static
	auto clone(connection<policy> const& conn, signal_impl<policy>& sig)
	{
		return new connection<policy>{sig, conn};
	}

	template<class policy, typename T, typename...Args>
	static auto
	make(signal_impl<policy>& sig, T& obj, mem_fn<void(T*,Args...)> fn)
	{
		return new connection<policy>{sig, obj, fn};
	}
};

class connection_invoker {
	template<class policy, class signature>
	friend class signal;

	template<class policy, typename... Args> static
	void invoke(connection<policy>& conn, Args&&...args)
	{
		conn.invoke(std::forward<Args>(args)...);
	}
};

template<class policy>
struct connection_pool : policy, memory::specific_pool<connection<policy>> {
	using base = memory::specific_pool<connection<policy>>;
	connection_pool() : base(4096) { }

	void* alloc()
	{
		typename policy::lock_type lock(*this);
		return base::alloc();
	}

	void dealloc(void* ptr)
	{
		typename policy::lock_type lock(*this);
		return base::dealloc(ptr);
	}
};

template<class policy>
void* connection<policy>::operator new(size_t count)
{
	using pool_type = connection_pool<policy>;
	using conn_pool = static_object<pool_type>;
	static auto& p = conn_pool::instance();
	return p.alloc();
}

template<class policy>
void connection<policy>::operator delete(void* ptr)
{
	using pool_type = connection_pool<policy>;
	using conn_pool = static_object<pool_type>;
	static auto& p = conn_pool::instance();
	p.dealloc(ptr);
}
} // namespace impl
} // namespace v1
} // namespace signals
} // namespace aw
#endif//aw_signals_connection_h
