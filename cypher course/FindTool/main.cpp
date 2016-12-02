#include <stack>
#include <iostream>
#include <stdarg.h>
#include <vector>

using namespace std;

const int OPERAND_N = 4;
const int VALARR_SIZE = 24;
const int ORDER_MAX = 5;

int valindex_arr[VALARR_SIZE][OPERAND_N];

typedef unsigned char uchar;
typedef uchar(*OpFunc)(char*, ...);

struct Node {
	bool isVal;
	int val_index;
	int op_index;
	int in_order;
	Node* firstChild;
	Node* nextSibling;
	Node* parent;
};

Node* getValNode(Node* tree, int n) {
	int count = 0;

	std::stack<Node*> stack;
	if (!tree) {
		return nullptr;
	}

	while (tree || !stack.empty()) {
		while (tree) {
			stack.push(tree);
			if (tree->isVal) {
				if (count == n)
					return tree;
				count++;
			}
			tree = tree->firstChild;
		}
		tree = stack.top();
		stack.pop();
		if (tree->nextSibling) {
			tree = tree->nextSibling;
		}
		else {
			if (!stack.empty()) {
				tree = stack.top()->nextSibling;
				stack.pop();
			}
			else {
				tree = nullptr;
			}
		}
	}

	return nullptr;
}


void showTree(Node* tree, uchar* val, OpFunc* op) {
	char str[10];

	if (tree->isVal)
		printf("%d", val[tree->val_index]);
	else {
		op[tree->op_index](str);
		std::cout << str << "(";

		Node* tmp = tree->firstChild;
		while (tmp) {
			showTree(tmp, val, op);
			tmp = tmp->nextSibling;

			if(tmp != nullptr)
				std::cout << ", ";
		}

		std::cout << ")";
	}
}

uchar calTree(Node* tree, uchar* val, OpFunc* op) {
	char str[10];

	if (tree->isVal)
		return val[tree->val_index];
	else {
		int n = op[tree->op_index](0);
		
		uchar* vals = new uchar[n];
		Node* tmp = tree->firstChild;
		for (int i = 0; i < n; i++) {
			vals[i] = calTree(tmp, val, op);
			tmp = tmp->nextSibling;
		}

		return op[tree->op_index](str, vals);
	}
}

Node* getNodeByInOrder(Node* tree, int order) {
	std::stack<Node*> stack;
	if (!tree) {
		return nullptr;
	}

	while (tree || !stack.empty()) {
		while (tree) {
			stack.push(tree);
			if (!tree->isVal && tree->in_order == order) {
				return tree;
			}
			tree = tree->firstChild;
		}
		tree = stack.top();
		stack.pop();
		if (tree->nextSibling) {
			tree = tree->nextSibling;
		}
		else {
			if (!stack.empty()) {
				tree = stack.top()->nextSibling;
				stack.pop();
			}
			else {
				tree = nullptr;
			}
		}
	}

	return nullptr;
}

void addValChildren(Node* node, int n) {
	node->firstChild = new Node;
	Node* p = node->firstChild;
	p->firstChild = nullptr;
	p->isVal = true;
	p->parent = node;
	p->nextSibling = nullptr;
	for (int i = 1; i < n; i++) {
		p->nextSibling = new Node;
		p = p->nextSibling;
		p->firstChild = nullptr;
		p->isVal = true;
		p->parent = node;
		p->nextSibling = nullptr;
	}
}

void deleteValChildren(Node* node) {
	Node* p = node->firstChild;
	node->firstChild = nullptr;

	while (p) {
		Node* tmp = p->nextSibling;
		delete p;
		p = tmp;
	}
}

bool switchToNext(int& op_count, std::stack<int>& indexStack, Node* expTree, int& operand_count, uchar* operand, OpFunc* oparr, int opn) {
	Node* lastNode = getNodeByInOrder(expTree, op_count - 1);
	deleteValChildren(lastNode);
	operand_count = operand_count - oparr[lastNode->op_index](0) + 1;
	if (lastNode->op_index == opn - 1) {
		//转去同级下一节点
		lastNode->isVal = true;
		int lastIndex = indexStack.top();
		Node* nextNode = getValNode(expTree, lastIndex + 1);
		if (nextNode) {
			//同级有效
			indexStack.pop();
			indexStack.push(lastIndex + 1);
			nextNode->isVal = false;
			nextNode->op_index = 0;
			nextNode->in_order = op_count - 1;
			addValChildren(nextNode, oparr[nextNode->op_index](0));
			operand_count = operand_count + oparr[nextNode->op_index](0) - 1;
		}
		else {
			//同级无效，转到上级
			indexStack.pop();
			op_count--;
			if (op_count <= 0)
				return false;
			if(switchToNext(op_count, indexStack, expTree, operand_count, operand, oparr, opn) == false)
				return false;
		}
	}
	else {
		//选择下一Op
		lastNode->op_index++;
		addValChildren(lastNode, oparr[lastNode->op_index](0));
		operand_count = operand_count + oparr[lastNode->op_index](0) - 1;
	}

	return true;
}


void arrange(vector<int> fixed, vector<int> free, int& count) {
	if (free.size() == 0) {
		for (int i = 0; i < fixed.size(); i++) {
			valindex_arr[count][i] = fixed[i];
		}
		count++;
	}
	else {
		vector<int> fixed_p, free_p;
		for (int i = 0; i < free.size(); i++) {
			fixed_p = fixed;
			fixed_p.push_back(free[i]);
			free_p = free;
			free_p.erase(free_p.begin() + i);
			arrange(fixed_p, free_p, count);
		}
	}
}

bool coreFunc(uchar* operand, OpFunc* oparr, int opn, uchar result) {
	OpFunc* oparr_s = new OpFunc[opn];
	int j = 0;
	for (int i = 0; i < opn; i++) {
		if (oparr[i](0) == 1) {
			j++;
			oparr_s[opn - j] = oparr[i];
		}
		else {
			oparr_s[i - j] = oparr[i];
		}
	}
	oparr = oparr_s;

	vector<int> fixed, free;
	for (int i = 0; i < 4; i++) {
		free.push_back(i);
	}
	int count = 0;
	arrange(fixed, free, count);

	int op_limit = 1;

	Node* expTree = new Node;
	expTree->isVal = true;
	expTree->firstChild = nullptr;
	expTree->nextSibling = nullptr;
	expTree->parent = nullptr;

	int operand_count = 1;
	int op_count = 0;

	std::stack<int> indexStack;

	uchar val_list[4] = {
		1, 2, 3, 4
	};

	int result_count = 0;

	bool returnFlag = true;
	Node* node = expTree;
	while (returnFlag) {
		if (op_count == op_limit) {
			if (operand_count == OPERAND_N) {
				
				for (int i = 0; i < VALARR_SIZE; i++) {
					for (int j = 0; j < OPERAND_N; j++) {
						getValNode(expTree, j)->val_index = valindex_arr[i][j];
					}

					uchar result_t = calTree(expTree, val_list, oparr);
					if (true) {
						result_count++;
						cout << result_count++ << " ";
						showTree(expTree, val_list, oparr);
						cout << " = " << (int)result_t << endl;
					}
				}
				
				
				if (switchToNext(op_count, indexStack, expTree, operand_count, operand, oparr, opn) == false)
					op_limit++;
			}
			else {
				if (switchToNext(op_count, indexStack, expTree, operand_count, operand, oparr, opn) == false)
					op_limit++;
			}
		}
		else{
			Node* nextNode = getValNode(expTree, 0);
			nextNode->isVal = false;
			nextNode->in_order = op_count;
			op_count++;
			nextNode->op_index = 0;
			indexStack.push(0);
			addValChildren(nextNode, oparr[nextNode->op_index](0));
			operand_count = operand_count + oparr[nextNode->op_index](0) - 1;
		}
	}

	return true;
}

uchar addBinFunc(char* funcName, ...) {
	if (funcName == NULL)
		return 2;

	strcpy_s(funcName, 10, "add");

	va_list arg;
	va_start(arg, funcName);
	uchar* v1 = va_arg(arg, uchar*);
	va_end(arg);

	return *v1 + *(v1 + 1);
}

uchar addOneMonFunc(char* funcName, ...) {
	if (funcName == NULL)
		return 1;

	strcpy_s(funcName, 10, "add_one");

	va_list arg;
	va_start(arg, funcName);
	uchar* v1 = va_arg(arg, uchar*);
	va_end(arg);

	return *v1 + 1;
}

int main() {
	OpFunc op[2] = { addBinFunc, addOneMonFunc };
	coreFunc(nullptr, op, 2, 24);

	getchar();
	return 0;
}
