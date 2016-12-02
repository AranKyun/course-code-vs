#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <map>
#include <algorithm>

using namespace std;

typedef vector<unsigned int> vec;

bool _OUTPUT = true;

enum operators {
	op_bracket_l = 1700, op_bracket_r = 1701,
	op_not = 1600,
	op_mul = 1500, op_div = 1501, op_mod = 1502,
	op_add = 1400, op_sub = 1401,
	op_shift_l = 1300, op_shift_r = 1301, op_shift_lc = 1302, op_shift_rc = 1303,
	op_and = 1200,
	op_xor = 1100,
	op_or = 1000
};

operators oplist[] = {
op_mul, op_div, op_mod,
op_add, op_sub,
op_shift_l, op_shift_r, op_shift_lc, op_shift_rc,
op_and,
op_xor,
op_or };


//0 --- none; 1 --- (; 2 --- ); 3 --- ~(;
int bklist[] = {
	0x0000, // 0 0 0 0 0 0 0 0
	0x4200, // 1 0 0 2 0 0 0 0
	0xC200, // 3 0 0 2 0 0 0 0
	0x0420, // 0 0 1 0 0 2 0 0
	0x0C20, // 0 0 3 0 0 2 0 0
	0x0042, // 0 0 0 0 1 0 0 2
	0x00C2, // 0 0 0 0 3 0 0 2
	0x4020, // 1 0 0 0 0 2 0 0
	0xC020, // 3 0 0 0 0 2 0 0
	0x0402, // 0 0 1 0 0 0 0 2
	0x0C02, // 0 0 3 0 0 0 0 2
	0x4242, // 1 0 0 2 1 0 0 2
	0xC242, // 3 0 0 2 1 0 0 2
	0x42C2, // 1 0 0 2 3 0 0 2
	0xC2C2  // 3 0 0 2 3 0 0 2
};

string opToChar(int op) {
	switch (op) {
	case op_bracket_l: return "("; break;
	case op_bracket_r: return ")"; break;
	case op_not: return "~"; break;
	case op_mul: return "*"; break;
	case op_div: return "/"; break;
	case op_mod: return "%"; break;
	case op_add: return "+"; break;
	case op_sub: return "-"; break;
	case op_shift_l: return "<<"; break;
	case op_shift_r: return ">>"; break;
	case op_shift_lc: return "<<<"; break;
	case op_shift_rc: return ">>>"; break;
	case op_and: return "&"; break;
	case op_xor: return "^"; break;
	case op_or: return "|"; break;
	}
}

vec toSuffix(vec exp) {
	vec result;
	vec op;
	for (int i = 0; i < exp.size(); i++) {
		if (exp[i] <1000) {
			result.push_back(exp[i]);
		}
		else if (exp[i] == op_bracket_l) {
			op.push_back(exp[i]);
		}
		else if (exp[i] == op_bracket_r) {
			while (op[op.size() - 1] != op_bracket_l) {
				result.push_back(op[op.size() - 1]);
				op.pop_back();
			}
			op.pop_back();
		}
		else {
				while (op.size() > 0 && op[op.size() - 1] / 100 >= exp[i] / 100 && op[op.size() - 1] != op_bracket_l) {
					result.push_back(op[op.size() - 1]);
					op.pop_back();
				}
				op.push_back(exp[i]);
		}
	}

	while (op.size() > 0) {
		result.push_back(op[op.size() - 1]);
		op.pop_back();
	}

	return result;
}

void displayExp(vec exp) {
	for (int i = 0; i < exp.size(); i++) {
		if (exp[i] < 1000) {
			cout << exp[i];
		}
		else {
			cout << opToChar(exp[i]);
		}
	}
}

void outputExp(vec exp, ofstream& f) {
	for (int i = 0; i < exp.size(); i++) {
		if (exp[i] < 1000) {
			f << exp[i];
		}
		else {
			f << opToChar(exp[i]);
		}
	}
}

short calExp(vec exp) {
	vec::iterator i = exp.begin();
	while (i != exp.end()) {
		if (*i >= 1000) {
			if (*i == op_not) {
				i = exp.erase(i);
				unsigned char v, r = (unsigned char)*(i - 1);
				v = ~r;
				*(i - 1) = v;
			}
			else {
				unsigned char v1 = (unsigned char)*(i - 2), v2 = (unsigned char)*(i - 1), r;
				switch (*i) {
				case op_mul: r = v1 * v2; break;
				case op_div: if (v2 == 0) throw 0; else r = v1 / v2; break;
				case op_mod: if (v2 == 0) throw 0; else r = v1 % v2; break;
				case op_add: r = v1 + v2; break;
				case op_sub: r = v1 - v2; break;
				case op_shift_l: r = v1 << v2; break;
				case op_shift_r: r = v1 >> v2; break;
				case op_shift_lc: r = (v2 >= 8) ? 0 : v1 << v2 | v1 >> (8 - v2); break;
				case op_shift_rc: r = (v2 >= 8) ? 0 : v1 >> v2 | v1 << (8 - v2); break;
				case op_and: r = v1 & v2; break;
				case op_xor: r = v1 ^ v2; break;
				case op_or: r = v1 | v2; break;
				}
				*(i - 2) = r;
				i = exp.erase(i);
				i--;
				i = exp.erase(i);
			}
			i = exp.begin();
		}
		else {
			i++;
		}
	}
	return exp[0];
}

int main() {

	ofstream file;
	file.open("out.txt", ios::trunc);
	
	bitset<13> checkbit[13][13][13];
	int n = 0;

	for (int b1 = 1; b1 < 14; b1++) {
		for (int b2 = 1; b2 < 14; b2++) {
			for (int b3 = 1; b3 < 14; b3++) {
				for (int b4 = 1; b4 < 14; b4++) {
					int index[] = { b1, b2, b3, b4 };
					bool found = false;

					cout << b1 << " " << b2 << " " << b3 << " " << b4 << " ==> ";
					if (_OUTPUT == true) file << b1 << " " << b2 << " " << b3 << " " << b4 << " ==> ";

					sort(index, index + 4);
					if (checkbit[index[0] - 1][index[1] - 1][index[2] - 1][index[3] - 1] == 1) {
						found = true;
						cout << "Founded" << endl;
						if (_OUTPUT == true) file << "Founded" << endl;
					}
					else {
						for (int orbit = 0; orbit < 16; orbit++) {
							for (int o1 = 0; o1 < 12; o1++) {
								for (int o2 = 0; o2 < 12; o2++) {
									for (int o3 = 0; o3 < 12; o3++) {
										for (int bk = 0; bk < 15; bk++) {
											vec exp;
											// [< ~ | ~( >] 1
											if (bklist[bk] / 0x1000 == 4) exp.push_back(op_bracket_l);
											else if (bklist[bk] / 0x1000 == 12) { exp.push_back(op_not); exp.push_back(op_bracket_l); }
											// [~]b1
											if (orbit / 8 == 1) exp.push_back(op_not);
											exp.push_back(b1);
											// [)] 1
											if (bklist[bk] / 0x1000 == 2) exp.push_back(op_bracket_r);
											// < op > 1
											exp.push_back(oplist[o1]);
											// [< ~ | ~( >] 2
											if (bklist[bk] / 0x100 % 0x10 == 4) exp.push_back(op_bracket_l);
											else if (bklist[bk] / 0x100 % 0x10 == 12) { exp.push_back(op_not); exp.push_back(op_bracket_l); }
											// [~]b2
											if (orbit / 4 % 2 == 1) exp.push_back(op_not);
											exp.push_back(b2);
											// [)] 2
											if (bklist[bk] / 0x100 % 0x10 == 2) exp.push_back(op_bracket_r);
											// < op > 2
											exp.push_back(oplist[o2]);
											// [< ~ | ~( >] 3
											if (bklist[bk] / 0x10 % 0x10 == 4) exp.push_back(op_bracket_l);
											else if (bklist[bk] / 0x10 % 0x10 == 12) { exp.push_back(op_not); exp.push_back(op_bracket_l); }
											// [~]b3
											if (orbit / 2 % 2 == 1) exp.push_back(op_not);
											exp.push_back(b3);
											// [)] 3
											if (bklist[bk] / 0x10 % 0x10 == 2) exp.push_back(op_bracket_r);
											// < op > 3
											exp.push_back(oplist[o3]);
											// [< ~ | ~( >] 4
											if (bklist[bk] % 0x10 == 4) exp.push_back(op_bracket_l);
											else if (bklist[bk] % 0x10 == 12) { exp.push_back(op_not); exp.push_back(op_bracket_l); }
											// [~]b4
											if (orbit % 2 == 1) exp.push_back(op_not);
											exp.push_back(b4);
											// [)] 4
											if (bklist[bk] % 0x10 == 2) exp.push_back(op_bracket_r);

											vec suf = toSuffix(exp);
											int result;
											try {
												result = calExp(suf);
											}
											catch (int s) {
												result = 0;
											}
											if (result == 24) {
												n++;
												cout << n << ": ";
												if (_OUTPUT == true) file << n << ": ";
												displayExp(exp);
												outputExp(exp, file);
												cout << "=24" << endl;
												if (_OUTPUT == true) file << "=24" << endl;
												found = true;
												checkbit[index[0] - 1][index[1] - 1][index[2] - 1][index[3] - 1] = 1;
												break;
											}
										}
										if (found == true) break;
									}
									if (found == true) break;
								}
								if (found == true) break;
							}
							if (found == true) break;
						}
					}
					if (found == false) {
						cout << "Not Found" << endl;
						if (_OUTPUT == true) file << "Not Found" << endl;
					}
				}
			}
		}
	}
	file.close();
	getchar();
	return 0;
}