#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include<fstream>
#include<iostream>
#include<iomanip>
#include<string>
#include<vector>
#include<map>
#include<tuple>
#include<stack>

const int MAX_BUFFER = 10000;
const int MAX_WORD_TABLE = 1000;

class Word;
class LexicalAnalysis;
class GrammaticalAnalysis;
class SemanticAnalysis;

// 词素属性表
enum class EnumWordProperties {
	// 标识结束的量 无意义
	TimeToStop,
	Unknow,
	Function,
	// 数字字面量
	EightIntNumber, TenIntNumber, SixteenIntNumber, FloatNumber, FloatEeNumber, CharNumber,
	// 标识符
	Identifier,
	// 字符串字面量
	String,
	// 类型
	Auto, Bool, Int, Float, Double, Char, Enum, Void,
	// 控制关键字
	If, Else, While, For, Do, Break, Continue, Goto, Return,
	// 分号
	Semicolon,
	// 算术运算符
	OperatorAdd, OperatorSubtract, OperatorMultiply, OperatorDivide, OperatorModulo,
	// 递变运算符
	OperatorAddAdd, OperatorSubSubtract,
	// 赋值运算符
	OperatorAssign, OperatorAddAssign, OperatorSubAssign, OperatorMulAssign, OperatorDivAssign,
	OperatorModAssign, OperatorLeftShiftAssign, OperatorRightShiftAssign,
	OperatorAndAssign, OperatorOrAssign, OperatorXOrAssign,
	// 括号运算符
	OperatorLeftRound, OperatorRightRound,
	OperatorLeftSquare, OperatorRightSquare,
	OperatorLeftBrace, OperatorRightBrace,
	// 逗号运算符
	OperatorComma,
	// 逻辑运算符
	OperatorAnd, OperatorOr, OperatorNot,
	// 条件运算符
	OperatorQuestion, OperatorColon,
	// 位运算符
	OperatorLeftShift, OperatorRightShift, OperatorBitAnd, OperatorBitOr, OperatorBitNot, OperatorXOr,
	// 关系运算符
	OperatorGreaterThan, OperatorSmallerThan, OperatorGreaterEqual, OperatorSmallerEqual, OperatorEqual,
	OperatorNotEqual,
	// 成员运算符
	OperatorPoint, OperatorArrow,
	// sizeof运算符
	OperatorSizeof
};

class Exception {
public:
	std::string str;
	// 异常位置词素
	Word& word;
	// 异常类型 0为语法错误 1为语义错误
	int exceptionType;
	Exception(std::string e, Word& w, int t) :str(e), word(w), exceptionType(t) {}
};

class Value {
public:
	Value(std::string v) :value(v) {}
	std::string value;
	virtual EnumWordProperties getWordProperties() { return EnumWordProperties::Unknow; }
	virtual void setWordProperties(EnumWordProperties e) {}
};

// 语义分析 字面量
class LiteralValue :public Value {
public:
	LiteralValue(EnumWordProperties p, std::string v) :wordProperties(p), Value(v) {}

	EnumWordProperties wordProperties;
	EnumWordProperties getWordProperties() {
		return wordProperties;
	}
	void setWordProperties(EnumWordProperties e) {
		wordProperties = e;
	}
};

// 语义分析 临时变量
class TempVariable :public Value {
public:
	TempVariable(EnumWordProperties p, std::string v) :wordProperties(p), Value(v) {}

	EnumWordProperties wordProperties;
	EnumWordProperties getWordProperties() {
		return wordProperties;
	}
	void setWordProperties(EnumWordProperties e) {
		wordProperties = e;
	}
};

// 语义分析 标识符
class Identifier :public Value {
public:
	Identifier(EnumWordProperties t, std::string n) :wordProperties(t), Value(n) {}

	EnumWordProperties wordProperties;
	EnumWordProperties getWordProperties() {
		return wordProperties;
	}
	void setWordProperties(EnumWordProperties e) {
		wordProperties = e;
	}
};

// 语义分析 标识符表
class IdentifierTable {
public:
	IdentifierTable() {
		fatherTablePointer = nullptr;
	}
	IdentifierTable(IdentifierTable* f) :fatherTablePointer(f) {}

	// 父表指针
	IdentifierTable* fatherTablePointer;
	// 表
	std::vector<Identifier> table;
	int IdentifierNumber = 0;

	int AddIdentifier(Identifier id) {
		table.push_back(id);
		IdentifierNumber++;
		return IdentifierNumber;
	}
};

// 语义分析 函数符
class FunctionIdentifier : public Identifier {
public:
	// 参数个数
	int parameterNumber;
	// 各参数类型 自右向左
	std::vector<EnumWordProperties> parameterType;
	// 返回类型
	EnumWordProperties returnType = Identifier::wordProperties;
};

// 语义分析 函数表
class FunctionIdentifierTable {
public:
};

// 语义分析 求值栈 手动返回值
class ReturnValueStack {
public:
	enum class LastReturnWordType {
		Identifier,
		LiteralValue,
		TempVariable
	};
	LastReturnWordType lastReturnWordType;
	EnumWordProperties lastReturnValueType;
	std::stack<Value*> returnValueStack;
};

// 语义分析 四元式
class Four {
public:
	Four(std::string o, std::string o1, std::string o2, std::string d) :op(o), op1(o1), op2(o2), dest(d) {}
	std::string op;
	std::string op1;
	std::string op2;
	std::string dest;
};

// 语义分析 四元式表
class FourTable {
public:
	int AddFour(std::string o, std::string o1, std::string o2, std::string dest) {
		table.push_back(Four(o, o1, o2, dest));
		// 如果回填没有问题 就自动管理行数
		// nowFourLine++;
		return nowFourLine;
	}

	// 四元式表
	std::vector<Four> table;

	// 待回填的行号
	std::stack<int> writeBack;

	// 四元式行号
	int nowFourLine = 0;
};

// 词素
class Word {
public:
	std::string word;
	EnumWordProperties property;
	// 该词素位于预处理之后的代码的哪一行
	// 因预处理将转移换行删去 无法确定是源文件的哪一行
	int line;

	Word() :property(EnumWordProperties::TimeToStop), line(0) {}
	Word(std::string w, EnumWordProperties p, int l) :word(w), property(p), line(l) {}
};

// 关键字表
class KeyWordTable {
public:
	std::map<std::string, EnumWordProperties> keyWord;
	KeyWordTable() {
		keyWord["auto"] = EnumWordProperties::Auto;
		keyWord["bool"] = EnumWordProperties::Bool;
		keyWord["break"] = EnumWordProperties::Break;
		keyWord["char"] = EnumWordProperties::Char;
		keyWord["continue"] = EnumWordProperties::Continue;
		keyWord["do"] = EnumWordProperties::Do;
		keyWord["double"] = EnumWordProperties::Double;
		keyWord["else"] = EnumWordProperties::Else;
		keyWord["enum"] = EnumWordProperties::Enum;
		keyWord["float"] = EnumWordProperties::Float;
		keyWord["for"] = EnumWordProperties::For;
		keyWord["goto"] = EnumWordProperties::Goto;
		keyWord["if"] = EnumWordProperties::If;
		keyWord["int"] = EnumWordProperties::Int;
		keyWord["return"] = EnumWordProperties::Return;
		keyWord["sizeof"] = EnumWordProperties::OperatorSizeof;
		keyWord["void"] = EnumWordProperties::Void;
		keyWord["while"] = EnumWordProperties::While;
	}
};

// Number自动机
enum class EnumNumber {
	Start, ZeroOrEight, Eight, Ten, TrueTen, Sixteen, TrueSixteen, FloatPoint, FloatNumber, FloatEe, FloatSign, FloatEeNumber
};

// String自动机
enum class EnumString {
	Start, In, Trope, Eight1, Eight2
};

// 词素表
static std::vector<Word> wordTableSource;

class LexicalAnalysis {
public:

	// 文件流
	std::ifstream fin;

	// 源文件字符串 足够大
	char source[MAX_BUFFER];

	// 标识源文件尾 代码字符串尾
	char* end;

	// 代码字符串
	char* code;

	// 字符指针
	char* p;
	char* q;

	// 行数
	int line = 1;

	// 词素表引用
	std::vector<Word>& wordTable;
	int wordTableIndex = 0;

	// 关键字表
	KeyWordTable keyWord;

	LexicalAnalysis(char* codeFileString, std::vector<Word>& wordTableSource);

	void Preprocess();
	bool CreateCode();
	bool Analyse();

	bool Number();
	bool Semicolon();
	bool Operator();
	bool Space();
	bool KeyWord();
	bool Identifier();
	bool String();
	bool Char();
	bool OutPut();

	bool IsOperator(char);

	bool ReSetType();
};

class GrammaticalAnalysis {
public:
	GrammaticalAnalysis(std::vector<Word>&);

	// 词素表引用
	std::vector<Word>& wordTable;
	// 词素表指针
	int p = 0;
	int q = 0;
	int r = 0;

	// 表达式相关分析器
	virtual bool PrimaryExpression();
	virtual bool PostfixExpression();
	virtual bool PostfixExpressionEliminateLeft();
	virtual bool ArgumentExpressionList();
	virtual bool ArgumentExpressionListEliminateLeft();
	virtual bool UnaryExpression();
	virtual bool UnaryOperator();
	virtual bool CastExpression();
	virtual bool MultiplicativeExpression();
	virtual bool MultiplicativeExpressionEliminateLeft();
	virtual bool AdditiveExpression();
	virtual bool AdditiveExpressionEliminateLeft();
	virtual bool ShiftExpression();
	virtual bool ShiftExpressionEliminateLeft();
	virtual bool RelationalExpression();
	virtual bool RelationalExpressionEliminateLeft();
	virtual bool EqualityExpression();
	virtual bool EqualityExpressionEliminateLeft();
	virtual bool BitAndExpression();
	virtual bool BitAndExpressionEliminateLeft();
	virtual bool BitXOrExpression();
	virtual bool BitXOrExpressionEliminateLeft();
	virtual bool BitOrExpression();
	virtual bool BitOrExpressionEliminateLeft();
	virtual bool AndExpression();
	virtual bool AndExpressionEliminateLeft();
	virtual bool OrExpression();
	virtual bool OrExpressionEliminateLeft();
	virtual bool ConditionalExpression();
	virtual bool AssignmentExpression();
	virtual bool AssignmentOperator();
	virtual bool Expression();
	virtual bool ExpressionEliminateLeft();

	// 声明相关分析器
	virtual bool Declaration();
	virtual bool InitDeclaratorList();
	virtual bool InitDeclaratorListEliminateLeft();
	virtual bool InitDeclarator();
	virtual bool DirectDeclarator();
	virtual bool DirectDeclaratorEliminateLeft();
	virtual	bool ParameterList();
	virtual	bool ParameterListEliminateLeft();
	virtual bool ParameterDeclaration();
	virtual	bool IdentifierList();
	virtual	bool IdentifierListEliminateLeft();

	// 语句相关分析器
	virtual bool Statement();
	virtual bool LabeledStatement();
	virtual	bool CompoundStatement();
	virtual	bool BlockItemList();
	virtual bool BlockItemListEliminateLeft();
	virtual	bool BlockItem();
	virtual	bool ExpressionStatement();
	virtual	bool SelectionStatement();
	virtual	bool IterationStatement();
	virtual	bool JumpStatement();

	// 外部定义相关分析器
	virtual bool TranslationUnit();
	virtual bool TranslationUnitEliminateLeft();
	virtual bool ExternalDeclaration();
	virtual bool FunctionDefinition();
	virtual bool DeclarationList();
	virtual bool DeclarationListEliminateLeft();

	bool IsTypeName(Word w);
	bool IsNumberLiteralValue(Word w);
	bool IsLiteralValue(Word w);
};

// 重写语法分析类
class SemanticAnalysis : public GrammaticalAnalysis {
public:
	SemanticAnalysis(std::vector<Word>&);

	// 字面量表
	std::vector<LiteralValue> literalValueTable;
	int literalValueTablePointer = 0;

	// 临时变量号
	int tempVariableTablePointer = 0;

	// 标识符表
	IdentifierTable* identifierTablePointer;

	// 当前声明类型
	EnumWordProperties declarationTypeName;

	// 当前声明标识符
	Identifier declarationIdentifier = Identifier(EnumWordProperties::Unknow, "");

	// 类型转换类型
	EnumWordProperties castTypeName;

	// 求值栈 手动返回值
	ReturnValueStack returnValueStack;

	// 四元式表
	FourTable fourTable;

	// 分支回填栈
	std::stack<int> selectBackFillStack;

	// 迭代回填栈
	std::stack<int> iterateBackFillStack;

	// 迭代层栈
	std::stack<std::vector<int>*> iterateContinueLayerStack;
	std::stack<std::vector<int>*> iterateBreakLayerStack;

	// 查找标识符
	auto SearchIdentifier(IdentifierTable* idTablePointer, std::string value);

	// 在当前表中查找标识符
	auto SearchIdentifierInCurrentTable(IdentifierTable* idTablePointer, std::string value);

	// 表达式相关分析器
	bool PrimaryExpression();
	bool PostfixExpression();
	bool PostfixExpressionEliminateLeft();
	bool ArgumentExpressionList();
	bool ArgumentExpressionListEliminateLeft();
	bool UnaryExpression();
	bool UnaryOperator();
	bool CastExpression();
	bool MultiplicativeExpression();
	bool MultiplicativeExpressionEliminateLeft();
	bool AdditiveExpression();
	bool AdditiveExpressionEliminateLeft();
	bool ShiftExpression();
	bool ShiftExpressionEliminateLeft();
	bool RelationalExpression();
	bool RelationalExpressionEliminateLeft();
	bool EqualityExpression();
	bool EqualityExpressionEliminateLeft();
	bool BitAndExpression();
	bool BitAndExpressionEliminateLeft();
	bool BitXOrExpression();
	bool BitXOrExpressionEliminateLeft();
	bool BitOrExpression();
	bool BitOrExpressionEliminateLeft();
	bool AndExpression();
	bool AndExpressionEliminateLeft();
	bool OrExpression();
	bool OrExpressionEliminateLeft();
	bool ConditionalExpression();
	bool AssignmentExpression();
	bool AssignmentOperator();
	bool Expression();
	bool ExpressionEliminateLeft();

	// 声明相关分析器
	bool Declaration();
	bool InitDeclaratorList();
	bool InitDeclaratorListEliminateLeft();
	bool InitDeclarator();
	bool DirectDeclarator();
	bool DirectDeclaratorEliminateLeft();
	bool ParameterList();
	bool ParameterListEliminateLeft();
	bool ParameterDeclaration();
	bool IdentifierList();
	bool IdentifierListEliminateLeft();

	// 语句相关分析器
	bool Statement();
	bool LabeledStatement();
	bool CompoundStatement();
	bool BlockItemList();
	bool BlockItemListEliminateLeft();
	bool BlockItem();
	bool ExpressionStatement();
	bool SelectionStatement();
	bool IterationStatement();
	bool JumpStatement();

	// 外部定义相关分析器
	bool TranslationUnit();
	bool TranslationUnitEliminateLeft();
	bool ExternalDeclaration();
	bool FunctionDefinition();
	bool DeclarationList();
	bool DeclarationListEliminateLeft();

	// 类型判断
	bool IsInt(EnumWordProperties e) {
		if (e == EnumWordProperties::EightIntNumber ||
			e == EnumWordProperties::TenIntNumber ||
			e == EnumWordProperties::SixteenIntNumber ||
			e == EnumWordProperties::CharNumber ||
			e == EnumWordProperties::Int ||
			e == EnumWordProperties::Char) {
			return true;
		}
		else {
			return false;
		}
	}

	bool IsTrueInt(EnumWordProperties e) {
		if (e == EnumWordProperties::EightIntNumber ||
			e == EnumWordProperties::TenIntNumber ||
			e == EnumWordProperties::SixteenIntNumber ||
			e == EnumWordProperties::Int) {
			return true;
		}
		else {
			return false;
		}
	}

	bool IsChar(EnumWordProperties e) {
		if (e == EnumWordProperties::CharNumber ||
			e == EnumWordProperties::Char) {
			return true;
		}
		else {
			return false;
		}
	}

	bool IsFloat(EnumWordProperties e) {
		if (e == EnumWordProperties::Float ||
			e == EnumWordProperties::FloatEeNumber ||
			e == EnumWordProperties::FloatNumber) {
			return true;
		}
		else {
			return false;
		}
	}

	// 类型转换
	bool ToBool();
};