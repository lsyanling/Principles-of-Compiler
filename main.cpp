#include"head.h"

int main(void) {

	// ��ʼ�����ر�
	wordTableSource.resize(MAX_WORD_TABLE);

	// ����Դ�����ļ�
	char source[] = ".\\Tests\\test.cpp";
	LexicalAnalysis lexical = LexicalAnalysis(source, wordTableSource);
	//GrammaticalAnalysis grammatical = GrammaticalAnalysis(wordTableSource);
	SemanticAnalysis semantic = SemanticAnalysis(wordTableSource);
	return 0;
}
