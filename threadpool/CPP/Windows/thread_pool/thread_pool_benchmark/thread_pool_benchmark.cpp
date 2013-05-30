// thread_pool_benchmark.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "..\thread_pool\thread_pool.h"

#include <iostream>

int fibonacci( const long v ) {

	if( v == 0 )
	{
		return 0;
	}
	else if( v == 1 )
	{
		return 1;
	}
	return fibonacci( v-1 ) + fibonacci(v-2);
}

std::vector<std::future<void>> fill_thread_pool( thread_pool::thread_pool& pool ) {
	std::vector<std::future<void>> futures;
	
	futures.push_back( pool.add_work( [](int x){ std::cout << "Hallo" << std::endl;}, 22 ) );
	futures.push_back( pool.add_work( [](){ std::cout << "das" << std::endl;} ) );
	futures.push_back( pool.add_work( [](){ std::cout << "ist" << std::endl;} ) );
	futures.push_back( pool.add_work( [](){ std::cout << "ein" << std::endl;} ) );
	futures.push_back( pool.add_work( [](){ std::cout << "Test" << std::endl;} ) );
	futures.push_back( pool.add_work( [](){ std::cout << "!" << std::endl;} ) );
	for( int i = 0; i < 100; ++i ) {
		futures.push_back( pool.add_work( [i](){ std::cout << i << ": " << fibonacci( 35 ) << std::endl;} ) );
	}
	return futures;
}

class test {
public:
	test() {
		std::cout << "Standard Konstruktor" << std::endl;
	}

	test( const test& t ) {
		std::cout << "Copy konstruktor" << std::endl;
	}

	void get() const {}
};

std::vector<std::function< void (void)>> foo() {
	std::vector<std::function< void (void)>> fs;
	for( int i = 0; i < 100; ++i ) fs.push_back([i](){ std::cout << i << std::endl; });
	return fs;
}

int main()
{
	thread_pool::thread_pool pool( 8 );
	std::cout << std::thread::hardware_concurrency() << std::endl;
	
	std::cout << "Anfang" << std::endl;
	auto futures = fill_thread_pool( pool );
	for( auto& ftr : futures ) ftr.wait();
	std::cout << "Ende" << std::endl;

	test t;
	
	std::vector<std::function< void (void)>> fs = foo();
	for( auto& f : fs ) {
		std::thread t( f );
		t.join();
	}


	char c;
	std::cin.get( c );
	return 0;
}

