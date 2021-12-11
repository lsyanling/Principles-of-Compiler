int main(void v) {
	int a = 1, b = 2, c = 9;
	char ch = '2333';
	string str = "2333";
	while (a + b) {
	label4:
		b++;
		if (a > 0)
			break;
		goto label2;
	}
label1:
	goto label3;
label3:
	c = 1;
	goto label4;
label2:
	a = 0;
	goto label1;
	a > 0;
	
	return;
}

