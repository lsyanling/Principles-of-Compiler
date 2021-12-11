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

// ���ұ�ʶ�� ������ھͷ��ر��б�ʶ��������
auto SemanticAnalysis::SearchIdentifier(IdentifierTable* idTablePointer, std::string value) {
	// ָ�벻��
	while (idTablePointer != nullptr) {
		// ������ǰ��
		for (auto& id : idTablePointer->table) {
			if (id.value == value) {
				return std::tuple<bool, Identifier&>(true, id);
			}
		}
		// ���ڵ�ǰ���� ���ϲ���
		idTablePointer = idTablePointer->fatherTablePointer;
	}
	// ��ʶ��δ����
	Identifier id = Identifier(EnumWordProperties::Unknow, "");
	return std::tuple<bool, Identifier&>(false, id);
}

// �ڵ�ǰ���в��ұ�ʶ�� ������ھͷ��ر��б�ʶ��������
auto SemanticAnalysis::SearchIdentifierInCurrentTable(IdentifierTable* idTablePointer, std::string value) {

	// ������ǰ��
	for (auto& id : idTablePointer->table) {
		if (id.value == value) {
			return std::tuple<bool, Identifier&>(true, id);
		}
	}
	// ���ڵ�ǰ���� ��ʶ��δ����
	Identifier id = Identifier(EnumWordProperties::Unknow, "");
	return std::tuple<bool, Identifier&>(false, id);
}

// func    ��ʼ���ʽʶ��
// param   
// return  
bool SemanticAnalysis::PrimaryExpression() {
	// ���浱ǰָ��
	int nowP = p;

	// Identifier
	if (wordTable[p].property == EnumWordProperties::Identifier) {
		// ���嶯��
		{
			// ���Ȳ��ұ�ʶ��
			auto [returnBool, id] = SearchIdentifier(identifierTablePointer, wordTable[p].word);
			// �ҵ���
			if (returnBool) {
				// ����ֵ����ֵջ
				returnValueStack.returnValueStack.push(&id);
				// �������һ�η���ֵ����
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::Identifier;
				returnValueStack.lastReturnValueType = id.wordProperties;
			}
			// δ����
			else {
				// �������
				throw(Exception("Identifier Not defined", wordTable[p], 1));
			}
		}
		p++;
		return true;
	}
	// ����������
	else if (wordTable[p].property == EnumWordProperties::TenIntNumber ||
		wordTable[p].property == EnumWordProperties::EightIntNumber ||
		wordTable[p].property == EnumWordProperties::SixteenIntNumber ||
		wordTable[p].property == EnumWordProperties::FloatNumber ||
		wordTable[p].property == EnumWordProperties::FloatEeNumber ||
		wordTable[p].property == EnumWordProperties::CharNumber ||
		wordTable[p].property == EnumWordProperties::String) {
		// ���嶯��
			{
				// ����ֵ����ֵջ
				returnValueStack.returnValueStack.push(new LiteralValue(wordTable[p].property, wordTable[p].word));
				// �������һ�η���ֵ����
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
				// ���嶯��
				{
					// ��ջ��ȡֵ Ȼ����ջ ����ʡ�Բ���
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

// func    ��׺���ʽʶ��
// param   
// return  
bool SemanticAnalysis::PostfixExpression() {
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
bool SemanticAnalysis::PostfixExpressionEliminateLeft() {
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
		// ���嶯��
		{
			bool isFunction = false;
			// ȡջ�� �鿴�Ƿ��Ǻ���
			auto lastReturnValue = returnValueStack.returnValueStack.top();
			for (auto& i : functionTable.functionTable) {
				if (i.value == lastReturnValue->value) {
					isFunction = true;
					break;
				}
			}
			if (!isFunction) {
				// ���Ǻ���
				throw(Exception("Not a Function", wordTable[p], 1));
			}
			// �������ñ��ʽ ������ʵ�� ��void������ÿ��ʵ����ջ
		}
		if (ArgumentExpressionList()) {
			if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
				p++;
				// ���嶯��
				{
					// ȡ������
					auto lastReturnValue = returnValueStack.returnValueStack.top();
					// ��������ջ
					returnValueStack.returnValueStack.pop();

					// ȡ��������ֵ����
					EnumWordProperties e;
					for (auto& i : functionTable.functionTable) {
						if (i.value == lastReturnValue->value) {
							// ������������
							i.currentParameter = 0;
							e = i.returnType;
							break;
						}
					}

					// ʵ����ջ��� ���ú���
					fourTable.AddFour("push", std::to_string(fourTable.nowFourLine + 2), "", "");
					fourTable.nowFourLine++;
					fourTable.AddFour("call", lastReturnValue->value, "", "");
					fourTable.nowFourLine++;
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					// ������� ȡ����ֵ
					fourTable.AddFour("pop", "", "", tempVarName);
					fourTable.nowFourLine++;

					// ����ֵջ ����ֵ���ͼ���ʱ�������� ʵ���Ǻ�������ֵ����
					returnValueStack.returnValueStack.push(new TempVariable(e, tempVarName));
					// �������һ�η���ֵ����
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
		// ���嶯��
		{
			// ���ȿ�ջ���ǲ��Ǳ�ʶ��
			if (returnValueStack.lastReturnWordType != ReturnValueStack::LastReturnWordType::Identifier) {
				// ������� ��׺���ʽҪ����һ����ֵ ��ǰ�����ֻ�б�ʶ������ֵ
				throw(Exception("Should be a Left-Value before ++", wordTable[p], 1));
			}
			// ջ���Ǳ�ʶ�� ��ջ
			auto lastReturnValue = returnValueStack.returnValueStack.top();
			returnValueStack.returnValueStack.pop();
			// ��ֵ�ǲ����ַ�������
			if (lastReturnValue->getWordProperties() == EnumWordProperties::String || lastReturnValue->getWordProperties() == EnumWordProperties::Function) {
				// ������� �ַ����ͺ�������ִ�к�׺++
				throw(Exception("Should be a Number before ++", wordTable[p], 1));
			}
			// ȡ��ʱ���� ������Ԫʽ
			std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
			tempVariableTablePointer++;
			fourTable.AddFour("=", lastReturnValue->value, "", tempVarName);
			fourTable.nowFourLine++;
			fourTable.AddFour("+", lastReturnValue->value, "1", lastReturnValue->value);
			fourTable.nowFourLine++;

			// ����ֵ����ֵջ ����ֵ���ͼ���ʱ�����������ʶ��������ͬ
			returnValueStack.returnValueStack.push(new TempVariable(lastReturnValue->getWordProperties(), tempVarName));
			// �������һ�η���ֵ����
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
		// ���嶯��
		{
			// ���ȿ�ջ���ǲ��Ǳ�ʶ��
			if (returnValueStack.lastReturnWordType != ReturnValueStack::LastReturnWordType::Identifier) {
				// ������� ��׺���ʽҪ����һ����ֵ ��ǰ�����ֻ�б�ʶ������ֵ
				throw(Exception("Should be a Left-Value before --", wordTable[p], 1));
			}
			// ջ���Ǳ�ʶ�� ��ջ
			auto lastReturnValue = returnValueStack.returnValueStack.top();
			returnValueStack.returnValueStack.pop();
			// ��ֵ�ǲ����ַ�������
			if (lastReturnValue->getWordProperties() == EnumWordProperties::String || lastReturnValue->getWordProperties() == EnumWordProperties::Function) {
				// ������� �ַ����ͺ�������ִ�к�׺--
				throw(Exception("Should be a Number before --", wordTable[p], 1));
			}
			// ȡ��ʱ���� ������Ԫʽ
			std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
			tempVariableTablePointer++;
			fourTable.AddFour("=", lastReturnValue->value, "", tempVarName);
			fourTable.nowFourLine++;
			fourTable.AddFour("-", lastReturnValue->value, "1", lastReturnValue->value);
			fourTable.nowFourLine++;

			// ����ֵ����ֵջ ����ֵ���ͼ���ʱ�����������ʶ��������ͬ
			returnValueStack.returnValueStack.push(new TempVariable(lastReturnValue->getWordProperties(), tempVarName));
			// �������һ�η���ֵ����
			returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
			returnValueStack.lastReturnValueType = lastReturnValue->getWordProperties();
		}
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
bool SemanticAnalysis::ArgumentExpressionList() {
	// ���浱ǰָ��
	int nowP = p;

	// AssignmentExpression ArgumentExpressionListEliminateLeft
	if (AssignmentExpression()) {
		// ���嶯��
		{
			// ȡʵ��ֵ
			auto lastReturnValue2 = returnValueStack.returnValueStack.top();
			returnValueStack.returnValueStack.pop();
			// ȡ�β�����
			auto lastReturnValue1 = returnValueStack.returnValueStack.top();
			EnumWordProperties e;
			for (auto& i : functionTable.functionTable) {
				if (i.value == lastReturnValue1->value) {
					e = i.parameterType[i.currentParameter++];
					break;
				}
			}
			// ����ת�� ��д��
			//if (returnValueStack.lastReturnValueType != e) {
			//	throw(Exception("ʵ�ε�����ת�� ��ʱ����д�� �͵�ת���˰�", wordTable[p], 0));
			//}

			// ������Ԫʽ ��������ѹջ
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

// func    �������ʽ�б�������ݹ�ʶ��
// param   
// return  
bool SemanticAnalysis::ArgumentExpressionListEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// ,AssignmentExpression ArgumentExpressionListEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorComma) {
		p++;
		if (AssignmentExpression()) {
			// ���嶯��
			{
				// ȡʵ��ֵ
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ȡ�β�����
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				EnumWordProperties e;
				for (auto& i : functionTable.functionTable) {
					if (i.value == lastReturnValue1->value) {
						e = i.parameterType[i.currentParameter++];
						break;
					}
				}
				// ����ת�� ��д��
				//if (returnValueStack.lastReturnValueType != e) {
				//	throw(Exception("ʵ�ε�����ת�� ��ʱ����д�� �͵�ת���˰�", wordTable[p], 0));
				//}

				// ������Ԫʽ ��������ѹջ
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
	// ��
	else {
		p = nowP;
		return true;
	}
}

// func    ��Ŀ���ʽʶ��
// param   
// return  
bool SemanticAnalysis::UnaryExpression() {
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
			// ���嶯��
			{
				// ���ȿ�ջ���ǲ��Ǳ�ʶ��
				if (returnValueStack.lastReturnWordType != ReturnValueStack::LastReturnWordType::Identifier) {
					// ������� ǰ׺++���ʽҪ����һ����ֵ ��ǰ�����ֻ�б�ʶ������ֵ
					throw(Exception("Should be a Left-Value after ++", wordTable[p], 1));
				}
				// ջ���Ǳ�ʶ�� ��ջ
				auto lastReturnValue = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ��ֵ�ǲ����ַ�������
				if (lastReturnValue->getWordProperties() == EnumWordProperties::String || lastReturnValue->getWordProperties() == EnumWordProperties::Function) {
					// ������� �ַ����ͺ�������ִ��ǰ׺++
					throw(Exception("Should be a Number after ++", wordTable[p], 1));
				}
				// ������Ԫʽ
				fourTable.AddFour("+", lastReturnValue->value, "1", lastReturnValue->value);
				fourTable.nowFourLine++;

				// ����ֵ����ֵջ ����ֵ����ǰ��ʶ��
				returnValueStack.returnValueStack.push(new Identifier(lastReturnValue->getWordProperties(), lastReturnValue->value));
				// �������һ�η���ֵ����
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
			// ���嶯��
			{
				// ���ȿ�ջ���ǲ��Ǳ�ʶ��
				if (returnValueStack.lastReturnWordType != ReturnValueStack::LastReturnWordType::Identifier) {
					// ������� ǰ׺--���ʽҪ����һ����ֵ ��ǰ�����ֻ�б�ʶ������ֵ
					throw(Exception("Should be a Left-Value after --", wordTable[p], 1));
				}
				// ջ���Ǳ�ʶ�� ��ջ
				auto lastReturnValue = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ��ֵ�ǲ����ַ�������
				if (lastReturnValue->getWordProperties() == EnumWordProperties::String || lastReturnValue->getWordProperties() == EnumWordProperties::Function) {
					// ������� �ַ����ͺ�������ִ��ǰ׺--
					throw(Exception("Should be a Number after --", wordTable[p], 1));
				}
				// ������Ԫʽ
				fourTable.AddFour("-", lastReturnValue->value, "1", lastReturnValue->value);
				fourTable.nowFourLine++;

				// ����ֵ����ֵջ ����ֵ����ǰ��ʶ��
				returnValueStack.returnValueStack.push(new Identifier(lastReturnValue->getWordProperties(), lastReturnValue->value));
				// �������һ�η���ֵ����
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
			// ���嶯��
			{
				// ���ȳ�ջ
				auto lastReturnValue = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ��ջ���ǲ��� �ַ�������
				if (lastReturnValue->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue->getWordProperties() == EnumWordProperties::Function) {
					// ������� ��Ŀ+���ʽҪ����һ����ֵ
					throw(Exception("Should be a Number after +", wordTable[p], 1));
				}
				// ����ֵ �鿴��ֵ���� ȡ��ʱ���� ������Ԫʽ
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				fourTable.AddFour("+", lastReturnValue->value, "", tempVarName);
				fourTable.nowFourLine++;
				tempVariableTablePointer++;

				// ����ֵ����ֵջ
				returnValueStack.returnValueStack.push(new TempVariable(lastReturnValue->getWordProperties(), tempVarName));
				// �������һ�η���ֵ����
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
			// ���嶯��
			{
				// ���ȳ�ջ
				auto lastReturnValue = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ��ջ���ǲ��� �ַ�������
				if (lastReturnValue->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue->getWordProperties() == EnumWordProperties::Function) {
					// ������� ��Ŀ+���ʽҪ����һ����ֵ
					throw(Exception("Should be a Number after +", wordTable[p], 1));
				}
				// ����ֵ �鿴��ֵ���� ȡ��ʱ���� ������Ԫʽ
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				fourTable.AddFour("-", "0", lastReturnValue->value, tempVarName);
				fourTable.nowFourLine++;
				tempVariableTablePointer++;

				// ����ֵ����ֵջ
				returnValueStack.returnValueStack.push(new TempVariable(lastReturnValue->getWordProperties(), tempVarName));
				// �������һ�η���ֵ����
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
			// ���嶯��
			{
				// ���ȳ�ջ
				auto lastReturnValue = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ��ջ���ǲ��� �ַ�������
				if (lastReturnValue->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue->getWordProperties() == EnumWordProperties::Function) {
					// �ַ����ͺ���ȡ����Ϊ�� ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("=", "0", "", tempVarName);
					fourTable.nowFourLine++;

					// ����ֵ����ֵջ
					returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
					// �������һ�η���ֵ����
					returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
					returnValueStack.lastReturnValueType = EnumWordProperties::Bool;
				}
				else {
					// ����ֵ ȡ��ʱ���� ������Ԫʽ
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

					// ����ֵ����ֵջ
					returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
					// �������һ�η���ֵ����
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

// func    ��Ŀ�����ʶ��
// param   
// return  
bool SemanticAnalysis::UnaryOperator() {
	// ���浱ǰָ��
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

// func    ����ת�����ʽʶ��
// param   
// return  
bool SemanticAnalysis::CastExpression() {
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
			// ���嶯��
			{
				// ���浱ǰ����ת������
				castTypeName = wordTable[p].property;
			}
			p++;
			if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
				p++;
				if (CastExpression()) {
					// ���嶯��
					{
						// ���ȳ�ջ
						auto lastReturnValue = returnValueStack.returnValueStack.top();
						returnValueStack.returnValueStack.pop();
						// ȡ��ʱ����
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						// �ж�Ŀ��ת������
						if (castTypeName == EnumWordProperties::Bool) {
							// ��ջ���ǲ��� �ַ�������
							if (lastReturnValue->getWordProperties() == EnumWordProperties::String ||
								lastReturnValue->getWordProperties() == EnumWordProperties::Function) {
								// �ַ����ͺ���ת��Ϊ Bool ��Ϊ�� ȡ��ʱ���� ������Ԫʽ
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
							// ����ֵ����ֵջ
							returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
							// �������һ�η���ֵ����
							returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
							returnValueStack.lastReturnValueType = EnumWordProperties::Bool;
						}
						// Ŀ��ת�������� Int
						else if (castTypeName == EnumWordProperties::Int) {
							// ���һ�η��������� Bool �� Char �� Int
							if (returnValueStack.lastReturnValueType == EnumWordProperties::Bool ||
								IsInt(returnValueStack.lastReturnValueType)) {
								fourTable.AddFour("=", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// ���һ�η��������� Float
							else if (IsFloat(returnValueStack.lastReturnValueType)) {
								fourTable.AddFour("FloatToInt", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// ���һ�η��������� String
							else if (returnValueStack.lastReturnValueType == EnumWordProperties::String) {
								fourTable.AddFour("StringToInt", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// ���һ�η��������� Function
							else if (returnValueStack.lastReturnValueType == EnumWordProperties::Function) {
								fourTable.AddFour("FunctionToInt", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// ����ֵ����ֵջ
							returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Int, tempVarName));
							// �������һ�η���ֵ����
							returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
							returnValueStack.lastReturnValueType = EnumWordProperties::Int;
						}
						// Ŀ��ת�������� Float
						else if (castTypeName == EnumWordProperties::Float) {
							// ���һ�η��������� Bool �� Char �� Int
							if (returnValueStack.lastReturnValueType == EnumWordProperties::Bool ||
								IsInt(returnValueStack.lastReturnValueType)) {
								fourTable.AddFour("IntToFloat", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// ���һ�η��������� Float
							else if (IsFloat(returnValueStack.lastReturnValueType)) {
								fourTable.AddFour("=", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// ���һ�η��������� String
							else if (returnValueStack.lastReturnValueType == EnumWordProperties::String) {
								// ������� String ����ת���� Float
								throw(Exception("Invalid Cast From String to Float", wordTable[p], 1));
							}
							// ���һ�η��������� Function
							else if (returnValueStack.lastReturnValueType == EnumWordProperties::Function) {
								// ������� Function ����ת���� Float
								throw(Exception("Invalid Cast From Function to Float", wordTable[p], 1));
							}
							// ����ֵ����ֵջ
							returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Float, tempVarName));
							// �������һ�η���ֵ����
							returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
							returnValueStack.lastReturnValueType = EnumWordProperties::Float;
						}
						// Ŀ��ת�������� Char
						else if (castTypeName == EnumWordProperties::Char) {
							// ���һ�η��������� Bool �� Char
							if (returnValueStack.lastReturnValueType == EnumWordProperties::Bool ||
								returnValueStack.lastReturnValueType == EnumWordProperties::Char ||
								returnValueStack.lastReturnValueType == EnumWordProperties::CharNumber) {
								fourTable.AddFour("=", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// ���һ�η��������� Int
							if (IsInt(returnValueStack.lastReturnValueType)) {
								fourTable.AddFour("IntToChar", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// ���һ�η��������� Float
							else if (IsFloat(returnValueStack.lastReturnValueType)) {
								fourTable.AddFour("FloatToChar", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// ���һ�η��������� String
							else if (returnValueStack.lastReturnValueType == EnumWordProperties::String) {
								fourTable.AddFour("StringToChar", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// ���һ�η��������� Function
							else if (returnValueStack.lastReturnValueType == EnumWordProperties::Function) {
								fourTable.AddFour("FunctionToChar", lastReturnValue->value, "", tempVarName);
								fourTable.nowFourLine++;
							}
							// ����ֵ����ֵջ
							returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Char, tempVarName));
							// �������һ�η���ֵ����
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

// func    �˳����ʽʶ��
// param   
// return  
bool SemanticAnalysis::MultiplicativeExpression() {
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
bool SemanticAnalysis::MultiplicativeExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// *CastExpression MultiplicativeExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorMultiply) {
		p++;
		if (CastExpression()) {
			// ���嶯��
			{
				// �Ѿ����������� CastExpression() ջ�д�����������ֵ ���������
				// ��ջ �鿴�ұߵ�����
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ��ջ �鿴��ߵ�����
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String �� Function ���ܲ�������
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// �������
					throw(Exception("Should be a Number To * Multiply", wordTable[p], 1));
				}
				// ��������
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// ����Ҫ����
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// û�� Float ֵ
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// ȡ��ʱ���� ������Ԫʽ
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour("*", lastReturnValue1->value, lastReturnValue2->value, tempVarName);
				fourTable.nowFourLine++;

				// ����ֵ����ֵջ
				returnValueStack.returnValueStack.push(new TempVariable(lastReturnValue1->getWordProperties(), tempVarName));
				// �������һ�η���ֵ����
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
			// ���嶯��
			{
				// �Ѿ����������� CastExpression() ջ�д�����������ֵ ���������
				// ��ջ �鿴�ұߵ�����
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ��ջ �鿴��ߵ�����
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String �� Function ���ܲ�������
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// �������
					throw(Exception("Should be a Number To / Divide", wordTable[p], 1));
				}
				// ��������
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// ����Ҫ����
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// û�� Float ֵ
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// ȡ��ʱ���� ������Ԫʽ
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour("/", lastReturnValue1->value, lastReturnValue2->value, tempVarName);
				fourTable.nowFourLine++;

				// ����ֵ����ֵջ
				returnValueStack.returnValueStack.push(new TempVariable(lastReturnValue1->getWordProperties(), tempVarName));
				// �������һ�η���ֵ����
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
			// ���嶯��
			{
				// �Ѿ����������� CastExpression() ջ�д�����������ֵ ���������
				// ��ջ �鿴�ұߵ�����
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ��ջ �鿴��ߵ�����
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String �� Function ���ܲ�������
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// �������
					throw(Exception("Should be a Number To % Mod", wordTable[p], 1));
				}
				// ��������
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// ������� ȡģ���������� Int
					throw(Exception("Should be a Int Value To % Mod", wordTable[p], 1));
				}
				// û�� Float ֵ
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// ȡ��ʱ���� ������Ԫʽ
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour("%", lastReturnValue1->value, lastReturnValue2->value, tempVarName);
				fourTable.nowFourLine++;

				// ����ֵ����ֵջ
				returnValueStack.returnValueStack.push(new TempVariable(lastReturnValue1->getWordProperties(), tempVarName));
				// �������һ�η���ֵ����
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = EnumWordProperties::Int;
			}
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
bool SemanticAnalysis::AdditiveExpression() {
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
bool SemanticAnalysis::AdditiveExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// +MultiplicativeExpression AdditiveExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorAdd) {
		p++;
		if (MultiplicativeExpression()) {
			// ���嶯��
			{
				// ��ջ �鿴�ұߵ�����
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ��ջ �鿴��ߵ�����
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String �� Function ���ܲ�������
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// �������
					throw(Exception("Should be a Number To + Add", wordTable[p], 1));
				}
				// ��������
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// ����Ҫ����
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// û�� Float ֵ
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// ȡ��ʱ���� ������Ԫʽ
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour("+", lastReturnValue1->value, lastReturnValue2->value, tempVarName);
				fourTable.nowFourLine++;

				// ����ֵ����ֵջ
				returnValueStack.returnValueStack.push(new TempVariable(lastReturnValue1->getWordProperties(), tempVarName));
				// �������һ�η���ֵ����
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
			// ���嶯��
			{
				// ��ջ �鿴�ұߵ�����
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ��ջ �鿴��ߵ�����
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String �� Function ���ܲ�������
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// �������
					throw(Exception("Should be a Number To - Subtract", wordTable[p], 1));
				}
				// ��������
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// ����Ҫ����
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// û�� Float ֵ
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// ȡ��ʱ���� ������Ԫʽ
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour("-", lastReturnValue1->value, lastReturnValue2->value, tempVarName);
				fourTable.nowFourLine++;

				// ����ֵ����ֵջ
				returnValueStack.returnValueStack.push(new TempVariable(lastReturnValue1->getWordProperties(), tempVarName));
				// �������һ�η���ֵ����
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = lastReturnValue1->getWordProperties();
			}
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
bool SemanticAnalysis::ShiftExpression() {
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
bool SemanticAnalysis::ShiftExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// << AdditiveExpression ShiftExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorLeftShift) {
		p++;
		if (AdditiveExpression()) {
			// ���嶯��
			{
				// ��ջ �鿴�ұߵ�����
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ��ջ �鿴��ߵ�����
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String �� Function ���ܲ�������
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// �������
					throw(Exception("Should be a Number To << Shift", wordTable[p], 1));
				}
				// ��������
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// ������� ȡģ���������� Int
					throw(Exception("Should be a Int Value To << Shift", wordTable[p], 1));
				}
				// û�� Float ֵ
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// ȡ��ʱ���� ������Ԫʽ
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour("<<", lastReturnValue1->value, lastReturnValue2->value, tempVarName);
				fourTable.nowFourLine++;

				// ����ֵ����ֵջ
				returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Int, tempVarName));
				// �������һ�η���ֵ����
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
			// ���嶯��
			{
				// ��ջ �鿴�ұߵ�����
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ��ջ �鿴��ߵ�����
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String �� Function ���ܲ�������
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// �������
					throw(Exception("Should be a Number To >> Shift", wordTable[p], 1));
				}
				// ��������
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// ������� ȡģ���������� Int
					throw(Exception("Should be a Int Value To >> Shift", wordTable[p], 1));
				}
				// û�� Float ֵ
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// ȡ��ʱ���� ������Ԫʽ
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				fourTable.AddFour(">>", lastReturnValue1->value, lastReturnValue2->value, tempVarName);
				fourTable.nowFourLine++;

				// ����ֵ����ֵջ
				returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Int, tempVarName));
				// �������һ�η���ֵ����
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = EnumWordProperties::Int;
			}
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
bool SemanticAnalysis::RelationalExpression() {
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
bool SemanticAnalysis::RelationalExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// < ShiftExpression RelationalExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorSmallerThan) {
		p++;
		if (ShiftExpression()) {
			// ���嶯��
			{
				// ��ջ �鿴�ұߵ�����
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ��ջ �鿴��ߵ�����
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String �� Function ���ܲ�������
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// �������
					throw(Exception("Should be a Number To Compare <", wordTable[p], 1));
				}
				// ��������
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// ����Ҫ����
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// û�� Float ֵ
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// ȡ��ʱ���� ������Ԫʽ
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

				// ����ֵ����ֵջ
				returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
				// �������һ�η���ֵ����
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
			// ���嶯��
			{
				// ��ջ �鿴�ұߵ�����
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ��ջ �鿴��ߵ�����
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String �� Function ���ܲ�������
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// �������
					throw(Exception("Should be a Number To Compare >", wordTable[p], 1));
				}
				// ��������
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// ����Ҫ����
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// û�� Float ֵ
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// ȡ��ʱ���� ������Ԫʽ
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

				// ����ֵ����ֵջ
				returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
				// �������һ�η���ֵ����
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
			// ���嶯��
			{
				// ��ջ �鿴�ұߵ�����
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ��ջ �鿴��ߵ�����
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String �� Function ���ܲ�������
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// �������
					throw(Exception("Should be a Number To Compare <=", wordTable[p], 1));
				}
				// ��������
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// ����Ҫ����
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// û�� Float ֵ
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// ȡ��ʱ���� ������Ԫʽ
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

				// ����ֵ����ֵջ
				returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
				// �������һ�η���ֵ����
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
			// ���嶯��
			{
				// ��ջ �鿴�ұߵ�����
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ��ջ �鿴��ߵ�����
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String �� Function ���ܲ�������
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// �������
					throw(Exception("Should be a Number To Compare >=", wordTable[p], 1));
				}
				// ��������
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// ����Ҫ����
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// û�� Float ֵ
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// ȡ��ʱ���� ������Ԫʽ
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

				// ����ֵ����ֵջ
				returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
				// �������һ�η���ֵ����
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = EnumWordProperties::Bool;
			}
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
bool SemanticAnalysis::EqualityExpression() {
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
bool SemanticAnalysis::EqualityExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// == RelationalExpression EqualityExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorEqual) {
		p++;
		if (RelationalExpression()) {
			// ���嶯��
			{
				// ��ջ �鿴�ұߵ�����
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ��ջ �鿴��ߵ�����
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String �� Function ���ܲ�������
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// �������
					throw(Exception("Should be a Number To Compare ==", wordTable[p], 1));
				}
				// ��������
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// ����Ҫ����
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// û�� Float ֵ
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// ȡ��ʱ���� ������Ԫʽ
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

				// ����ֵ����ֵջ
				returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
				// �������һ�η���ֵ����
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
			// ���嶯��
			{
				// ��ջ �鿴�ұߵ�����
				auto lastReturnValue2 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// ��ջ �鿴��ߵ�����
				auto lastReturnValue1 = returnValueStack.returnValueStack.top();
				returnValueStack.returnValueStack.pop();
				// String �� Function ���ܲ�������
				if (lastReturnValue1->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::Function ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
					// �������
					throw(Exception("Should be a Number To Compare !=", wordTable[p], 1));
				}
				// ��������
				if (IsFloat(lastReturnValue1->getWordProperties()) ||
					IsFloat(lastReturnValue2->getWordProperties())) {
					// ����Ҫ����
					if (IsFloat(lastReturnValue1->getWordProperties()) &&
						IsFloat(lastReturnValue2->getWordProperties())) {
					}
					else if (IsFloat(lastReturnValue1->getWordProperties())) {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
					else {
						// ȡ��ʱ���� ������Ԫʽ
						std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
						tempVariableTablePointer++;
						fourTable.AddFour("IntToFloat", lastReturnValue1->value, "", tempVarName);
						fourTable.nowFourLine++;
						lastReturnValue1 = new TempVariable(EnumWordProperties::Float, tempVarName);
					}
				}
				// û�� Float ֵ
				else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue1->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue1->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue1 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
					lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
					// ȡ��ʱ���� ������Ԫʽ
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
					fourTable.nowFourLine++;
					lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
				}
				// ȡ��ʱ���� ������Ԫʽ
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

				// ����ֵ����ֵջ
				returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
				// �������һ�η���ֵ����
				returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
				returnValueStack.lastReturnValueType = EnumWordProperties::Bool;
			}
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
bool SemanticAnalysis::BitAndExpression() {
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
bool SemanticAnalysis::BitAndExpressionEliminateLeft() {
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
bool SemanticAnalysis::BitXOrExpression() {
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
bool SemanticAnalysis::BitXOrExpressionEliminateLeft() {
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
bool SemanticAnalysis::BitOrExpression() {
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
bool SemanticAnalysis::BitOrExpressionEliminateLeft() {
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
bool SemanticAnalysis::AndExpression() {
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
bool SemanticAnalysis::AndExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// && BitOrExpression AndExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorAnd) {
		p++;
		// ���嶯��
		{
			// ջ��ֵת bool ֵ
			ToBool();
			// ȡ bool ֵ
			auto lastReturnValue = returnValueStack.returnValueStack.top();
			returnValueStack.returnValueStack.pop();

			// �����к���ջ
			andBackFillStack.push(fourTable.nowFourLine);

			// ��ת��� ������
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
	// ��
	else {
		// ���嶯��
		{
			// ֻ������Ҫ�����ʱ���ִ�� ��ֹ����Ҫ�� bool ת��
			if (andBackFillStack.size()) {

				// ջ��ֵת bool ֵ �����߼�����ʽ��ֵ����ǰջ���� bool ֵ
				ToBool();

				// ����ջ�������к�
				while (andBackFillStack.size()) {
					int i = andBackFillStack.top();
					andBackFillStack.pop();
					// ָ��� 0 ����ʱ��������
					fourTable.table[i].dest = std::to_string(fourTable.nowFourLine - 1);
				}
			}
		}
		p = nowP;
		return true;
	}
}

// func    �߼�����ʽʶ��
// param   
// return  
bool SemanticAnalysis::OrExpression() {
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
bool SemanticAnalysis::OrExpressionEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// || AndExpression OrExpressionEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorOr) {
		p++;
		// ���嶯��
		{
			// ջ��ֵת bool ֵ
			ToBool();
			// ȡ bool ֵ
			auto lastReturnValue = returnValueStack.returnValueStack.top();
			returnValueStack.returnValueStack.pop();

			// �����к���ջ
			orBackFillStack.push(fourTable.nowFourLine);

			// ��ת��� ������
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
	// ��
	else {
		// ���嶯��
		{
			// ֻ������Ҫ�����ʱ���ִ�� ��ֹ����Ҫ�� bool ת��
			if (orBackFillStack.size()) {

				// ջ��ֵת bool ֵ �����߼�����ʽ��ֵ����ǰջ���� bool ֵ
				ToBool();

				// ����ջ�������к�
				while (orBackFillStack.size()) {
					int i = orBackFillStack.top();
					orBackFillStack.pop();
					// ָ��� 1 ����ʱ��������
					fourTable.table[i].dest = std::to_string(fourTable.nowFourLine - 3);
				}
			}
		}
		p = nowP;
		return true;
	}
}

// func    �������ʽʶ��
// param   
// return  
bool SemanticAnalysis::ConditionalExpression() {
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
bool SemanticAnalysis::AssignmentExpression() {
	// ���浱ǰָ��
	int nowP = p;
	int nowTempVariableTablePointer = tempVariableTablePointer;
	// ������Ԫʽ��
	auto nowFourTable = FourTable(fourTable);
	// ������ֵջ
	auto nowReturnValueStack = ReturnValueStack(returnValueStack);

	if (UnaryExpression()) {
		if (AssignmentOperator()) {
			// ���浱ǰ��ֵ���������
			EnumWordProperties e = wordTable[p - 1].property;
			// ���嶯��
			{
				// ���ȿ�ջ���ǲ��Ǳ�ʶ��
				if (returnValueStack.lastReturnWordType != ReturnValueStack::LastReturnWordType::Identifier) {
					// ������� ��ֵ���ʽҪ����һ����ֵ ��ǰ�����ֻ�б�ʶ������ֵ
					throw(Exception("Should be a Left-Value after = Assignment", wordTable[p], 1));
				}
			}
			if (AssignmentExpression()) {
				// ���嶯��
				{
					// ��ջ �鿴�ұߵ�����
					auto lastReturnValue2 = returnValueStack.returnValueStack.top();
					returnValueStack.returnValueStack.pop();
					// ��ջ �鿴��ߵ�����
					auto lastReturnValue1 = returnValueStack.returnValueStack.top();
					returnValueStack.returnValueStack.pop();

					// ���ҷ��ű�
					auto [returnBool, id] = SearchIdentifier(identifierTablePointer, lastReturnValue1->value);
					// δ����
					if (!returnBool) {
						// �������
						throw(Exception("Identifier Not defined", wordTable[p], 1));
					}
					if (e != EnumWordProperties::OperatorModAssign) {
						// ����ת��
						if (!((IsFloat(lastReturnValue1->getWordProperties()) &&
							IsFloat(lastReturnValue2->getWordProperties())) ||
							(IsTrueInt(lastReturnValue1->getWordProperties()) &&
								(IsTrueInt(lastReturnValue2->getWordProperties())) ||
								(IsChar(lastReturnValue1->getWordProperties()) &&
									(IsChar(lastReturnValue2->getWordProperties()))))
							)) {
							// String �� Function �������Զ�����ת��
							if (lastReturnValue2->getWordProperties() == EnumWordProperties::String ||
								lastReturnValue2->getWordProperties() == EnumWordProperties::Function) {
								// �������
								throw(Exception("Can't Cast To Bool", wordTable[p], 1));
							}
							// תΪ Bool
							if (lastReturnValue1->getWordProperties() == EnumWordProperties::Bool) {
								// ȡ��ʱ����
								std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
								tempVariableTablePointer++;
								// ������Ԫʽ
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
							// תΪ Float
							else if (IsFloat(lastReturnValue1->getWordProperties()) &&
								!IsFloat(lastReturnValue2->getWordProperties())) {
								// ȡ��ʱ���� ������Ԫʽ
								std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
								tempVariableTablePointer++;
								fourTable.AddFour("IntToFloat", lastReturnValue2->value, "", tempVarName);
								fourTable.nowFourLine++;
								lastReturnValue2 = new TempVariable(EnumWordProperties::Float, tempVarName);
							}
							// Float ת Int
							else if (IsInt(lastReturnValue1->getWordProperties()) &&
								IsFloat(lastReturnValue2->getWordProperties())) {
								// ȡ��ʱ���� ������Ԫʽ
								std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
								tempVariableTablePointer++;
								fourTable.AddFour("FloatToInt", lastReturnValue2->value, "", tempVarName);
								fourTable.nowFourLine++;
								lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
							}
							// Int ת Char
							else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char &&
								IsInt(lastReturnValue2->getWordProperties())) {
								// ȡ��ʱ���� ������Ԫʽ
								std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
								tempVariableTablePointer++;
								fourTable.AddFour("IntToChar", lastReturnValue2->value, "", tempVarName);
								fourTable.nowFourLine++;
								lastReturnValue2 = new TempVariable(EnumWordProperties::Char, tempVarName);
							}
							// Float ת Char ����
							else if (lastReturnValue1->getWordProperties() == EnumWordProperties::Char &&
								IsFloat(lastReturnValue2->getWordProperties())) {
								// �������
								throw(Exception("Can't Cast From Float To Char", wordTable[p], 1));
							}
						}
						if (e == EnumWordProperties::OperatorAssign) {
							// ��ֵ��Ԫʽ
							fourTable.AddFour("=", lastReturnValue2->value, "", lastReturnValue1->value);
							fourTable.nowFourLine++;
						}
						else if (e == EnumWordProperties::OperatorAddAssign) {
							// ��ֵ��Ԫʽ
							fourTable.AddFour("+", lastReturnValue1->value, lastReturnValue2->value, lastReturnValue1->value);
							fourTable.nowFourLine++;
						}
						else if (e == EnumWordProperties::OperatorSubAssign) {
							// ��ֵ��Ԫʽ
							fourTable.AddFour("-", lastReturnValue1->value, lastReturnValue2->value, lastReturnValue1->value);
							fourTable.nowFourLine++;
						}
						else if (e == EnumWordProperties::OperatorMulAssign) {
							// ��ֵ��Ԫʽ
							fourTable.AddFour("*", lastReturnValue1->value, lastReturnValue2->value, lastReturnValue1->value);
							fourTable.nowFourLine++;
						}
						else if (e == EnumWordProperties::OperatorDivAssign) {
							// ��ֵ��Ԫʽ
							fourTable.AddFour("/", lastReturnValue1->value, lastReturnValue2->value, lastReturnValue1->value);
							fourTable.nowFourLine++;
						}
					}
					else {
						if (IsFloat(lastReturnValue1->getWordProperties()) ||
							IsFloat(lastReturnValue2->getWordProperties())) {
							// ������� ȡģ���������� Int
							throw(Exception("Should be a Int Value To % Mod", wordTable[p], 1));
						}
						// û�� Float ֵ
						else if (lastReturnValue2->getWordProperties() == EnumWordProperties::Char ||
							lastReturnValue2->getWordProperties() == EnumWordProperties::CharNumber) {
							// ȡ��ʱ���� ������Ԫʽ
							std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
							tempVariableTablePointer++;
							fourTable.AddFour("CharToInt", lastReturnValue2->value, "", tempVarName);
							fourTable.nowFourLine++;
							lastReturnValue2 = new TempVariable(EnumWordProperties::Int, tempVarName);
						}
						// ��ֵ��Ԫʽ
						fourTable.AddFour("%", lastReturnValue1->value, lastReturnValue2->value, lastReturnValue1->value);
						fourTable.nowFourLine++;
					}

					// ����ֵ����ֵջ
					returnValueStack.returnValueStack.push(lastReturnValue1);
					// �������һ�η���ֵ����
					returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::Identifier;
					returnValueStack.lastReturnValueType = lastReturnValue1->getWordProperties();
				}
				return true;
			}
			throw(Exception("Not a Assignment Expression because Expect a Assignment Expression", wordTable[p], 0));
		}
		// ����
		{
			p = nowP;
			tempVariableTablePointer = nowTempVariableTablePointer;
			// ������ǰ��ֵջ
			returnValueStack.~ReturnValueStack();
			// �ָ���ֵջ
			returnValueStack = ReturnValueStack(nowReturnValueStack);
			// ������ǰ��Ԫʽ��
			fourTable.~FourTable();
			// �ָ���Ԫʽ��
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

// func    ��ֵ�����ʶ��
// param   
// return  
bool SemanticAnalysis::AssignmentOperator() {
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
bool SemanticAnalysis::Expression() {
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
bool SemanticAnalysis::ExpressionEliminateLeft() {
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
bool SemanticAnalysis::Declaration() {
	// ���浱ǰָ��
	int nowP = p;

	// TypeName InitDeclaratorList;
	if (IsTypeName(wordTable[p])) {
		// ���嶯��
		{
			// ���浱ǰ��������
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

// func    ��ʼ�������б�ʶ��
// param   
// return  
bool SemanticAnalysis::InitDeclaratorList() {
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
bool SemanticAnalysis::InitDeclaratorListEliminateLeft() {
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
bool SemanticAnalysis::InitDeclarator() {
	// ���浱ǰָ��
	int nowP = p;

	// DirectDeclarator
	if (DirectDeclarator()) {
		// DirectDeclarator = AssignmentExpression
		if (wordTable[p].property == EnumWordProperties::OperatorAssign) {
			p++;
			if (AssignmentExpression()) {
				// ���嶯��
				{
					// �øñ��ʽ��ʼ����ʶ��
					// ��ջ �鿴�ұߵ�����
					auto lastReturnValue = returnValueStack.returnValueStack.top();
					returnValueStack.returnValueStack.pop();

					// ֱ�ӳ�ʼ��
					if ((declarationTypeName == EnumWordProperties::Int &&
						IsTrueInt(lastReturnValue->getWordProperties())) ||
						(declarationTypeName == EnumWordProperties::Float &&
							IsFloat(lastReturnValue->getWordProperties())) ||
						(declarationTypeName == EnumWordProperties::StringType &&
							lastReturnValue->getWordProperties() == EnumWordProperties::String) ||
						(declarationTypeName == EnumWordProperties::Char &&
							lastReturnValue->getWordProperties() == EnumWordProperties::CharNumber)) {
						// ��ʼ����Ԫʽ
						fourTable.AddFour("=", lastReturnValue->value, "", declarationIdentifier.value);
						fourTable.nowFourLine++;
					}
					// ����ת��
					else if (!((IsFloat(declarationTypeName) &&
						IsFloat(lastReturnValue->getWordProperties())) ||
						(IsTrueInt(declarationTypeName) &&
							(IsTrueInt(lastReturnValue->getWordProperties())) ||
							(IsChar(declarationTypeName) &&
								(IsChar(lastReturnValue->getWordProperties())))))) {
						// String �� Function �������Զ�����ת��
						if (lastReturnValue->getWordProperties() == EnumWordProperties::String ||
							lastReturnValue->getWordProperties() == EnumWordProperties::Function) {
							// �������
							throw(Exception("Can't Cast", wordTable[p], 1));
						}
						// תΪ Bool
						if (declarationTypeName == EnumWordProperties::Bool) {
							// ȡ��ʱ����
							std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
							tempVariableTablePointer++;
							// ������Ԫʽ
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
						// תΪ Float
						else if (IsFloat(declarationTypeName) &&
							!IsFloat(lastReturnValue->getWordProperties())) {
							// ȡ��ʱ���� ������Ԫʽ
							std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
							tempVariableTablePointer++;
							fourTable.AddFour("IntToFloat", lastReturnValue->value, "", tempVarName);
							fourTable.nowFourLine++;
							lastReturnValue = new TempVariable(EnumWordProperties::Float, tempVarName);
						}
						// Float ת Int
						else if (IsInt(declarationTypeName) &&
							IsFloat(lastReturnValue->getWordProperties())) {
							// ȡ��ʱ���� ������Ԫʽ
							std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
							tempVariableTablePointer++;
							fourTable.AddFour("FloatToInt", lastReturnValue->value, "", tempVarName);
							fourTable.nowFourLine++;
							lastReturnValue = new TempVariable(EnumWordProperties::Int, tempVarName);
						}
						// Int ת Char
						else if (declarationTypeName == EnumWordProperties::Char &&
							IsInt(lastReturnValue->getWordProperties())) {
							// ȡ��ʱ���� ������Ԫʽ
							std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
							tempVariableTablePointer++;
							fourTable.AddFour("IntToChar", lastReturnValue->value, "", tempVarName);
							fourTable.nowFourLine++;
							lastReturnValue = new TempVariable(EnumWordProperties::Char, tempVarName);
						}
						// Float ת Char ����
						else if (declarationTypeName == EnumWordProperties::Char &&
							IsFloat(lastReturnValue->getWordProperties())) {
							// �������
							throw(Exception("Can't Cast From Float To Char", wordTable[p], 1));
						}
						// ���� ת String ����
						else if (declarationTypeName == EnumWordProperties::StringType) {
							// �������
							throw(Exception("Can't Cast To String", wordTable[p], 1));
						}

						// ת������ ��ʼ��
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

// func    ֱ��������ʶ��
// param   
// return  
bool SemanticAnalysis::DirectDeclarator() {
	// ���浱ǰָ��
	int nowP = p;

	// Identifier DirectDeclaratorEliminateLeft
	if (wordTable[p].property == EnumWordProperties::Identifier) {
		// ���嶯��
		{
			bool isFunction = false;
			// ���ǲ��Ǻ���
			for (auto& i : functionTable.functionTable) {
				// �Ǻ���
				if (i.value == wordTable[p].word) {
					lastFunctionIdentifier = wordTable[p].word;
					isFunction = true;
					break;
				}
			}
			
			// ���ҵ�ǰ�� �Ƿ��ض���
			auto [returnBool, id] = SearchIdentifierInCurrentTable(identifierTablePointer, wordTable[p].word);
			// �ض���
			if (!isFunction&&returnBool) {
				// �������
				throw(Exception("Identifier have defined", wordTable[p], 1));
			}
			if (!returnBool) {
				// ��ʶ����� �ݴ�ñ�ʶ��
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

// func    ֱ��������������ݹ�ʶ��
// param   
// return  
bool SemanticAnalysis::DirectDeclaratorEliminateLeft() {
	// ���浱ǰָ��
	int nowP = p;

	// (ParameterList) DirectDeclaratorEliminateLeft
	if (wordTable[p].property == EnumWordProperties::OperatorLeftRound) {
		p++;
		// ���嶯��
		// ˵���Ǻ��� ����׼�����뺯������
		{
			bool isDefined = false;
			// �ض���
			for (auto& i : functionTable.functionTable) {
				if (i.value == identifierTablePointer->table[identifierTablePointer->IdentifierNumber - 1].value) {
					isDefined = true;
				}
			}
			if (!isDefined) {
				// �������뺯����
				functionTable.AddFunction(FunctionIdentifier(declarationTypeName,
					identifierTablePointer->table[identifierTablePointer->IdentifierNumber - 1].value));
			}

			// ���ڽ������ǲ����б� �����б��е�ÿ�������������������ͺ�ֱ�����������
			// ֱ����������鿴��ʶ���ض��� ��˺���������Ҫһ���µ�������
			// �µ������� ����ͷ������β����һ����
			identifierTablePointer = new IdentifierTable(identifierTablePointer);
			// ���ű�Ҫ��������Ŀ�
			functionHeadIdentifierTablePointer = identifierTablePointer;
		}
		// �ڲ����б��� ÿ����������ӵ��������ĺ�����
		if (ParameterList()) {
			if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
				p++;
				// ���嶯��
				{
					// ����ͷ���ֽ����� �������� ������ͷ�������� �Ա����
					identifierTablePointer = identifierTablePointer->fatherTablePointer;
				}
				// ���￴���� ������ (ParameterList) ?
				if (DirectDeclaratorEliminateLeft()) {
					return true;
				}
			}
			throw(Exception("Not a Function Declaration because Expect a )", wordTable[p], 0));
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
bool SemanticAnalysis::ParameterList() {
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
bool SemanticAnalysis::ParameterListEliminateLeft() {
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
bool SemanticAnalysis::ParameterDeclaration() {
	// ���浱ǰָ��
	int nowP = p;

	// TypeName DirectDeclarator
	if (IsTypeName(wordTable[p])) {
		p++;
		if (DirectDeclarator()) {
			// ���嶯��
			{
				// �������� ����������Ӳ�������
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

// func    ���ʶ��
// param   
// return  
bool SemanticAnalysis::Statement() {
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
bool SemanticAnalysis::LabeledStatement() {
	// ���浱ǰָ��
	int nowP = p;

	// Identifier : Statement
	if (wordTable[p].property == EnumWordProperties::Identifier) {
		p++;
		if (wordTable[p].property == EnumWordProperties::OperatorColon) {
			// ���嶯��
			{
				// ����Ҫ�ѱ�ǩ�����ǩ��
				// ȡ��ǩ��ջ��
				auto& labelTable = labelTableStack.top();
				auto [exist, defined, i] = labelTable.SearchLabelIdentifier(wordTable[p - 1].word);
				// �ض���
				if (exist && defined) {
					throw(Exception("Label have defined", wordTable[p - 1], 1));
				}
				if (exist) {
					labelTable.definedLabel[i] = true;
					labelTable.backFillLines[i] = fourTable.nowFourLine;
				}
				else {
					// ���Ѷ����ǩ���
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

// func    �������ʶ��
// param   
// return  
bool SemanticAnalysis::CompoundStatement() {
	// ���浱ǰָ��
	int nowP = p;

	// { BlockItemList }
	if (wordTable[p].property == EnumWordProperties::OperatorLeftBrace) {
		p++;
		// ���嶯��
		{
			// �µ�������
			identifierTablePointer = new IdentifierTable(identifierTablePointer);

			// �Ƿ�Ϊ������
			if (functionLeftCompound) {
				// �������
				for (auto& i : (functionHeadIdentifierTablePointer->table)) {
					identifierTablePointer->AddIdentifier(i);
				}
				// �������־�� false
				functionLeftCompound = false;

				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				// ��ջ����λ��
				fourTable.AddFour("pop", "", "", tempVarName);
				fourTable.nowFourLine++;

				// �βγ�ջ
				for (auto& i : identifierTablePointer->table) {
					// ��Ԫʽ
					fourTable.AddFour("pop", "", "", i.value);
					fourTable.nowFourLine++;
				}

				// ��ջ����λ��
				fourTable.AddFour("push", tempVarName, "", "");
				fourTable.nowFourLine++;
			}
		}
		if (BlockItemList()) {
			if (wordTable[p].property == EnumWordProperties::OperatorRightBrace) {
				p++;
				// ���嶯��
				{
					// �뿪������
					identifierTablePointer = identifierTablePointer->fatherTablePointer;
					// ����Ǻ���������
					if (identifierTablePointer->fatherTablePointer == nullptr) {
						// ȡ��ǩ��ջ��
						auto& labelTable = labelTableStack.top();

						// ������ǩ
						for (int i = 0; i < labelTable.labelTable.size(); i++) {
							if (!labelTable.definedLabel[i]) {
								// ����Ŀ���ǩδ�������ת���
								throw(Exception("Label Not defined", wordTable[p], 1));
							}
							// ������ǩ�Ĵ�������
							for (auto j : labelTable.backFillTable[i]) {
								// �����ǩ
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

// func    ����Ŀ�б�ʶ��
// param   
// return  
bool SemanticAnalysis::BlockItemList() {
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
bool SemanticAnalysis::BlockItemListEliminateLeft() {
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
bool SemanticAnalysis::BlockItem() {
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
bool SemanticAnalysis::ExpressionStatement() {
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
bool SemanticAnalysis::SelectionStatement() {
	// ���浱ǰָ��
	int nowP = p;

	// If (Expression) Statement
	if (wordTable[p].property == EnumWordProperties::If) {
		p++;
		if (wordTable[p].property == EnumWordProperties::OperatorLeftRound) {
			p++;
			if (Expression()) {
				if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
					// ���嶯��
					{
						// ��ת Bool ֵ
						ToBool();
						auto lastReturnValue = returnValueStack.returnValueStack.top();
						returnValueStack.returnValueStack.pop();
						// �����к���ջ
						selectBackFillStack.push(fourTable.nowFourLine);
						// ��ת��� ������
						fourTable.AddFour("jz", lastReturnValue->value, "", "");
						fourTable.nowFourLine++;
					}
					p++;
					if (Statement()) {
						// ���嶯��
						{
							// ����
							int backFillLine = selectBackFillStack.top();
							selectBackFillStack.pop();
							fourTable.table[backFillLine].dest = std::to_string(fourTable.nowFourLine + 1);
						}
						// If (Expression) Statement Else Statement
						if (wordTable[p].property == EnumWordProperties::Else) {
							p++;
							// ���嶯��
							{
								// �����к���ջ
								selectBackFillStack.push(fourTable.nowFourLine);
								// ��ת��� ������
								fourTable.AddFour("jmp", "", "", "");
								fourTable.nowFourLine++;
							}
							if (Statement()) {
								// ���嶯��
								{
									// ����
									int backFillLine = selectBackFillStack.top();
									selectBackFillStack.pop();
									fourTable.table[backFillLine].dest = std::to_string(fourTable.nowFourLine);
								}
								return true;
							}
						}
						// If (Expression) Statement
						else {
							// ���嶯��
							{
								// ��ת��� ��else ����һ��
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

// func    �������ʶ��
// param   
// return  
bool SemanticAnalysis::IterationStatement() {
	// ���浱ǰָ��
	int nowP = p;

	// While(Expression) Statement
	if (wordTable[p].property == EnumWordProperties::While) {
		p++;
		if (wordTable[p].property == EnumWordProperties::OperatorLeftRound) {
			p++;
			// ���嶯��
			{
				// ����ο��кż���ջ
				iterateBackFillStack.push(fourTable.nowFourLine);
				// �µĵ�����
				iterateBreakLayerStack.push(new std::vector<int>);
				iterateContinueLayerStack.push(new std::vector<int>);
			}
			if (Expression()) {
				if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
					// ���嶯��
					{
						// ��ת Bool ֵ
						ToBool();
						auto lastReturnValue = returnValueStack.returnValueStack.top();
						returnValueStack.returnValueStack.pop();
						// �����к���ջ
						iterateBackFillStack.push(fourTable.nowFourLine);
						// ��ת��� ������
						fourTable.AddFour("jz", lastReturnValue->value, "", "");
						fourTable.nowFourLine++;
					}
					p++;
					if (Statement()) {
						// ���嶯��
						{
							// ����
							int backFillLine = iterateBackFillStack.top();
							iterateBackFillStack.pop();
							fourTable.table[backFillLine].dest = std::to_string(fourTable.nowFourLine + 1);
							// ȡ�ο��к�
							backFillLine = iterateBackFillStack.top();
							iterateBackFillStack.pop();
							// ��ǰ�������ջ
							auto currentBreakLayer = iterateBreakLayerStack.top();
							iterateBreakLayerStack.pop();
							auto currentContinueLayer = iterateContinueLayerStack.top();
							iterateContinueLayerStack.pop();
							// ���ǰ������� break continue
							for (auto i : *currentBreakLayer) {
								fourTable.table[i].dest = std::to_string(fourTable.nowFourLine + 1);
							}
							for (auto i : *currentContinueLayer) {
								fourTable.table[i].dest = std::to_string(backFillLine);
							}
							// ������ǰ������
							currentBreakLayer->~vector();
							currentContinueLayer->~vector();
							// ��ת���
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
		// ���嶯��
		{
			// ����ο��кż���ջ
			iterateBackFillStack.push(fourTable.nowFourLine);
			// �µĵ�����
			iterateBreakLayerStack.push(new std::vector<int>);
			iterateContinueLayerStack.push(new std::vector<int>);
		}
		if (Statement()) {
			if (wordTable[p].property == EnumWordProperties::While) {
				p++;
				if (wordTable[p].property == EnumWordProperties::OperatorLeftRound) {
					p++;
					// ���嶯��
					{
						// ��ǰ�������ջ
						auto currentContinueLayer = iterateContinueLayerStack.top();
						iterateContinueLayerStack.pop();
						// ���ǰ������� continue
						for (auto i : *currentContinueLayer) {
							fourTable.table[i].dest = std::to_string(fourTable.nowFourLine);
						}
						// ������ǰ������
						currentContinueLayer->~vector();
					}
					if (Expression()) {
						if (wordTable[p].property == EnumWordProperties::OperatorRightRound) {
							p++;
							if (wordTable[p].property == EnumWordProperties::Semicolon) {
								p++;
								// ���嶯��
								{
									// ��ת Bool ֵ
									ToBool();
									auto lastReturnValue = returnValueStack.returnValueStack.top();
									returnValueStack.returnValueStack.pop();
									// ȡ�ο��к�
									int backFillLine = iterateBackFillStack.top();
									iterateBackFillStack.pop();
									// ��ת���
									fourTable.AddFour("jnz", lastReturnValue->value, "", std::to_string(backFillLine));
									fourTable.nowFourLine++;
									// ��ǰ�������ջ
									auto currentBreakLayer = iterateBreakLayerStack.top();
									iterateBreakLayerStack.pop();
									// ���ǰ������� break
									for (auto i : *currentBreakLayer) {
										fourTable.table[i].dest = std::to_string(fourTable.nowFourLine);
									}
									// ������ǰ������
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

// func    ��ת���ʶ��
// param   
// return  
bool SemanticAnalysis::JumpStatement() {
	// ���浱ǰָ��
	int nowP = p;

	// Goto Identifier;
	if (wordTable[p].property == EnumWordProperties::Goto) {
		p++;
		if (wordTable[p].property == EnumWordProperties::Identifier) {
			p++;
			if (wordTable[p].property == EnumWordProperties::Semicolon) {
				// ���嶯��
				{
					// ȡ��ǩ��ջ��
					auto& labelTable = labelTableStack.top();
					auto [exist, defined, i] = labelTable.SearchLabelIdentifier(wordTable[p - 1].word);
					// �Ѵ���
					if (exist) {
						// ����������
						labelTable.AddLabelBackFillLine(fourTable.nowFourLine, wordTable[p - 1].word);
					}
					else {
						// �ñ�ǩ���
						labelTable.AddUndefinedLabel(wordTable[p - 1].word);
						// ����������
						labelTable.AddLabelBackFillLine(fourTable.nowFourLine, wordTable[p - 1].word);
					}
					// ��ת���
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
			// ���嶯��
			{
				// ������ continue
				if (iterateContinueLayerStack.empty()) {
					// �������
					throw(Exception("Not Find Iteration Statement But a continue Statement", wordTable[p], 1));
				}
				// ����ο��к���ջ
				iterateContinueLayerStack.top()->push_back(fourTable.nowFourLine);
				// ��ת���
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
			// ���嶯��
			{
				// ������ break
				if (iterateBreakLayerStack.empty()) {
					// �������
					throw(Exception("Not Find Iteration Statement But a break Statement", wordTable[p], 1));
				}
				// ����ο��к���ջ
				iterateBreakLayerStack.top()->push_back(fourTable.nowFourLine);
				// ��ת���
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
			// ���嶯��
			{
				std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
				tempVariableTablePointer++;
				// ��ջ����λ��
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
				// ���嶯��
				{
					auto lastReturnValue = returnValueStack.returnValueStack.top();
					returnValueStack.returnValueStack.pop();

					// ��ʱ���� �淵��λ��
					std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
					tempVariableTablePointer++;
					// ��ջ����λ��
					fourTable.AddFour("pop", "", "", tempVarName);
					fourTable.nowFourLine++;

					// ����ת�� ����д��
					// ��ջ����ֵ
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

// func    ���뵥Ԫʶ��
// param   
// return  
bool SemanticAnalysis::TranslationUnit() {
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
bool SemanticAnalysis::TranslationUnitEliminateLeft() {
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
bool SemanticAnalysis::ExternalDeclaration() {
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
bool SemanticAnalysis::FunctionDefinition() {
	// ���浱ǰָ��
	int nowP = p;

	// TypeName DirectDeclarator /* DeclarationList */ CompoundStatement
	if (IsTypeName(wordTable[p])) {
		p++;
		// ������
		if (DirectDeclarator()) {
			// ��Ȼ�� { ��ͷ
			// TypeName DirectDeclarator CompoundStatement
			if (wordTable[p].property == EnumWordProperties::OperatorLeftBrace) {
				// ���嶯��
				{
					// �ض���
					for (auto& i : functionTable.functionTable) {
						if (i.value == lastFunctionIdentifier&& i.defined) {
							throw(Exception("Function have defined", wordTable[p], 1));
						}
					}

					// ָʾ��ǰΪ����ͷ �Ա㽫�������ı�ϲ���� Ӧ���ڽ��뺯����������� false
					functionLeftCompound = true;
					// ������ǰ�����ı�ǩ��
					labelTableStack.push(*(new LabelIdentifierTable()));

					// ��������Ϊ�Ѿ�����
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

// func    �����б�ʶ��
// param   
// return  
bool SemanticAnalysis::DeclarationList() {
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
bool SemanticAnalysis::DeclarationListEliminateLeft() {
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

// func    ����ת�� Ҫ����ֵջ����һ��ֵ ת�����ֵ�������ֵջ
// param   
// return  
bool SemanticAnalysis::ToBool() {
	// �鿴����
	auto lastReturnValue = returnValueStack.returnValueStack.top();
	auto e = lastReturnValue->getWordProperties();
	// ԭ���� Bool
	if (e == EnumWordProperties::Bool) {
		return true;
	}

	// ���� Bool ��ջ
	returnValueStack.returnValueStack.pop();
	// ֱ�ӱ�
	if (returnValueStack.lastReturnValueType == EnumWordProperties::String ||
		returnValueStack.lastReturnValueType == EnumWordProperties::Function) {
		// ȡ��ʱ���� ������Ԫʽ
		std::string tempVarName = "T" + std::to_string(tempVariableTablePointer);
		tempVariableTablePointer++;
		fourTable.AddFour("=", "1", "", tempVarName);
		fourTable.nowFourLine++;

		// ����ֵջ
		returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
		returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
		returnValueStack.lastReturnValueType = EnumWordProperties::Bool;
	}
	// ��������
	else {
		// ȡ��ʱ���� ������Ԫʽ
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

		// ����ֵջ
		returnValueStack.returnValueStack.push(new TempVariable(EnumWordProperties::Bool, tempVarName));
		returnValueStack.lastReturnWordType = ReturnValueStack::LastReturnWordType::TempVariable;
		returnValueStack.lastReturnValueType = EnumWordProperties::Bool;
	}
	return true;
}