#ifndef THREAD_POOL
#define THREAD_POOL

#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <type_traits>
#include <future>
#include <functional>

namespace thread_pool {

	class thread_pool {
	
	private:
		std::vector<std::thread> _threads;
		std::deque<std::function< void (void)>> _work_deque;
		std::mutex _work_deque_mutex;
		std::condition_variable _new_work;
		bool running;

	public:
		thread_pool( const unsigned int thread_count )
		{
			for( unsigned int i = 0; i < thread_count; ++i )
			{
				_threads.push_back( std::thread( [this](){
					do {
						std::unique_lock<std::mutex> lock(this->_work_deque_mutex);
						_new_work.wait( lock, [this](){ return !this->_work_deque.empty() || !this->running; } );
						if( !this->running ) break;
						auto work = this->_work_deque.back();
						this->_work_deque.pop_back();
						if( !this->_work_deque.empty() ) _new_work.notify_one();
						lock.unlock();
						work();
					} while( this->running );
					return 0;
				} ) );
			}
		}

		~thread_pool() {
			this->running = false;
			_new_work.notify_all();
			for( auto& t : _threads ) {
				t.join();
			}
		}
		
		template<class Function>
		std::future<typename std::result_of<Function()>::type > add_work( Function&& fun ) {
			
			std::lock_guard<std::mutex> lock(_work_deque_mutex);
			auto prms = new std::promise< typename std::result_of<Function()>::type >();
			auto ftr = prms->get_future();
			auto f = [this](std::promise< typename std::result_of<Function()>::type >* prms , Function& fun  ){
				this->call_wrapper( prms, fun );
				delete prms;
			};
			auto p = std::move( prms );
			std::function< void (void)> f2 = std::bind( f, prms, std::move( fun ) ); 

			_work_deque.push_front( f2 );
			_new_work.notify_one();
			
			return ftr;
		}

		template<class Function, class... ARGS>
		std::future<typename std::result_of<Function(ARGS...)>::type > add_work( Function&& fun, ARGS&&... args ) {
			std::lock_guard<std::mutex> lock(_work_deque_mutex);
			auto prms = new std::promise< typename std::result_of<Function(ARGS...)>::type >();
			auto ftr = prms->get_future(); 
			auto f = [this](std::promise< typename std::result_of<Function(ARGS...)>::type >* prms, Function& fun, ARGS&... args){
				this->call_wrapper( prms, fun, args... );
				delete prms;
			};

			auto f2 = std::bind(f, prms, std::move( fun ), std::move( args )... );
			_work_deque.push_front( f2 );
			_new_work.notify_one();
			
			return ftr;
		}

	private:
		template<typename R, typename Function, typename... ARGS>
		void call_wrapper( std::promise<R>* prms, Function& fun, ARGS&... args ) {
			prms->set_value( fun( args... ) );	
		}

		template<typename Function, typename... ARGS>
		void call_wrapper( std::promise<void>* prms, Function& fun, ARGS&... args ) {
			fun( args... );
			prms->set_value();	
		}

	};
}

#endif