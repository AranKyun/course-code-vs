#include "calcore.h"

uchar addBinFunc(char* funcName, ...) {
	if (funcName == NULL)
		return 2;

	strcpy_s(funcName, 10, "add");

	va_list arg;
	va_start(arg, funcName);
	uchar v1 = va_arg(arg, uchar), v2 = va_arg(arg, uchar);
	va_end(arg);

	return v1 + v2;
}

uchar mulBinFunc(char* funcName, ...) {
	if (funcName == NULL)
		return 2;

	strcpy_s(funcName, 10, "mul");

	va_list arg;
	va_start(arg, funcName);
	uchar v1 = va_arg(arg, uchar), v2 = va_arg(arg, uchar);
	va_end(arg);

	return v1 * v2;
}

uchar divBinFunc(char* funcName, ...) {
	if (funcName == NULL)
		return 2;

	strcpy_s(funcName, 10, "div");

	va_list arg;
	va_start(arg, funcName);
	uchar v1 = va_arg(arg, uchar), v2 = va_arg(arg, uchar);
	va_end(arg);

	if (v2 == 0)
		throw "IllegalOperandException";
	else
		return v1 / v2;
}

uchar notMonFunc(char* funcName, ...) {
	if (funcName == NULL)
		return 1;

	strcpy_s(funcName, 10, "not");

	va_list arg;
	va_start(arg, funcName);
	uchar v1 = va_arg(arg, uchar);
	va_end(arg);

	return ~v1;
}

uchar condTerFunc(char* funcName, ...) {
	if (funcName == NULL)
		return 3;

	strcpy_s(funcName, 10, "cond");

	va_list arg;
	va_start(arg, funcName);
	uchar v1 = va_arg(arg, uchar), v2 = va_arg(arg, uchar), v3 = va_arg(arg, uchar);
	va_end(arg);

	return v1 ? v2 : v3;
}

int main() {
	
	OpFunc op[] = { addBinFunc, condTerFunc, notMonFunc };

	uchar val[] = { 1, 28, 82, 187 };

	calFunc(val, op, 2);

	getchar();
	return 0;
}