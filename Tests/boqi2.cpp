int main(int m) {
	int a = 2 * 3 - 5, b = a++, c = a *= (b)+5;
	a = a > 5 + b / 3;
	b = b <= (b *= (5 + a - b * b) / 9);
	(++c)++;
}

int main(int m) {
	int a = 2 * 3 - 5, b = a++, c = a *= (b)+5;
	a = a > 5 + b / 3;
	b = b <= b *= (5 + a - b * b) / 9;
	(++c)++;
}