#include"head.h"

// 词法分析

// func    词法分析构造函数
// param   源文件路径
// return  
LexicalAnalysis::LexicalAnalysis(char* codeFileString, std::vector<Word>& wordTableSource) :wordTable(wordTableSource) {

	// 标记缓冲区尾
	end = source + MAX_BUFFER;

	// 打开源文件
	fin.open(codeFileString, std::ios::in | std::ios::binary);

	// 读取源文件
	fin.read(source, end - source);

	// 源代码预处理
	Preprocess();

	// 词法分析
	if (Analyse()) {
		// 输出词素
		OutPut();
	}
}

// func    源代码预处理 将source转换为code 包括删除转移换行 删除源文件连续空白字符
// param   
// return  
void LexicalAnalysis::Preprocess() {

	// 代码字符串
	code = new char[strlen(source) + 1];

	// 标记源文件尾
	end = source + strlen(source) + 1;

	// TODO 
	// 源字符映射 这里不做
	;

	// 转移换行 删除\后接换行符的实例
	// 删除多余的空白字符 生成code字符串
	CreateCode();

	// TODO
	// 删除注释 这里不做

	// TODO
	// 连续字符串连接 这里不做

	// 标记代码字符串尾
	end = code + strlen(code) + 1;

	// 预置指针
	p = code;

	return;
}

// func    生成代码字符串
// param
// return  
bool LexicalAnalysis::CreateCode() {

	// 借用type alpha不表示实际含义
	bool inString = false;

	int i = 0, n = 0;
	for (; source + i + 2 <= end; i++) {

		// 代码字符串中\\\r\n表示一个转移换行实例
		if (source[i] == '\\' && source[i + 1] == '\r' && source[i + 2] == '\n') {
			i += 2;
			continue;
		}
		if (!inString) {

			// 字符串外的\t和连续空格均为空白实例
			if (source[i] == '\t') {
				continue;
			}
			if (source[i] == ' ' && source[i + 1] == ' ') {
				continue;
			}

			// 字符串始
			if (source[i] == '"') {
				inString = true;
				code[n++] = source[i];
				continue;
			}
		}
		if (inString) {

			// 字符串尾
			if (source[i] == '"') {
				inString = false;
				code[n++] = source[i];
				continue;
			}
			code[n++] = source[i];
			continue;
		}
		// 普通字符
		code[n++] = source[i];
	}

	code[n] = 0;

	return true;
}

// func    词法分析
// param   
// return  
bool LexicalAnalysis::Analyse() {

	while (p + 1 < end) {

		// 保存指针
		q = p;

		// 调用各有穷状态自动机 由短路效果保证只提取一次词素 需要先判断关键字后判断标识符
		if (Number() || Semicolon() || Operator() || Space() ||
			KeyWord() || Identifier() || String() || Char()) {
			wordTable;
			line;
			continue;
		}
		std::cout << "Lexical Error in line " << line << ": Can't identify the sign" << std::endl;
		break;
	}
	return true;
}

// func    数字字面量识别 自动机
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
				// 重置指针
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

				// 奇技淫巧 保存该字符
				char ch = *p;
				*p = '\0';
				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumWordProperties::TenIntNumber;
				wordTable[wordTableIndex].line = this->line;
				// 恢复该字符
				*p = ch;
				// 下一个词素
				wordTableIndex++;
				return true;
			}
			else {
				// 重置指针
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

				// 奇技淫巧 保存该字符
				char ch = *p;
				*p = '\0';
				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumWordProperties::EightIntNumber;
				wordTable[wordTableIndex].line = this->line;
				// 恢复该字符
				*p = ch;
				// 下一个词素
				wordTableIndex++;
				return true;
			}
			else {
				// 重置指针
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

				// 奇技淫巧 保存该字符
				char ch = *p;
				*p = '\0';
				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumWordProperties::TenIntNumber;
				wordTable[wordTableIndex].line = this->line;
				// 恢复该字符
				*p = ch;
				// 下一个词素
				wordTableIndex++;
				return true;
			}
			else {
				// 重置指针
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
				// 重置指针
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

				// 奇技淫巧 保存该字符
				char ch = *p;
				*p = '\0';
				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumWordProperties::SixteenIntNumber;
				wordTable[wordTableIndex].line = this->line;
				// 恢复该字符
				*p = ch;
				// 下一个词素
				wordTableIndex++;
				return true;
			}
			else {
				// 重置指针
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
			else if (isspace(*p) || *p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '%' ||
				*p == '&' || *p == '|' || *p == '!' || *p == '<' || *p == '>' || *p == '=' ||
				*p == ')' || *p == ']' || *p == '?' || *p == ':' || *p == '^' || *p == ',' ||
				*p == ';') {

				// 奇技淫巧 保存下一个字符
				char ch = *(p+1);
				*(p+1) = '\0';
				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumWordProperties::SixteenIntNumber;
				wordTable[wordTableIndex].line = this->line;
				// 恢复下一个字符
				*(p+1) = ch;
				// 下一个词素
				wordTableIndex++;
				return true;
			}
			else {
				// 重置指针
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

				// 奇技淫巧 保存该字符
				char ch = *p;
				*p = '\0';
				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumWordProperties::FloatNumber;
				wordTable[wordTableIndex].line = this->line;
				// 恢复该字符
				*p = ch;
				// 下一个词素
				wordTableIndex++;
				return true;
			}
			else {
				// 重置指针
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
				// 重置指针
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
				// 重置指针
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

				// 奇技淫巧 保存该字符
				char ch = *p;
				*p = '\0';
				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumWordProperties::FloatNumber;
				wordTable[wordTableIndex].line = this->line;
				// 恢复该字符
				*p = ch;
				// 下一个词素
				wordTableIndex++;
				return true;
			}
			else {
				// 重置指针
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

// func    分号识别 不是自动机
// param   
// return  
bool LexicalAnalysis::Semicolon() {
	if (*p == ';') {
		// 奇技淫巧 保存下一个字符
		char ch = *(p + 1);
		*(p + 1) = '\0';
		wordTable[wordTableIndex].word = std::string(q);
		wordTable[wordTableIndex].property = EnumWordProperties::Semicolon;
		wordTable[wordTableIndex].line = this->line;
		// 恢复下一个字符
		*(p + 1) = ch;
		// 下一个词素
		p++;
		wordTableIndex++;
		return true;
	}
	else {
		// 重置指针
		p = q;
		return false;
	}
}

// func    运算符识别 不是自动机
// param   
// return  
bool LexicalAnalysis::Operator() {
	// 是运算符
	if (IsOperator(*p)) {
		EnumWordProperties e = EnumWordProperties::Auto;
		int length = 3;
		// 三个字符长的运算符
		// 左移赋值
		if (*p == '<' && *(p + 1) == '<' && *(p + 2) == '=') {
			e = EnumWordProperties::OperatorLeftShiftAssign;
		}
		// 右移赋值
		else if (*p == '>' && *(p + 1) == '>' && *(p + 2) == '=') {
			e = EnumWordProperties::OperatorRightShiftAssign;
		}

		// 两个字符长的运算符
		// 递增运算符
		else if (*p == '+' && *(p + 1) == '+') {
			e = EnumWordProperties::OperatorAddAdd;
			length = 2;
		}
		// 递减运算符
		else if (*p == '-' && *(p + 1) == '-') {
			e = EnumWordProperties::OperatorSubSubtract;
			length = 2;
		}
		// 相等运算符
		else if (*p == '=' && *(p + 1) == '=') {
			e = EnumWordProperties::OperatorEqual;
			length = 2;
		}
		// 大于等于
		else if (*p == '>' && *(p + 1) == '=') {
			e = EnumWordProperties::OperatorGreaterEqual;
			length = 2;
		}
		// 小于等于
		else if (*p == '<' && *(p + 1) == '=') {
			e = EnumWordProperties::OperatorSmallerEqual;
			length = 2;
		}
		// 不等
		else if (*p == '!' && *(p + 1) == '=') {
			e = EnumWordProperties::OperatorNotEqual;
			length = 2;
		}
		// 左移
		else if (*p == '<' && *(p + 1) == '<') {
			e = EnumWordProperties::OperatorLeftShift;
			length = 2;
		}
		// 右移
		else if (*p == '>' && *(p + 1) == '>') {
			e = EnumWordProperties::OperatorRightShift;
			length = 2;
		}
		// 算术赋值
		else if (*p == '+' && *(p + 1) == '=') {
			e = EnumWordProperties::OperatorAddAssign;
			length = 2;
		}
		else if (*p == '-' && *(p + 1) == '=') {
			e = EnumWordProperties::OperatorSubAssign;
			length = 2;
		}
		else if (*p == '*' && *(p + 1) == '=') {
			e = EnumWordProperties::OperatorMulAssign;
			length = 2;
		}
		else if (*p == '/' && *(p + 1) == '=') {
			e = EnumWordProperties::OperatorDivAssign;
			length = 2;
		}
		else if (*p == '%' && *(p + 1) == '=') {
			e = EnumWordProperties::OperatorModAssign;
			length = 2;
		}
		// 按位与赋值
		else if (*p == '&' && *(p + 1) == '=') {
			e = EnumWordProperties::OperatorAndAssign;
			length = 2;
		}
		// 按位或赋值
		else if (*p == '|' && *(p + 1) == '=') {
			e = EnumWordProperties::OperatorAssign;
			length = 2;
		}
		// 按位异或赋值
		else if (*p == '^' && *(p + 1) == '=') {
			e = EnumWordProperties::OperatorXOrAssign;
			length = 2;
		}
		// 逻辑与
		else if (*p == '&' && *(p + 1) == '&') {
			e = EnumWordProperties::OperatorAnd;
			length = 2;
		}
		// 逻辑或
		else if (*p == '|' && *(p + 1) == '|') {
			e = EnumWordProperties::OperatorOr;
			length = 2;
		}
		// 箭头运算符
		else if (*p == '-' && *(p + 1) == '>') {
			e = EnumWordProperties::OperatorArrow;
			length = 2;
		}

		// 一个字符长的运算符
		// 算术运算符
		else if (*p == '+') {
			e = EnumWordProperties::OperatorAdd;
			length = 1;
		}
		else if (*p == '-') {
			e = EnumWordProperties::OperatorSubtract;
			length = 1;
		}
		else if (*p == '*') {
			e = EnumWordProperties::OperatorMultiply;
			length = 1;
		}
		else if (*p == '/') {
			e = EnumWordProperties::OperatorDivide;
			length = 1;
		}
		else if (*p == '%') {
			e = EnumWordProperties::OperatorModulo;
			length = 1;
		}
		// 成员运算符
		else if (*p == '.') {
			e = EnumWordProperties::OperatorPoint;
			length = 1;
		}
		// 括号运算符
		else if (*p == '(') {
			e = EnumWordProperties::OperatorLeftRound;
			length = 1;
		}
		else if (*p == ')') {
			e = EnumWordProperties::OperatorRightRound;
			length = 1;
		}
		else if (*p == '[') {
			e = EnumWordProperties::OperatorLeftSquare;
			length = 1;
		}
		else if (*p == ']') {
			e = EnumWordProperties::OperatorRightSquare;
			length = 1;
		}
		else if (*p == '{') {
			e = EnumWordProperties::OperatorLeftBrace;
			length = 1;
		}
		else if (*p == '}') {
			e = EnumWordProperties::OperatorRightBrace;
			length = 1;
		}
		// 条件运算符
		else if (*p == '?') {
			e = EnumWordProperties::OperatorQuestion;
			length = 1;
		}
		else if (*p == ':') {
			e = EnumWordProperties::OperatorColon;
			length = 1;
		}
		// 逻辑非
		else if (*p == '!') {
			e = EnumWordProperties::OperatorNot;
			length = 1;
		}
		// 逗号
		else if (*p == ',') {
			e = EnumWordProperties::OperatorComma;
			length = 1;
		}
		// 大于
		else if (*p == '>') {
			e = EnumWordProperties::OperatorGreaterThan;
			length = 1;
		}
		// 小于
		else if (*p == '<') {
			e = EnumWordProperties::OperatorSmallerThan;
			length = 1;
		}
		// 赋值
		else if (*p == '=') {
			e = EnumWordProperties::OperatorAssign;
			length = 1;
		}
		// 按位取反
		else if (*p == '~') {
			e = EnumWordProperties::OperatorBitNot;
			length = 1;
		}
		// 按位与
		else if (*p == '&') {
			e = EnumWordProperties::OperatorBitAnd;
			length = 1;
		}
		// 按位或
		else if (*p == '|') {
			e = EnumWordProperties::OperatorBitOr;
			length = 1;
		}
		// 按位异或
		else if (*p == '^') {
			e = EnumWordProperties::OperatorXOr;
			length = 1;
		}

		// 奇技淫巧 保存下一个字符
		char ch = *(p + length);
		*(p + length) = '\0';
		wordTable[wordTableIndex].word = std::string(q);
		wordTable[wordTableIndex].property = e;
		wordTable[wordTableIndex].line = this->line;
		// 恢复下一个字符
		*(p + length) = ch;
		// 下一个词素
		p += length;
		wordTableIndex++;
		return true;
	}
	else {
		// 重置指针
		p = q;
		return false;
	}
}

// func    空白识别 计算行数 不是自动机
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
		// 重置指针
		p = q;
		return false;
	}
}

// func    关键字识别 不是自动机 因为关键字里没有数字和下划线 简化
// param   
// return  
bool LexicalAnalysis::KeyWord() {
	if (isalpha(*p)) {
		while (isalpha(*p)) {
			p++;
		}
		// 关键字之后必是空白 分号 左圆括号 左花括号
		if (!isspace(*p) && *p != ';' && *p != '(' && *p != '{') {
			// 重置指针
			p = q;
			return false;
		}
		// 奇技淫巧 保存该字符
		char ch = *p;
		*p = '\0';

		auto iter = keyWord.keyWord.find(q);
		// 是关键字
		if (iter != keyWord.keyWord.end()) {
			wordTable[wordTableIndex].word = std::string(q);
			wordTable[wordTableIndex].property = (*iter).second;
			wordTable[wordTableIndex].line = this->line;
			// 恢复该字符
			*p = ch;
			// 下一个词素
			wordTableIndex++;
			return true;
		}
		else {
			// 恢复该字符
			*p = ch;
			// 重置指针
			p = q;
			return false;
		}
	}
}

// func    标识符识别 自动机
// param   
// return  
bool LexicalAnalysis::Identifier() {
	if (isalpha(*p) || *p == '_') {
		int autoMachine = 0;
		while (isalpha(*p) || isdigit(*p) || *p == '_') {
			p++;
		}

		// 无法识别的标记
		if (*p == '\\') {
			// 重置指针
			p = q;
			return false;
		}

		// 奇技淫巧 保存该字符
		char ch = *p;
		*p = '\0';
		wordTable[wordTableIndex].word = std::string(q);
		wordTable[wordTableIndex].property = EnumWordProperties::Identifier;
		wordTable[wordTableIndex].line = this->line;
		// 恢复该字符
		*p = ch;
		// 下一个词素
		wordTableIndex++;
		return true;
	}
	else {
		// 重置指针
		p = q;
		return false;
	}
}

// func    字符串字面量识别 自动机
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
				// 重置指针
				p = q;
				return false;
			}
			break;
		}
		case EnumString::In:
		{
			if (*p == '\0') {
				// 奇技淫巧 保存下一个字符
				char ch = *(p + 1);
				*p = '"';
				*(p + 1) = '\0';

				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumWordProperties::String;
				wordTable[wordTableIndex].line = this->line;

				// 恢复下一个字符
				*p = '\0';
				*(p + 1) = ch;

				// 下一个词素
				wordTableIndex++;
				// 定位下一个引号
				while (!(*p == '"' && *(p - 1) != '\\')) {
					p++;
				}
				p++;
				q = p;
				return true;
			}
			else if (*p == '"') {
				// 奇技淫巧 保存下一个字符
				char ch = *(p + 1);
				*(p + 1) = '\0';
				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumWordProperties::String;
				wordTable[wordTableIndex].line = this->line;
				// 恢复下一个字符
				*(p + 1) = ch;
				p++;
				// 下一个词素
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
			// 转义序列
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
				// 奇技淫巧 保存下一个字符
				char ch = *p;
				char ch1 = *(p + 1);
				*p = '"';
				*(p + 1) = '\0';

				wordTable[wordTableIndex].word = std::string(q);
				wordTable[wordTableIndex].property = EnumWordProperties::String;
				wordTable[wordTableIndex].line = this->line;

				// 恢复下一个字符
				*p = ch;
				*(p + 1) = ch1;

				// 下一个词素
				wordTableIndex++;
				// 定位下一个引号
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
				// 重置指针
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

// func    字符字面量识别 不是自动机
// param   
// return  
bool LexicalAnalysis::Char() {
	if (*p == '\'' && *(p + 1) != '\\') {
		// 保存字符
		char ch2 = *(p + 2);
		char ch3 = *(p + 3);
		*(p + 2) = '\'';
		*(p + 3) = '\0';
		wordTable[wordTableIndex].word = std::string(q);
		wordTable[wordTableIndex].property = EnumWordProperties::CharNumber;
		wordTable[wordTableIndex].line = this->line;
		// 恢复字符
		*(p + 2) = ch2;
		*(p + 3) = ch3;
		// 下一个词素
		p++;
		wordTableIndex++;
		// 定位下一个单引号
		while (*p != '\'') {
			p++;
		}
		p++;
		q = p;
		return true;
	}
	else if (*p == '\'' && *(p + 1) == '\\') {
		// 保存字符
		char ch3 = *(p + 3);
		char ch4 = *(p + 4);
		*(p + 3) = '\'';
		*(p + 4) = '\0';
		wordTable[wordTableIndex].word = std::string(q);
		wordTable[wordTableIndex].property = EnumWordProperties::CharNumber;
		wordTable[wordTableIndex].line = this->line;
		// 恢复字符
		*(p + 3) = ch3;
		*(p + 4) = ch4;
		// 下一个词素
		p++;
		wordTableIndex++;
		// 定位下一个单引号
		while (*p != '\'') {
			p++;
		}
		p++;
		q = p;
		return true;
	}
	else {
		// 重置指针
		p = q;
		return false;
	}
}

// func    输出词素
// param   
// return  
bool LexicalAnalysis::OutPut() {
	for (int i = 0; i < wordTable.size(); i++) {
		if (wordTable[i].property == EnumWordProperties::TimeToStop) {
			wordTable.resize(i + 1);
			break;
		}
		std::cout.setf(std::ios::left);
		std::cout << "\t" << std::setw(16) << wordTable[i].word << std::setw(12) <<
			(int)wordTable[i].property << std::setw(12) << wordTable[i].line << std::endl;
	}
	std::cout << std::endl;
	return true;
}

// func    运算符字符判断
// param   字符指针
// return  
bool LexicalAnalysis::IsOperator(char ch) {
	if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '&' || ch == '|' ||
		ch == '!' || ch == '<' || ch == '>' || ch == '=' || ch == '(' || ch == ')' || ch == '[' ||
		ch == ']' || ch == '{' || ch == '}' || ch == '.' || ch == '?' || ch == ':' || ch == '~' ||
		ch == '^' || ch == ',')
		return true;
	return false;
}

bool LexicalAnalysis::ReSetType() {
	for (auto& i : wordTable) {
		if (i.property == EnumWordProperties::EightIntNumber ||
			i.property == EnumWordProperties::SixteenIntNumber ||
			i.property == EnumWordProperties::TenIntNumber) {
			i.property = EnumWordProperties::TenIntNumber;
		}
		if (i.property == EnumWordProperties::FloatEeNumber ||
			i.property == EnumWordProperties::FloatNumber) {
			i.property = EnumWordProperties::FloatNumber;
		}
	}
	return true;
}