#include"head.h"

int main(void) {

	// ��ʼ�����ر�
	wordTableSource.resize(MAX_WORD_TABLE);

	// ����Դ�����ļ�
	char source[] = ".\\Tests\\boqi2.cpp";
	LexicalAnalysis lexical = LexicalAnalysis(source, wordTableSource);
	GrammaticalAnalysis grammatical = GrammaticalAnalysis(wordTableSource);

	return 0;
}
