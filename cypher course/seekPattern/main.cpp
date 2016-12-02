#include <iostream>

using namespace std;

typedef unsigned char uchar;

uchar operand[100];
int oplist[100];
int top = -1;

uchar arr[] = { 4, 8, 11, 8, 11, 8};
int n = 6;

uchar calFunc(uchar l, uchar r, int op) {
	switch (op) {
	case 0: return l + r;
	case 1: return l - r;
	case 2: return r - l;
	case 3: return l * r;
	case 4: if (r == 0) throw 0; return l / r;
	case 5: if (l == 0) throw 0; return r / l;
	case 6: if (r == 0) throw 0; return l % r;
	case 7: if (l == 0) throw 0; return r % l;
	default: return 0;
	}
}


bool check() {
	bool match = true;
	for (int i = 2; i < n; i++) {
		uchar result = arr[i - 1];
		for (int j = 0; j <= top; j++) {
			result = calFunc(result, operand[j], oplist[j]);
		}
		match = match && (result == arr[i]);
	}
	return match;
}

bool loop(uchar c, uchar t, int layer, int max) {
	top++;
		for (int j = 0; j < 8; j++) {
			oplist[top] = j;
			for (int iv = 0; iv <= 255; iv++) {
				uchar v = (uchar)iv;
				operand[top] = v;
				try {
					if (max > layer) {
						if (j == 3)
							cout << "";
						if (loop(calFunc(c, v, j), t, layer + 1, max))
							return true;
					}
					else {
						if (calFunc(c, v, j) == t) {
							if (check())
								return true;
						}
					}
				}
				catch (int i) {
					continue;
				}
			}
		}
		top--;
		return false;
}


int main() {
	for (int i = 0; i < 5; i++) {
		if (loop(arr[0], arr[1], 1, i + 1))
			break;
	}

	for (int i = 0; i <= top; i++) {
		cout << oplist[i] << " " << (int)operand[i] << endl;
	}
	

	getchar();
	return 0;
}