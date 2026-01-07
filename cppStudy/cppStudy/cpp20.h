#pragma once

#include <iostream>
#include <vector>

#include <thread>
#include <chrono>

// c++20 추가된 기능들에 대한 예제 코드들을 모아놓은 헤더 파일
#include <concepts>
#include <compare>

#include <syncstream>
#include <format>
#include <latch>
#include <barrier>
#include <semaphore>
#include <coroutine>
#include <source_location>
#include <numbers>
#include <span>
#include <ranges>


// c++20 변경사항 예제 함수들
namespace cpp20_examples {

	// <Concepts> : 템플릿 매개변수에 대한 제약 조건을 정의하는 기능
	namespace Concepts_ex{
		template<typename T>
		concept Incrementable = requires(T a) {
			{ ++a } -> std::same_as<T&>;
			{ a++ } -> std::same_as<T>;
		};

		template<Incrementable T>
		T increment(T value) {
			return ++value;
		}

		decltype(auto) getval(auto s) {
			return s;
		}

		// 위의 Concepts 예제 사용
		void example(auto x) {
			auto val = getval(x);
			std::cout << "Incremented value: " << increment(x) << "\n";

			// Incrementable 개념을 만족하지 않는 타입의 경우 컴파일 에러 발생
			// std::string str = "Hello";
			// std::cout << "Incremented string: " << increment(str) << "\n"; // 오류
		}

		// 위의 concepts보다 더 구체적인 예제
		template<typename T>
		concept Addable = requires(T a, T b) {
			{ a + b } -> std::same_as<T>;
		};

		template<Addable T>
		T add(T a, T b) {
			return a + b;
		}

		void example2(int a = 0, int b = 0) {
			std::cout << "Sum: " << add(a, b) << "\n";

			// 서로 다른 타입의 경우 컴파일 에러 발생
			//std::cout << "Sum: " << add(a, 3.5) << "\n"; // 오류
		}
	}

	
	// <compare> : 3-way 비교 연산자 (스페이스십 연산자)
	// 자동으로 모든 비교 연산자(==, !=, <, <=, >, >=)를 생성
	// 기존의 비교 연산자들을 한 번에 처리할 수 있어 코드 간결성 향상
	namespace Three_way_compare_ex{
		struct Point {
			int x;
			int y;
			auto operator<=>(const Point&) const = default;
		};

		// 3-way 비교 연산자 예제 함수
		void example() {
			Point p1{ 1, 2 };
			Point p2{ 1, 3 };

			auto result = p1 <=> p2;

			// result의 값이 0 : equal, <0 : less, >0 : greater
			if (result < 0)
				std::cout << "p1 is less than p2\n";
			else if (result == 0)
				std::cout << "p1 is equal than p2\n";
			else
				std::cout << "p1 is greater to p2\n";
		}
	}


	// <syncstream> : 뮤텍스를 사용하는 스트림 인터페이스 라이브러리
	// https://en.cppreference.com/w/cpp/header/syncstream.html
	
	// 멀티스레드 환경에서 표준 출력 스트림에 대한 동기화된 접근을 제공
	// std::osyncstream 클래스를 사용하여 출력 스트림에 대한 동기화된 쓰기를 수행
	// 멀티스레드 프로그램에서 출력이 섞이는 문제를 방지
	namespace Syncstream_ex{
		// <syncstream>를 사용하는 자세한 예제 함수
		void syncstream_example() {
			// 멀티스레드에서 std::cout에 대한 출력이 섞이지 않도록 std::osyncstream을 사용하는 예제
			const int num_threads = 4;
			const int messages_per_thread = 5;

			std::vector<std::thread> threads;
			threads.reserve(num_threads);

			for (int t = 0; t < num_threads; ++t) {
				threads.emplace_back([t, messages_per_thread]() {
					for (int i = 0; i < messages_per_thread; ++i) {
						// std::osyncstream을 생성할 때 std::cout을 전달하면,
						// 이 스코프에서 작성한 내용이 버퍼에 모였다가 emit() 또는 소멸 시 한 번에 출력된다.
						std::osyncstream out(std::cout);
						out << "Thread " << t << " - message " << i << "\n";
						// out.emit(); // 명시적으로 호출해도 되지만 소멸자가 자동으로 동기화한다.
						std::this_thread::sleep_for(std::chrono::milliseconds(10));
					}
					});
			}

			for (auto& th : threads) {
				if (th.joinable())
					th.join();
			}
		}

		// std::osyncstream을 사용하지 않아 출력이 섞이는 예제
		void mixed_output_example() {
			const int num_threads = 4;
			const int messages_per_thread = 5;

			std::vector<std::thread> threads;
			threads.reserve(num_threads);

			for (int t = 0; t < num_threads; ++t) {
				threads.emplace_back([t, messages_per_thread]() {
					for (int i = 0; i < messages_per_thread; ++i) {
						// 여러 개의 << 연산으로 나눠서 출력하면 스레드 간에 부분적으로 섞일 가능성이 높아진다.
						std::cout << "[Thread " << t << "] part1 - message " << i << " ";
						std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 인터리브 발생 확률 상승
						std::cout << "part2\n";
					}
					});
			}

			for (auto& th : threads) {
				if (th.joinable())
					th.join();
			}
		}
	}
	

} // namespace cpp20_examples