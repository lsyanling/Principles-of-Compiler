#include"head.h"

int main(void) {

	// 初始化词素表
	wordTableSource.resize(MAX_WORD_TABLE);

	// 导入源代码文件
	char source[] = ".\\Tests\\test.cpp";
	LexicalAnalysis lexical = LexicalAnalysis(source, wordTableSource);
	//GrammaticalAnalysis grammatical = GrammaticalAnalysis(wordTableSource);
	SemanticAnalysis semantic = SemanticAnalysis(wordTableSource);
	return 0;
}
