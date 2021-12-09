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

// �������Ա�
enum class EnumProperties {
	// ��ʶ�������� ������
	TimeToStop,
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

// ����
class Word {
public:
	std::string word;
	EnumProperties property;
	// �ô���λ��Ԥ����֮��Ĵ������һ��
	// ��Ԥ����ת�ƻ���ɾȥ �޷�ȷ����Դ�ļ�����һ��
	int line;

	Word() :property(EnumProperties::TimeToStop), line(0) {}
	Word(std::string w, EnumProperties p, int l) :word(w), property(p), line(l) {}
};

// �ؼ��ֱ�
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
private:

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

	// ���ر�����
	std::vector<Word>& wordTable;
	// ���ر�ָ��
	int p = 0;
	int q = 0;
	int r = 0;

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
	// �����������
	bool InitializerList();

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

	bool IsTypeName(Word w);
	bool IsNumberLiteralValue(Word w);
	bool IsLiteralValue(Word w);
};