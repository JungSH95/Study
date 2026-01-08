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
	
	// <format> : 문자열 형식화 라이브러리
	// std::format 함수를 사용하여 문자열을 포맷팅
	namespace Format_ex {
		void example() {
			std::string name = "Jung";
			int age = 30;
			double height = 171.3;
			std::string formatted = std::format("Name: {}, Age: {}, Height: {:.1f} feet", name, age, height);
			std::cout << formatted << "\n";
		}

		// 추가적인 format 예제 함수
		void example2() {
			// 정수 포맷팅
			int number = 255;
			std::cout << std::format("Decimal: {}, Hex: {:#x}, Octal: {:#o}, Binary: {:#b}\n", number, number, number, number);
			
			// 부동 소수점 포맷팅
			double pi = 3.141592653589793;
			std::cout << std::format("Pi to 3 decimal places: {:.3f}\n", pi);
			
			// 정렬 및 폭 지정
			std::cout << std::format("|{:<10}|{:^10}|{:>10}|\n", "left", "center", "right");
		}
	}

	// <latch> : 횟수 세기 기반의 1회용 동기화 클래스
	// 여러 스레드가 작업을 완료할 때까지 기다린 후 진행
	// 초기화 작업 후 스레드 일제히 시작, 완료 후 해제 시 아무 행동 없음
	// jthread와 함께 사용 시 수동 join() 없이도 스레드가 안전하게 정리됨
	namespace Latch_ex {
		// latch 예제 함수
		void example() {
			const int num_threads = 3;
			std::latch latch(num_threads);
			auto worker = [&latch](int id) {
				std::this_thread::sleep_for(std::chrono::milliseconds(100 * id)); // 작업 시뮬레이션
				std::cout << "Worker " << id << " done\n";
				latch.count_down(); // 작업 완료 알림
			};

			std::vector<std::thread> threads;
			for (int i = 0; i < num_threads; ++i) {
				threads.emplace_back(worker, i + 1);
			}

			latch.wait(); // 모든 작업이 완료될 때까지 대기
			std::cout << "All workers done. Proceeding...\n";

			for (auto& th : threads) {
				if (th.joinable())
					th.join();
			}
		}

		// jthread를 사용한 latch 예제 함수
		void example2() {
			const int num_threads = 3;
			std::latch latch(num_threads);
			auto worker = [&latch](int id) {
				std::this_thread::sleep_for(std::chrono::milliseconds(100 * id)); // 작업 시뮬레이션
				std::cout << "Worker " << id << " done\n";
				latch.count_down(); // 작업 완료 알림
			};

			std::vector<std::jthread> threads;
			for (int i = 0; i < num_threads; ++i) {
				threads.emplace_back(worker, i + 1);
			}

			latch.wait(); // 모든 작업이 완료될 때까지 대기
			std::cout << "All workers done. Proceeding...\n";

			// jthread는 자동으로 join되므로 별도의 join 호출 불필요
		}
	}

	// <barrier> : 반복 가능한 스레드 동기화 클래스
	// 여러 스레드가 특정 지점에 도달할 때까지 대기하도록 함
	// 반복되는 작업 단계별 스레드간 동기화, 완료 후 해제 시 콜백 함수 실행 가능
	// jthread와 함께 사용 시 수동 join() 없이도 스레드가 안전하게 정리됨
	namespace Barrier_ex {
		// barrier 예제 함수
		void example() {
			const int num_threads = 3;
			
			std::barrier sync_point(num_threads, []() noexcept {
				// 작업 완료 후 실행할 콜백 함수
				std::cout << "All threads reached the barrier. Proceeding to next phase...\n";
			});

			auto worker = [&sync_point](int id) {
				for (int phase = 1; phase <= 3; ++phase) {
					std::this_thread::sleep_for(std::chrono::milliseconds(100 * id)); // 작업 시뮬레이션
					std::cout << "Worker " << id << " reached phase " << phase << "\n";
					sync_point.arrive_and_wait(); // 장벽에 도달하고 대기
				}
			};

			std::vector<std::jthread> threads;
			for (int i = 0; i < num_threads; ++i) {
				threads.emplace_back(worker, i + 1);
			}

			// jthread는 자동으로 join되므로 별도의 join 호출 불필요
		}
	}

	// <semaphore> : 접근하는 스레드의 수를 제한하는 동기화 클래스
	// 제한된 리소스에 대한 동시 접근 제어, 특정 개수 이상의 스레드가 접근하지 못하도록 함
	// 단일 리소스 보호의 경우 std::mutex가 더 직관적
	namespace Semaphore_ex {
		// semaphore 예제 함수
		void example() {
			const int max_concurrent = 2;
			std::counting_semaphore<max_concurrent> semaphore(max_concurrent);
			auto worker = [&semaphore](int id) {
				// 1. 입장 시도
				semaphore.acquire();

				// 작업 시뮬레이션
				std::cout << "Worker " << id << " acquired the semaphore.\n";
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				std::cout << "Worker " << id << " releasing the semaphore.\n";

				// 2. 작업 완료 후 퇴장, 대기자 깨움
				semaphore.release();
			};
			
			std::vector<std::jthread> threads;
			for (int i = 0; i < 5; ++i) {
				threads.emplace_back(worker, i + 1);
			}

			// jthread는 자동으로 join되므로 별도의 join 호출 불필요
		}
	}

	// <coroutine> : 코루틴 지원 라이브러리
	/*
	* 1. c++20 표준에 코루틴이 포함되었지만, 라이브러리라기 보단 프레임워크에 가깝다.
	* 2. co_await, co_yield, co_return 키워드 사용
	* 3. c++이 정의한 규칙을 구현한 '코루틴 반환 객체'를 호출자에게 리턴해야함
	*		promise : "코루틴 내부"에서 관리되는 객체 코루틴의 결과나 예외를 이 객체를 통해 호출자에게 전달하는 용도
	*		coroutine_handle : "코루틴 외부"에서 관리되는 객체, 코루틴의 상태를 제어하고 접근하는 용도
	*		coroutine_state : "힙 메모리 영역"에 할당되는 코루틴의 상태를 나타내는 열거형	* 
	*/

	// 코루틴 실행 시 일어나는 일련의 과정
	// 0. 코루틴이 최초 실행되면 new를 통해 힙 메모리 영역에 coroutine state를 생성됨
	// 1. 코루틴이 호출되면, 코루틴 프레임이 힙에 할당되고 promise 객체가 생성됨
	// 2. 코루틴이 일시 중단(co_await, co_yield)되면, 현재 상태가 저장되고 제어가 호출자에게 반환됨
	// 3. 호출자는 coroutine_handle을 통해 코루틴을 재개하거나 상태를 확인할 수 있음
	// 4. 코루틴이 종료되면, promise 객체를 통해 결과나 예외가 호출자에게 전달되고, 코루틴 프레임이 해제됨
	// 5. 호출자는 coroutine_handle을 통해 코루틴의 상태를 확인하고 필요한 후처리를 수행할 수 있음

	// 코루틴이란 ?
	// 1. 비동기 프로그래밍, 제너레이터, 협력적 멀티태스킹 등에 활용
	// 2. 일반 함수와 달리 여러 번 호출될 수 있으며, 호출 간에 상태를 유지
	// 3. 코루틴은 호출자와 피호출자 간의 제어 흐름을 명시적으로 관리할 수 있게 해줌
	// 4. 코루틴은 비동기 작업을 보다 직관적이고 간결하게 표현할 수 있게 해줌
	// 5. 코루틴은 상태 머신(state machine)을 구현하는 데 유용함
	// 6. 코루틴은 협력적 멀티태스킹(cooperative multitasking)을 구현하는 데 사용될 수 있음
	// 7. 코루틴은 제너레이터(generator)를 구현하는 데 사용될 수 있음
	// 8. 코루틴은 비동기 스트림(asynchronous streams)을 구현하는 데 사용될 수 있음
	// 9. 코루틴은 이벤트 기반 프로그래밍(event-driven programming)을 구현하는 데 사용될 수 있음

	namespace Coroutine_ex {
		// 숫자를 순차적으로 생성 (co_yield 사용)
		struct Generator {
			struct promise_type {
				int current_value;

				// 코루틴이 처음 생성될 때 즉시 실행하지 않도록 suspend
				std::suspend_always initial_suspend() { return {}; }

				// 각 yield 이후 호출되어 호출자에게 제어를 반환
				std::suspend_always yield_value(int value) {
					current_value = value;
					return {};
				}

				// 코루틴이 정상 종료할 때 호출
				void return_void() {}

				// 예외 처리: 여기서는 단순히 terminate
				void unhandled_exception() { std::terminate(); }

				// 코루틴 종료 후 호출자에게 제어를 반환
				std::suspend_always final_suspend() noexcept { return {}; }

				Generator get_return_object() {
					return Generator{ std::coroutine_handle<promise_type>::from_promise(*this) };
				}
			};

			using handle_type = std::coroutine_handle<promise_type>;

			handle_type coro;

			explicit Generator(handle_type h) : coro(h) {}
			Generator(const Generator&) = delete;
			Generator& operator=(const Generator&) = delete;

			Generator(Generator&& other) noexcept : coro(other.coro) { other.coro = nullptr; }
			Generator& operator=(Generator&& other) noexcept {
				if (this != &other) {
					if (coro) coro.destroy();
					coro = other.coro;
					other.coro = nullptr;
				}
				return *this;
			}

			~Generator() {
				if (coro) coro.destroy();
			}

			// 다음 값으로 진행하고 완료 여부를 반환
			bool next() {
				if (!coro || coro.done()) return false;

				// 코루틴 재개
				std::cout << "Resuming coroutine...\n";
				coro.resume();
				std::cout << "Coroutine resumed.\n";

				return !coro.done();
			}

			int value() const {
				return coro.promise().current_value;
			}
		};

		// 간단한 숫자 생성 코루틴: 1부터 n 까지 co_yield
		Generator range_generator(int n) {
			for (int i = 1; i <= n; ++i) {
				std::cout << "range_generator value1 : " << i << "\n";

				// 값을 호출자에게 전달 하고 일시 중단
				co_yield i;

				std::cout << "range_generator value2 : " << i << "\n";
			}
		}

		// 제너레이터 사용
		void example() {
			auto gen = range_generator(5);
			// 최초 호출 전에 코루틴은 suspended 상태이므로 먼저 resume 필요
			while (gen.next()) {
				std::cout << "Generated: " << gen.value() << "\n";
			}
			std::cout << "Generator finished.\n";
		}
	}

	// <source_location> : c의 미리 정의된 표준 매크로의 문제점을 개선하기 위해 만들어진 라이브러리
	namespace Source_Location_ex {
		void log_message(const std::string& message,
			const std::source_location location = std::source_location::current()) {
			std::cout << "Log: " << message << "\n"
				<< "  at " << location.file_name() 
				<< ":" << location.line() 
				<< " in " << location.function_name() << "\n";
		}
		void example() {
			log_message("This is a test log message.");
		}
	}

	// <numbers> : 수학 상수 라이브러리
	namespace Numbers_ex {
		void example() {
			double pi = std::numbers::pi;
			double e = std::numbers::e;
			std::cout << "Pi: " << pi << ", e: " << e << "\n";


			// 추가 예제
			double golden_ratio = std::numbers::phi;
			std::cout << "Golden Ratio: " << golden_ratio << "\n";

			// 추가 예제 2
			double sqrt2 = std::numbers::sqrt2;
			std::cout << "Square Root of 2: " << sqrt2 << "\n";
		}
	}

	// <span> : 연속된 메모리 블록에 대한 뷰(view)를 제공하는 라이브러리
	// 왜 사용하나?
	// 1. 배열, 벡터 등 다양한 컨테이너 타입에 대해 일관된 인터페이스 제공
	// 2. 메모리 복사 없이 부분 배열이나 서브시퀀스에 대한 뷰 생성 가능
	// 3. 함수 인자로 전달할 때 복사 비용 절감
	
	// 주의사항
	// 1. std::span은 메모리 소유권을 가지지 않음
	// 2. std::span은 기본적으로 읽기 전용 뷰를 제공하지만, 가변(span<T>)과 불변(span<const T>) 버전을 모두 지원
	// 3. std::span은 크기가 고정된 배열, 동적 배열, 벡터 등 다양한 컨테이너와 호환
	// 4. std::span은 경계 검사를 수행하지 않으므로, 잘못된 인덱스 접근 시 정의되지 않은 동작이 발생할 수 있음
	namespace Span_ex {
		void example() {
			int arr[] = { 1, 2, 3, 4, 5 };
			std::span<int> sp(arr); // 배열로부터 span 생성

			std::cout << "Span elements: ";
			for (const auto& elem : sp) {
				std::cout << elem << " ";
			}
			std::cout << "\n";

			// 부분 span 생성
			std::span<int> sub_sp = sp.subspan(1, 3); // 인덱스 1부터 3개 요소
			std::cout << "Sub-span elements: ";
			for (const auto& elem : sub_sp) {
				std::cout << elem << " ";
			}
			std::cout << "\n";
		}
	}


	// <ranges> : 범위 기반 알고리즘 및 뷰 라이브러리
	namespace Ranges_ex {
		void example() {
			std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };
			// 짝수만 필터링하고 제곱하여 출력
			auto even_squares = vec 
				| std::views::filter([](int n) { return n % 2 == 0; }) 
				| std::views::transform([](int n) { return n * n; });
			std::cout << "Even squares: ";
			for (const auto& val : even_squares) {
				std::cout << val << " ";
			}
			std::cout << "\n";
		}

		// 추가적인 ranges 예제 함수
		void example2() {
			std::vector<std::string> words = { "apple", "banana", "cherry", "date" };
			// 길이가 5 이상인 단어만 필터링하고 대문자로 변환하여 출력
			auto long_uppercase = words
				| std::views::filter([](const std::string& s) { return s.size() >= 5; })
				| std::views::transform([](const std::string& s) {
					std::string upper;
					for (char c : s) upper += std::toupper(c);
					return upper;
					});
			std::cout << "Long uppercase words: ";
			for (const auto& word : long_uppercase) {
				std::cout << word << " ";
			}
			std::cout << "\n";
		}
	}

} // namespace cpp20_examples