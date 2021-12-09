#include"head.h"

// �ʷ�����

// func    �ʷ��������캯��
// param   Դ�ļ�·��
// return  
LexicalAnalysis::LexicalAnalysis(char* codeFileString, std::vector<Word>& wordTableSource) :wordTable(wordTableSource) {

	// ��ǻ�����β
	end = source + MAX_BUFFER;

	// ��Դ�ļ�
	fin.open(codeFileString, std::ios::in | std::ios::binary);

	// ��ȡԴ�ļ�
	fin.read(source, end - source);

	// Դ����Ԥ����
	Preprocess();

	// �ʷ�����
	if (Analyse()) {
		// �������
		OutPut();
	}
}

// func    Դ����Ԥ���� ��sourceת��Ϊcode ����ɾ��ת�ƻ��� ɾ��Դ�ļ������հ��ַ�
// param   
// return  
void LexicalAnalysis::Preprocess() {

	// �����ַ���
	code = new char[strlen(source) + 1];

	// ���Դ�ļ�β
	end = source + strlen(source) + 1;

	// TODO 
	// Դ�ַ�ӳ�� ���ﲻ��
	;

	// ת�ƻ��� ɾ��\��ӻ��з���ʵ��
	// ɾ������Ŀհ��ַ� ����code�ַ���
	CreateCode();

	// TODO
	// ɾ��ע�� ���ﲻ��

	// TODO
	// �����ַ������� ���ﲻ��

	// ��Ǵ����ַ���β
	end = code + strlen(code) + 1;

	// Ԥ��ָ��
	p = code;

	return;
}

// func    ���ɴ����ַ���
// param
// return  
bool LexicalAnalysis::CreateCode() {

	// ����type alpha����ʾʵ�ʺ���
	bool inString = false;

	int i = 0, n = 0;
	for (; source + i + 2 <= end; i++) {

		// �����ַ�����\\\r\n��ʾһ��ת�ƻ���ʵ��
		if (source[i] == '\\' && source[i + 1] == '\r' && source[i + 2] == '\n') {
			i += 2;
			continue;
		}
		if (!inString) {

			// �ַ������\t�������ո��Ϊ�հ�ʵ��
			if (source[i] == '\t') {
				continue;
			}
			if (source[i] == ' ' && source[i + 1] == ' ') {
				continue;
			}

			// �ַ���ʼ
			if (source[i] == '"') {
				inString = true;
				code[n++] = source[i];
				continue;
			}
		}
		if (inString) {

			// �ַ���β
			if (source[i] == '"') {
				inString = false;
				code[n++] = source[i];
				continue;
			}
			code[n++] = source[i];
			continue;
		}
		// ��ͨ�ַ�
		code[n++] = source[i];
	}

	code[n] = 0;

	return true;
}

// func    �ʷ�����
// param   
// return  
bool LexicalAnalysis::Analyse() {

	while (p + 1 < end) {

		// ����ָ��
		q = p;

		// ���ø�����״̬�Զ��� �ɶ�·Ч����ֻ֤��ȡһ�δ��� ��Ҫ���жϹؼ��ֺ��жϱ�ʶ��
		if (Number() || Semicolon() || Operator() || Space() ||
			KeyWord() || Identifier() || String() || Char()) {
			wordTable;
			line;
			continue;
		}
		std::cout << "Lexical Error in line " << line << ": Can't identify the sign." << std::endl;

	}
	return true;
}

// func    ����������ʶ�� �Զ���
// param   
// return  
bool LexicalAnalysis::Number() {
	EnumNumber state = EnumNumber::Start;
	while (true) {
		switch (state) {
		case EnumNumber::Start:
		{
			if (*p == '0') {
				state = EnumNumber::ZeroOrEight;
			}
			else if (isdigit(*p)) {
				state = EnumNumber::Ten;
			}
			else if (*p == '.') {
				state = EnumNumber::FloatPoint;
			}
			else {
				// ����ָ��
				p = q;
				return false;
			}
			break;
		}
		case EnumNumber::ZeroOrEight:
		{
			if (*p == '.') {
				state = EnumNumber::FloatPoint;
			}
			else if (*p == 'x' || *p == 'X') {
				state = EnumNumber::Sixteen;
			}
			else if (*p == 'e' || *p == 'E') {
				state = EnumNumber::FloatEe;
			}
			else if (*p >= '0' && *p <= '7') {
				state = EnumNumber::Eight;
			}
			else if (isspace(*p) || *p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '%' ||
				*p == '&' || *p == '|' || *p == '!' || *p == '<' || *p == '>' || *p == '=' ||
				*p == ')' || *p == ']' || *p == '?' || *p == ':' || *p == '^' || *p == ',' ||
				*p == ';') {

				// �漼���� ������ַ�
				char ch = *p;
				*p = '\0';
				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumProperties::TenIntNumber;
				wordTable[wordTableIndex].line = this->line;
				// �ָ����ַ�
				*p = ch;
				// ��һ������
				wordTableIndex++;
				return true;
			}
			else {
				// ����ָ��
				p = q;
				return false;
			}
			break;
		}
		case EnumNumber::Eight:
		{
			if (*p >= '0' && *p <= '7') {
				state = EnumNumber::Eight;
			}
			else if (isspace(*p) || *p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '%' ||
				*p == '&' || *p == '|' || *p == '!' || *p == '<' || *p == '>' || *p == '=' ||
				*p == ')' || *p == ']' || *p == '?' || *p == ':' || *p == '^' || *p == ',' ||
				*p == ';') {

				// �漼���� ������ַ�
				char ch = *p;
				*p = '\0';
				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumProperties::EightIntNumber;
				wordTable[wordTableIndex].line = this->line;
				// �ָ����ַ�
				*p = ch;
				// ��һ������
				wordTableIndex++;
				return true;
			}
			else {
				// ����ָ��
				p = q;
				return false;
			}
			break;
		}
		case EnumNumber::Ten:
		{
			if (*p == '.') {
				state = EnumNumber::FloatPoint;
			}
			else if (*p == 'e' || *p == 'E') {
				state = EnumNumber::FloatEe;
			}
			else if (isdigit(*p)) {
				state = EnumNumber::Ten;
			}
			else if (isspace(*p) || *p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '%' ||
				*p == '&' || *p == '|' || *p == '!' || *p == '<' || *p == '>' || *p == '=' ||
				*p == ')' || *p == ']' || *p == '?' || *p == ':' || *p == '^' || *p == ',' ||
				*p == ';') {

				// �漼���� ������ַ�
				char ch = *p;
				*p = '\0';
				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumProperties::TenIntNumber;
				wordTable[wordTableIndex].line = this->line;
				// �ָ����ַ�
				*p = ch;
				// ��һ������
				wordTableIndex++;
				return true;
			}
			else {
				// ����ָ��
				p = q;
				return false;
			}
			break;
		}
		case EnumNumber::Sixteen:
		{
			if (isdigit(*p) || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')) {
				state = EnumNumber::TrueSixteen;
			}
			else {
				// ����ָ��
				p = q;
				return false;
			}
			break;
		}
		case EnumNumber::TrueSixteen:
		{
			if (isdigit(*p) || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')) {
				state = EnumNumber::TrueSixteen;
			}
			else if (isspace(*p) || *p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '%' ||
				*p == '&' || *p == '|' || *p == '!' || *p == '<' || *p == '>' || *p == '=' ||
				*p == ')' || *p == ']' || *p == '?' || *p == ':' || *p == '^' || *p == ',' ||
				*p == ';') {

				// �漼���� ������ַ�
				char ch = *p;
				*p = '\0';
				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumProperties::SixteenIntNumber;
				wordTable[wordTableIndex].line = this->line;
				// �ָ����ַ�
				*p = ch;
				// ��һ������
				wordTableIndex++;
				return true;
			}
			else {
				// ����ָ��
				p = q;
				return false;
			}
			break;
		}
		case EnumNumber::FloatPoint:
		{
			if (isdigit(*p)) {
				state = EnumNumber::FloatNumber;
			}
			else {
				// ����ָ��
				p = q;
				return false;
			}
			break;
		}
		case EnumNumber::FloatNumber:
		{
			if (isdigit(*p)) {
				state = EnumNumber::FloatNumber;
			}
			else if (*p == 'e' || *p == 'E') {
				state = EnumNumber::FloatEe;
			}
			else if (isspace(*p) || *p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '%' ||
				*p == '&' || *p == '|' || *p == '!' || *p == '<' || *p == '>' || *p == '=' ||
				*p == ')' || *p == ']' || *p == '?' || *p == ':' || *p == '^' || *p == ',' ||
				*p == ';') {

				// �漼���� ������ַ�
				char ch = *p;
				*p = '\0';
				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumProperties::FloatNumber;
				wordTable[wordTableIndex].line = this->line;
				// �ָ����ַ�
				*p = ch;
				// ��һ������
				wordTableIndex++;
				return true;
			}
			else {
				// ����ָ��
				p = q;
				return false;
			}
			break;
		}
		case EnumNumber::FloatEe:
		{
			if (*p == '+' || *p == '-') {
				state = EnumNumber::FloatSign;
			}
			else if (isdigit(*p)) {
				state = EnumNumber::FloatEeNumber;
			}
			else {
				// ����ָ��
				p = q;
				return false;
			}
			break;
		}
		case EnumNumber::FloatSign:
		{
			if (isdigit(*p)) {
				state = EnumNumber::FloatEeNumber;
			}
			else {
				// ����ָ��
				p = q;
				return false;
			}
			break;
		}
		case EnumNumber::FloatEeNumber:
		{
			if (isdigit(*p)) {
				state = EnumNumber::FloatEeNumber;
			}
			else if (isspace(*p) || *p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '%' ||
				*p == '&' || *p == '|' || *p == '!' || *p == '<' || *p == '>' || *p == '=' ||
				*p == ')' || *p == ']' || *p == '?' || *p == ':' || *p == '^' || *p == ',' ||
				*p == ';') {

				// �漼���� ������ַ�
				char ch = *p;
				*p = '\0';
				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumProperties::FloatNumber;
				wordTable[wordTableIndex].line = this->line;
				// �ָ����ַ�
				*p = ch;
				// ��һ������
				wordTableIndex++;
				return true;
			}
			else {
				// ����ָ��
				p = q;
				return false;
			}
			break;
		}
		default:
			break;
		}
		p++;
	}
}

// func    �ֺ�ʶ�� �����Զ���
// param   
// return  
bool LexicalAnalysis::Semicolon() {
	if (*p == ';') {
		// �漼���� ������һ���ַ�
		char ch = *(p + 1);
		*(p + 1) = '\0';
		wordTable[wordTableIndex].word = std::string(q);
		wordTable[wordTableIndex].property = EnumProperties::Semicolon;
		wordTable[wordTableIndex].line = this->line;
		// �ָ���һ���ַ�
		*(p + 1) = ch;
		// ��һ������
		p++;
		wordTableIndex++;
		return true;
	}
	else {
		// ����ָ��
		p = q;
		return false;
	}
}

// func    �����ʶ�� �����Զ���
// param   
// return  
bool LexicalAnalysis::Operator() {
	// �������
	if (IsOperator(*p)) {
		EnumProperties e = EnumProperties::Auto;
		int length = 3;
		// �����ַ����������
		// ���Ƹ�ֵ
		if (*p == '<' && *(p + 1) == '<' && *(p + 2) == '=') {
			e = EnumProperties::OperatorLeftShiftAssign;
		}
		// ���Ƹ�ֵ
		else if (*p == '>' && *(p + 1) == '>' && *(p + 2) == '=') {
			e = EnumProperties::OperatorRightShiftAssign;
		}

		// �����ַ����������
		// ���������
		else if (*p == '+' && *(p + 1) == '+') {
			e = EnumProperties::OperatorAddAdd;
			length = 2;
		}
		// �ݼ������
		else if (*p == '-' && *(p + 1) == '-') {
			e = EnumProperties::OperatorSubSubtract;
			length = 2;
		}
		// ��������
		else if (*p == '=' && *(p + 1) == '=') {
			e = EnumProperties::OperatorEqual;
			length = 2;
		}
		// ���ڵ���
		else if (*p == '>' && *(p + 1) == '=') {
			e = EnumProperties::OperatorGreaterEqual;
			length = 2;
		}
		// С�ڵ���
		else if (*p == '<' && *(p + 1) == '=') {
			e = EnumProperties::OperatorSmallerEqual;
			length = 2;
		}
		// ����
		else if (*p == '!' && *(p + 1) == '=') {
			e = EnumProperties::OperatorNotEqual;
			length = 2;
		}
		// ����
		else if (*p == '<' && *(p + 1) == '<') {
			e = EnumProperties::OperatorLeftShift;
			length = 2;
		}
		// ����
		else if (*p == '>' && *(p + 1) == '>') {
			e = EnumProperties::OperatorRightShift;
			length = 2;
		}
		// ������ֵ
		else if (*p == '+' && *(p + 1) == '=') {
			e = EnumProperties::OperatorAddAssign;
			length = 2;
		}
		else if (*p == '-' && *(p + 1) == '=') {
			e = EnumProperties::OperatorSubAssign;
			length = 2;
		}
		else if (*p == '*' && *(p + 1) == '=') {
			e = EnumProperties::OperatorMulAssign;
			length = 2;
		}
		else if (*p == '/' && *(p + 1) == '=') {
			e = EnumProperties::OperatorDivAssign;
			length = 2;
		}
		else if (*p == '%' && *(p + 1) == '=') {
			e = EnumProperties::OperatorModAssign;
			length = 2;
		}
		// ��λ�븳ֵ
		else if (*p == '&' && *(p + 1) == '=') {
			e = EnumProperties::OperatorAndAssign;
			length = 2;
		}
		// ��λ��ֵ
		else if (*p == '|' && *(p + 1) == '=') {
			e = EnumProperties::OperatorAssign;
			length = 2;
		}
		// ��λ���ֵ
		else if (*p == '^' && *(p + 1) == '=') {
			e = EnumProperties::OperatorXOrAssign;
			length = 2;
		}
		// �߼���
		else if (*p == '&' && *(p + 1) == '&') {
			e = EnumProperties::OperatorAnd;
			length = 2;
		}
		// �߼���
		else if (*p == '|' && *(p + 1) == '|') {
			e = EnumProperties::OperatorOr;
			length = 2;
		}
		// ��ͷ�����
		else if (*p == '-' && *(p + 1) == '>') {
			e = EnumProperties::OperatorArrow;
			length = 2;
		}

		// һ���ַ����������
		// ���������
		else if (*p == '+') {
			e = EnumProperties::OperatorAdd;
			length = 1;
		}
		else if (*p == '-') {
			e = EnumProperties::OperatorSubtract;
			length = 1;
		}
		else if (*p == '*') {
			e = EnumProperties::OperatorMultiply;
			length = 1;
		}
		else if (*p == '/') {
			e = EnumProperties::OperatorDivide;
			length = 1;
		}
		else if (*p == '%') {
			e = EnumProperties::OperatorModulo;
			length = 1;
		}
		// ��Ա�����
		else if (*p == '.') {
			e = EnumProperties::OperatorPoint;
			length = 1;
		}
		// ���������
		else if (*p == '(') {
			e = EnumProperties::OperatorLeftRound;
			length = 1;
		}
		else if (*p == ')') {
			e = EnumProperties::OperatorRightRound;
			length = 1;
		}
		else if (*p == '[') {
			e = EnumProperties::OperatorLeftSquare;
			length = 1;
		}
		else if (*p == ']') {
			e = EnumProperties::OperatorRightSquare;
			length = 1;
		}
		else if (*p == '{') {
			e = EnumProperties::OperatorLeftBrace;
			length = 1;
		}
		else if (*p == '}') {
			e = EnumProperties::OperatorRightBrace;
			length = 1;
		}
		// ���������
		else if (*p == '?') {
			e = EnumProperties::OperatorQuestion;
			length = 1;
		}
		else if (*p == ':') {
			e = EnumProperties::OperatorColon;
			length = 1;
		}
		// �߼���
		else if (*p == '!') {
			e = EnumProperties::OperatorNot;
			length = 1;
		}
		// ����
		else if (*p == ',') {
			e = EnumProperties::OperatorComma;
			length = 1;
		}
		// ����
		else if (*p == '>') {
			e = EnumProperties::OperatorGreaterThan;
			length = 1;
		}
		// С��
		else if (*p == '<') {
			e = EnumProperties::OperatorSmallerThan;
			length = 1;
		}
		// ��ֵ
		else if (*p == '=') {
			e = EnumProperties::OperatorAssign;
			length = 1;
		}
		// ��λȡ��
		else if (*p == '~') {
			e = EnumProperties::OperatorBitNot;
			length = 1;
		}
		// ��λ��
		else if (*p == '&') {
			e = EnumProperties::OperatorBitAnd;
			length = 1;
		}
		// ��λ��
		else if (*p == '|') {
			e = EnumProperties::OperatorBitOr;
			length = 1;
		}
		// ��λ���
		else if (*p == '^') {
			e = EnumProperties::OperatorXOr;
			length = 1;
		}

		// �漼���� ������һ���ַ�
		char ch = *(p + length);
		*(p + length) = '\0';
		wordTable[wordTableIndex].word = std::string(q);
		wordTable[wordTableIndex].property = e;
		wordTable[wordTableIndex].line = this->line;
		// �ָ���һ���ַ�
		*(p + length) = ch;
		// ��һ������
		p += length;
		wordTableIndex++;
		return true;
	}
	else {
		// ����ָ��
		p = q;
		return false;
	}
}

// func    �հ�ʶ�� �������� �����Զ���
// param   
// return  
bool LexicalAnalysis::Space() {
	if (*p == ' ') {
		p++;
		return true;
	}
	else if (*p == '\r' && *(p + 1) == '\n') {
		p += 2;
		line++;
		return true;
	}
	else {
		// ����ָ��
		p = q;
		return false;
	}
}

// func    �ؼ���ʶ�� �����Զ��� ��Ϊ�ؼ�����û�����ֺ��»��� ��
// param   
// return  
bool LexicalAnalysis::KeyWord() {
	if (isalpha(*p)) {
		while (isalpha(*p)) {
			p++;
		}
		// �ؼ���֮����ǿհ� �ֺ� ��Բ���� ������
		if (!isspace(*p) && *p != ';' && *p != '(' && *p != '{') {
			// ����ָ��
			p = q;
			return false;
		}
		// �漼���� ������ַ�
		char ch = *p;
		*p = '\0';

		auto iter = keyWord.keyWord.find(q);
		// �ǹؼ���
		if (iter != keyWord.keyWord.end()) {
			wordTable[wordTableIndex].word = std::string(q);
			wordTable[wordTableIndex].property = (*iter).second;
			wordTable[wordTableIndex].line = this->line;
			// �ָ����ַ�
			*p = ch;
			// ��һ������
			wordTableIndex++;
			return true;
		}
		else {
			// �ָ����ַ�
			*p = ch;
			// ����ָ��
			p = q;
			return false;
		}
	}
}

// func    ��ʶ��ʶ�� �Զ���
// param   
// return  
bool LexicalAnalysis::Identifier() {
	if (isalpha(*p) || *p == '_') {
		int autoMachine = 0;
		while (isalpha(*p) || isdigit(*p) || *p == '_') {
			p++;
		}

		// �޷�ʶ��ı��
		if (*p == '\\') {
			// ����ָ��
			p = q;
			return false;
		}

		// �漼���� ������ַ�
		char ch = *p;
		*p = '\0';
		wordTable[wordTableIndex].word = std::string(q);
		wordTable[wordTableIndex].property = EnumProperties::Identifier;
		wordTable[wordTableIndex].line = this->line;
		// �ָ����ַ�
		*p = ch;
		// ��һ������
		wordTableIndex++;
		return true;
	}
	else {
		// ����ָ��
		p = q;
		return false;
	}
}

// func    �ַ���������ʶ�� �Զ���
// param   
// return  
bool LexicalAnalysis::String() {
	EnumString state = EnumString::Start;
	while (true) {
		switch (state) {
		case EnumString::Start:
		{
			if (*p == '"') {
				state = EnumString::In;
			}
			else {
				// ����ָ��
				p = q;
				return false;
			}
			break;
		}
		case EnumString::In:
		{
			if (*p == '\0') {
				// �漼���� ������һ���ַ�
				char ch = *(p + 1);
				*p = '"';
				*(p + 1) = '\0';

				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumProperties::String;
				wordTable[wordTableIndex].line = this->line;

				// �ָ���һ���ַ�
				*p = '\0';
				*(p + 1) = ch;

				// ��һ������
				wordTableIndex++;
				// ��λ��һ������
				while (!(*p == '"' && *(p - 1) != '\\')) {
					p++;
				}
				p++;
				q = p;
				return true;
			}
			else if (*p == '"') {
				// �漼���� ������һ���ַ�
				char ch = *(p + 1);
				*(p + 1) = '\0';
				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumProperties::String;
				wordTable[wordTableIndex].line = this->line;
				// �ָ���һ���ַ�
				*(p + 1) = ch;
				p++;
				// ��һ������
				wordTableIndex++;
				return true;
			}
			else if (*p == '\\') {
				state = EnumString::Trope;
			}
			else if (isalpha(*p) || isdigit(*p) || isspace(*p) || ispunct(*p)) {}
			break;
		}
		case EnumString::Trope:
		{
			if (*p == '0') {
				state = EnumString::Eight1;
			}
			// ת������
			else if (*p == 'r' || *p == 'n' || *p == 't' || *p == 'b' ||
				*p == '\\' || *p == '"' || *p == '\'') {
				state = EnumString::In;
			}
			else {}
			break;
		}
		case EnumString::Eight1:
		{
			if (*p >= '0' && *p <= '7') {
				state = EnumString::Eight2;
			}
			else {
				// �漼���� ������һ���ַ�
				char ch = *p;
				char ch1 = *(p + 1);
				*p = '"';
				*(p + 1) = '\0';

				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumProperties::String;
				wordTable[wordTableIndex].line = this->line;

				// �ָ���һ���ַ�
				*p = ch;
				*(p + 1) = ch1;

				// ��һ������
				wordTableIndex++;
				// ��λ��һ������
				while (!(*p == '"' && *(p - 1) != '\\')) {
					p++;
				}
				p++;
				q = p;
				return true;
			}
			break;
		}
		case EnumString::Eight2:
		{
			if (*p >= '0' && *p <= '7') {
				state = EnumString::In;
			}
			else {
				// ����ָ��
				p = q;
				return false;
			}
			break;
		}
		default:
			break;
		}
		p++;
	}
}

// func    �ַ�������ʶ�� �����Զ���
// param   
// return  
bool LexicalAnalysis::Char() {
	if (*p == '\'' && *(p + 1) != '\\') {
		// �����ַ�
		char ch2 = *(p + 2);
		char ch3 = *(p + 3);
		*(p + 2) = '\'';
		*(p + 3) = '\0';
		wordTable[wordTableIndex].word = std::string(q);
		wordTable[wordTableIndex].property = EnumProperties::CharNumber;
		wordTable[wordTableIndex].line = this->line;
		// �ָ��ַ�
		*(p + 2) = ch2;
		*(p + 3) = ch3;
		// ��һ������
		p++;
		wordTableIndex++;
		// ��λ��һ��������
		while (*p != '\'') {
			p++;
		}
		p++;
		q = p;
		return true;
	}
	else if (*p == '\'' && *(p + 1) == '\\') {
		// �����ַ�
		char ch3 = *(p + 3);
		char ch4 = *(p + 4);
		*(p + 3) = '\'';
		*(p + 4) = '\0';
		wordTable[wordTableIndex].word = std::string(q);
		wordTable[wordTableIndex].property = EnumProperties::CharNumber;
		wordTable[wordTableIndex].line = this->line;
		// �ָ��ַ�
		*(p + 3) = ch3;
		*(p + 4) = ch4;
		// ��һ������
		p++;
		wordTableIndex++;
		// ��λ��һ��������
		while (*p != '\'') {
			p++;
		}
		p++;
		q = p;
		return true;
	}
	else {
		// ����ָ��
		p = q;
		return false;
	}
}

// func    �������
// param   
// return  
bool LexicalAnalysis::OutPut() {
	for (int i = 0; i < wordTable.size(); i++) {
		if (wordTable[i].property == EnumProperties::TimeToStop) {
			wordTable.resize(i);
			break;
		}
		std::cout << wordTable[i].word << "\t\t\t\t\t" << (int)wordTable[i].property <<
			"\t\t\t\t\t" << wordTable[i].line << std::endl;
	}
	return true;
}

// func    ������ַ��ж�
// param   �ַ�ָ��
// return  
bool LexicalAnalysis::IsOperator(char ch) {
	if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '&' || ch == '|' ||
		ch == '!' || ch == '<' || ch == '>' || ch == '=' || ch == '(' || ch == ')' || ch == '[' ||
		ch == ']' || ch == '{' || ch == '}' || ch == '.' || ch == '?' || ch == ':' || ch == '~' ||
		ch == '^' || ch == ',')
		return true;
	return false;
}
