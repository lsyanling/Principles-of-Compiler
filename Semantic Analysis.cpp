#include"head.h"

SemanticAnalysis::SemanticAnalysis(std::vector<Word>& w) : GrammaticalAnalysis(w) {
	identifierTablePointer = new IdentifierTable();
	try {
		TranslationUnit();
	}
	catch (Exception e) {
		std::cout << "Grammatical Error in line " << e.word.line << " " << e.str << std::endl;
	}
	for (size_t i = 0; i < fourTable.table.size(); i++) {
		std::cout << "\t" << std::setw(8) << i << std::setw(16) << fourTable.table[i].op <<
			std::setw(12) << fourTable.table[i].op1 << std::setw(12) << fourTable.table[i].op2 <<
			std::setw(12) << fourTable.table[i].dest << std::endl;
	}
}

// 查找标识符 如果存在就返回表中标识符的引用
auto SemanticAnalysis::SearchIdentifier(IdentifierTable* idTablePointer, std::string value) {
	// 指针不空
	while (idTablePointer != nullptr) {
		// 遍历当前表
		for (auto& id : idTablePointer->table) {
			if (id.value == value) {
				return std::tuple<bool, Identifier&>(true, id);
			}
		}
		// 不在当前表中 向上查找
		idTablePointer = idTablePointer->fatherTablePointer;
	}
	// 标识符未定义
	Identifier id = Identifier(EnumWordProperties::Unknow, "");
	return std::tuple<bool, Identifier&>(false, id);
}

// 在当前表中查找标识符 如果存在就返回表中标识符的引用
auto SemanticAnalysis::SearchIdentifierInCurrentTable(IdentifierTable* idTablePointer, std::string value) {

	// 遍历当前表
	for (auto& id : idTablePointer->table) {
		if (id.value == value) {
			return std::tuple<bool, Identifier&>(true, id);
		}
	}
	// 不在当前表中 标识符未定义
	Identifier id = Identifier(EnumWordProperties::Unknow, "");
	return std::tuple<bool, Identifier&>(false, id);
}

// func    初始表达式识别
// param   
// return  
bool SemanticAnalysis::PrimaryExpression() {
	// 保存当前指针
	int nowP = p;

	// Identifier
	if (wordTable[p].property == EnumWordProperties::Identifier) {
		// 语义动作
		{
			// 首先查找标识符
			auto [returnBool, id] = SearchIdentifier(identifierTablePointer, wordTable[p].word);
			// 找到了
			if (returnBool) {
				// 返回值入求值栈
				returnValueStack.returnValueStack.push(&id);
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::Identifier;
				returnValueStack.lastReturnValueType = id.wordProperties;
			}
			// 未定义
			else {
				// 语义错误
				throw(Exception("Identifier Not defined", wordTable[p], 1));
			}
		}
		p++;
		return true;
	}
	// 数字字面量
	else if (wordTable[p].property == EnumWordProperties::TenIntNumber ||
		wordTable[p].property == EnumWordProperties::EightIntNumber ||
		wordTable[p].property == EnumWordProperties::SixteenIntNumber ||
		wordTable[p].property == EnumWordProperties::FloatNumber ||
		wordTable[p].property == EnumWordProperties::FloatEeNumber ||
		wordTable[p].property == EnumWordProperties::CharNumber ||
		wordTable[p].property == EnumWordProperties::String) {
		// 语义动作
			{
				// 返回值入求值栈
				returnValueStack.returnValueStack.push(new LiteralValue(wordTable[p].property, wordTable[p].word));
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::LiteralValue;
				returnValueStack.lastReturnValueType = wordTable[p].property;
			}
			p++;
			return true;
	}
	// (Expression)
	else if (wordTable[p].property == EnumWordProperties::OperatorLeftRound) {
		p++;
		if (Expression()) {
			if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
				p++;
				// 语义动作
				{
					// 从栈顶取值 然后入栈 这里省略步骤
				}
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

// func    后缀表达式识别
// param   
// return  
bool SemanticAnalysis::PostfixExpression() {
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
bool SemanticAnalysis::PostfixExpressionEliminateLeft() {
	// 保存当前指针
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
		// 语义动作
		{
			bool isFunction = false;
			// 取栈顶 查看是否是函数
			auto lastReturnValue = returnValueStack.returnValueStack.top();
			for (auto& i : functionTable.functionTable) {
				if (i.value == lastReturnValue->value) {
					isFunction = true;
					break;
				}
			}
			if (!isFunction) {
				// 不是函数
				throw(Exception("Not a Function", wordTable[p], 1));
			}
			// 函数调用表达式 下面是实参 除void类型外每个实参入栈
		}
		if (ArgumentExpressionList()) {
			if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
				p++;
				// 语义动作
				{
					// 取函数名
					auto lastReturnValue = returnValueStack.returnValueStack.top();
					// 函数名出栈
					returnValueStack.returnValueStack.pop();

					// 取函数返回值类型
					EnumWordProperties e;
					for (auto& i : functionTable.functionTable) {
						if (i.value == lastReturnValue->value) {
							// 参数计算重置
							i.currentParameter = 0;
							e = i.returnType;
							break;
						}
					}

					// 实参入栈完毕 调用函数
					fourTable.AddFour("push", std::to_string(fourTable.nowFourLine + 2), "", "");
					fourTable.nowFourLine++;
					fourTable.AddFour("call", lastReturnValue->value, "", "");
					fourTable.nowFourLine++;
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					// 调用完毕 取返回值
					fourTable.AddFour("pop", "", "", tempVarName);
					fourTable.nowFourLine++;

					// 入求值栈 返回值类型即临时变量类型 实际是函数返回值类型
					returnValueStack.returnValueStack.push(new TempVariable(e, tempVarName));
					// 设置最后一次返回值类型
					returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
					returnValueStack.lastReturnValueType = e;
				}
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
		// 语义动作
		{
			// 首先看栈顶是不是标识符
			if (returnValueStack.lastReturnWordType != ReturnValueStack::LastReturnWordType::Identifier) {
				// 语义错误 后缀表达式要求是一个左值 当前情况下只有标识符是左值
				throw(Exception("Should be a Left-Value before ++", wordTable[p], 1));
			}
			// 栈顶是标识符 出栈
			auto lastReturnValue = returnValueStack.returnValueStack.top();
			returnValueStack.returnValueStack.pop();
			// 看值是不是字符串或函数
			if (lastReturnValue->getWordProperties() == EnumWordProperties::String || lastReturnValue->getWordProperties() == EnumWordProperties::Function) {
				// 语义错误 字符串和函数不能执行后缀++
				throw(Exception("Should be a Number before ++", wordTable[p], 1));
			}
			// 取临时变量 生成四元式
			std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
			tempVariableTablePointer++;
			fourTable.AddFour("=", lastReturnValue->value, "", tempVarName);
			fourTable.nowFourLine++;
			fourTable.AddFour("+", lastReturnValue->value, "1", lastReturnValue->value);
			fourTable.nowFourLine++;

			// 返回值入求值栈 返回值类型即临时变量类型与标识符类型相同
			returnValueStack.returnValueStack.push(new TempVariable(lastReturnValue->getWordProperties(), tempVarName));
			// 设置最后一次返回值类型
			returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
			returnValueStack.lastReturnValueType = lastReturnValue->getWordProperties();
		}
		p++;
		if (PostfixExpressionEliminateLeft()) {
			return true;
		}
		throw(Exception("Not a ++ Postfix Expression because Expect a Postfix Expression", wordTable[p], 0));
	}
	// --PostfixExpressionEliminateLeft
	else if (wordTable[p].property == EnumWordProperties::OperatorSubSubtract) {
		// 语义动作
		{
			// 首先看栈顶是不是标识符
			if (returnValueStack.lastReturnWordType != ReturnValueStack::LastReturnWordType::Identifier) {
				// 语义错误 后缀表达式要求是一个左值 当前情况下只有标识符是左值
				throw(Exception("Should be a Left-Value before --", wordTable[p], 1));
			}
			// 栈顶是标识符 出栈
			auto lastReturnValue = returnValueStack.returnValueStack.top();
			returnValueStack.returnValueStack.pop();
			// 看值是不是字符串或函数
			if (lastReturnValue->getWordProperties() == EnumWordProperties::String || lastReturnValue->getWordProperties() == EnumWordProperties::Function) {
				// 语义错误 字符串和函数不能执行后缀--
				throw(Exception("Should be a Number before --", wordTable[p], 1));
			}
			// 取临时变量 生成四元式
			std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
			tempVariableTablePointer++;
			fourTable.AddFour("=", lastReturnValue->value, "", tempVarName);
			fourTable.nowFourLine++;
			fourTable.AddFour("-", lastReturnValue->value, "1", lastReturnValue->value);
			fourTable.nowFourLine++;

			// 返回值入求值栈 返回值类型即临时变量类型与标识符类型相同
			returnValueStack.returnValueStack.push(new TempVariable(lastReturnValue->getWordProperties(), tempVarName));
			// 设置最后一次返回值类型
			returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
			returnValueStack.lastReturnValueType = lastReturnValue->getWordProperties();
		}
		p++;
		if (PostfixExpressionEliminateLeft()) {
			return true;
		}
		throw(Exception("Not a -- Postfix Expression because Expect a Postfix Expression", wordTable[p], 0));
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
bool SemanticAnalysis::ArgumentExpressionList() {
	// 保存当前指针
	int nowP = p;

	// AssignmentExpression ArgumentExpressionListEliminateLeft
	if (AssignmentExpression()) {
		// 语义动作
		{
			// 取实参值
			auto lastReturnValue2 = returnValueStack.returnValueStack.top();
			returnValueStack.returnValueStack.pop();
			// 取形参类型
			auto lastReturnValue1 = returnValueStack.returnValueStack.top();
			EnumWordProperties e;
			for (auto& i : functionTable.functionTable) {
				if (i.value == lastReturnValue1->value) {
					e = i.parameterType[i.currentParameter++];
					break;
				}
			}
			// 类型转换 不写了
			//if (returnValueStack.lastReturnValueType != e) {
			//	throw(Exception("实参的类型转换 暂时不想写了 就当转换了吧", wordTable[p], 0));
			//}

			// 生成四元式 从左向右压栈
			fourTable.AddFour("push", lastReturnValue2->value, "", "");
			fourTable.nowFourLine++;
		}
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
bool SemanticAnalysis::ArgumentExpressionListEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// ,AssignmentExpression ArgumentExpressionListEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorComma) {
		p++;
		if (AssignmentExpression()) {
			// 语义动作
			{
				// 取实参值
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 取形参类型
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				EnumWordProperties e;
				for (auto& i : functionTable.functionTable) {
					if (i.value == lastReturnValue1->value) {
						e = i.parameterType[i.currentParameter++];
						break;
					}
				}
				// 类型转换 不写了
				//if (returnValueStack.lastReturnValueType != e) {
				//	throw(Exception("实参的类型转换 暂时不想写了 就当转换了吧", wordTable[p], 0));
				//}

				// 生成四元式 从左向右压栈
				fourTable.AddFour("push", lastReturnValue2->value, "", "");
				fourTable.nowFourLine++;
			}
			if (ArgumentExpressionListEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a Argument Expression List because Expect a Assignment Expression", wordTable[p], 0));
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
bool SemanticAnalysis::UnaryExpression() {
	// 保存当前指针
	int nowP = p;

	// PostfixExpression
	if (PostfixExpression()) {
		return true;
	}
	// ++UnaryExpression
	else if (wordTable[p].property == EnumWordProperties::OperatorAddAdd) {
		p++;
		if (UnaryExpression()) {
			// 语义动作
			{
				// 首先看栈顶是不是标识符
				if (returnValueStack.lastReturnWordType != ReturnValueStack::LastReturnWordType::Identifier) {
					// 语义错误 前缀++表达式要求是一个左值 当前情况下只有标识符是左值
					throw(Exception("Should be a Left-Value after ++", wordTable[p], 1));
				}
				// 栈顶是标识符 出栈
				auto lastReturnValue = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 看值是不是字符串或函数
				if (lastReturnValue->getWordProperties() == EnumWordProperties::String || lastReturnValue->getWordProperties() == EnumWordProperties::Function) {
					// 语义错误 字符串和函数不能执行前缀++
					throw(Exception("Should be a Number after ++", wordTable[p], 1));
				}
				// 生成四元式
				fourTable.AddFour("+", lastReturnValue->value, "1", lastReturnValue->value);
				fourTable.nowFourLine++;

				// 返回值入求值栈 返回值即当前标识符
				returnValueStack.returnValueStack.push(new Identifier(lastReturnValue->getWordProperties(), lastReturnValue->value));
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::Identifier;
				returnValueStack.lastReturnValueType = lastReturnValue->getWordProperties();
			}
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
			// 语义动作
			{
				// 首先看栈顶是不是标识符
				if (returnValueStack.lastReturnWordType != ReturnValueStack::LastReturnWordType::Identifier) {
					// 语义错误 前缀--表达式要求是一个左值 当前情况下只有标识符是左值
					throw(Exception("Should be a Left-Value after --", wordTable[p], 1));
				}
				// 栈顶是标识符 出栈
				auto lastReturnValue = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 看值是不是字符串或函数
				if (lastReturnValue->getWordProperties() == EnumWordProperties::String || lastReturnValue->getWordProperties() == EnumWordProperties::Function) {
					// 语义错误 字符串和函数不能执行前缀--
					throw(Exception("Should be a Number after --", wordTable[p], 1));
				}
				// 生成四元式
				fourTable.AddFour("-", lastReturnValue->value, "1", lastReturnValue->value);
				fourTable.nowFourLine++;

				// 返回值入求值栈 返回值即当前标识符
				returnValueStack.returnValueStack.push(new Identifier(lastReturnValue->getWordProperties(), lastReturnValue->value));
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::Identifier;
				returnValueStack.lastReturnValueType = lastReturnValue->getWordProperties();
			}
			return true;
		}
		throw(Exception("Not a -- Unary Expression because Expect a Unary Expression", wordTable[p], 0));
		p = nowP;
		return false;
	}
	// +CastExpression
	else if (wordTable[p].property == EnumWordProperties::OperatorAdd) {
		p++;
		if (CastExpression()) {
			// 语义动作
			{
				// 首先出栈
				auto lastReturnValue = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 看栈顶是不是 字符串或函数
				if (lastReturnValue->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue->getWordProperties() == EnumWordProperties::Function) {
					// 语义错误 单目+表达式要求是一个数值
					throw(Exception("Should be a Number after +", wordTable[p], 1));
				}
				// 是数值 查看数值类型 取临时变量 生成四元式
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				fourTable.AddFour("+", lastReturnValue->value, "", tempVarName);
				fourTable.nowFourLine++;
				tempVariableTablePointer++;

				// 返回值入求值栈
				returnValueStack.returnValueStack.push(new TempVariable(lastReturnValue->getWordProperties(), tempVarName));
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = lastReturnValue->getWordProperties();
			}
			return true;
		}
		p = nowP;
		return false;
	}
	// -CastExpression
	else if (wordTable[p].property == EnumWordProperties::OperatorSubtract) {
		p++;
		if (CastExpression()) {
			// 语义动作
			{
				// 首先出栈
				auto lastReturnValue = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 看栈顶是不是 字符串或函数
				if (lastReturnValue->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue->getWordProperties() == EnumWordProperties::Function) {
					// 语义错误 单目+表达式要求是一个数值
					throw(Exception("Should be a Number after +", wordTable[p], 1));
				}
				// 是数值 查看数值类型 取临时变量 生成四元式
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				fourTable.AddFour("-", "0", lastReturnValue->value, tempVarName);
				fourTable.nowFourLine++;
				tempVariableTablePointer++;

				// 返回值入求值栈
				returnValueStack.returnValueStack.push(new TempVariable(lastReturnValue->getWordProperties(), tempVarName));
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = lastReturnValue->getWordProperties();
			}
			return true;
		}
		p = nowP;
		return false;
	}
	// !CastExpression
	else if (wordTable[p].property == EnumWordProperties::OperatorNot) {
		p++;
		if (CastExpression()) {
			// 语义动作
			{
				// 首先出栈
				auto lastReturnValue = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 看栈顶是不是 字符串或函数
				if (lastReturnValue->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue->getWordProperties() == EnumWordProperties::Function) {
					// 字符串和函数取反必为假 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("=", "0", "", tempVarName);
					fourTable.nowFourLine++;

					// 返回值入求值栈
					returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
					// 设置最后一次返回值类型
					returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
					returnValueStack.lastReturnValueType = EnumWordProperties::Bool;
				}
				else {
					// 是数值 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("jz", lastReturnValue->value, "", std::to_string(fourTable.nowFourLine + 3));
					fourTable.nowFourLine++;
					fourTable.AddFour("=", "0", "", tempVarName);
					fourTable.nowFourLine++;
					fourTable.AddFour("jmp", "", "", std::to_string(fourTable.nowFourLine + 2));
					fourTable.nowFourLine++;
					fourTable.AddFour("=", "1", "", tempVarName);
					fourTable.nowFourLine++;

					// 返回值入求值栈
					returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
					// 设置最后一次返回值类型
					returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
					returnValueStack.lastReturnValueType = EnumWordProperties::Bool;
				}
			}
			return true;
		}
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

// func    单目运算符识别
// param   
// return  
bool SemanticAnalysis::UnaryOperator() {
	// 保存当前指针
	int nowP = p;

	if (wordTable[p].property == EnumWordProperties::OperatorMultiply ||
		wordTable[p].property == EnumWordProperties::OperatorBitAnd ||
		wordTable[p].property == EnumWordProperties::OperatorBitNot) {
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
bool SemanticAnalysis::CastExpression() {
	// 保存当前指针
	int nowP = p;

	// UnaryExpression
	if (UnaryExpression()) {
		return true;
	}
	// (TypeName)CastExpression
	else if (wordTable[p].property == EnumWordProperties::OperatorLeftRound) {
		p++;
		if (IsTypeName(wordTable[p])) {
			// 语义动作
			{
				// 保存当前类型转换类型
				castTypeName = wordTable[p].property;
			}
			p++;
			if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
				p++;
				if (CastExpression()) {
					// 语义动作
					{
						// 首先出栈
						auto lastReturnValue = returnValueStack.returnValueStack.top();
						returnValueStack.returnValueStack.pop();
						// 取临时变量
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						// 判断目标转换类型
						if (castTypeName == EnumWordProperties::Bool) {
							// 看栈顶是不是 字符串或函数
							if (lastReturnValue->getWordProperties() == EnumWordProperties::String ||
								lastReturnValue->getWordProperties() == EnumWordProperties::Function) {
								// 字符串和函数转换为 Bool 必为真 取临时变量 生成四元式
								fourTable.AddFour("=", "1", "", tempVarName);
								fourTable.nowFourLine++;
							}
							else {
								fourTable.AddFour("jz", lastReturnValue->value, "", std::to_string(fourTable.nowFourLine + 3));
								fourTable.nowFourLine++;
								fourTable.AddFour("=", "1", "", tempVarName);
								fourTable.nowFourLine++;
								fourTable.AddFour("jmp", "", "", std::to_string(fourTable.nowFourLine + 2));
								fourTable.nowFourLine++;
								fourTable.AddFour("=", "0", "", tempVarName);
								fourTable.nowFourLine++;
							}
							// 返回值入求值栈
							returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
							// 设置最后一次返回值类型
							returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
							returnValueStack.lastReturnValueType = EnumWordProperties::Bool;
						}
						// 目标转换类型是 Int
						else if (castTypeName == EnumWordProperties::Int) {
							// 最后一次返回类型是 Bool 或 Char 或 Int
							if (returnValueStack.lastReturnValueType == EnumWordProperties::Bool ||
								IsInt(returnValueStack.lastReturnValueType)) {
								fourTable.AddFour("=", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// 最后一次返回类型是 Float
							else if (IsFloat(returnValueStack.lastReturnValueType)) {
								fourTable.AddFour("FloatToInt", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// 最后一次返回类型是 String
							else if (returnValueStack.lastReturnValueType == EnumWordProperties::String) {
								fourTable.AddFour("StringToInt", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// 最后一次返回类型是 Function
							else if (returnValueStack.lastReturnValueType == EnumWordProperties::Function) {
								fourTable.AddFour("FunctionToInt", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// 返回值入求值栈
							returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Int, tempVarName));
							// 设置最后一次返回值类型
							returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
							returnValueStack.lastReturnValueType = EnumWordProperties::Int;
						}
						// 目标转换类型是 Float
						else if (castTypeName == EnumWordProperties::Float) {
							// 最后一次返回类型是 Bool 或 Char 或 Int
							if (returnValueStack.lastReturnValueType == EnumWordProperties::Bool ||
								IsInt(returnValueStack.lastReturnValueType)) {
								fourTable.AddFour("IntToFloat", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// 最后一次返回类型是 Float
							else if (IsFloat(returnValueStack.lastReturnValueType)) {
								fourTable.AddFour("=", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// 最后一次返回类型是 String
							else if (returnValueStack.lastReturnValueType == EnumWordProperties::String) {
								// 语义错误 String 不能转换成 Float
								throw(Exception("Invalid Cast From String to Float", wordTable[p], 1));
							}
							// 最后一次返回类型是 Function
							else if (returnValueStack.lastReturnValueType == EnumWordProperties::Function) {
								// 语义错误 Function 不能转换成 Float
								throw(Exception("Invalid Cast From Function to Float", wordTable[p], 1));
							}
							// 返回值入求值栈
							returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Float, tempVarName));
							// 设置最后一次返回值类型
							returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
							returnValueStack.lastReturnValueType = EnumWordProperties::Float;
						}
						// 目标转换类型是 Char
						else if (castTypeName == EnumWordProperties::Char) {
							// 最后一次返回类型是 Bool 或 Char
							if (returnValueStack.lastReturnValueType == EnumWordProperties::Bool ||
								returnValueStack.lastReturnValueType == EnumWordProperties::Char ||
								returnValueStack.lastReturnValueType == EnumWordProperties::CharNumber) {
								fourTable.AddFour("=", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// 最后一次返回类型是 Int
							if (IsInt(returnValueStack.lastReturnValueType)) {
								fourTable.AddFour("IntToChar", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// 最后一次返回类型是 Float
							else if (IsFloat(returnValueStack.lastReturnValueType)) {
								fourTable.AddFour("FloatToChar", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// 最后一次返回类型是 String
							else if (returnValueStack.lastReturnValueType == EnumWordProperties::String) {
								fourTable.AddFour("StringToChar", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// 最后一次返回类型是 Function
							else if (returnValueStack.lastReturnValueType == EnumWordProperties::Function) {
								fourTable.AddFour("FunctionToChar", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// 返回值入求值栈
							returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Char, tempVarName));
							// 设置最后一次返回值类型
							returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
							returnValueStack.lastReturnValueType = EnumWordProperties::Char;
						}
					}
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

// func    乘除表达式识别
// param   
// return  
bool SemanticAnalysis::MultiplicativeExpression() {
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
bool SemanticAnalysis::MultiplicativeExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// *CastExpression MultiplicativeExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorMultiply) {
		p++;
		if (CastExpression()) {
			// 语义动作
			{
				// 已经调用了两次 CastExpression() 栈中存着两个返回值 将它们相乘
				// 出栈 查看右边的类型
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 出栈 查看左边的类型
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String 和 Function 不能参与运算
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// 语义错误
					throw(Exception("Should be a Number To * Multiply", wordTable[p], 1));
				}
				// 类型提升
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// 不需要提升
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// 没有 Float 值
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// 取临时变量 生成四元式
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour("*", lastReturnValue1->value, lastReturnValue2->value, tempVarName);
				fourTable.nowFourLine++;

				// 返回值入求值栈
				returnValueStack.returnValueStack.push(new TempVariable(lastReturnValue1->getWordProperties(), tempVarName));
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = lastReturnValue1->getWordProperties();
			}
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
			// 语义动作
			{
				// 已经调用了两次 CastExpression() 栈中存着两个返回值 将它们相除
				// 出栈 查看右边的类型
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 出栈 查看左边的类型
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String 和 Function 不能参与运算
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// 语义错误
					throw(Exception("Should be a Number To / Divide", wordTable[p], 1));
				}
				// 类型提升
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// 不需要提升
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// 没有 Float 值
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// 取临时变量 生成四元式
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour("/", lastReturnValue1->value, lastReturnValue2->value, tempVarName);
				fourTable.nowFourLine++;

				// 返回值入求值栈
				returnValueStack.returnValueStack.push(new TempVariable(lastReturnValue1->getWordProperties(), tempVarName));
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = lastReturnValue1->getWordProperties();
			}
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
			// 语义动作
			{
				// 已经调用了两次 CastExpression() 栈中存着两个返回值 将它们相乘
				// 出栈 查看右边的类型
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 出栈 查看左边的类型
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String 和 Function 不能参与运算
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// 语义错误
					throw(Exception("Should be a Number To % Mod", wordTable[p], 1));
				}
				// 类型提升
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// 语义错误 取模操作必须是 Int
					throw(Exception("Should be a Int Value To % Mod", wordTable[p], 1));
				}
				// 没有 Float 值
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// 取临时变量 生成四元式
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour("%", lastReturnValue1->value, lastReturnValue2->value, tempVarName);
				fourTable.nowFourLine++;

				// 返回值入求值栈
				returnValueStack.returnValueStack.push(new TempVariable(lastReturnValue1->getWordProperties(), tempVarName));
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = EnumWordProperties::Int;
			}
			if (MultiplicativeExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a % Multiplicative Expression because Expect a Cast Expression", wordTable[p], 0));
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
bool SemanticAnalysis::AdditiveExpression() {
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
bool SemanticAnalysis::AdditiveExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// +MultiplicativeExpression AdditiveExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorAdd) {
		p++;
		if (MultiplicativeExpression()) {
			// 语义动作
			{
				// 出栈 查看右边的类型
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 出栈 查看左边的类型
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String 和 Function 不能参与运算
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// 语义错误
					throw(Exception("Should be a Number To + Add", wordTable[p], 1));
				}
				// 类型提升
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// 不需要提升
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// 没有 Float 值
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// 取临时变量 生成四元式
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour("+", lastReturnValue1->value, lastReturnValue2->value, tempVarName);
				fourTable.nowFourLine++;

				// 返回值入求值栈
				returnValueStack.returnValueStack.push(new TempVariable(lastReturnValue1->getWordProperties(), tempVarName));
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = lastReturnValue1->getWordProperties();
			}
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
			// 语义动作
			{
				// 出栈 查看右边的类型
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 出栈 查看左边的类型
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String 和 Function 不能参与运算
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// 语义错误
					throw(Exception("Should be a Number To - Subtract", wordTable[p], 1));
				}
				// 类型提升
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// 不需要提升
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// 没有 Float 值
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// 取临时变量 生成四元式
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour("-", lastReturnValue1->value, lastReturnValue2->value, tempVarName);
				fourTable.nowFourLine++;

				// 返回值入求值栈
				returnValueStack.returnValueStack.push(new TempVariable(lastReturnValue1->getWordProperties(), tempVarName));
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = lastReturnValue1->getWordProperties();
			}
			if (AdditiveExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a - Additive Expression because Expect a Multiplicative Expression", wordTable[p], 0));
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
bool SemanticAnalysis::ShiftExpression() {
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
bool SemanticAnalysis::ShiftExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// << AdditiveExpression ShiftExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorLeftShift) {
		p++;
		if (AdditiveExpression()) {
			// 语义动作
			{
				// 出栈 查看右边的类型
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 出栈 查看左边的类型
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String 和 Function 不能参与运算
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// 语义错误
					throw(Exception("Should be a Number To << Shift", wordTable[p], 1));
				}
				// 类型提升
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// 语义错误 取模操作必须是 Int
					throw(Exception("Should be a Int Value To << Shift", wordTable[p], 1));
				}
				// 没有 Float 值
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// 取临时变量 生成四元式
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour("<<", lastReturnValue1->value, lastReturnValue2->value, tempVarName);
				fourTable.nowFourLine++;

				// 返回值入求值栈
				returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Int, tempVarName));
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = EnumWordProperties::Int;
			}
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
			// 语义动作
			{
				// 出栈 查看右边的类型
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 出栈 查看左边的类型
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String 和 Function 不能参与运算
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// 语义错误
					throw(Exception("Should be a Number To >> Shift", wordTable[p], 1));
				}
				// 类型提升
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// 语义错误 取模操作必须是 Int
					throw(Exception("Should be a Int Value To >> Shift", wordTable[p], 1));
				}
				// 没有 Float 值
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// 取临时变量 生成四元式
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour(">>", lastReturnValue1->value, lastReturnValue2->value, tempVarName);
				fourTable.nowFourLine++;

				// 返回值入求值栈
				returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Int, tempVarName));
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = EnumWordProperties::Int;
			}
			if (ShiftExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a >> Expression because Expect a Additive Expression", wordTable[p], 0));
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
bool SemanticAnalysis::RelationalExpression() {
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
bool SemanticAnalysis::RelationalExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// < ShiftExpression RelationalExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorSmallerThan) {
		p++;
		if (ShiftExpression()) {
			// 语义动作
			{
				// 出栈 查看右边的类型
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 出栈 查看左边的类型
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String 和 Function 不能参与运算
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// 语义错误
					throw(Exception("Should be a Number To Compare <", wordTable[p], 1));
				}
				// 类型提升
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// 不需要提升
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// 没有 Float 值
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// 取临时变量 生成四元式
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour("j<", lastReturnValue1->value, lastReturnValue2->value, std::to_string(fourTable.nowFourLine + 3));
				fourTable.nowFourLine++;
				fourTable.AddFour("=", "0", "", tempVarName);
				fourTable.nowFourLine++;
				fourTable.AddFour("jmp", "", "", std::to_string(fourTable.nowFourLine + 2));
				fourTable.nowFourLine++;
				fourTable.AddFour("=", "1", "", tempVarName);
				fourTable.nowFourLine++;

				// 返回值入求值栈
				returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = EnumWordProperties::Bool;
			}
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
			// 语义动作
			{
				// 出栈 查看右边的类型
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 出栈 查看左边的类型
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String 和 Function 不能参与运算
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// 语义错误
					throw(Exception("Should be a Number To Compare >", wordTable[p], 1));
				}
				// 类型提升
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// 不需要提升
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// 没有 Float 值
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// 取临时变量 生成四元式
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour("j>", lastReturnValue1->value, lastReturnValue2->value, std::to_string(fourTable.nowFourLine + 3));
				fourTable.nowFourLine++;
				fourTable.AddFour("=", "0", "", tempVarName);
				fourTable.nowFourLine++;
				fourTable.AddFour("jmp", "", "", std::to_string(fourTable.nowFourLine + 2));
				fourTable.nowFourLine++;
				fourTable.AddFour("=", "1", "", tempVarName);
				fourTable.nowFourLine++;

				// 返回值入求值栈
				returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = EnumWordProperties::Bool;
			}
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
			// 语义动作
			{
				// 出栈 查看右边的类型
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 出栈 查看左边的类型
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String 和 Function 不能参与运算
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// 语义错误
					throw(Exception("Should be a Number To Compare <=", wordTable[p], 1));
				}
				// 类型提升
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// 不需要提升
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// 没有 Float 值
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// 取临时变量 生成四元式
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour("j<=", lastReturnValue1->value, lastReturnValue2->value, std::to_string(fourTable.nowFourLine + 3));
				fourTable.nowFourLine++;
				fourTable.AddFour("=", "0", "", tempVarName);
				fourTable.nowFourLine++;
				fourTable.AddFour("jmp", "", "", std::to_string(fourTable.nowFourLine + 2));
				fourTable.nowFourLine++;
				fourTable.AddFour("=", "1", "", tempVarName);
				fourTable.nowFourLine++;

				// 返回值入求值栈
				returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = EnumWordProperties::Bool;
			}
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
			// 语义动作
			{
				// 出栈 查看右边的类型
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 出栈 查看左边的类型
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String 和 Function 不能参与运算
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// 语义错误
					throw(Exception("Should be a Number To Compare >=", wordTable[p], 1));
				}
				// 类型提升
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// 不需要提升
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// 没有 Float 值
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// 取临时变量 生成四元式
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour("j>=", lastReturnValue1->value, lastReturnValue2->value, std::to_string(fourTable.nowFourLine + 3));
				fourTable.nowFourLine++;
				fourTable.AddFour("=", "0", "", tempVarName);
				fourTable.nowFourLine++;
				fourTable.AddFour("jmp", "", "", std::to_string(fourTable.nowFourLine + 2));
				fourTable.nowFourLine++;
				fourTable.AddFour("=", "1", "", tempVarName);
				fourTable.nowFourLine++;

				// 返回值入求值栈
				returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = EnumWordProperties::Bool;
			}
			if (RelationalExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a >= Relational Expression because Expect a Shift Expression", wordTable[p], 0));
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
bool SemanticAnalysis::EqualityExpression() {
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
bool SemanticAnalysis::EqualityExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// == RelationalExpression EqualityExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorEqual) {
		p++;
		if (RelationalExpression()) {
			// 语义动作
			{
				// 出栈 查看右边的类型
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 出栈 查看左边的类型
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String 和 Function 不能参与运算
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// 语义错误
					throw(Exception("Should be a Number To Compare ==", wordTable[p], 1));
				}
				// 类型提升
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// 不需要提升
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// 没有 Float 值
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// 取临时变量 生成四元式
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour("j==", lastReturnValue1->value, lastReturnValue2->value, std::to_string(fourTable.nowFourLine + 3));
				fourTable.nowFourLine++;
				fourTable.AddFour("=", "0", "", tempVarName);
				fourTable.nowFourLine++;
				fourTable.AddFour("jmp", "", "", std::to_string(fourTable.nowFourLine + 2));
				fourTable.nowFourLine++;
				fourTable.AddFour("=", "1", "", tempVarName);
				fourTable.nowFourLine++;

				// 返回值入求值栈
				returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = EnumWordProperties::Bool;
			}
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
			// 语义动作
			{
				// 出栈 查看右边的类型
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// 出栈 查看左边的类型
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String 和 Function 不能参与运算
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// 语义错误
					throw(Exception("Should be a Number To Compare !=", wordTable[p], 1));
				}
				// 类型提升
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// 不需要提升
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// 取临时变量 生成四元式
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// 没有 Float 值
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// 取临时变量 生成四元式
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// 取临时变量 生成四元式
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour("j!=", lastReturnValue1->value, lastReturnValue2->value, std::to_string(fourTable.nowFourLine + 3));
				fourTable.nowFourLine++;
				fourTable.AddFour("=", "0", "", tempVarName);
				fourTable.nowFourLine++;
				fourTable.AddFour("jmp", "", "", std::to_string(fourTable.nowFourLine + 2));
				fourTable.nowFourLine++;
				fourTable.AddFour("=", "1", "", tempVarName);
				fourTable.nowFourLine++;

				// 返回值入求值栈
				returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
				// 设置最后一次返回值类型
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = EnumWordProperties::Bool;
			}
			if (EqualityExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a != Equality Expression because Expect a Relational Expression", wordTable[p], 0));
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
bool SemanticAnalysis::BitAndExpression() {
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
bool SemanticAnalysis::BitAndExpressionEliminateLeft() {
	// 保存当前指针
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
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    按位异或表达式识别
// param   
// return  
bool SemanticAnalysis::BitXOrExpression() {
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
bool SemanticAnalysis::BitXOrExpressionEliminateLeft() {
	// 保存当前指针
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
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    按位或表达式识别
// param   
// return  
bool SemanticAnalysis::BitOrExpression() {
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
bool SemanticAnalysis::BitOrExpressionEliminateLeft() {
	// 保存当前指针
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
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    逻辑与表达式识别
// param   
// return  
bool SemanticAnalysis::AndExpression() {
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
bool SemanticAnalysis::AndExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// && BitOrExpression AndExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorAnd) {
		p++;
		// 语义动作
		{
			// 栈顶值转 bool 值
			ToBool();
			// 取 bool 值
			auto lastReturnValue = returnValueStack.returnValueStack.top();
			returnValueStack.returnValueStack.pop();

			// 回填行号入栈
			andBackFillStack.push(fourTable.nowFourLine);

			// 跳转语句 待回填
			fourTable.AddFour("jz", lastReturnValue->value, "", "");
			fourTable.nowFourLine++;
		}
		if (BitOrExpression()) {
			if (AndExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a && Expression because Expect a || Expression", wordTable[p], 0));
	}
	// 空
	else {
		// 语义动作
		{
			// 只有在需要回填的时候才执行 防止不必要的 bool 转换
			if (andBackFillStack.size()) {

				// 栈顶值转 bool 值 整个逻辑与表达式的值即当前栈顶的 bool 值
				ToBool();

				// 回填栈中所有行号
				while (andBackFillStack.size()) {
					int i = andBackFillStack.top();
					andBackFillStack.pop();
					// 指向把 0 给临时变量的行
					fourTable.table[i].dest = std::to_string(fourTable.nowFourLine - 1);
				}
			}
		}
		p = nowP;
		return true;
	}
}

// func    逻辑或表达式识别
// param   
// return  
bool SemanticAnalysis::OrExpression() {
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
bool SemanticAnalysis::OrExpressionEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// || AndExpression OrExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorOr) {
		p++;
		// 语义动作
		{
			// 栈顶值转 bool 值
			ToBool();
			// 取 bool 值
			auto lastReturnValue = returnValueStack.returnValueStack.top();
			returnValueStack.returnValueStack.pop();

			// 回填行号入栈
			orBackFillStack.push(fourTable.nowFourLine);

			// 跳转语句 待回填
			fourTable.AddFour("jnz", lastReturnValue->value, "", "");
			fourTable.nowFourLine++;
		}
		if (AndExpression()) {
			if (OrExpressionEliminateLeft()) {
				return true;
			}
		}
		throw(Exception("Not a || Expression because Expect a && Expression", wordTable[p], 0));
	}
	// 空
	else {
		// 语义动作
		{
			// 只有在需要回填的时候才执行 防止不必要的 bool 转换
			if (orBackFillStack.size()) {

				// 栈顶值转 bool 值 整个逻辑与表达式的值即当前栈顶的 bool 值
				ToBool();

				// 回填栈中所有行号
				while (orBackFillStack.size()) {
					int i = orBackFillStack.top();
					orBackFillStack.pop();
					// 指向把 1 给临时变量的行
					fourTable.table[i].dest = std::to_string(fourTable.nowFourLine - 3);
				}
			}
		}
		p = nowP;
		return true;
	}
}

// func    条件表达式识别
// param   
// return  
bool SemanticAnalysis::ConditionalExpression() {
	// 保存当前指针
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

// func    赋值表达式识别
// param   
// return  
bool SemanticAnalysis::AssignmentExpression() {
	// 保存当前指针
	int nowP = p;
	int nowTempVariableTablePointer = tempVariableTablePointer;
	// 保存四元式表
	auto nowFourTable = FourTable(fourTable);
	// 保存求值栈
	auto nowReturnValueStack = ReturnValueStack(returnValueStack);

	if (UnaryExpression()) {
		if (AssignmentOperator()) {
			// 保存当前赋值运算符类型
			EnumWordProperties e = wordTable[p - 1].property;
			// 语义动作
			{
				// 首先看栈顶是不是标识符
				if (returnValueStack.lastReturnWordType != ReturnValueStack::LastReturnWordType::Identifier) {
					// 语义错误 赋值表达式要求是一个左值 当前情况下只有标识符是左值
					throw(Exception("Should be a Left-Value after = Assignment", wordTable[p], 1));
				}
			}
			if (AssignmentExpression()) {
				// 语义动作
				{
					// 出栈 查看右边的类型
					auto lastReturnValue2 = returnValueStack.returnValueStack.top();
					returnValueStack.returnValueStack.pop();
					// 出栈 查看左边的类型
					auto lastReturnValue1 = returnValueStack.returnValueStack.top();
					returnValueStack.returnValueStack.pop();

					// 查找符号表
					auto [returnBool, id] = SearchIdentifier(identifierTablePointer, lastReturnValue1->value);
					// 未定义
					if (!returnBool) {
						// 语义错误
						throw(Exception("Identifier Not defined", wordTable[p], 1));
					}
					if (e != EnumWordProperties::OperatorModAssign) {
						// 类型转换
						if (!((IsFloat(lastReturnValue1->getWordProperties()) &&
							IsFloat(lastReturnValue2->getWordProperties())) ||
							(IsTrueInt(lastReturnValue1->getWordProperties()) &&
								(IsTrueInt(lastReturnValue2->getWordProperties())) ||
								(IsChar(lastReturnValue1->getWordProperties()) &&
									(IsChar(lastReturnValue2->getWordProperties()))))
							)) {
							// String 和 Function 不允许自动类型转换
							if (lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
								lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
								// 语义错误
								throw(Exception("Can't Cast To Bool", wordTable[p], 1));
							}
							// 转为 Bool
							if (lastReturnValue1->getWordProperties() == EnumWordProperties::Bool) {
								// 取临时变量
								std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
								tempVariableTablePointer++;
								// 生成四元式
								fourTable.AddFour("jz", lastReturnValue2->value, "", std::to_string(fourTable.nowFourLine + 3));
								fourTable.nowFourLine++;
								fourTable.AddFour("=", "1", "", tempVarName);
								fourTable.nowFourLine++;
								fourTable.AddFour("jmp", "", "", std::to_string(fourTable.nowFourLine + 2));
								fourTable.nowFourLine++;
								fourTable.AddFour("=", "0", "", tempVarName);
								fourTable.nowFourLine++;
								lastReturnValue2 = new TempVariable(EnumWordProperties::Bool, tempVarName);
							}
							// 转为 Float
							else if (IsFloat(lastReturnValue1->getWordProperties()) &&
								!IsFloat(lastReturnValue2->getWordProperties())) {
								// 取临时变量 生成四元式
								std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
								tempVariableTablePointer++;
								fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
								fourTable.nowFourLine++;
								lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
							}
							// Float 转 Int
							else if (IsInt(lastReturnValue1->getWordProperties()) &&
								IsFloat(lastReturnValue2->getWordProperties())) {
								// 取临时变量 生成四元式
								std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
								tempVariableTablePointer++;
								fourTable.AddFour("FloatToInt", lastReturnValue2->value, "", tempVarName);
								fourTable.nowFourLine++;
								lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
							}
							// Int 转 Char
							else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char &&
								IsInt(lastReturnValue2->getWordProperties())) {
								// 取临时变量 生成四元式
								std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
								tempVariableTablePointer++;
								fourTable.AddFour("IntToChar", lastReturnValue2->value, "", tempVarName);
								fourTable.nowFourLine++;
								lastReturnValue2 = new TempVariable(EnumWordProperties::Char, tempVarName);
							}
							// Float 转 Char 错误
							else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char &&
								IsFloat(lastReturnValue2->getWordProperties())) {
								// 语义错误
								throw(Exception("Can't Cast From Float To Char", wordTable[p], 1));
							}
						}
						if (e == EnumWordProperties::OperatorAssign) {
							// 赋值四元式
							fourTable.AddFour("=", lastReturnValue2->value, "", lastReturnValue1->value);
							fourTable.nowFourLine++;
						}
						else if (e == EnumWordProperties::OperatorAddAssign) {
							// 赋值四元式
							fourTable.AddFour("+", lastReturnValue1->value, lastReturnValue2->value, lastReturnValue1->value);
							fourTable.nowFourLine++;
						}
						else if (e == EnumWordProperties::OperatorSubAssign) {
							// 赋值四元式
							fourTable.AddFour("-", lastReturnValue1->value, lastReturnValue2->value, lastReturnValue1->value);
							fourTable.nowFourLine++;
						}
						else if (e == EnumWordProperties::OperatorMulAssign) {
							// 赋值四元式
							fourTable.AddFour("*", lastReturnValue1->value, lastReturnValue2->value, lastReturnValue1->value);
							fourTable.nowFourLine++;
						}
						else if (e == EnumWordProperties::OperatorDivAssign) {
							// 赋值四元式
							fourTable.AddFour("/", lastReturnValue1->value, lastReturnValue2->value, lastReturnValue1->value);
							fourTable.nowFourLine++;
						}
					}
					else {
						if (IsFloat(lastReturnValue1->getWordProperties()) ||
							IsFloat(lastReturnValue2->getWordProperties())) {
							// 语义错误 取模操作必须是 Int
							throw(Exception("Should be a Int Value To % Mod", wordTable[p], 1));
						}
						// 没有 Float 值
						else if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
							lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
							// 取临时变量 生成四元式
							std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
							tempVariableTablePointer++;
							fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
							fourTable.nowFourLine++;
							lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
						}
						// 赋值四元式
						fourTable.AddFour("%", lastReturnValue1->value, lastReturnValue2->value, lastReturnValue1->value);
						fourTable.nowFourLine++;
					}

					// 返回值入求值栈
					returnValueStack.returnValueStack.push(lastReturnValue1);
					// 设置最后一次返回值类型
					returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::Identifier;
					returnValueStack.lastReturnValueType = lastReturnValue1->getWordProperties();
				}
				return true;
			}
			throw(Exception("Not a Assignment Expression because Expect a Assignment Expression", wordTable[p], 0));
		}
		// 回溯
		{
			p = nowP;
			tempVariableTablePointer = nowTempVariableTablePointer;
			// 析构当前求值栈
			returnValueStack.~ReturnValueStack();
			// 恢复求值栈
			returnValueStack = ReturnValueStack(nowReturnValueStack);
			// 析构当前四元式表
			fourTable.~FourTable();
			// 恢复四元式表
			fourTable = FourTable(nowFourTable);
		}
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
bool SemanticAnalysis::AssignmentOperator() {
	// 保存当前指针
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

// func    表达式识别
// param   
// return  
bool SemanticAnalysis::Expression() {
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
bool SemanticAnalysis::ExpressionEliminateLeft() {
	// 保存当前指针
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
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    声明识别
// param   
// return  
bool SemanticAnalysis::Declaration() {
	// 保存当前指针
	int nowP = p;

	// TypeName InitDeclaratorList;
	if (IsTypeName(wordTable[p])) {
		// 语义动作
		{
			// 保存当前声明类型
			declarationTypeName = wordTable[p].property;
		}
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

// func    初始化声明列表识别
// param   
// return  
bool SemanticAnalysis::InitDeclaratorList() {
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
bool SemanticAnalysis::InitDeclaratorListEliminateLeft() {
	// 保存当前指针
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
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    初始化声明符识别
// param   
// return  
bool SemanticAnalysis::InitDeclarator() {
	// 保存当前指针
	int nowP = p;

	// DirectDeclarator
	if (DirectDeclarator()) {
		// DirectDeclarator = AssignmentExpression
		if (wordTable[p].property == EnumWordProperties::OperatorAssign) {
			p++;
			if (AssignmentExpression()) {
				// 语义动作
				{
					// 用该表达式初始化标识符
					// 出栈 查看右边的类型
					auto lastReturnValue = returnValueStack.returnValueStack.top();
					returnValueStack.returnValueStack.pop();

					// 直接初始化
					if ((declarationTypeName == EnumWordProperties::Int &&
						IsTrueInt(lastReturnValue->getWordProperties())) ||
						(declarationTypeName == EnumWordProperties::Float &&
							IsFloat(lastReturnValue->getWordProperties())) ||
						(declarationTypeName == EnumWordProperties::StringType &&
							lastReturnValue->getWordProperties() == EnumWordProperties::String) ||
						(declarationTypeName == EnumWordProperties::Char &&
							lastReturnValue->getWordProperties() == EnumWordProperties::CharNumber)) {
						// 初始化四元式
						fourTable.AddFour("=", lastReturnValue->value, "", declarationIdentifier.value);
						fourTable.nowFourLine++;
					}
					// 类型转换
					else if (!((IsFloat(declarationTypeName) &&
						IsFloat(lastReturnValue->getWordProperties())) ||
						(IsTrueInt(declarationTypeName) &&
							(IsTrueInt(lastReturnValue->getWordProperties())) ||
							(IsChar(declarationTypeName) &&
								(IsChar(lastReturnValue->getWordProperties())))))) {
						// String 和 Function 不允许自动类型转换
						if (lastReturnValue->getWordProperties() == EnumWordProperties::String ||
							lastReturnValue->getWordProperties() == EnumWordProperties::Function) {
							// 语义错误
							throw(Exception("Can't Cast", wordTable[p], 1));
						}
						// 转为 Bool
						if (declarationTypeName == EnumWordProperties::Bool) {
							// 取临时变量
							std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
							tempVariableTablePointer++;
							// 生成四元式
							fourTable.AddFour("jz", lastReturnValue->value, "", std::to_string(fourTable.nowFourLine + 3));
							fourTable.nowFourLine++;
							fourTable.AddFour("=", "1", "", tempVarName);
							fourTable.nowFourLine++;
							fourTable.AddFour("jmp", "", "", std::to_string(fourTable.nowFourLine + 2));
							fourTable.nowFourLine++;
							fourTable.AddFour("=", "0", "", tempVarName);
							fourTable.nowFourLine++;
							lastReturnValue = new TempVariable(EnumWordProperties::Bool, tempVarName);
						}
						// 转为 Float
						else if (IsFloat(declarationTypeName) &&
							!IsFloat(lastReturnValue->getWordProperties())) {
							// 取临时变量 生成四元式
							std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
							tempVariableTablePointer++;
							fourTable.AddFour("IntToFloat", lastReturnValue->value, "", tempVarName);
							fourTable.nowFourLine++;
							lastReturnValue = new TempVariable(EnumWordProperties::Float, tempVarName);
						}
						// Float 转 Int
						else if (IsInt(declarationTypeName) &&
							IsFloat(lastReturnValue->getWordProperties())) {
							// 取临时变量 生成四元式
							std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
							tempVariableTablePointer++;
							fourTable.AddFour("FloatToInt", lastReturnValue->value, "", tempVarName);
							fourTable.nowFourLine++;
							lastReturnValue = new TempVariable(EnumWordProperties::Int, tempVarName);
						}
						// Int 转 Char
						else if (declarationTypeName == EnumWordProperties::Char &&
							IsInt(lastReturnValue->getWordProperties())) {
							// 取临时变量 生成四元式
							std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
							tempVariableTablePointer++;
							fourTable.AddFour("IntToChar", lastReturnValue->value, "", tempVarName);
							fourTable.nowFourLine++;
							lastReturnValue = new TempVariable(EnumWordProperties::Char, tempVarName);
						}
						// Float 转 Char 错误
						else if (declarationTypeName == EnumWordProperties::Char &&
							IsFloat(lastReturnValue->getWordProperties())) {
							// 语义错误
							throw(Exception("Can't Cast From Float To Char", wordTable[p], 1));
						}
						// 其他 转 String 错误
						else if (declarationTypeName == EnumWordProperties::StringType) {
							// 语义错误
							throw(Exception("Can't Cast To String", wordTable[p], 1));
						}

						// 转换完了 初始化
						fourTable.AddFour("=", lastReturnValue->value, "", declarationIdentifier.value);
						fourTable.nowFourLine++;
					}
				}
				return true;
			}
			throw(Exception("Not a Declaration because Expect a Assignment Expression", wordTable[p], 0));
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
bool SemanticAnalysis::DirectDeclarator() {
	// 保存当前指针
	int nowP = p;

	// Identifier DirectDeclaratorEliminateLeft
	if (wordTable[p].property == EnumWordProperties::Identifier) {
		// 语义动作
		{
			bool isFunction = false;
			// 看是不是函数
			for (auto& i : functionTable.functionTable) {
				// 是函数
				if (i.value == wordTable[p].word) {
					lastFunctionIdentifier = wordTable[p].word;
					isFunction = true;
					break;
				}
			}
			
			// 查找当前表 是否重定义
			auto [returnBool, id] = SearchIdentifierInCurrentTable(identifierTablePointer, wordTable[p].word);
			// 重定义
			if (!isFunction&&returnBool) {
				// 语义错误
				throw(Exception("Identifier have defined", wordTable[p], 1));
			}
			if (!returnBool) {
				// 标识符入表 暂存该标识符
				declarationIdentifier = Identifier(declarationTypeName, wordTable[p].word);
				identifierTablePointer->AddIdentifier(Identifier(declarationTypeName, wordTable[p].word));
			}
		}
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
bool SemanticAnalysis::DirectDeclaratorEliminateLeft() {
	// 保存当前指针
	int nowP = p;

	// (ParameterList) DirectDeclaratorEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorLeftRound) {
		p++;
		// 语义动作
		// 说明是函数 可以准备送入函数表了
		{
			bool isDefined = false;
			// 重定义
			for (auto& i : functionTable.functionTable) {
				if (i.value == identifierTablePointer->table[identifierTablePointer->IdentifierNumber - 1].value) {
					isDefined = true;
				}
			}
			if (!isDefined) {
				// 函数符入函数表
				functionTable.AddFunction(FunctionIdentifier(declarationTypeName,
					identifierTablePointer->table[identifierTablePointer->IdentifierNumber - 1].value));
			}

			// 由于接下来是参数列表 参数列表中的每个参数声明由声明类型和直接声明符组成
			// 直接声明符会查看标识符重定义 因此函数声明需要一个新的作用域
			// 新的作用域 函数头到函数尾将是一个表
			identifierTablePointer = new IdentifierTable(identifierTablePointer);
			// 这张表要交给后面的块
			functionHeadIdentifierTablePointer = identifierTablePointer;
		}
		// 在参数列表中 每个参数被添加到函数表顶的函数里
		if (ParameterList()) {
			if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
				p++;
				// 语义动作
				{
					// 函数头部分结束了 表被收起来 但函数头作用域保留 以便合入
					identifierTablePointer = identifierTablePointer->fatherTablePointer;
				}
				// 这里看不懂 连续的 (ParameterList) ?
				if (DirectDeclaratorEliminateLeft()) {
					return true;
				}
			}
			throw(Exception("Not a Function Declaration because Expect a )", wordTable[p], 0));
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
bool SemanticAnalysis::ParameterList() {
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
bool SemanticAnalysis::ParameterListEliminateLeft() {
	// 保存当前指针
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
	// 空
	else {
		p = nowP;
		return true;
	}
}

// func    参数声明识别
// param   
// return  
bool SemanticAnalysis::ParameterDeclaration() {
	// 保存当前指针
	int nowP = p;

	// TypeName DirectDeclarator
	if (IsTypeName(wordTable[p])) {
		p++;
		if (DirectDeclarator()) {
			// 语义动作
			{
				// 参数到手 向函数符里添加参数类型
				functionTable.functionTable[functionTable.FunctionNumber - 1].AddParameter(declarationTypeName);
			}
			return true;
		}
		throw(Exception("Not a Parameter Declaration because Expect a Identifier", wordTable[p], 0));
		p = nowP;
		return false;
	}
	else {
		p = nowP;
		return false;
	}
}

// func    语句识别
// param   
// return  
bool SemanticAnalysis::Statement() {
	// 保存当前指针
	int nowP = p;

	// Goto Continue Break Return 是跳转语句
	if (wordTable[p].property == EnumWordProperties::Goto ||
		wordTable[p].property == EnumWordProperties::Continue ||
		wordTable[p].property == EnumWordProperties::Break ||
		wordTable[p].property == EnumWordProperties::Return) {
		JumpStatement();
		return true;
	}
	// If 是选择语句
	else if (wordTable[p].property == EnumWordProperties::If) {
		SelectionStatement();
		return true;
	}
	// { 是复合语句
	else if (wordTable[p].property == EnumWordProperties::OperatorLeftBrace) {
		CompoundStatement();
		return true;
	}
	// While Do 是迭代语句
	else if (wordTable[p].property == EnumWordProperties::While ||
		wordTable[p].property == EnumWordProperties::Do) {
		IterationStatement();
		return true;
	}
	// 空语句 是表达式语句
	else if (wordTable[p].property == EnumWordProperties::Semicolon) {
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
bool SemanticAnalysis::LabeledStatement() {
	// 保存当前指针
	int nowP = p;

	// Identifier : Statement
	if (wordTable[p].property == EnumWordProperties::Identifier) {
		p++;
		if (wordTable[p].property == EnumWordProperties::OperatorColon) {
			// 语义动作
			{
				// 这里要把标签放入标签表
				// 取标签表栈顶
				auto& labelTable = labelTableStack.top();
				auto [exist, defined, i] = labelTable.SearchLabelIdentifier(wordTable[p - 1].word);
				// 重定义
				if (exist && defined) {
					throw(Exception("Label have defined", wordTable[p - 1], 1));
				}
				if (exist) {
					labelTable.definedLabel[i] = true;
					labelTable.backFillLines[i] = fourTable.nowFourLine;
				}
				else {
					// 该已定义标签入表
					labelTable.AddDefinedLabel(fourTable.nowFourLine, wordTable[p - 1].word);
				}
			}
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

// func    复合语句识别
// param   
// return  
bool SemanticAnalysis::CompoundStatement() {
	// 保存当前指针
	int nowP = p;

	// { BlockItemList }
	if (wordTable[p].property == EnumWordProperties::OperatorLeftBrace) {
		p++;
		// 语义动作
		{
			// 新的作用域
			identifierTablePointer = new IdentifierTable(identifierTablePointer);

			// 是否为函数块
			if (functionLeftCompound) {
				// 将表合入
				for (auto& i : (functionHeadIdentifierTablePointer->table)) {
					identifierTablePointer->AddIdentifier(i);
				}
				// 函数块标志置 false
				functionLeftCompound = false;

				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				// 出栈调用位置
				fourTable.AddFour("pop", "", "", tempVarName);
				fourTable.nowFourLine++;

				// 形参出栈
				for (auto& i : identifierTablePointer->table) {
					// 四元式
					fourTable.AddFour("pop", "", "", i.value);
					fourTable.nowFourLine++;
				}

				// 入栈调用位置
				fourTable.AddFour("push", tempVarName, "", "");
				fourTable.nowFourLine++;
			}
		}
		if (BlockItemList()) {
			if (wordTable[p].property == EnumWordProperties::OperatorRightBrace) {
				p++;
				// 语义动作
				{
					// 离开作用域
					identifierTablePointer = identifierTablePointer->fatherTablePointer;
					// 如果是函数作用域
					if (identifierTablePointer->fatherTablePointer == nullptr) {
						// 取标签表栈顶
						auto& labelTable = labelTableStack.top();

						// 遍历标签
						for (int i = 0; i < labelTable.labelTable.size(); i++) {
							if (!labelTable.definedLabel[i]) {
								// 存在目标标签未定义的跳转语句
								throw(Exception("Label Not defined", wordTable[p], 1));
							}
							// 遍历标签的待回填行
							for (auto j : labelTable.backFillTable[i]) {
								// 回填标签
								fourTable.table[j].dest = std::to_string(labelTable.backFillLines[i]);
							}
						}
					}
				}
				return true;
			}
			throw(Exception("Not a Compound Statement because Expect a }", wordTable[p], 0));
		}
		throw(Exception("Not a Compound Statement because Expect a Block Item", wordTable[p], 0));
	}
	throw(Exception("Not a Compound Statement because Expect a {", wordTable[p], 0));
}

// func    块项目列表识别
// param   
// return  
bool SemanticAnalysis::BlockItemList() {
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
bool SemanticAnalysis::BlockItemListEliminateLeft() {
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
bool SemanticAnalysis::BlockItem() {
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
bool SemanticAnalysis::ExpressionStatement() {
	// 保存当前指针
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

// func    选择语句识别
// param   
// return  
bool SemanticAnalysis::SelectionStatement() {
	// 保存当前指针
	int nowP = p;

	// If (Expression) Statement
	if (wordTable[p].property == EnumWordProperties::If) {
		p++;
		if (wordTable[p].property == EnumWordProperties::OperatorLeftRound) {
			p++;
			if (Expression()) {
				if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
					// 语义动作
					{
						// 先转 Bool 值
						ToBool();
						auto lastReturnValue = returnValueStack.returnValueStack.top();
						returnValueStack.returnValueStack.pop();
						// 回填行号入栈
						selectBackFillStack.push(fourTable.nowFourLine);
						// 跳转语句 待回填
						fourTable.AddFour("jz", lastReturnValue->value, "", "");
						fourTable.nowFourLine++;
					}
					p++;
					if (Statement()) {
						// 语义动作
						{
							// 回填
							int backFillLine = selectBackFillStack.top();
							selectBackFillStack.pop();
							fourTable.table[backFillLine].dest = std::to_string(fourTable.nowFourLine + 1);
						}
						// If (Expression) Statement Else Statement
						if (wordTable[p].property == EnumWordProperties::Else) {
							p++;
							// 语义动作
							{
								// 回填行号入栈
								selectBackFillStack.push(fourTable.nowFourLine);
								// 跳转语句 待回填
								fourTable.AddFour("jmp", "", "", "");
								fourTable.nowFourLine++;
							}
							if (Statement()) {
								// 语义动作
								{
									// 回填
									int backFillLine = selectBackFillStack.top();
									selectBackFillStack.pop();
									fourTable.table[backFillLine].dest = std::to_string(fourTable.nowFourLine);
								}
								return true;
							}
						}
						// If (Expression) Statement
						else {
							// 语义动作
							{
								// 跳转语句 无else 到下一行
								fourTable.AddFour("jmp", "", "", std::to_string(fourTable.nowFourLine + 1));
								fourTable.nowFourLine++;
							}
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

// func    迭代语句识别
// param   
// return  
bool SemanticAnalysis::IterationStatement() {
	// 保存当前指针
	int nowP = p;

	// While(Expression) Statement
	if (wordTable[p].property == EnumWordProperties::While) {
		p++;
		if (wordTable[p].property == EnumWordProperties::OperatorLeftRound) {
			p++;
			// 语义动作
			{
				// 回填参考行号集入栈
				iterateBackFillStack.push(fourTable.nowFourLine);
				// 新的迭代层
				iterateBreakLayerStack.push(new std::vector<int>);
				iterateContinueLayerStack.push(new std::vector<int>);
			}
			if (Expression()) {
				if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
					// 语义动作
					{
						// 先转 Bool 值
						ToBool();
						auto lastReturnValue = returnValueStack.returnValueStack.top();
						returnValueStack.returnValueStack.pop();
						// 回填行号入栈
						iterateBackFillStack.push(fourTable.nowFourLine);
						// 跳转语句 待回填
						fourTable.AddFour("jz", lastReturnValue->value, "", "");
						fourTable.nowFourLine++;
					}
					p++;
					if (Statement()) {
						// 语义动作
						{
							// 回填
							int backFillLine = iterateBackFillStack.top();
							iterateBackFillStack.pop();
							fourTable.table[backFillLine].dest = std::to_string(fourTable.nowFourLine + 1);
							// 取参考行号
							backFillLine = iterateBackFillStack.top();
							iterateBackFillStack.pop();
							// 当前迭代层出栈
							auto currentBreakLayer = iterateBreakLayerStack.top();
							iterateBreakLayerStack.pop();
							auto currentContinueLayer = iterateContinueLayerStack.top();
							iterateContinueLayerStack.pop();
							// 回填当前迭代层的 break continue
							for (auto i : *currentBreakLayer) {
								fourTable.table[i].dest = std::to_string(fourTable.nowFourLine + 1);
							}
							for (auto i : *currentContinueLayer) {
								fourTable.table[i].dest = std::to_string(backFillLine);
							}
							// 析构当前迭代层
							currentBreakLayer->~vector();
							currentContinueLayer->~vector();
							// 跳转语句
							fourTable.AddFour("jmp", "", "", std::to_string(backFillLine));
							fourTable.nowFourLine++;
						}
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
		// 语义动作
		{
			// 回填参考行号集入栈
			iterateBackFillStack.push(fourTable.nowFourLine);
			// 新的迭代层
			iterateBreakLayerStack.push(new std::vector<int>);
			iterateContinueLayerStack.push(new std::vector<int>);
		}
		if (Statement()) {
			if (wordTable[p].property == EnumWordProperties::While) {
				p++;
				if (wordTable[p].property == EnumWordProperties::OperatorLeftRound) {
					p++;
					// 语义动作
					{
						// 当前迭代层出栈
						auto currentContinueLayer = iterateContinueLayerStack.top();
						iterateContinueLayerStack.pop();
						// 回填当前迭代层的 continue
						for (auto i : *currentContinueLayer) {
							fourTable.table[i].dest = std::to_string(fourTable.nowFourLine);
						}
						// 析构当前迭代层
						currentContinueLayer->~vector();
					}
					if (Expression()) {
						if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
							p++;
							if (wordTable[p].property == EnumWordProperties::Semicolon) {
								p++;
								// 语义动作
								{
									// 先转 Bool 值
									ToBool();
									auto lastReturnValue = returnValueStack.returnValueStack.top();
									returnValueStack.returnValueStack.pop();
									// 取参考行号
									int backFillLine = iterateBackFillStack.top();
									iterateBackFillStack.pop();
									// 跳转语句
									fourTable.AddFour("jnz", lastReturnValue->value, "", std::to_string(backFillLine));
									fourTable.nowFourLine++;
									// 当前迭代层出栈
									auto currentBreakLayer = iterateBreakLayerStack.top();
									iterateBreakLayerStack.pop();
									// 回填当前迭代层的 break
									for (auto i : *currentBreakLayer) {
										fourTable.table[i].dest = std::to_string(fourTable.nowFourLine);
									}
									// 析构当前迭代层
									currentBreakLayer->~vector();
								}
								return true;
							}
							throw(Exception("Not a do while Statement because Exprect a ;", wordTable[p], 0));
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

// func    跳转语句识别
// param   
// return  
bool SemanticAnalysis::JumpStatement() {
	// 保存当前指针
	int nowP = p;

	// Goto Identifier;
	if (wordTable[p].property == EnumWordProperties::Goto) {
		p++;
		if (wordTable[p].property == EnumWordProperties::Identifier) {
			p++;
			if (wordTable[p].property == EnumWordProperties::Semicolon) {
				// 语义动作
				{
					// 取标签表栈顶
					auto& labelTable = labelTableStack.top();
					auto [exist, defined, i] = labelTable.SearchLabelIdentifier(wordTable[p - 1].word);
					// 已存在
					if (exist) {
						// 待回填行数
						labelTable.AddLabelBackFillLine(fourTable.nowFourLine, wordTable[p - 1].word);
					}
					else {
						// 该标签入表
						labelTable.AddUndefinedLabel(wordTable[p - 1].word);
						// 待回填行数
						labelTable.AddLabelBackFillLine(fourTable.nowFourLine, wordTable[p - 1].word);
					}
					// 跳转语句
					fourTable.AddFour("jmp", "", "", "");
					fourTable.nowFourLine++;
				}
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
			// 语义动作
			{
				// 空悬的 continue
				if (iterateContinueLayerStack.empty()) {
					// 语义错误
					throw(Exception("Not Find Iteration Statement But a continue Statement", wordTable[p], 1));
				}
				// 回填参考行号入栈
				iterateContinueLayerStack.top()->push_back(fourTable.nowFourLine);
				// 跳转语句
				fourTable.AddFour("jmp", "", "", "");
				fourTable.nowFourLine++;
			}
			return true;
		}
		throw(Exception("Not a continue Statement because Exprect a ;", wordTable[p], 0));
	}
	// Break;
	else if (wordTable[p].property == EnumWordProperties::Break) {
		p++;
		if (wordTable[p].property == EnumWordProperties::Semicolon) {
			p++;
			// 语义动作
			{
				// 空悬的 break
				if (iterateBreakLayerStack.empty()) {
					// 语义错误
					throw(Exception("Not Find Iteration Statement But a break Statement", wordTable[p], 1));
				}
				// 回填参考行号入栈
				iterateBreakLayerStack.top()->push_back(fourTable.nowFourLine);
				// 跳转语句
				fourTable.AddFour("jmp", "", "", "");
				fourTable.nowFourLine++;
			}
			return true;
		}
		throw(Exception("Not a break Statement because Exprect a ;", wordTable[p], 0));
	}
	// Return
	else if (wordTable[p].property == EnumWordProperties::Return) {
		p++;
		// Return;
		if (wordTable[p].property == EnumWordProperties::Semicolon) {
			// 语义动作
			{
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				// 出栈调用位置
				fourTable.AddFour("pop", "", "", tempVarName);
				fourTable.nowFourLine++;
				fourTable.AddFour("ret", "", "", tempVarName);
				fourTable.nowFourLine++;
			}
			p++;
			return true;
		}
		// Return Expression;
		else if (Expression()) {
			if (wordTable[p].property == EnumWordProperties::Semicolon) {
				// 语义动作
				{
					auto lastReturnValue = returnValueStack.returnValueStack.top();
					returnValueStack.returnValueStack.pop();

					// 临时变量 存返回位置
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					// 出栈调用位置
					fourTable.AddFour("pop", "", "", tempVarName);
					fourTable.nowFourLine++;

					// 类型转换 懒得写了
					// 入栈返回值
					fourTable.AddFour("push", lastReturnValue->value, "", "");
					fourTable.nowFourLine++;

					fourTable.AddFour("ret", "", "", tempVarName);
					fourTable.nowFourLine++;
				}
				p++;
				return true;
			}
		}
		throw(Exception("Not a Return Statement", wordTable[p], 0));
	}
	throw(Exception("Not a Jump Statement", wordTable[p], 0));
}

// func    翻译单元识别
// param   
// return  
bool SemanticAnalysis::TranslationUnit() {
	// 保存当前指针
	int nowP = p;

	// ExternalDeclaration TranslationUnitEliminateLeft
	if (ExternalDeclaration()) {
		if (TranslationUnitEliminateLeft()) {
			return true;
		}
	}

	throw(Exception("Not a Translation Unit", wordTable[p], 0));
}

// func    翻译单元消除左递归识别
// param   
// return  
bool SemanticAnalysis::TranslationUnitEliminateLeft() {
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
bool SemanticAnalysis::ExternalDeclaration() {
	// 保存当前指针
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
	throw(Exception("Not a Function Definition or a Declaration", wordTable[p], 0));
}

// func    函数定义识别
// param   
// return  
bool SemanticAnalysis::FunctionDefinition() {
	// 保存当前指针
	int nowP = p;

	// TypeName DirectDeclarator /* DeclarationList */ CompoundStatement
	if (IsTypeName(wordTable[p])) {
		p++;
		// 声明符
		if (DirectDeclarator()) {
			// 必然以 { 开头
			// TypeName DirectDeclarator CompoundStatement
			if (wordTable[p].property == EnumWordProperties::OperatorLeftBrace) {
				// 语义动作
				{
					// 重定义
					for (auto& i : functionTable.functionTable) {
						if (i.value == lastFunctionIdentifier&& i.defined) {
							throw(Exception("Function have defined", wordTable[p], 1));
						}
					}

					// 指示当前为函数头 以便将声明处的表合并入块 应当在进入函数块后马上置 false
					functionLeftCompound = true;
					// 创建当前函数的标签表
					labelTableStack.push(*(new LabelIdentifierTable()));

					// 将函数设为已经定义
					for (auto& i : functionTable.functionTable) {
						if (i.value == lastFunctionIdentifier) {
							i.defined = true;
							break;
						}
					}
				}
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

// func    声明列表识别
// param   
// return  
bool SemanticAnalysis::DeclarationList() {
	// 保存当前指针
	int nowP = p;

	// Declaration DeclarationListEliminateLeft
	if (Declaration()) {
		if (DeclarationListEliminateLeft()) {
			return true;
		}
	}
	throw(Exception("Not a Declaration", wordTable[p], 0));
}

// func    声明列表消除左递归识别
// param   
// return  
bool SemanticAnalysis::DeclarationListEliminateLeft() {
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

// func    类型转换 要求求值栈中有一个值 转换后的值会进入求值栈
// param   
// return  
bool SemanticAnalysis::ToBool() {
	// 查看类型
	auto lastReturnValue = returnValueStack.returnValueStack.top();
	auto e = lastReturnValue->getWordProperties();
	// 原来是 Bool
	if (e == EnumWordProperties::Bool) {
		return true;
	}

	// 不是 Bool 出栈
	returnValueStack.returnValueStack.pop();
	// 直接变
	if (returnValueStack.lastReturnValueType == EnumWordProperties::String ||
		returnValueStack.lastReturnValueType == EnumWordProperties::Function) {
		// 取临时变量 生成四元式
		std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
		tempVariableTablePointer++;
		fourTable.AddFour("=", "1", "", tempVarName);
		fourTable.nowFourLine++;

		// 入求值栈
		returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
		returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
		returnValueStack.lastReturnValueType = EnumWordProperties::Bool;
	}
	// 其他类型
	else {
		// 取临时变量 生成四元式
		std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
		tempVariableTablePointer++;
		fourTable.AddFour("jz", lastReturnValue->value, "", std::to_string(fourTable.nowFourLine + 3));
		fourTable.nowFourLine++;
		fourTable.AddFour("=", "1", "", tempVarName);
		fourTable.nowFourLine++;
		fourTable.AddFour("jmp", "", "", std::to_string(fourTable.nowFourLine + 2));
		fourTable.nowFourLine++;
		fourTable.AddFour("=", "0", "", tempVarName);
		fourTable.nowFourLine++;

		// 入求值栈
		returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
		returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
		returnValueStack.lastReturnValueType = EnumWordProperties::Bool;
	}
	return true;
}