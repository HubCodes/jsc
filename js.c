#include "js.h"

int main(int argc, char **argv)
{
	char code[] = "let data2 = 0;";
	Lexer *lexer = Lexer_new(code, sizeof(code));
	Token *token;
	while ((token = Lexer_next(lexer)) != NULL)
	{
		printf("%d\n", token->kind);
	}
	return 0;
}
