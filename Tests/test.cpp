int func1(int i, int j);
int func1(int a, int b);
int main(void v);

int main(void v) {
	int a = 2, b = 3.5e-2;
	int c = func1(a, b);
	char ch = 'a';
	char chn = 'abc';
	bool bo;
	bo = 2;
	string str = "abc\r\n";
	bo = str;
	bo = func1;

	if (a + b > ch) {
		float f = 3.5;
		a++;
		++b;
	lab1:
		a %= a *= b;
		a = (a--, b++, f / 2);
	}
	else if (a + b > a || a - b > c && a-- || a ? b : a + b) {
	lab2:
		a&& b;
		int i = 6;
		while (1) {
			if (i-- < 0) {
				break;
			}
			else {
				continue;
			}
		}
		goto lab1;
	}
	else
		goto lab1;
	goto lab3;
lab3:
	goto lab2;

	return 0;
}

int func1(int a, int b) {
	while (a > b) {
		a--;
		if (a < 0) {
			break;
		}
	}
	{
		char c = '233';
		{
			int c;
			float d;
			{
				float c = 666;
				bool d;
				do
					c + d;
					while (c < d);
			}
		}
	}
	return a + b;
}