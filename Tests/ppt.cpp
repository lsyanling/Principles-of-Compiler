
using namespace std;

class Solution {
public:
	void solveSudoku(vector<vector<char>>& board) {
		array<array<bool, 9>, 9> row{};
		array<array<bool, 9>, 9> column{};
		array<array<array<bool, 9>, 3>, 3> box{};
		int s = 0;
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (board[i][j] != '.') {
					row[i][board[i][j] - '1'] = true;
					column[j][board[i][j] - '1'] = true;
					box[i / 3][j / 3][board[i][j] - '1'] = true;
					s++;
				}
			}
		}
		array<array<bool, 9>, 9> rowS = row;
		array<array<bool, 9>, 9> columnS = column;
		array<array<array<bool, 9>, 3>, 3> boxS = box;
		vector<vector<char>> boardS = board;
		int m = 0;
		bool flag = false;
		vector<int> SW(81 - s, 0);
	restart:
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (board[i][j] == '.') {
					for (char k = '1'; k <= '9'; k++) {
						if (row[i][k - '1'] || column[j][k - '1'] || box[i / 3][j / 3][k - '1'])
							continue;
						if (k <= SW[m])
							continue;
						row[i][k - '1'] = true;
						column[j][k - '1'] = true;
						box[i / 3][j / 3][k - '1'] = true;
						m++;
						SW[m] = k;
						flag = true;
					}
					if (!flag) {
						row = rowS;
						column = columnS;
						box = boxS;
						m = 0;
						goto restart;
					}
					flag = false;
				}
			}
		}
	}
};

int main(void) {
	vector<vector<char>> board{ {'5','3','.','5','3','.','5','3','.'},{'5','3','.','5','3','.','5','3','.'}, {'5','3','.','5','3','.','5','3','.'}, {'5','3','.','5','3','.','5','3','.'}, {'5','3','.','5','3','.','5','3','.'}, {'5','3','.','5','3','.','5','3','.'}, {'5','3','.','5','3','.','5','3','.'}, {'5','3','.','5','3','.','5','3','.'}, {'5','3','.','5','3','.','5','3','.'}, };
	Solution s;
	s.solveSudoku(board);
}



int main() {
	std::ofstream os;
	os.open("123.txt", std::ios::out);
	os << "switch (x) {" << std::endl;
	for (int i = 0; i < 100000; i++) {
		int finger = i;
		int fingers = 1;
		int bigFinger, eatFinger, middleFinger, noNameFinger, smallFinger;
		for (int hand = finger / 10; hand; hand /= 10)
			fingers++;
		os << "case " << finger << ":" << std::endl;
		os << "\tcout << \"是" << fingers << "位数\" << endl;" << std::endl;
		if (fingers > 0) {
			bigFinger = finger % 10;
			finger /= 10;
			os << "\tcout << \"个位数是：" << bigFinger << "\" << endl;" << std::endl;
		}
		if (fingers > 1) {
			eatFinger = finger % 10;
			finger /= 10;
			os << "\tcout << \"十位数是：" << eatFinger << "\" << endl;" << std::endl;
		}
		if (fingers > 2) {
			middleFinger = finger % 10;
			finger /= 10;
			os << "\tcout << \"百位数是：" << middleFinger << "\" << endl;" << std::endl;
		}
		if (fingers > 3) {
			noNameFinger = finger % 10;
			finger /= 10;
			os << "\tcout << \"千位数是：" << noNameFinger << "\" << endl;" << std::endl;
		}
		if (fingers > 4) {
			smallFinger = finger % 10;
			finger /= 10;
			os << "\tcout << \"万位数是：" << smallFinger << "\" << endl;" << std::endl;
		}
		os << "\tcout << \"倒过来是：" << (fingers > 1 ? fingers > 2 ? fingers > 3 ? fingers > 4 ? bigFinger * 10000 + eatFinger * 1000 + middleFinger * 100 + noNameFinger * 10 + smallFinger : bigFinger * 1000 + eatFinger * 100 + middleFinger * 10 + noNameFinger : bigFinger * 100 + eatFinger * 10 + middleFinger : bigFinger * 10 + eatFinger : bigFinger) << "\" << endl;" << std::endl;
		os << "\tbreak;" << std::endl;
	}
	os << "}" << std::endl;
}




class A {
public:
	const int ppp() { pp(); return 0; }
	int pp() { return 0; }
	inline static int a = 5;
};

int main() {

}



void fun(int reg, int data) {
	reg += 1;
	data++;
	;
}

int main(void) {
	FUN(1, 2, (3, 4))
		;
	;
}


using namespace std;
class MyClass {
	int val;
	int copynumber;
	int* p = nullptr;
public:
	// Normal constructor.
	MyClass(int i) {
		val = i;
		copynumber = 0;
		cout << "Inside normal constructor\n";
	}
	// Copy constructor
	MyClass(const MyClass& o) {
		val = o.val;
		p = new int;
		copynumber = o.copynumber + 1;
		cout << "Inside copy constructor.\n";
	}
	~MyClass() {
		if (copynumber == 0)
			cout << "Destructing original.\n";
		else
			cout << "Destructing copy " <<
			copynumber << "\n";
		delete p;
	}
	int getval() { return val; }
};
void display(MyClass ob) {
	cout << ob.getval() << '\n';
}



bool isPalindrome(int x) {
    if (x < 0)
        return false;
    int m = 0;
    while (x / 10 < m) {
        m = m * 10 + x % 10;
        x /= 10;
    }
    if (x == m || x == m * 10 + x % 10)
        return true;
    return false;
}

int main() {
    std::cout<<isPalindrome(121);
}


using namespace std;

string longestPalindrome(string s) {
	int length = s.size();
	if (length == 1)
		return s;
	int maxSize = 1, l = 0;
	vector<vector<bool>> v(length, vector<bool>(length, true));
	for (int j = 1; j < length; j++) {
		for (int i = 0; i < j; i++) {
			if (s[i] == s[j] && v[i + 1][j - 1]) {
				if (j - i + 1 > maxSize) {
					maxSize = j - i + 1;
					l = i;
				}
			}
			else v[i][j] = false;
		}
	}
	return s.substr(l, maxSize);
}

int main() {
	cout<<longestPalindrome("aaaaa");
}

string convert(string s, int numRows) {
    int length = s.size();
    if (length <= numRows)
        return s;
    if (numRows == 1)
        return s;
    string ret;
    int d1, d2;
    for (int i = 0; i < numRows; i++) {
        d1 = 2 * (numRows - i) - 2;
        d2 = 2 * i;
        if (!d1)
            d1 = d2;
        if (!d2)
            d2 = d1;
        for (int j = i, r = 1; j < length;) {
            if (r) {
                r = 0;
                ret += s[j];
                j += d1;
            }
            else {
                r = 1;
                ret += s[j];
                j += d2;
            }
        }
    }
    return ret;
}

int main() {
    cout << convert("abcde",3);
}



using namespace std;

int main(void) {
    double h1, h2, d, L, x1, x2;
    cin >> h1 >> h2 >> d;
    L = d;
    x1 = h2 + (h1 - h2) / 2;
    x2 = x1 - L;

    cout << setiosflags(ios::fixed) << setprecision(4);
    cout << L << ' ' << x1 << ' ' << x2;

    return 0;
}


using namespace std;

int main() {
	int n, K, P;
	cin >> n >> K >> P;
	int max = 0;
	int* v = new int[n];
	int* k = new int[n];
	int* p = new int[n];
	int* choice = new int[n];
	int now = 0;
	int nowv = 0;
	int nowk = 0;
	int nowp = 0;
	for (int i = 0; i < n; i++) {
		cin >> k[i] >> v[i] >> p[i];
		choice[i] = 0;
	}
	while (now < n) {
		choice[now] = 1;
		nowv += v[now];
		nowk += k[now];
		nowp += p[now];
		if (max < nowv) {
			max = nowv;
		}
		if (nowk <= K && nowp <= P) {
			now++;
		}
		else {
			choice[now] = 0;
			now--;
		}
			

	}
}




using namespace std;

int main() {
	int n;
	cin >> n;
	int* a = new int[n + 2];
	bool flag = false;
	for (int i = 1; i < n + 1; i++) {
		cin >> a[i];
		if (a[i] == 1)
			flag = true;
	}
	if (!flag) {
		cout << -1;
		return 0;
	}
	a[0] = 0;
	a[n + 1] = 0;
	flag = false;
	int days = 0;
	for (int j = 1; j < n + 1; j++) {
		if (a[j] != 1)
			break;
		if (j == n) {
			cout << days;
			return 0;
		}
	}
	while (true) {
		for (int i = 1; i < n + 1; i++) {
			if (flag) {
				flag = false;
				continue;
			}
			if (a[i] == 1) {
				a[i - 1] = 1;
				if (a[i + 1] != 1) {
					flag = true;
					a[i + 1] = 1;
				}
			}
		}
		days++;
		for (int j = 1; j < n + 1; j++) {
			if (a[j] != 1)
				break;
			if (j == n) {
				cout << days;
				return 0;
			}
		}
	}
}




int main(void) {
	__asm {

	}
}