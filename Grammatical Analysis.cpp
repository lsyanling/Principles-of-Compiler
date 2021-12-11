#include"head.h"

GrammaticalAnalysis::GrammaticalAnalysis(std::vector<Word>& w) :wordTable(w) {
	//try {
	//	TranslationUnit();
	//}
	//catch (Exception e) {
	//	std::cout << "Grammatical Error in line " << e.word.line << " " << e.str << std::endl;
	//}
}

// func    ��ʼ���ʽʶ��
// param   
// return  
bool GrammaticalAnalysis::PrimaryExpression() {
	// ���浱ǰָ��
	int nowP = p;

	// Identifier
	if (wordTable[p].property == EnumWordProperties::Identifier ||
		// ����������
		wordTable[p].property == EnumWordProperties::TenIntNumber ||
		wordTable[p].property == EnumWordProperties::EightIntNumber ||
		wordTable[p].property == EnumWordProperties::SixteenIntNumber ||
		wordTable[p].property == EnumWordProperties::FloatNumber ||
		wordTable[p].property == EnumWordProperties::FloatEeNumber ||
		wordTable[p].property == EnumWordProperties::CharNumber) {
		p++;
		return true;
	}
	// (Expression)
	else if (wordTable[p].property == EnumWordProperties::OperatorLeftRound) {
		p++;
		if (Expression()) {
			if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
				p++;
				return true;
			}
			throw(Exception("Not a Primary Expression because Expect a )", wordTable[p], 0));
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ��׺���ʽʶ��
// param   
// return  
bool GrammaticalAnalysis::PostfixExpression() {
	// ���浱ǰָ��
	int nowP = p;

	// PrimaryExpression PostfixExpressionEliminateLeft
	if (PrimaryExpression()) {
		if (PostfixExpressionEliminateLeft()) {
			return true;
		}
		else {
			p = nowP;
			return false;
		}
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ��׺���ʽ������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::PostfixExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// [Expression] PostfixExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorLeftSquare) {
		p++;
		if (Expression()) {
			if (wordTable[p].property == EnumWordProperties::OperatorRightSquare) {
				p++;
				if (PostfixExpressionEliminateLeft()) {
					return true;
				}
			}
			throw(Exception("Not [] Postfix Expression because Expect a ]", wordTable[p], 0));
		}
		throw(Exception("Not a [] Postfix Expression because Expect a Expression", wordTable[p], 0));
	}
	// (ArgumentExpressionList) PostfixExpressionEliminateLeft
	else if (wordTable[p].property == EnumWordProperties::OperatorLeftRound) {
		p++;
		if (ArgumentExpressionList()) {
			if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
				p++;
				if (PostfixExpressionEliminateLeft()) {
					return true;
				}
			}
			throw(Exception("Not a Postfix Expression because Expect a )", wordTable[p], 0));
		}
		p = nowP;
		return false;
	}
	// .Identifier PostfixExpressionEliminateLeft
	else if (wordTable[p].property == EnumWordProperties::OperatorPoint) {
		p++;
		if (wordTable[p].property == EnumWordProperties::Identifier) {
			p++;
			if (PostfixExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a -> Postfix Expression because Expect a Identifier", wordTable[p], 0));
	}
	// ->Identifier PostfixExpressionEliminateLeft
	else if (wordTable[p].property == EnumWordProperties::OperatorArrow) {
		p++;
		if (wordTable[p].property == EnumWordProperties::Identifier) {
			p++;
			if (PostfixExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a -> Postfix Expression because Expect a Identifier", wordTable[p], 0));
	}
	// ++PostfixExpressionEliminateLeft
	else if (wordTable[p].property == EnumWordProperties::OperatorAddAdd) {
		p++;
		if (PostfixExpressionEliminateLeft()) {
			return true;
		}
		throw(Exception("Not a ++ Postfix Expression because Expect a Postfix Expression", wordTable[p], 0));
	}
	// --PostfixExpressionEliminateLeft
	else if (wordTable[p].property == EnumWordProperties::OperatorSubSubtract) {
		p++;
		if (PostfixExpressionEliminateLeft()) {
			return true;
		}
		throw(Exception("Not a -- Postfix Expression because Expect a Postfix Expression", wordTable[p], 0));
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    �������ʽ�б�ʶ��
// param   
// return  
bool GrammaticalAnalysis::ArgumentExpressionList() {
	// ���浱ǰָ��
	int nowP = p;

	// AssignmentExpression ArgumentExpressionListEliminateLeft
	if (AssignmentExpression()) {
		if (ArgumentExpressionListEliminateLeft()) {
			return true;
		}
		else {
			p = nowP;
			return false;
		}
	}
	else {
		p = nowP;
		return false;
	}
}

// func    �������ʽ�б�������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::ArgumentExpressionListEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// ,AssignmentExpression ArgumentExpressionListEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorComma) {
		p++;
		if (AssignmentExpression()) {
			if (ArgumentExpressionListEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a Argument Expression List because Expect a Assignment Expression", wordTable[p], 0));
		p = nowP;
		return false;
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    ��Ŀ���ʽʶ��
// param   
// return  
bool GrammaticalAnalysis::UnaryExpression() {
	// ���浱ǰָ��
	int nowP = p;

	// PostfixExpression
	if (PostfixExpression()) {
		return true;
	}
	// ++UnaryExpression
	else if (wordTable[p].property == EnumWordProperties::OperatorAddAdd) {
		p++;
		if (UnaryExpression()) {
			return true;
		}
		throw(Exception("Not a ++ Unary Expression because Expect a Unary Expression", wordTable[p], 0));
		p = nowP;
		return false;
	}
	// --UnaryExpression
	else if (wordTable[p].property == EnumWordProperties::OperatorSubSubtract) {
		p++;
		if (UnaryExpression()) {
			return true;
		}
		throw(Exception("Not a -- Unary Expression because Expect a Unary Expression", wordTable[p], 0));
		p = nowP;
		return false;
	}
	// UnaryOperator CastExpression
	else if (UnaryOperator()) {
		if (CastExpression()) {
			return true;
		}
		p = nowP;
		return false;
	}
	// sizeof
	else if (wordTable[p].property == EnumWordProperties::OperatorSizeof) {
		p++;
		// sizeof UnaryExpression
		if (UnaryExpression()) {
			return true;
		}
		// sizeof (TypeName)
		if (wordTable[p].property == EnumWordProperties::OperatorLeftRound) {
			p++;
			if (IsTypeName(wordTable[p])) {
				p++;
				if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
					p++;
					return true;
				}
				throw(Exception("Not a sizeof Expression because Expect a (", wordTable[p], 0));
			}
			throw(Exception("Not a sizeof Expression because Expect a (", wordTable[p], 0));
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ��Ŀ�����ʶ��
// param   
// return  
bool GrammaticalAnalysis::UnaryOperator() {
	// ���浱ǰָ��
	int nowP = p;

	if (wordTable[p].property == EnumWordProperties::OperatorMultiply ||
		wordTable[p].property == EnumWordProperties::OperatorAdd ||
		wordTable[p].property == EnumWordProperties::OperatorSubtract ||
		wordTable[p].property == EnumWordProperties::OperatorBitAnd ||
		wordTable[p].property == EnumWordProperties::OperatorNot ||
		wordTable[p].property == EnumWordProperties::OperatorBitNot) {
		p++;
		return true;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ����ת�����ʽʶ��
// param   
// return  
bool GrammaticalAnalysis::CastExpression() {
	// ���浱ǰָ��
	int nowP = p;

	// UnaryExpression
	if (UnaryExpression()) {
		return true;
	}
	// (TypeName)CastExpression
	else if (wordTable[p].property == EnumWordProperties::OperatorLeftRound) {
		p++;
		if (IsTypeName(wordTable[p])) {
			p++;
			if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
				p++;
				if (CastExpression()) {
					return true;
				}
				throw(Exception("Not a Cast Expression because Expect a Cast Expression", wordTable[p], 0));
			}
			throw(Exception("Not a Cast Expression because Expect a )", wordTable[p], 0));
		}
		throw(Exception("Not a Cast Expression because Expect a Type Name", wordTable[p], 0));
	}
	else {
		p = nowP;
		return false;
	}
}

// func    �˳����ʽʶ��
// param   
// return  
bool GrammaticalAnalysis::MultiplicativeExpression() {
	// ���浱ǰָ��
	int nowP = p;

	// CastExpression MultiplicativeExpressionEliminateLeft
	if (CastExpression()) {
		if (MultiplicativeExpressionEliminateLeft()) {
			return true;
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    �˳����ʽ������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::MultiplicativeExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// *CastExpression MultiplicativeExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorMultiply) {
		p++;
		if (CastExpression()) {
			if (MultiplicativeExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a * Multiplicative Expression because Expect a Cast Expression", wordTable[p], 0));
	}
	// /CastExpression MultiplicativeExpressionEliminateLeft
	else if (wordTable[p].property == EnumWordProperties::OperatorDivide) {
		p++;
		if (CastExpression()) {
			if (MultiplicativeExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a / Multiplicative Expression because Expect a Cast Expression", wordTable[p], 0));
	}
	// %CastExpression MultiplicativeExpressionEliminateLeft
	else if (wordTable[p].property == EnumWordProperties::OperatorModulo) {
		p++;
		if (CastExpression()) {
			if (MultiplicativeExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a % Multiplicative Expression because Expect a Cast Expression", wordTable[p], 0));
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    �Ӽ����ʽʶ��
// param   
// return  
bool GrammaticalAnalysis::AdditiveExpression() {
	// ���浱ǰָ��
	int nowP = p;

	// MultiplicativeExpression AdditiveExpressionEliminateLeft
	if (MultiplicativeExpression()) {
		if (AdditiveExpressionEliminateLeft()) {
			return true;
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    �Ӽ����ʽ������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::AdditiveExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// +MultiplicativeExpression AdditiveExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorAdd) {
		p++;
		if (MultiplicativeExpression()) {
			if (AdditiveExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a + Additive Expression because Expect a Multiplicative Expression", wordTable[p], 0));
	}
	// -MultiplicativeExpression AdditiveExpressionEliminateLeft
	else if (wordTable[p].property == EnumWordProperties::OperatorSubtract) {
		p++;
		if (MultiplicativeExpression()) {
			if (AdditiveExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a - Additive Expression because Expect a Multiplicative Expression", wordTable[p], 0));
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    ��λ���ʽʶ��
// param   
// return  
bool GrammaticalAnalysis::ShiftExpression() {
	// ���浱ǰָ��
	int nowP = p;

	// AdditiveExpression ShiftExpressionEliminateLeft
	if (AdditiveExpression()) {
		if (ShiftExpressionEliminateLeft()) {
			return true;
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ��λ���ʽ������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::ShiftExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// << AdditiveExpression ShiftExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorLeftShift) {
		p++;
		if (AdditiveExpression()) {
			if (ShiftExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a << Expression because Expect a Additive Expression", wordTable[p], 0));
	}
	// >> AdditiveExpression ShiftExpressionEliminateLeft
	else if (wordTable[p].property == EnumWordProperties::OperatorRightShift) {
		p++;
		if (AdditiveExpression()) {
			if (ShiftExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a >> Expression because Expect a Additive Expression", wordTable[p], 0));
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    ��ϵ���ʽʶ��
// param   
// return  
bool GrammaticalAnalysis::RelationalExpression() {
	// ���浱ǰָ��
	int nowP = p;

	// ShiftExpression RelationalExpressionEliminateLeft
	if (ShiftExpression()) {
		if (RelationalExpressionEliminateLeft()) {
			return true;
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ��ϵ���ʽ������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::RelationalExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// < ShiftExpression RelationalExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorSmallerThan) {
		p++;
		if (ShiftExpression()) {
			if (RelationalExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a < Relational Expression because Expect a Shift Expression", wordTable[p], 0));
	}
	// > ShiftExpression RelationalExpressionEliminateLeft
	else if (wordTable[p].property == EnumWordProperties::OperatorGreaterThan) {
		p++;
		if (ShiftExpression()) {
			if (RelationalExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a > Relational Expression because Expect a Shift Expression", wordTable[p], 0));
	}
	// <= ShiftExpression RelationalExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorSmallerEqual) {
		p++;
		if (ShiftExpression()) {
			if (RelationalExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a <= Relational Expression because Expect a Shift Expression", wordTable[p], 0));
	}
	// >= ShiftExpression RelationalExpressionEliminateLeft
	else if (wordTable[p].property == EnumWordProperties::OperatorGreaterEqual) {
		p++;
		if (ShiftExpression()) {
			if (RelationalExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a >= Relational Expression because Expect a Shift Expression", wordTable[p], 0));
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    ��ȱ��ʽʶ��
// param   
// return  
bool GrammaticalAnalysis::EqualityExpression() {
	// ���浱ǰָ��
	int nowP = p;

	// RelationalExpression EqualityExpressionEliminateLeft
	if (RelationalExpression()) {
		if (EqualityExpressionEliminateLeft()) {
			return true;
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ��ȱ��ʽ������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::EqualityExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// == RelationalExpression EqualityExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorEqual) {
		p++;
		if (RelationalExpression()) {
			if (EqualityExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a == Equality Expression because Expect a Relational Expression", wordTable[p], 0));
	}
	// != RelationalExpression EqualityExpressionEliminateLeft
	else if (wordTable[p].property == EnumWordProperties::OperatorNotEqual) {
		p++;
		if (RelationalExpression()) {
			if (EqualityExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a != Equality Expression because Expect a Relational Expression", wordTable[p], 0));
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    ��λ����ʽʶ��
// param   
// return  
bool GrammaticalAnalysis::BitAndExpression() {
	// ���浱ǰָ��
	int nowP = p;

	// EqualityExpression BitAndExpressionEliminateLeft
	if (EqualityExpression()) {
		if (BitAndExpressionEliminateLeft()) {
			return true;
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ��λ����ʽ������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::BitAndExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// & EqualityExpression BitAndExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorBitAnd) {
		p++;
		if (EqualityExpression()) {
			if (BitAndExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a & Expression because Expect a Equality Expression", wordTable[p], 0));
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    ��λ�����ʽʶ��
// param   
// return  
bool GrammaticalAnalysis::BitXOrExpression() {
	// ���浱ǰָ��
	int nowP = p;

	// BitAndExpression BitXOrExpressionEliminateLeft
	if (BitAndExpression()) {
		if (BitXOrExpressionEliminateLeft()) {
			return true;
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ��λ�����ʽ������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::BitXOrExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// ^ BitAndExpression BitXOrExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorXOr) {
		p++;
		if (BitAndExpression()) {
			if (BitXOrExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a ^ Expression because Expect a & Expression", wordTable[p], 0));
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    ��λ����ʽʶ��
// param   
// return  
bool GrammaticalAnalysis::BitOrExpression() {
	// ���浱ǰָ��
	int nowP = p;

	// BitXOrExpression BitOrExpressionEliminateLeft
	if (BitXOrExpression()) {
		if (BitOrExpressionEliminateLeft()) {
			return true;
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ��λ����ʽ������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::BitOrExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// | BitXOrExpression BitOrExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorBitOr) {
		p++;
		if (BitXOrExpression()) {
			if (BitOrExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a | Expression because Expect a ^ Expression", wordTable[p], 0));
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    �߼�����ʽʶ��
// param   
// return  
bool GrammaticalAnalysis::AndExpression() {
	// ���浱ǰָ��
	int nowP = p;

	// BitOrExpression AndExpressionEliminateLeft
	if (BitOrExpression()) {
		if (AndExpressionEliminateLeft()) {
			return true;
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    �߼�����ʽ������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::AndExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// && BitOrExpression AndExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorAnd) {
		p++;
		if (BitOrExpression()) {
			if (AndExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a && Expression because Expect a | Expression", wordTable[p], 0));
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    �߼�����ʽʶ��
// param   
// return  
bool GrammaticalAnalysis::OrExpression() {
	// ���浱ǰָ��
	int nowP = p;

	// AndExpression OrExpressionEliminateLeft
	if (AndExpression()) {
		if (OrExpressionEliminateLeft()) {
			return true;
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    �߼�����ʽ������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::OrExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// || AndExpression OrExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorOr) {
		p++;
		if (AndExpression()) {
			if (OrExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a || Expression because Expect a && Expression", wordTable[p], 0));
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    �������ʽʶ��
// param   
// return  
bool GrammaticalAnalysis::ConditionalExpression() {
	// ���浱ǰָ��
	int nowP = p;

	// OrExpression
	if (OrExpression()) {
		// OrExpression ? Expression : ConditionalExpression
		if (wordTable[p].property == EnumWordProperties::OperatorQuestion) {
			p++;
			if (Expression()) {
				if (wordTable[p].property == EnumWordProperties::OperatorColon) {
					p++;
					if (ConditionalExpression()) {
						return true;
					}
				}
				throw(Exception("Not a ? : Conditional Expression because Expect a :", wordTable[p], 0));
			}
			throw(Exception("Not a ? : Conditional Expression because Expect a Expression", wordTable[p], 0));
		}
		// OrExpression
		else {
			return true;
		}
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ��ֵ���ʽʶ��
// param   
// return  
bool GrammaticalAnalysis::AssignmentExpression() {
	// ���浱ǰָ��
	int nowP = p;

	if (UnaryExpression()) {
		if (AssignmentOperator()) {
			if (AssignmentExpression()) {
				return true;
			}
			throw(Exception("Not a Assignment Expression because Expect a Assignment Expression", wordTable[p], 0));
		}
		p = nowP;
	}
	if (ConditionalExpression()) {
		return true;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ��ֵ�����ʶ��
// param   
// return  
bool GrammaticalAnalysis::AssignmentOperator() {
	// ���浱ǰָ��
	int nowP = p;

	if (wordTable[p].property == EnumWordProperties::OperatorAssign ||
		wordTable[p].property == EnumWordProperties::OperatorAddAssign ||
		wordTable[p].property == EnumWordProperties::OperatorAndAssign ||
		wordTable[p].property == EnumWordProperties::OperatorSubAssign ||
		wordTable[p].property == EnumWordProperties::OperatorDivAssign ||
		wordTable[p].property == EnumWordProperties::OperatorMulAssign ||
		wordTable[p].property == EnumWordProperties::OperatorLeftShiftAssign ||
		wordTable[p].property == EnumWordProperties::OperatorRightShiftAssign ||
		wordTable[p].property == EnumWordProperties::OperatorOrAssign ||
		wordTable[p].property == EnumWordProperties::OperatorModAssign ||
		wordTable[p].property == EnumWordProperties::OperatorXOrAssign) {
		p++;
		return true;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ���ʽʶ��
// param   
// return  
bool GrammaticalAnalysis::Expression() {
	// ���浱ǰָ��
	int nowP = p;

	// AssignmentExpression ExpressionEliminateLeft
	if (AssignmentExpression()) {
		if (ExpressionEliminateLeft()) {
			return true;
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ���ʽ������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::ExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// ,AssignmentExpression ExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorComma) {
		p++;
		if (AssignmentExpression()) {
			if (ExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a Expression because Expect a Assignment Expression", wordTable[p], 0));
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    ����ʶ��
// param   
// return  
bool GrammaticalAnalysis::Declaration() {
	// ���浱ǰָ��
	int nowP = p;

	// TypeName InitDeclaratorList;
	if (IsTypeName(wordTable[p])) {
		p++;
		if (InitDeclaratorList()) {
			if (wordTable[p].property == EnumWordProperties::Semicolon) {
				p++;
				return true;
			}
			throw(Exception("Not a Declaration because Expect a ;", wordTable[p], 0));
		}
		p = nowP;
		return  false;
	}
	else {
		p = nowP;
		return  false;
	}
}

// func    ��ʼ�������б�ʶ��
// param   
// return  
bool GrammaticalAnalysis::InitDeclaratorList() {
	// ���浱ǰָ��
	int nowP = p;

	// InitDeclarator InitDeclaratorListEliminateLeft
	if (InitDeclarator()) {
		if (InitDeclaratorListEliminateLeft()) {
			return true;
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ��ʼ�������б�������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::InitDeclaratorListEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// ,InitDeclarator InitDeclaratorListEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorComma) {
		p++;
		if (InitDeclarator()) {
			if (InitDeclaratorListEliminateLeft()) {
				return true;
			}
		}
		p = nowP;
		return false;
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    ��ʼ��������ʶ��
// param   
// return  
bool GrammaticalAnalysis::InitDeclarator() {
	// ���浱ǰָ��
	int nowP = p;

	// DirectDeclarator
	if (DirectDeclarator()) {
		// DirectDeclarator = AssignmentExpression
		if (wordTable[p].property == EnumWordProperties::OperatorAssign) {
			p++;
			if (AssignmentExpression()) {
				return true;
			}
		}
		// DirectDeclarator
		else {
			return true;
		}
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ֱ��������ʶ��
// param   
// return  
bool GrammaticalAnalysis::DirectDeclarator() {
	// ���浱ǰָ��
	int nowP = p;

	// Identifier DirectDeclaratorEliminateLeft
	if (wordTable[p].property == EnumWordProperties::Identifier) {
		p++;
		if (DirectDeclaratorEliminateLeft()) {
			return true;
		}
		else {
			p = nowP;
			return false;
		}
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ֱ��������������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::DirectDeclaratorEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// (ParameterList) DirectDeclaratorEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorLeftRound) {
		p++;
		if (ParameterList()) {
			if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
				p++;
				if (DirectDeclaratorEliminateLeft()) {
					return true;
				}
			}
		}
		p = nowP;
		return false;
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    �����б�ʶ��
// param   
// return  
bool GrammaticalAnalysis::ParameterList() {
	// ���浱ǰָ��
	int nowP = p;

	// ParameterDeclaration ParameterListEliminateLeft
	if (ParameterDeclaration()) {
		if (ParameterListEliminateLeft()) {
			return true;
		}
		else {
			p = nowP;
			return false;
		}
	}
	else {
		p = nowP;
		return false;
	}
}

// func    �����б�������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::ParameterListEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// ,ParameterDeclaration ParameterListEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorComma) {
		p++;
		if (ParameterDeclaration()) {
			if (ParameterListEliminateLeft()) {
				return true;
			}
		}
		p = nowP;
		return false;
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    ��������ʶ��
// param   
// return  
bool GrammaticalAnalysis::ParameterDeclaration() {
	// ���浱ǰָ��
	int nowP = p;

	// TypeName DirectDeclarator
	if (IsTypeName(wordTable[p])) {
		p++;
		if (DirectDeclarator()) {
			return true;
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ��ʶ���б�ʶ��
// param   
// return  
bool GrammaticalAnalysis::IdentifierList() {
	// ���浱ǰָ��
	int nowP = p;

	// Identifier IdentifierListEliminateLeft
	if (wordTable[p].property == EnumWordProperties::Identifier) {
		p++;
		if (IdentifierListEliminateLeft()) {
			return true;
		}
		else {
			p = nowP;
			return false;
		}
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ��ʶ���б�������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::IdentifierListEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// ,Identifier IdentifierListEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorComma) {
		p++;
		if (wordTable[p].property == EnumWordProperties::Identifier) {
			p++;
			if (IdentifierListEliminateLeft()) {
				return true;
			}
		}
		p = nowP;
		return false;
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    ���ʶ��
// param   
// return  
bool GrammaticalAnalysis::Statement() {
	// ���浱ǰָ��
	int nowP = p;

	// Goto Continue Break Return ����ת���
	if (wordTable[p].property == EnumWordProperties::Goto ||
		wordTable[p].property == EnumWordProperties::Continue ||
		wordTable[p].property == EnumWordProperties::Break ||
		wordTable[p].property == EnumWordProperties::Return) {
		JumpStatement();
		return true;
	}
	// If ��ѡ�����
	else if (wordTable[p].property == EnumWordProperties::If) {
		SelectionStatement();
		return true;
	}
	// { �Ǹ������
	else if (wordTable[p].property == EnumWordProperties::OperatorLeftBrace) {
		CompoundStatement();
		return true;
	}
	// While Do �ǵ������
	else if (wordTable[p].property == EnumWordProperties::While ||
		wordTable[p].property == EnumWordProperties::Do) {
		IterationStatement();
		return true;
	}
	// ����� �Ǳ��ʽ���
	else if (wordTable[p].property == EnumWordProperties::Semicolon) {
		ExpressionStatement();
		return true;
	}
	// ��ǩ���ͱ��ʽ�����Ҫ����
	// ��˱�ǩ������ڴ�����throw
	if (LabeledStatement()) {
		return true;
	}
	if (ExpressionStatement()) {
		return true;
	}
	// ������ �������
	else {
		p = nowP;
		return false;
	}
}

// func    ��ǩ���ʶ��
// param   
// return  
bool GrammaticalAnalysis::LabeledStatement() {
	// ���浱ǰָ��
	int nowP = p;

	// Identifier : Statement
	if (wordTable[p].property == EnumWordProperties::Identifier) {
		p++;
		if (wordTable[p].property == EnumWordProperties::OperatorColon) {
			p++;
			if (Statement()) {
				return true;
			}
			throw(Exception("Not a Labeled Statement because Expect a Statement", wordTable[p], 0));
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    �������ʶ��
// param   
// return  
bool GrammaticalAnalysis::CompoundStatement() {
	// ���浱ǰָ��
	int nowP = p;

	// { BlockItemList }
	if (wordTable[p].property == EnumWordProperties::OperatorLeftBrace) {
		p++;
		if (BlockItemList()) {
			if (wordTable[p].property == EnumWordProperties::OperatorRightBrace) {
				p++;
				return true;
			}
			throw(Exception("Not a Compound Statement because Expect a }", wordTable[p], 0));
		}
		throw(Exception("Not a Compound Statement because Expect a Block Item", wordTable[p], 0));
	}
	throw(Exception("Not a Compound Statement because Expect a {", wordTable[p], 0));
}

// func    ����Ŀ�б�ʶ��
// param   
// return  
bool GrammaticalAnalysis::BlockItemList() {
	// ���浱ǰָ��
	int nowP = p;

	// BlockItem BlockItemListEliminateLeft
	if (BlockItem()) {
		if (BlockItemListEliminateLeft()) {
			return true;
		}
		else {
			p = nowP;
			return false;
		}
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ����Ŀ�б�������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::BlockItemListEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// BlockItem BlockItemListEliminateLeft
	if (BlockItem()) {
		if (BlockItemListEliminateLeft()) {
			return true;
		}
		p = nowP;
		return false;
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    ����Ŀʶ��
// param   
// return  
bool GrammaticalAnalysis::BlockItem() {
	// ���浱ǰָ��
	int nowP = p;

	// Declaration
	if (Declaration()) {
		return true;
	}
	// Statement
	if (Statement()) {
		return true;
	}
	p = nowP;
	return false;
}

// func    ���ʽ���ʶ��
// param   
// return  
bool GrammaticalAnalysis::ExpressionStatement() {
	// ���浱ǰָ��
	int nowP = p;

	// ;
	if (wordTable[p].property == EnumWordProperties::Semicolon) {
		p++;
		return true;
	}
	// Expression;
	else if (Expression()) {
		if (wordTable[p].property == EnumWordProperties::Semicolon) {
			p++;
			return true;
		}
		throw(Exception("Not a Expression Statement because Exprect a ;", wordTable[p], 0));
	}
	else {
		p = nowP;
		return false;
	}
}

// func    ѡ�����ʶ��
// param   
// return  
bool GrammaticalAnalysis::SelectionStatement() {
	// ���浱ǰָ��
	int nowP = p;

	// If (Expression) Statement
	if (wordTable[p].property == EnumWordProperties::If) {
		p++;
		if (wordTable[p].property == EnumWordProperties::OperatorLeftRound) {
			p++;
			if (Expression()) {
				if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
					p++;
					if (Statement()) {
						// If (Expression) Statement Else Statement
						if (wordTable[p].property == EnumWordProperties::Else) {
							p++;
							if (Statement()) {
								return true;
							}
						}
						// If (Expression) Statement
						else {
							return true;
						}
					}
					throw(Exception("Not a if Statement because Exprect a Statement", wordTable[p], 0));
				}
				throw(Exception("Not a if Statement because Exprect a )", wordTable[p], 0));
			}
			throw(Exception("Not a if Statement because Exprect a Expression", wordTable[p], 0));
		}
		throw(Exception("Not a if Statement because Exprect a (", wordTable[p], 0));
	}
	throw(Exception("Not a if Statement", wordTable[p], 0));
}

// func    �������ʶ��
// param   
// return  
bool GrammaticalAnalysis::IterationStatement() {
	// ���浱ǰָ��
	int nowP = p;

	// While(Expression) Statement
	if (wordTable[p].property == EnumWordProperties::While) {
		p++;
		if (wordTable[p].property == EnumWordProperties::OperatorLeftRound) {
			p++;
			if (Expression()) {
				if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
					p++;
					if (Statement()) {
						return true;
					}
				}
				throw(Exception("Not a while Statement because Exprect a )", wordTable[p], 0));
			}
			throw(Exception("Not a while Statement because Exprect a Expression", wordTable[p], 0));
		}
		throw(Exception("Not a while Statement because Exprect a (", wordTable[p], 0));
	}
	// Do Statement While(Expression);
	if (wordTable[p].property == EnumWordProperties::Do) {
		p++;
		if (Statement()) {
			if (wordTable[p].property == EnumWordProperties::While) {
				p++;
				if (wordTable[p].property == EnumWordProperties::OperatorLeftRound) {
					p++;
					if (Expression()) {
						if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
							p++;
							return true;
						}
						throw(Exception("Not a do while Statement because Exprect a )", wordTable[p], 0));
					}
					throw(Exception("Not a do while Statement because Exprect a Expression", wordTable[p], 0));
				}
				throw(Exception("Not a do while Statement because Exprect a (", wordTable[p], 0));
			}
			throw(Exception("Not a do while Statement because Exprect a while", wordTable[p], 0));
		}
		throw(Exception("Not a do while Statement because Exprect a Statement", wordTable[p], 0));
	}
	throw(Exception("Not a IterationStatement Statement", wordTable[p], 0));
}

// func    ��ת���ʶ��
// param   
// return  
bool GrammaticalAnalysis::JumpStatement() {
	// ���浱ǰָ��
	int nowP = p;

	// Goto Identifier;
	if (wordTable[p].property == EnumWordProperties::Goto) {
		p++;
		if (wordTable[p].property == EnumWordProperties::Identifier) {
			p++;
			if (wordTable[p].property == EnumWordProperties::Semicolon) {
				p++;
				return true;
			}
			throw(Exception("Not a goto Statement because Exprect a ;", wordTable[p], 0));
		}
		throw(Exception("Not a goto Statement because Exprect a Identifier", wordTable[p], 0));
	}
	// Continue;
	else if (wordTable[p].property == EnumWordProperties::Continue) {
		p++;
		if (wordTable[p].property == EnumWordProperties::Semicolon) {
			p++;
			return true;
		}
		throw(Exception("Not a continue Statement because Exprect a ;", wordTable[p], 0));
	}
	// Break;
	else if (wordTable[p].property == EnumWordProperties::Break) {
		p++;
		if (wordTable[p].property == EnumWordProperties::Semicolon) {
			p++;
			return true;
		}
		throw(Exception("Not a break Statement because Exprect a ;", wordTable[p], 0));
	}
	// Return
	else if (wordTable[p].property == EnumWordProperties::Return) {
		p++;
		// Return;
		if (wordTable[p].property == EnumWordProperties::Semicolon) {
			p++;
			return true;
		}
		// Return Expression;
		else if (Expression()) {
			if (wordTable[p].property == EnumWordProperties::Semicolon) {
				p++;
				return true;
			}
		}
		throw(Exception("Not a Return Statement", wordTable[p], 0));
	}
	throw(Exception("Not a Jump Statement", wordTable[p], 0));
}

// func    ���뵥Ԫʶ��
// param   
// return  
bool GrammaticalAnalysis::TranslationUnit() {
	// ���浱ǰָ��
	int nowP = p;

	// ExternalDeclaration TranslationUnitEliminateLeft
	if (ExternalDeclaration()) {
		if (TranslationUnitEliminateLeft()) {
			return true;
		}
	}

	throw(Exception("Not a Translation Unit", wordTable[p], 0));
}

// func    ���뵥Ԫ������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::TranslationUnitEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// ExternalDeclaration TranslationUnitEliminateLeft
	if (ExternalDeclaration()) {
		if (TranslationUnitEliminateLeft()) {
			return true;
		}
		p = nowP;
		return false;
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    �ⲿ����ʶ��
// param   
// return  
bool GrammaticalAnalysis::ExternalDeclaration() {
	// ���浱ǰָ��
	int nowP = p;
	int firstSemicolon = p;
	int firstLeftBrace = p;
	for (int i = p; true; i++) {
		if (wordTable[i].property == EnumWordProperties::Semicolon) {
			firstSemicolon = i;
			break;
		}
		if (wordTable[i].property == EnumWordProperties::TimeToStop) {
			firstSemicolon = -1;
			break;
		}
	}
	for (int i = p; true; i++) {
		if (wordTable[i].property == EnumWordProperties::OperatorLeftBrace) {
			firstLeftBrace = i;
			break;
		}
		if (wordTable[i].property == EnumWordProperties::TimeToStop) {
			firstLeftBrace = -1;
			break;
		}
	}
	if (firstSemicolon < 0 && firstLeftBrace < 0) {
		p = nowP;
		return false;
	}
	// ���������� TypeName DirectDeclarator /* DeclarationList */ CompoundStatement
	// DeclarationList �� TypeName ��ͷ
	// CompoundStatement �� { ��ͷ
	// DirectDeclarator �� Identifier ��ͷ ��� (ParameterList) DirectDeclaratorEliminateLeft
	// ��� ���������Ȼ�� TypeName Identifier ( ��ͷ
	// �Һ��������Ȼ�� { �� ; ��ǰ��
	// FunctionDefinition
	if (firstSemicolon < 0 || firstLeftBrace < firstSemicolon) {
		if (FunctionDefinition()) {
			return true;
		}
	}
	// ������ TypeName InitDeclaratorList;
	// InitDeclaratorList �� InitDeclarator ��ͷ
	// InitDeclarator �� Identifier ��ͷ
	// ��� ������Ȼ�� ; �� { ��ǰ��
	// Declaration
	else if (firstLeftBrace < 0 || firstSemicolon < firstLeftBrace) {
		if (Declaration()) {
			return true;
		}
	}
	throw(Exception("Not a Function Definition or a Declaration", wordTable[p], 0));
}

// func    ��������ʶ��
// param   
// return  
bool GrammaticalAnalysis::FunctionDefinition() {
	// ���浱ǰָ��
	int nowP = p;

	// TypeName DirectDeclarator /* DeclarationList */ CompoundStatement
	if (IsTypeName(wordTable[p])) {
		p++;
		if (DirectDeclarator()) {
			// ��Ȼ�� { ��ͷ
			// TypeName DirectDeclarator CompoundStatement
			if (wordTable[p].property == EnumWordProperties::OperatorLeftBrace) {
				if (CompoundStatement()) {
					return true;
				}
			}
			// TypeName DirectDeclarator DeclarationList CompoundStatement
			else if (DeclarationList()) {
				if (CompoundStatement()) {
					return true;
				}
			}
		}
		throw(Exception("Not a Direct Declarator", wordTable[p], 0));
	}
	throw(Exception("Not a Function Definition because Expect a Type Name", wordTable[p], 0));
}

// func    �����б�ʶ��
// param   
// return  
bool GrammaticalAnalysis::DeclarationList() {
	// ���浱ǰָ��
	int nowP = p;

	// Declaration DeclarationListEliminateLeft
	if (Declaration()) {
		if (DeclarationListEliminateLeft()) {
			return true;
		}
	}
	throw(Exception("Not a Declaration", wordTable[p], 0));
}

// func    �����б�������ݹ�ʶ��
// param   
// return  
bool GrammaticalAnalysis::DeclarationListEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// Declaration DeclarationListEliminateLeft
	if (Declaration()) {
		if (DeclarationListEliminateLeft()) {
			return true;
		}
		p = nowP;
		return false;
	}
	// ��
	else {
		p = nowP;
		return true;
	}
}





// func    �����Ƿ�������
// param   
// return  
bool GrammaticalAnalysis::IsTypeName(Word w) {
	if (w.property == EnumWordProperties::Bool ||
		w.property == EnumWordProperties::Char ||
		w.property == EnumWordProperties::Int ||
		w.property == EnumWordProperties::Float ||
		w.property == EnumWordProperties::Double ||
		w.property == EnumWordProperties::StringType ||
		w.property == EnumWordProperties::Enum ||
		w.property == EnumWordProperties::Void) {
		return true;
	}
	else {
		return false;
	}
}

// func    �����Ƿ�������������
// param   
// return  
bool GrammaticalAnalysis::IsNumberLiteralValue(Word w) {
	if (w.property == EnumWordProperties::TenIntNumber ||
		w.property == EnumWordProperties::EightIntNumber ||
		w.property == EnumWordProperties::SixteenIntNumber ||
		w.property == EnumWordProperties::FloatNumber ||
		w.property == EnumWordProperties::FloatEeNumber ||
		w.property == EnumWordProperties::CharNumber) {
		return true;
	}
	else {
		return false;
	}
}

// func    �����Ƿ���������
// param   
// return  
bool GrammaticalAnalysis::IsLiteralValue(Word w) {
	if (IsNumberLiteralValue(w) ||
		w.property == EnumWordProperties::String) {
		return true;
	}
	else {
		return false;
	}
}