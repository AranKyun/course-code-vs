#include <iostream>

using namespace std;
typedef int(*FuncType)(const void*, const void*);

void sortalgo(void* base, int low, int high, int width, FuncType func) {
	if (low >= high) {
		return;
	}

	char *cfirst, *clast;

	int first = low;
	int last = high;

	char* key = new char[width];
	cfirst = (char*)base + width*first;
	for (int i = 0; i < width; i++) {
		*(key + i) = *(cfirst + i);
	}

	while (first < last) {
		while (first < last && func((char*)base + width*last, key) >= 0) {
			--last;
		}

		cfirst = (char*)base + width*first;
		clast = (char*)base + width*last;
		for (int i = 0; i < width; i++) {
			*(cfirst + i) = *(clast + i);
		}

		while (first < last && func((char*)base + width*first, key) <= 0) {
			++first;
		}

		cfirst = (char*)base + width*first;
		clast = (char*)base + width*last;
		for (int i = 0; i < width; i++) {
			*(clast + i) = *(cfirst + i);
		}
	}

	cfirst = (char*)base + width*first;
	for (int i = 0; i < width; i++) {
		*(cfirst + i) = *(key + i);
	}

	sortalgo(base, low, first - 1, width, func);
	sortalgo(base, first + 1, high, width, func);
}

void MyQsort(void* base, int nelem, int width, FuncType func) {
	sortalgo(base, 0, nelem - 1, width, func);

}

int compChar(const void* a, const void* b) {
	//ignore case
	char aa = tolower(*(char*)a);
	char bb = tolower(*(char*)b);

	return aa - bb;
}

int compInt(const void* a, const void* b) {
	return *(int*)a - *(int*)b;
}

int main() {
	//qsort characters
	cout << "Sort characters:" << endl;
	char str[] = "HelloWorld";

	for (int i = 0; i < 10; i++)
		cout << str[i];
	cout << endl;

	MyQsort(str, 10, sizeof(char), compChar);

	for (int i = 0; i < 10; i++)
		cout << str[i];
	cout << endl << endl;

	//qsort integers
	cout << "Sort integers:" << endl;
	int arr[] = { 4,7,2,9,1,3,6,0,8,5 };

	for (int i = 0; i < 10; i++)
		cout << arr[i] << " ";
	cout << endl;

	MyQsort(arr, 10, sizeof(int), compInt);

	for (int i = 0; i < 10; i++)
		cout << arr[i] << " ";
	cout << endl << endl;

	getchar();
	return 0;
}