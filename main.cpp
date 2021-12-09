#include"head.h"

int main(void) {

	// 初始化词素表
	wordTableSource.resize(MAX_WORD_TABLE);

	// 导入源代码文件
	char source[] = ".\\Tests\\boqi2.cpp";
	LexicalAnalysis lexical = LexicalAnalysis(source, wordTableSource);
	GrammaticalAnalysis grammatical = GrammaticalAnalysis(wordTableSource);

	return 0;
}
