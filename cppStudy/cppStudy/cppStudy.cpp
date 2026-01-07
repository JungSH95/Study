#include <iostream>
#include "cpp20.h"

int main()
{
	// c++20 예제 함수들 호출
	
	// <Concepts>
	cpp20_examples::Concepts_ex::example(6);
	cpp20_examples::Concepts_ex::example2();

	// <compare>
	cpp20_examples::Three_way_compare_ex::example();

	// <syncstream>
	cpp20_examples::Syncstream_ex::syncstream_example();
	cpp20_examples::Syncstream_ex::mixed_output_example();


	return 0;
}