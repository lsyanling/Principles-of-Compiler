#include"head.h"

GrammaticalAnalysis::GrammaticalAnalysis(std::vector<Word>& w) :wordTable(w) {
	try {
		TranslationUnit();
	}
	catch (Exception e) {
		std::cout << "Grammatical Error in line " << e.word.line << " " << e.str << std::endl;
	}
}

// func    初始表达式识别
// param   
// return  
bool GrammaticalAnalysis::PrimaryExpression() {
	// 保存当前指针
	int nowP = p;

	// Identifier
	if (wordTable[p].property == EnumProperties::Identifier ||
		// 数字字面量
		wordTable[p].property == EnumProperties::TenIntNumber ||
		wordTable[p].property == EnumProperties::EightIntNumber ||
		wordTable[p].property == EnumProperties::SixteenIntNumber ||
		wordTable[p].property == EnumProperties::FloatNumber ||
		wordTable[p].property == EnumProperties::FloatEeNumber ||
		wordTable[p].property == EnumProperties::CharNumber) {
		p++;
		return true;
	}
	// (Expression)
	else if (wordTable[p].property == EnumProperties::OperatorLeftRound) {
		p++;
		if (Expression()) {
			if (wordTable[p].property == EnumProperties::OperatorRightRound) {
				p++;
				return true;
			}
			throw(Exception("Not a Primary Expression because Expect a )", wordTable[p]));
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    后缀表达式识别
// param   
// return  
bool GrammaticalAnalysis::PostfixExpression() {
	// 保存当前指针
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

// func    后缀表达式消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::PostfixExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// [Expression] PostfixExpressionEliminateLeft
	if (wordTable[p].property == EnumProperties::OperatorLeftSquare) {
		p++;
		if (Expression()) {
			if (wordTable[p].property == EnumProperties::OperatorRightSquare) {
				p++;
				if (PostfixExpressionEliminateLeft()) {
					return true;
				}
			}
			throw(Exception("Not [] Postfix Expression because Expect a ]", wordTable[p]));
		}
		throw(Exception("Not a [] Postfix Expression because Expect a Expression", wordTable[p]));
	}
	// (ArgumentExpressionList) PostfixExpressionEliminateLeft
	else if (wordTable[p].property == EnumProperties::OperatorLeftRound) {
		p++;
		if (ArgumentExpressionList()) {
			if (wordTable[p].property == EnumProperties::OperatorRightRound) {
				p++;
				if (PostfixExpressionEliminateLeft()) {
					return true;
				}
			}
			throw(Exception("Not a Postfix Expression because Expect a )", wordTable[p]));
		}
		p = nowP;
		return false;
	}
	// .Identifier PostfixExpressionEliminateLeft
	else if (wordTable[p].property == EnumProperties::OperatorPoint) {
		p++;
		if (wordTable[p].property == EnumProperties::Identifier) {
			p++;
			if (PostfixExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a -> Postfix Expression because Expect a Identifier", wordTable[p]));
	}
	// ->Identifier PostfixExpressionEliminateLeft
	else if (wordTable[p].property == EnumProperties::OperatorArrow) {
		p++;
		if (wordTable[p].property == EnumProperties::Identifier) {
			p++;
			if (PostfixExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a -> Postfix Expression because Expect a Identifier", wordTable[p]));
	}
	// ++PostfixExpressionEliminateLeft
	else if (wordTable[p].property == EnumProperties::OperatorAddAdd) {
		p++;
		if (PostfixExpressionEliminateLeft()) {
			return true;
		}
		throw(Exception("Not a ++ Postfix Expression because Expect a Postfix Expression", wordTable[p]));
	}
	// --PostfixExpressionEliminateLeft
	else if (wordTable[p].property == EnumProperties::OperatorSubSubtract) {
		p++;
		if (PostfixExpressionEliminateLeft()) {
			return true;
		}
		throw(Exception("Not a -- Postfix Expression because Expect a Postfix Expression", wordTable[p]));
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    参数表达式列表识别
// param   
// return  
bool GrammaticalAnalysis::ArgumentExpressionList() {
	// 保存当前指针
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

// func    参数表达式列表消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::ArgumentExpressionListEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// ,AssignmentExpression ArgumentExpressionListEliminateLeft
	if (wordTable[p].property == EnumProperties::OperatorComma) {
		p++;
		if (AssignmentExpression()) {
			if (ArgumentExpressionListEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a Argument Expression List because Expect a Assignment Expression", wordTable[p]));
		p = nowP;
		return false;
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    单目表达式识别
// param   
// return  
bool GrammaticalAnalysis::UnaryExpression() {
	// 保存当前指针
	int nowP = p;

	// PostfixExpression
	if (PostfixExpression()) {
		return true;
	}
	// ++UnaryExpression
	else if (wordTable[p].property == EnumProperties::OperatorAddAdd) {
		p++;
		if (UnaryExpression()) {
			return true;
		}
		throw(Exception("Not a ++ Unary Expression because Expect a Unary Expression", wordTable[p]));
		p = nowP;
		return false;
	}
	// --UnaryExpression
	else if (wordTable[p].property == EnumProperties::OperatorSubSubtract) {
		p++;
		if (UnaryExpression()) {
			return true;
		}
		throw(Exception("Not a -- Unary Expression because Expect a Unary Expression", wordTable[p]));
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
	else if (wordTable[p].property == EnumProperties::OperatorSizeof) {
		p++;
		// sizeof UnaryExpression
		if (UnaryExpression()) {
			return true;
		}
		// sizeof (TypeName)
		if (wordTable[p].property == EnumProperties::OperatorLeftRound) {
			p++;
			if (IsTypeName(wordTable[p])) {
				p++;
				if (wordTable[p].property == EnumProperties::OperatorRightRound) {
					p++;
					return true;
				}
				throw(Exception("Not a sizeof Expression because Expect a (", wordTable[p]));
			}
			throw(Exception("Not a sizeof Expression because Expect a (", wordTable[p]));
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    单目运算符识别
// param   
// return  
bool GrammaticalAnalysis::UnaryOperator() {
	// 保存当前指针
	int nowP = p;

	if (wordTable[p].property == EnumProperties::OperatorMultiply ||
		wordTable[p].property == EnumProperties::OperatorAdd ||
		wordTable[p].property == EnumProperties::OperatorSubtract ||
		wordTable[p].property == EnumProperties::OperatorBitAnd ||
		wordTable[p].property == EnumProperties::OperatorNot ||
		wordTable[p].property == EnumProperties::OperatorBitNot) {
		p++;
		return true;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    类型转换表达式识别
// param   
// return  
bool GrammaticalAnalysis::CastExpression() {
	// 保存当前指针
	int nowP = p;

	// UnaryExpression
	if (UnaryExpression()) {
		return true;
	}
	// (TypeName)CastExpression
	else if (wordTable[p].property == EnumProperties::OperatorLeftRound) {
		p++;
		if (IsTypeName(wordTable[p])) {
			p++;
			if (wordTable[p].property == EnumProperties::OperatorRightRound) {
				p++;
				if (CastExpression()) {
					return true;
				}
				throw(Exception("Not a Cast Expression because Expect a Cast Expression", wordTable[p]));
			}
			throw(Exception("Not a Cast Expression because Expect a )", wordTable[p]));
		}
		throw(Exception("Not a Cast Expression because Expect a Type Name", wordTable[p]));
	}
	else {
		p = nowP;
		return false;
	}
}

// func    乘除表达式识别
// param   
// return  
bool GrammaticalAnalysis::MultiplicativeExpression() {
	// 保存当前指针
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

// func    乘除表达式消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::MultiplicativeExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// *CastExpression MultiplicativeExpressionEliminateLeft
	if (wordTable[p].property == EnumProperties::OperatorMultiply) {
		p++;
		if (CastExpression()) {
			if (MultiplicativeExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a * Multiplicative Expression because Expect a Cast Expression", wordTable[p]));
	}
	// /CastExpression MultiplicativeExpressionEliminateLeft
	else if (wordTable[p].property == EnumProperties::OperatorDivide) {
		p++;
		if (CastExpression()) {
			if (MultiplicativeExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a / Multiplicative Expression because Expect a Cast Expression", wordTable[p]));
	}
	// %CastExpression MultiplicativeExpressionEliminateLeft
	else if (wordTable[p].property == EnumProperties::OperatorModulo) {
		p++;
		if (CastExpression()) {
			if (MultiplicativeExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a % Multiplicative Expression because Expect a Cast Expression", wordTable[p]));
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    加减表达式识别
// param   
// return  
bool GrammaticalAnalysis::AdditiveExpression() {
	// 保存当前指针
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

// func    加减表达式消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::AdditiveExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// +MultiplicativeExpression AdditiveExpressionEliminateLeft
	if (wordTable[p].property == EnumProperties::OperatorAdd) {
		p++;
		if (MultiplicativeExpression()) {
			if (AdditiveExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a + Additive Expression because Expect a Multiplicative Expression", wordTable[p]));
	}
	// -MultiplicativeExpression AdditiveExpressionEliminateLeft
	else if (wordTable[p].property == EnumProperties::OperatorSubtract) {
		p++;
		if (MultiplicativeExpression()) {
			if (AdditiveExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a - Additive Expression because Expect a Multiplicative Expression", wordTable[p]));
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    移位表达式识别
// param   
// return  
bool GrammaticalAnalysis::ShiftExpression() {
	// 保存当前指针
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

// func    移位表达式消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::ShiftExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// << AdditiveExpression ShiftExpressionEliminateLeft
	if (wordTable[p].property == EnumProperties::OperatorLeftShift) {
		p++;
		if (AdditiveExpression()) {
			if (ShiftExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a << Expression because Expect a Additive Expression", wordTable[p]));
	}
	// >> AdditiveExpression ShiftExpressionEliminateLeft
	else if (wordTable[p].property == EnumProperties::OperatorRightShift) {
		p++;
		if (AdditiveExpression()) {
			if (ShiftExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a >> Expression because Expect a Additive Expression", wordTable[p]));
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    关系表达式识别
// param   
// return  
bool GrammaticalAnalysis::RelationalExpression() {
	// 保存当前指针
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

// func    关系表达式消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::RelationalExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// < ShiftExpression RelationalExpressionEliminateLeft
	if (wordTable[p].property == EnumProperties::OperatorSmallerThan) {
		p++;
		if (ShiftExpression()) {
			if (RelationalExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a < Relational Expression because Expect a Shift Expression", wordTable[p]));
	}
	// > ShiftExpression RelationalExpressionEliminateLeft
	else if (wordTable[p].property == EnumProperties::OperatorGreaterThan) {
		p++;
		if (ShiftExpression()) {
			if (RelationalExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a > Relational Expression because Expect a Shift Expression", wordTable[p]));
	}
	// <= ShiftExpression RelationalExpressionEliminateLeft
	if (wordTable[p].property == EnumProperties::OperatorSmallerEqual) {
		p++;
		if (ShiftExpression()) {
			if (RelationalExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a <= Relational Expression because Expect a Shift Expression", wordTable[p]));
	}
	// >= ShiftExpression RelationalExpressionEliminateLeft
	else if (wordTable[p].property == EnumProperties::OperatorGreaterEqual) {
		p++;
		if (ShiftExpression()) {
			if (RelationalExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a >= Relational Expression because Expect a Shift Expression", wordTable[p]));
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    相等表达式识别
// param   
// return  
bool GrammaticalAnalysis::EqualityExpression() {
	// 保存当前指针
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

// func    相等表达式消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::EqualityExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// == RelationalExpression EqualityExpressionEliminateLeft
	if (wordTable[p].property == EnumProperties::OperatorEqual) {
		p++;
		if (RelationalExpression()) {
			if (EqualityExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a == Equality Expression because Expect a Relational Expression", wordTable[p]));
	}
	// != RelationalExpression EqualityExpressionEliminateLeft
	else if (wordTable[p].property == EnumProperties::OperatorNotEqual) {
		p++;
		if (RelationalExpression()) {
			if (EqualityExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a != Equality Expression because Expect a Relational Expression", wordTable[p]));
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    按位与表达式识别
// param   
// return  
bool GrammaticalAnalysis::BitAndExpression() {
	// 保存当前指针
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

// func    按位与表达式消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::BitAndExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// & EqualityExpression BitAndExpressionEliminateLeft
	if (wordTable[p].property == EnumProperties::OperatorBitAnd) {
		p++;
		if (EqualityExpression()) {
			if (BitAndExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a & Expression because Expect a Equality Expression", wordTable[p]));
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    按位异或表达式识别
// param   
// return  
bool GrammaticalAnalysis::BitXOrExpression() {
	// 保存当前指针
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

// func    按位异或表达式消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::BitXOrExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// ^ BitAndExpression BitXOrExpressionEliminateLeft
	if (wordTable[p].property == EnumProperties::OperatorXOr) {
		p++;
		if (BitAndExpression()) {
			if (BitXOrExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a ^ Expression because Expect a & Expression", wordTable[p]));
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    按位或表达式识别
// param   
// return  
bool GrammaticalAnalysis::BitOrExpression() {
	// 保存当前指针
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

// func    按位或表达式消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::BitOrExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// | BitXOrExpression BitOrExpressionEliminateLeft
	if (wordTable[p].property == EnumProperties::OperatorBitOr) {
		p++;
		if (BitXOrExpression()) {
			if (BitOrExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a | Expression because Expect a ^ Expression", wordTable[p]));
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    逻辑与表达式识别
// param   
// return  
bool GrammaticalAnalysis::AndExpression() {
	// 保存当前指针
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

// func    逻辑与表达式消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::AndExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// && BitOrExpression AndExpressionEliminateLeft
	if (wordTable[p].property == EnumProperties::OperatorAnd) {
		p++;
		if (BitOrExpression()) {
			if (AndExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a && Expression because Expect a | Expression", wordTable[p]));
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    逻辑或表达式识别
// param   
// return  
bool GrammaticalAnalysis::OrExpression() {
	// 保存当前指针
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

// func    逻辑或表达式消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::OrExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// || AndExpression OrExpressionEliminateLeft
	if (wordTable[p].property == EnumProperties::OperatorOr) {
		p++;
		if (AndExpression()) {
			if (OrExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a || Expression because Expect a && Expression", wordTable[p]));
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    条件表达式识别
// param   
// return  
bool GrammaticalAnalysis::ConditionalExpression() {
	// 保存当前指针
	int nowP = p;

	// OrExpression
	if (OrExpression()) {
		// OrExpression ? Expression : ConditionalExpression
		if (wordTable[p].property == EnumProperties::OperatorQuestion) {
			p++;
			if (Expression()) {
				if (wordTable[p].property == EnumProperties::OperatorColon) {
					p++;
					if (ConditionalExpression()) {
						return true;
					}
				}
				throw(Exception("Not a ? : Conditional Expression because Expect a :", wordTable[p]));
			}
			throw(Exception("Not a ? : Conditional Expression because Expect a Expression", wordTable[p]));
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

// func    赋值表达式识别
// param   
// return  
bool GrammaticalAnalysis::AssignmentExpression() {
	// 保存当前指针
	int nowP = p;

	if (UnaryExpression()) {
		if (AssignmentOperator()) {
			if (AssignmentExpression()) {
				return true;
			}
			throw(Exception("Not a Assignment Expression because Expect a Assignment Expression", wordTable[p]));
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

// func    赋值运算符识别
// param   
// return  
bool GrammaticalAnalysis::AssignmentOperator() {
	// 保存当前指针
	int nowP = p;

	if (wordTable[p].property == EnumProperties::OperatorAssign ||
		wordTable[p].property == EnumProperties::OperatorAddAssign ||
		wordTable[p].property == EnumProperties::OperatorAndAssign ||
		wordTable[p].property == EnumProperties::OperatorSubAssign ||
		wordTable[p].property == EnumProperties::OperatorDivAssign ||
		wordTable[p].property == EnumProperties::OperatorMulAssign ||
		wordTable[p].property == EnumProperties::OperatorLeftShiftAssign ||
		wordTable[p].property == EnumProperties::OperatorRightShiftAssign ||
		wordTable[p].property == EnumProperties::OperatorOrAssign ||
		wordTable[p].property == EnumProperties::OperatorModAssign ||
		wordTable[p].property == EnumProperties::OperatorXOrAssign) {
		p++;
		return true;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    表达式识别
// param   
// return  
bool GrammaticalAnalysis::Expression() {
	// 保存当前指针
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

// func    表达式消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::ExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// ,AssignmentExpression ExpressionEliminateLeft
	if (wordTable[p].property == EnumProperties::OperatorComma) {
		p++;
		if (AssignmentExpression()) {
			if (ExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a Expression because Expect a Assignment Expression", wordTable[p]));
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    声明识别
// param   
// return  
bool GrammaticalAnalysis::Declaration() {
	// 保存当前指针
	int nowP = p;

	// TypeName InitDeclaratorList;
	if (IsTypeName(wordTable[p])) {
		p++;
		if (InitDeclaratorList()) {
			if (wordTable[p].property == EnumProperties::Semicolon) {
				p++;
				return true;
			}
			throw(Exception("Not a Declaration because Expect a ;", wordTable[p]));
		}
		p = nowP;
		return  false;
	}
	else {
		p = nowP;
		return  false;
	}
}

// func    初始化声明列表识别
// param   
// return  
bool GrammaticalAnalysis::InitDeclaratorList() {
	// 保存当前指针
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

// func    初始化声明列表消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::InitDeclaratorListEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// ,InitDeclarator InitDeclaratorListEliminateLeft
	if (wordTable[p].property == EnumProperties::OperatorComma) {
		p++;
		if (InitDeclarator()) {
			if (InitDeclaratorListEliminateLeft()) {
				return true;
			}
		}
		p = nowP;
		return false;
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    初始化声明符识别
// param   
// return  
bool GrammaticalAnalysis::InitDeclarator() {
	// 保存当前指针
	int nowP = p;

	// DirectDeclarator
	if (DirectDeclarator()) {
		// DirectDeclarator = AssignmentExpression
		if (wordTable[p].property == EnumProperties::OperatorAssign) {
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

// func    直接声明符识别
// param   
// return  
bool GrammaticalAnalysis::DirectDeclarator() {
	// 保存当前指针
	int nowP = p;

	// Identifier DirectDeclaratorEliminateLeft
	if (wordTable[p].property == EnumProperties::Identifier) {
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

// func    直接声明符消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::DirectDeclaratorEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// (ParameterList) DirectDeclaratorEliminateLeft
	if (wordTable[p].property == EnumProperties::OperatorLeftRound) {
		p++;
		if (ParameterList()) {
			if (wordTable[p].property == EnumProperties::OperatorRightRound) {
				p++;
				if (DirectDeclaratorEliminateLeft()) {
					return true;
				}
			}
		}
		p = nowP;
		return false;
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    参数列表识别
// param   
// return  
bool GrammaticalAnalysis::ParameterList() {
	// 保存当前指针
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

// func    参数列表消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::ParameterListEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// ,ParameterDeclaration ParameterListEliminateLeft
	if (wordTable[p].property == EnumProperties::OperatorComma) {
		p++;
		if (ParameterDeclaration()) {
			if (ParameterListEliminateLeft()) {
				return true;
			}
		}
		p = nowP;
		return false;
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    参数声明识别
// param   
// return  
bool GrammaticalAnalysis::ParameterDeclaration() {
	// 保存当前指针
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

// func    标识符列表识别
// param   
// return  
bool GrammaticalAnalysis::IdentifierList() {
	// 保存当前指针
	int nowP = p;

	// Identifier IdentifierListEliminateLeft
	if (wordTable[p].property == EnumProperties::Identifier) {
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

// func    标识符列表消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::IdentifierListEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// ,Identifier IdentifierListEliminateLeft
	if (wordTable[p].property == EnumProperties::OperatorComma) {
		p++;
		if (wordTable[p].property == EnumProperties::Identifier) {
			p++;
			if (IdentifierListEliminateLeft()) {
				return true;
			}
		}
		p = nowP;
		return false;
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    语句识别
// param   
// return  
bool GrammaticalAnalysis::Statement() {
	// 保存当前指针
	int nowP = p;

	// Goto Continue Break Return 是跳转语句
	if (wordTable[p].property == EnumProperties::Goto ||
		wordTable[p].property == EnumProperties::Continue ||
		wordTable[p].property == EnumProperties::Break ||
		wordTable[p].property == EnumProperties::Return) {
		JumpStatement();
		return true;
	}
	// If 是选择语句
	else if (wordTable[p].property == EnumProperties::If) {
		SelectionStatement();
		return true;
	}
	// { 是复合语句
	else if (wordTable[p].property == EnumProperties::OperatorLeftBrace) {
		CompoundStatement();
		return true;
	}
	// While Do 是迭代语句
	else if (wordTable[p].property == EnumProperties::While ||
		wordTable[p].property == EnumProperties::Do) {
		IterationStatement();
		return true;
	}
	// 空语句 是表达式语句
	else if (wordTable[p].property == EnumProperties::Semicolon) {
		ExpressionStatement();
		return true;
	}
	// 标签语句和表达式语句需要回溯
	// 因此标签语句的入口处不能throw
	if (LabeledStatement()) {
		return true;
	}
	if (ExpressionStatement()) {
		return true;
	}
	// 都不是 不是语句
	else {
		p = nowP;
		return false;
	}
}

// func    标签语句识别
// param   
// return  
bool GrammaticalAnalysis::LabeledStatement() {
	// 保存当前指针
	int nowP = p;

	// Identifier : Statement
	if (wordTable[p].property == EnumProperties::Identifier) {
		p++;
		if (wordTable[p].property == EnumProperties::OperatorColon) {
			p++;
			if (Statement()) {
				return true;
			}
			throw(Exception("Not a Labeled Statement because Expect a Statement", wordTable[p]));
		}
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    复合语句识别
// param   
// return  
bool GrammaticalAnalysis::CompoundStatement() {
	// 保存当前指针
	int nowP = p;

	// { BlockItemList }
	if (wordTable[p].property == EnumProperties::OperatorLeftBrace) {
		p++;
		if (BlockItemList()) {
			if (wordTable[p].property == EnumProperties::OperatorRightBrace) {
				p++;
				return true;
			}
			throw(Exception("Not a Compound Statement because Expect a }", wordTable[p]));
		}
		throw(Exception("Not a Compound Statement because Expect a Block Item", wordTable[p]));
	}
	throw(Exception("Not a Compound Statement because Expect a {", wordTable[p]));
}

// func    块项目列表识别
// param   
// return  
bool GrammaticalAnalysis::BlockItemList() {
	// 保存当前指针
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

// func    块项目列表消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::BlockItemListEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// BlockItem BlockItemListEliminateLeft
	if (BlockItem()) {
		if (BlockItemListEliminateLeft()) {
			return true;
		}
		p = nowP;
		return false;
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    块项目识别
// param   
// return  
bool GrammaticalAnalysis::BlockItem() {
	// 保存当前指针
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

// func    表达式语句识别
// param   
// return  
bool GrammaticalAnalysis::ExpressionStatement() {
	// 保存当前指针
	int nowP = p;

	// ;
	if (wordTable[p].property == EnumProperties::Semicolon) {
		p++;
		return true;
	}
	// Expression;
	else if (Expression()) {
		if (wordTable[p].property == EnumProperties::Semicolon) {
			p++;
			return true;
		}
		throw(Exception("Not a Expression Statement because Exprect a ;", wordTable[p]));
	}
	else {
		p = nowP;
		return false;
	}
}

// func    选择语句识别
// param   
// return  
bool GrammaticalAnalysis::SelectionStatement() {
	// 保存当前指针
	int nowP = p;

	// If (Expression) Statement
	if (wordTable[p].property == EnumProperties::If) {
		p++;
		if (wordTable[p].property == EnumProperties::OperatorLeftRound) {
			p++;
			if (Expression()) {
				if (wordTable[p].property == EnumProperties::OperatorRightRound) {
					p++;
					if (Statement()) {
						// If (Expression) Statement Else Statement
						if (wordTable[p].property == EnumProperties::Else) {
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
					throw(Exception("Not a if Statement because Exprect a Statement", wordTable[p]));
				}
				throw(Exception("Not a if Statement because Exprect a )", wordTable[p]));
			}
			throw(Exception("Not a if Statement because Exprect a Expression", wordTable[p]));
		}
		throw(Exception("Not a if Statement because Exprect a (", wordTable[p]));
	}
	throw(Exception("Not a if Statement", wordTable[p]));
}

// func    迭代语句识别
// param   
// return  
bool GrammaticalAnalysis::IterationStatement() {
	// 保存当前指针
	int nowP = p;

	// While(Expression) Statement
	if (wordTable[p].property == EnumProperties::While) {
		p++;
		if (wordTable[p].property == EnumProperties::OperatorLeftRound) {
			p++;
			if (Expression()) {
				if (wordTable[p].property == EnumProperties::OperatorRightRound) {
					p++;
					if (Statement()) {
						return true;
					}
				}
				throw(Exception("Not a while Statement because Exprect a )", wordTable[p]));
			}
			throw(Exception("Not a while Statement because Exprect a Expression", wordTable[p]));
		}
		throw(Exception("Not a while Statement because Exprect a (", wordTable[p]));
	}
	// Do Statement While(Expression);
	if (wordTable[p].property == EnumProperties::Do) {
		p++;
		if (Statement()) {
			if (wordTable[p].property == EnumProperties::While) {
				p++;
				if (wordTable[p].property == EnumProperties::OperatorLeftRound) {
					p++;
					if (Expression()) {
						if (wordTable[p].property == EnumProperties::OperatorRightRound) {
							p++;
							return true;
						}
						throw(Exception("Not a do while Statement because Exprect a )", wordTable[p]));
					}
					throw(Exception("Not a do while Statement because Exprect a Expression", wordTable[p]));
				}
				throw(Exception("Not a do while Statement because Exprect a (", wordTable[p]));
			}
			throw(Exception("Not a do while Statement because Exprect a while", wordTable[p]));
		}
		throw(Exception("Not a do while Statement because Exprect a Statement", wordTable[p]));
	}
	throw(Exception("Not a IterationStatement Statement", wordTable[p]));
}

// func    跳转语句识别
// param   
// return  
bool GrammaticalAnalysis::JumpStatement() {
	// 保存当前指针
	int nowP = p;

	// Goto Identifier;
	if (wordTable[p].property == EnumProperties::Goto) {
		p++;
		if (wordTable[p].property == EnumProperties::Identifier) {
			p++;
			if (wordTable[p].property == EnumProperties::Semicolon) {
				p++;
				return true;
			}
			throw(Exception("Not a goto Statement because Exprect a ;", wordTable[p]));
		}
		throw(Exception("Not a goto Statement because Exprect a Identifier", wordTable[p]));
	}
	// Continue;
	else if (wordTable[p].property == EnumProperties::Continue) {
		p++;
		if (wordTable[p].property == EnumProperties::Semicolon) {
			p++;
			return true;
		}
		throw(Exception("Not a continue Statement because Exprect a ;", wordTable[p]));
	}
	// Break;
	else if (wordTable[p].property == EnumProperties::Break) {
		p++;
		if (wordTable[p].property == EnumProperties::Semicolon) {
			p++;
			return true;
		}
		throw(Exception("Not a break Statement because Exprect a ;", wordTable[p]));
	}
	// Return
	else if (wordTable[p].property == EnumProperties::Return) {
		p++;
		// Return;
		if (wordTable[p].property == EnumProperties::Semicolon) {
			p++;
			return true;
		}
		// Return Expression;
		else if (Expression()) {
			if (wordTable[p].property == EnumProperties::Semicolon) {
				p++;
				return true;
			}
		}
		throw(Exception("Not a Return Statement", wordTable[p]));
	}
	throw(Exception("Not a Jump Statement", wordTable[p]));
}

// func    翻译单元识别
// param   
// return  
bool GrammaticalAnalysis::TranslationUnit() {
	// 保存当前指针
	int nowP = p;

	// ExternalDeclaration TranslationUnitEliminateLeft
	if (ExternalDeclaration()) {
		if (TranslationUnitEliminateLeft()) {
			return true;
		}
	}

	throw(Exception("Not a Translation Unit", wordTable[p]));
}

// func    翻译单元消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::TranslationUnitEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// ExternalDeclaration TranslationUnitEliminateLeft
	if (ExternalDeclaration()) {
		if (TranslationUnitEliminateLeft()) {
			return true;
		}
		p = nowP;
		return false;
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    外部声明识别
// param   
// return  
bool GrammaticalAnalysis::ExternalDeclaration() {
	// 保存当前指针
	int nowP = p;
	int firstSemicolon = p;
	int firstLeftBrace = p;
	for (int i = p; true; i++) {
		if (wordTable[i].property == EnumProperties::Semicolon) {
			firstSemicolon = i;
			break;
		}
		if (wordTable[i].property == EnumProperties::TimeToStop) {
			firstSemicolon = -1;
			break;
		}
	}
	for (int i = p; true; i++) {
		if (wordTable[i].property == EnumProperties::OperatorLeftBrace) {
			firstLeftBrace = i;
			break;
		}
		if (wordTable[i].property == EnumProperties::TimeToStop) {
			firstLeftBrace = -1;
			break;
		}
	}
	if (firstSemicolon < 0 && firstLeftBrace < 0) {
		p = nowP;
		return false;
	}
	// 函数定义是 TypeName DirectDeclarator /* DeclarationList */ CompoundStatement
	// DeclarationList 以 TypeName 开头
	// CompoundStatement 以 { 开头
	// DirectDeclarator 以 Identifier 开头 后接 (ParameterList) DirectDeclaratorEliminateLeft
	// 因此 函数定义必然是 TypeName Identifier ( 开头
	// 且函数定义必然有 { 在 ; 的前面
	// FunctionDefinition
	if (firstSemicolon < 0 || firstLeftBrace < firstSemicolon) {
		if (FunctionDefinition()) {
			return true;
		}
	}
	// 声明是 TypeName InitDeclaratorList;
	// InitDeclaratorList 以 InitDeclarator 开头
	// InitDeclarator 以 Identifier 开头
	// 因此 声明必然有 ; 在 { 的前面
	// Declaration
	else if (firstLeftBrace < 0 || firstSemicolon < firstLeftBrace) {
		if (Declaration()) {
			return true;
		}
	}
	throw(Exception("Not a Function Definition or a Declaration", wordTable[p]));
}

// func    函数定义识别
// param   
// return  
bool GrammaticalAnalysis::FunctionDefinition() {
	// 保存当前指针
	int nowP = p;

	// TypeName DirectDeclarator /* DeclarationList */ CompoundStatement
	if (IsTypeName(wordTable[p])) {
		p++;
		if (DirectDeclarator()) {
			// 必然以 { 开头
			// TypeName DirectDeclarator CompoundStatement
			if (wordTable[p].property == EnumProperties::OperatorLeftBrace) {
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
		throw(Exception("Not a Direct Declarator", wordTable[p]));
	}
	throw(Exception("Not a Function Definition because Expect a Type Name", wordTable[p]));
}

// func    声明列表识别
// param   
// return  
bool GrammaticalAnalysis::DeclarationList() {
	// 保存当前指针
	int nowP = p;

	// Declaration DeclarationListEliminateLeft
	if (Declaration()) {
		if (DeclarationListEliminateLeft()) {
			return true;
		}
	}
	throw(Exception("Not a Declaration", wordTable[p]));
}

// func    声明列表消除左递归识别
// param   
// return  
bool GrammaticalAnalysis::DeclarationListEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// Declaration DeclarationListEliminateLeft
	if (Declaration()) {
		if (DeclarationListEliminateLeft()) {
			return true;
		}
		p = nowP;
		return false;
	}
	// 空
	else {
		p = nowP;
		return true;
	}
}





// func    词素是否是类型
// param   
// return  
bool GrammaticalAnalysis::IsTypeName(Word w) {
	if (w.property == EnumProperties::Bool ||
		w.property == EnumProperties::Char ||
		w.property == EnumProperties::Int ||
		w.property == EnumProperties::Float ||
		w.property == EnumProperties::Double ||
		w.property == EnumProperties::Enum) {
		return true;
	}
	else {
		return false;
	}
}

// func    词素是否是数字字面量
// param   
// return  
bool GrammaticalAnalysis::IsNumberLiteralValue(Word w) {
	if (w.property == EnumProperties::TenIntNumber ||
		w.property == EnumProperties::EightIntNumber ||
		w.property == EnumProperties::SixteenIntNumber ||
		w.property == EnumProperties::FloatNumber ||
		w.property == EnumProperties::FloatEeNumber ||
		w.property == EnumProperties::CharNumber) {
		return true;
	}
	else {
		return false;
	}
}

// func    词素是否是字面量
// param   
// return  
bool GrammaticalAnalysis::IsLiteralValue(Word w) {
	if (IsNumberLiteralValue(w) ||
		w.property == EnumProperties::String) {
		return true;
	}
	else {
		return false;
	}
}