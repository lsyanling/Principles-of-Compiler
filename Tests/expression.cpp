int main(int m) {
	int a = 0, b = 2, c;
	float f = 3.25;
	c = a += b *= 2;
	if (a > b) {
		a++;
		do {
			c--;
			break;
		} while (a + b != 3);
	}
	else if (a > c) {
		a = b + c;
		while (1 < 0) {
			a++;
			if (f > 2) {
				f = a + b + c;
				a++;
			}
			else {
				break;
			}
			f /= a;
			continue;
		}
	}
	else {
		a + b + c;
	}
}