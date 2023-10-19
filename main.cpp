
#include"head.h"

int main(void) {

	wordTableSource.resize(MAX_WORD_TABLE);

	char source[] = ".\\Tests\\test.cpp";
	LexicalAnalysis lexical = LexicalAnalysis(source, wordTableSource);
	//GrammaticalAnalysis grammatical = GrammaticalAnalysis(wordTableSource);
	SemanticAnalysis semantic = SemanticAnalysis(wordTableSource);
	return 0;
}
