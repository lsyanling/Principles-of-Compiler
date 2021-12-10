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

// �������Ա�
enum class EnumWordProperties {
	// ��ʶ�������� ������
	TimeToStop,
	Unknow,
	Function,
	// ����������
	EightIntNumber, TenIntNumber, SixteenIntNumber, FloatNumber, FloatEeNumber, CharNumber,
	// ��ʶ��
	Identifier,
	// �ַ���������
	String,
	// ����
	Auto, Bool, Int, Float, Double, Char, Enum, Void,
	// ���ƹؼ���
	If, Else, While, For, Do, Break, Continue, Goto, Return,
	// �ֺ�
	Semicolon,
	// ���������
	OperatorAdd, OperatorSubtract, OperatorMultiply, OperatorDivide, OperatorModulo,
	// �ݱ������
	OperatorAddAdd, OperatorSubSubtract,
	// ��ֵ�����
	OperatorAssign, OperatorAddAssign, OperatorSubAssign, OperatorMulAssign, OperatorDivAssign,
	OperatorModAssign, OperatorLeftShiftAssign, OperatorRightShiftAssign,
	OperatorAndAssign, OperatorOrAssign, OperatorXOrAssign,
	// ���������
	OperatorLeftRound, OperatorRightRound,
	OperatorLeftSquare, OperatorRightSquare,
	OperatorLeftBrace, OperatorRightBrace,
	// ���������
	OperatorComma,
	// �߼������
	OperatorAnd, OperatorOr, OperatorNot,
	// ���������
	OperatorQuestion, OperatorColon,
	// λ�����
	OperatorLeftShift, OperatorRightShift, OperatorBitAnd, OperatorBitOr, OperatorBitNot, OperatorXOr,
	// ��ϵ�����
	OperatorGreaterThan, OperatorSmallerThan, OperatorGreaterEqual, OperatorSmallerEqual, OperatorEqual,
	OperatorNotEqual,
	// ��Ա�����
	OperatorPoint, OperatorArrow,
	// sizeof�����
	OperatorSizeof
};

class Exception {
public:
	std::string str;
	// �쳣λ�ô���
	Word& word;
	// �쳣���� 0Ϊ�﷨���� 1Ϊ�������
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

// ������� ������
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

// ������� ��ʱ����
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

// ������� ��ʶ��
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

// ������� ��ʶ����
class IdentifierTable {
public:
	IdentifierTable() {
		fatherTablePointer = nullptr;
	}
	IdentifierTable(IdentifierTable* f) :fatherTablePointer(f) {}

	// ����ָ��
	IdentifierTable* fatherTablePointer;
	// ��
	std::vector<Identifier> table;
	int IdentifierNumber = 0;

	int AddIdentifier(Identifier id) {
		table.push_back(id);
		IdentifierNumber++;
		return IdentifierNumber;
	}
};

// ������� ������
class FunctionIdentifier : public Identifier {
public:
	// ��������
	int parameterNumber;
	// ���������� ��������
	std::vector<EnumWordProperties> parameterType;
	// ��������
	EnumWordProperties returnType = Identifier::wordProperties;
};

// ������� ������
class FunctionIdentifierTable {
public:
};

// ������� ��ֵջ �ֶ�����ֵ
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

// ������� ��Ԫʽ
class Four {
public:
	Four(std::string o, std::string o1, std::string o2, std::string d) :op(o), op1(o1), op2(o2), dest(d) {}
	std::string op;
	std::string op1;
	std::string op2;
	std::string dest;
};

// ������� ��Ԫʽ��
class FourTable {
public:
	int AddFour(std::string o, std::string o1, std::string o2, std::string dest) {
		table.push_back(Four(o, o1, o2, dest));
		// �������û������ ���Զ���������
		// nowFourLine++;
		return nowFourLine;
	}

	// ��Ԫʽ��
	std::vector<Four> table;

	// ��������к�
	std::stack<int> writeBack;

	// ��Ԫʽ�к�
	int nowFourLine = 0;
};

// ����
class Word {
public:
	std::string word;
	EnumWordProperties property;
	// �ô���λ��Ԥ����֮��Ĵ������һ��
	// ��Ԥ����ת�ƻ���ɾȥ �޷�ȷ����Դ�ļ�����һ��
	int line;

	Word() :property(EnumWordProperties::TimeToStop), line(0) {}
	Word(std::string w, EnumWordProperties p, int l) :word(w), property(p), line(l) {}
};

// �ؼ��ֱ�
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

// Number�Զ���
enum class EnumNumber {
	Start, ZeroOrEight, Eight, Ten, TrueTen, Sixteen, TrueSixteen, FloatPoint, FloatNumber, FloatEe, FloatSign, FloatEeNumber
};

// String�Զ���
enum class EnumString {
	Start, In, Trope, Eight1, Eight2
};

// ���ر�
static std::vector<Word> wordTableSource;

class LexicalAnalysis {
public:

	// �ļ���
	std::ifstream fin;

	// Դ�ļ��ַ��� �㹻��
	char source[MAX_BUFFER];

	// ��ʶԴ�ļ�β �����ַ���β
	char* end;

	// �����ַ���
	char* code;

	// �ַ�ָ��
	char* p;
	char* q;

	// ����
	int line = 1;

	// ���ر�����
	std::vector<Word>& wordTable;
	int wordTableIndex = 0;

	// �ؼ��ֱ�
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

	// ���ر�����
	std::vector<Word>& wordTable;
	// ���ر�ָ��
	int p = 0;
	int q = 0;
	int r = 0;

	// ���ʽ��ط�����
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

	// ������ط�����
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

	// �����ط�����
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

	// �ⲿ������ط�����
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

// ��д�﷨������
class SemanticAnalysis : public GrammaticalAnalysis {
public:
	SemanticAnalysis(std::vector<Word>&);

	// ��������
	std::vector<LiteralValue> literalValueTable;
	int literalValueTablePointer = 0;

	// ��ʱ������
	int tempVariableTablePointer = 0;

	// ��ʶ����
	IdentifierTable* identifierTablePointer;

	// ��ǰ��������
	EnumWordProperties declarationTypeName;

	// ��ǰ������ʶ��
	Identifier declarationIdentifier = Identifier(EnumWordProperties::Unknow, "");

	// ����ת������
	EnumWordProperties castTypeName;

	// ��ֵջ �ֶ�����ֵ
	ReturnValueStack returnValueStack;

	// ��Ԫʽ��
	FourTable fourTable;

	// ��֧����ջ
	std::stack<int> selectBackFillStack;

	// ��������ջ
	std::stack<int> iterateBackFillStack;

	// ������ջ
	std::stack<std::vector<int>*> iterateContinueLayerStack;
	std::stack<std::vector<int>*> iterateBreakLayerStack;

	// ���ұ�ʶ��
	auto SearchIdentifier(IdentifierTable* idTablePointer, std::string value);

	// �ڵ�ǰ���в��ұ�ʶ��
	auto SearchIdentifierInCurrentTable(IdentifierTable* idTablePointer, std::string value);

	// ���ʽ��ط�����
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

	// ������ط�����
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

	// �����ط�����
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

	// �ⲿ������ط�����
	bool TranslationUnit();
	bool TranslationUnitEliminateLeft();
	bool ExternalDeclaration();
	bool FunctionDefinition();
	bool DeclarationList();
	bool DeclarationListEliminateLeft();

	// �����ж�
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

	// ����ת��
	bool ToBool();
};