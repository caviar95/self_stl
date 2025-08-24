#include <iostream>
#include <stdexcept>

void test() noexcept {
	throw std::runtime_error("error in except");
}

int main() {
	try {
		test();
	} catch (const std::exception& e) {
		std::cout << "Caught: " << e.what() << std::endl;
	}

	return 0;
}
