#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<map>

const int MAX_BUFFER = 80000;
const int MAX_WORD_TABLE = 2500;

class Exception {
public:
	std::string str;
	int line;
	Exception(std::string e, int l) :str(e), line(l) {}
};

// 词素属性表
enum class EnumProperties {
	// 标识结束的量 无意义
	TimeToStop,
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

// 词素
class Word {
public:
	std::string word;
	EnumProperties property;
	// 该词素位于预处理之后的代码的哪一行
	// 因预处理将转移换行删去 无法确定是源文件的哪一行
	int line;

	Word() :property(EnumProperties::TimeToStop), line(0) {}
	Word(std::string w, EnumProperties p, int l) :word(w), property(p), line(l) {}
};

// 关键字表
class KeyWordTable {
public:
	std::map<std::string, EnumProperties> keyWord;
	KeyWordTable() {
		keyWord["auto"] = EnumProperties::Auto;
		keyWord["bool"] = EnumProperties::Bool;
		keyWord["break"] = EnumProperties::Break;
		keyWord["char"] = EnumProperties::Char;
		keyWord["continue"] = EnumProperties::Continue;
		keyWord["do"] = EnumProperties::Do;
		keyWord["double"] = EnumProperties::Double;
		keyWord["else"] = EnumProperties::Else;
		keyWord["enum"] = EnumProperties::Enum;
		keyWord["float"] = EnumProperties::Float;
		keyWord["for"] = EnumProperties::For;
		keyWord["goto"] = EnumProperties::Goto;
		keyWord["if"] = EnumProperties::If;
		keyWord["int"] = EnumProperties::Int;
		keyWord["return"] = EnumProperties::Return;
		keyWord["sizeof"] = EnumProperties::OperatorSizeof;
		keyWord["void"] = EnumProperties::Void;
		keyWord["while"] = EnumProperties::While;
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
private:

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

public:
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
	// 这个不打算做
	bool InitializerList();

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

	bool IsTypeName(Word w);
	bool IsNumberLiteralValue(Word w);
	bool IsLiteralValue(Word w);
};