#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <vector>
#include <stdarg.h>

using namespace std;

const bool _OUTPUT = true;
ofstream file;

typedef unsigned char uchar;

typedef uchar(*OpFunc)(char*, ...);

const int MAXSIZE = 20;

uchar voidMonFunc(char* funcName, ...) {
	if (funcName == NULL)
		return 1;

	strcpy_s(funcName, 10, "");

	va_list arg;
	va_start(arg, funcName);
	uchar result = va_arg(arg, uchar);
	va_end(arg);

	return result;
}


char* treeTypeA[] = {
	"m(b(m(b(m(b(m(v),m(v))),m(v))),m(v)))",
	"m(b(m(b(m(v),m(b(m(v),m(v))))),m(v)))",
	"m(b(m(b(m(v),m(v))),m(b(m(v),m(v)))))",
	"m(b(m(v),m(b(m(b(m(v),m(v))),m(v)))))",
	"m(b(m(v),m(b(m(v),m(b(m(v),m(v)))))))"
};

char* treeTypeB[] = {
	"m(t(m(b(m(v),m(v))),m(v),m(v)))",
	"m(t(m(v),m(b(m(v),m(v))),m(v)))",
	"m(t(m(v),m(v),m(b(m(v),m(v)))))",
	"m(b(m(t(m(v),m(v),m(v))),m(v)))",
	"m(b(m(v),m(t(m(v),m(v),m(v)))))"
};

struct Node {
	bool isVal;
	uchar* val;
	OpFunc* func;
	Node* child1;
	Node* child2;
	Node* child3;
};


Node* createExpTree(char* str, OpFunc* mon, OpFunc* bin, OpFunc* ter, uchar* val) {
	Node* St[MAXSIZE], *p;
	int top = -1, k, j = 0;
	char ch;
	Node* b = NULL;
	ch = str[j];
	while (ch != '\0') {
		switch (ch) {
		case '(': top++; St[top] = p; k = 1; break;
		case ')': top--; break;
		case ',': k++; break;
		default:
			p = new Node;
			p->child1 = p->child2 = p->child3 = NULL;
			if (b == NULL)
				b = p;
			else {
				switch (k) {
				case 1: St[top]->child1 = p; break;
				case 2: St[top]->child2 = p; break;
				case 3: St[top]->child3 = p; break;
				}
			}
			switch (ch) {
			case 'm':
				p->isVal = false;
				p->func = mon;
				mon++;
				break;
			case 'b':
				p->isVal = false;
				p->func = bin;
				bin++;
				break;
			case 't':
				p->isVal = false;
				p->func = ter;
				ter++;
				break;
			case 'v':
				p->isVal = true;
				p->val = val;
				val++;
				break;
			default: break;
			}

		}
		j++;
		ch = str[j];
	}

	return b;
}


uchar calTree(Node* node) {
	char str[10];

	if (node->isVal) {
		return *(node->val);
	}
	else {
		switch ((*node->func)(0)) {
		case 1: return (*node->func)(str, calTree(node->child1));
		case 2: return (*node->func)(str, calTree(node->child1), calTree(node->child2));
		case 3: return (*node->func)(str, calTree(node->child1), calTree(node->child2), calTree(node->child3));
		}
	}
}


void showTree(Node* node) {
	char str[10];

	if (node->isVal) {
		cout << (int)*(node->val);
		if (_OUTPUT) file << (int)*(node->val);
	}
	else {
		if ((*node->func) == voidMonFunc) {
			showTree(node->child1);
		}
		else {
			try {
				(*node->func)(str);
			}
			catch (char* s) {
				if (s != "IllegalOperandException") throw s;
			}
			cout << str << "(";
			if (_OUTPUT) file << str << "(";

			switch ((*node->func)(0)) {
			case 1:
				showTree(node->child1);
				break;
			case 2:
				showTree(node->child1);
				cout << ", ";
				if (_OUTPUT) file << ", ";
				showTree(node->child2);
				break;
			case 3:
				showTree(node->child1);
				cout << ", ";
				if (_OUTPUT) file << ", ";
				showTree(node->child2);
				cout << ", ";
				if (_OUTPUT) file << ", ";
				showTree(node->child3);
				break;
			}
			cout << ")";
			if (_OUTPUT) file << ")";
		}
	}
}


bool calFunc(const uchar* operand, const OpFunc* oparr, int opn) {
	int counter = 0;

	if (opn == 0)
		return false;

	file.open("out.txt", ios::trunc);

	vector<OpFunc> Funcs[3];

	for (int i = 0; i < opn; i++)
		Funcs[oparr[i](0) - 1].push_back(*(oparr + i));

	Funcs[0].push_back(voidMonFunc);

	// binary operators are necessary
	if (Funcs[1].size() == 0)
		return false;

	// use three binary operators and alternative monadic operators
	OpFunc bin[3];
	OpFunc mon[7];
	uchar val[4];

	for (int itype = 0; itype < 5; itype++) {
		Node* tree = createExpTree(treeTypeA[itype], mon, bin, NULL, val);
		for (int ibin = 0; ibin < pow(Funcs[1].size(), 3); ibin++) {
			for (int k = 0; k < 3; k++)
				bin[k] = Funcs[1][ibin / (int)pow(Funcs[1].size(), k) % Funcs[1].size()];

			for (int imon = 0; imon < pow(Funcs[0].size(), 7); imon++) {
				for (int k = 0; k < 7; k++)
					mon[k] = Funcs[0][imon / (int)pow(Funcs[0].size(), k) % Funcs[0].size()];

				for (int ival = 0; ival < 4 * 4 * 4 * 4; ival++) {
					for (int k = 0; k < 4; k++)
						val[k] = operand[ival / (int)pow(4, k) % 4];

					uchar value;
					try {
						value = calTree(tree);
					}
					catch (char* e) {
						if (e == "IllegalOperandException")
							continue;
						else
							throw e;
					}
					counter++;
					cout << setw(6) << counter << "  ";
					if (_OUTPUT) file << setw(6) << counter << "  ";
					showTree(tree);
					cout << " = " << (int)value << endl;
					if (_OUTPUT) file << " = " << (int)value << endl;
				}
			}
		}
	}

	// use one binary operator and one ternary operator

	if (Funcs[2].size() > 0) {
		OpFunc bin[1];
		OpFunc mon[6];
		OpFunc ter[1];
		uchar val[4];

		for (int itype = 0; itype < 5; itype++) {
			Node* tree = createExpTree(treeTypeB[itype], mon, bin, ter, val);
			
			for (int ibin = 0; ibin < Funcs[1].size(); ibin++) {
				bin[0] = Funcs[1][ibin];

				for (int iter = 0; iter < Funcs[2].size(); iter++) {
					ter[0] = Funcs[2][iter];

					for (int imon = 0; imon < pow(Funcs[0].size(), 6); imon++) {
						for (int k = 0; k < 6; k++)
							mon[k] = Funcs[0][imon / (int)pow(Funcs[0].size(), k) % Funcs[0].size()];

						for (int ival = 0; ival < 4 * 4 * 4 * 4; ival++) {
							for (int k = 0; k < 4; k++)
								val[k] = operand[ival / (int)pow(4, k) % 4];

							uchar value;
							try {
								value = calTree(tree);
							}
							catch (char* e) {
								if (e == "IllegalOperandException")
									continue;
								else
									throw e;
							}
							counter++;
							cout << setw(6) << counter << "  ";
							if (_OUTPUT) file << setw(6) << counter << "  ";
							showTree(tree);
							cout << " = " << (int)value << endl;
							if (_OUTPUT) file << " = " << (int)value << endl;
						}
					}
				}
			}
		}
	}

	file.close();

	return true;

}