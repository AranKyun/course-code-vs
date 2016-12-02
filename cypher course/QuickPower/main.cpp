#include <iostream>

typedef unsigned char uchar;

uchar QuickPower(uchar x, uchar y, uchar z) {
	uchar ai = x % z;
	uchar value = 1;

	while(y > 0) {
		if (y % 2 == 1) {
			value = value * ai % z;
		}
		ai = ai * ai % z;
		y = y / 2;
	}

	return value;
}

int main() {
	std::cout << (int)QuickPower(5, 0, 17) << std::endl;
	getchar();
	return 0;
}