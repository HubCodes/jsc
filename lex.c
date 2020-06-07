#include "js.h"

/*
 * Token: Distinguished source code elements
 */

Token *Token_new(TokenKind kind, Loc loc, TokenData data)
{
	Token *this = calloc(sizeof(Token), 1);
	this->kind = kind;
	this->loc = loc;
	this->data = data;
	return this;
}

/*
 * Lexer: Tokenize raw source code into Token
 */

#define MAKE_TOKEN(name)                 \
	String *name = String_new();     \
	Pos start = Lexer_get_pos(this); \
	Pos end;                         \
	Loc loc;                         \
	TokenData token_data;

Lexer *Lexer_new(const char *code, int code_size)
{
	Lexer *this = calloc(sizeof(Lexer), 1);
	this->code = calloc(sizeof(char), code_size);
	this->code_size = code_size;
	this->pos = 0;
	this->line = 0;
	this->col = 0;
	strncpy(this->code, code, code_size);
	return this;
}

static int is_newline(int ch);
static int is_id_start(int ch);
static int is_id_content(int ch);
static int is_number_start(int ch);
static int is_keyword(String *string);

static Pos Lexer_get_pos(Lexer *this);
static int Lexer_get_char(Lexer *this);
static int Lexer_peek_char(Lexer *this);
static void Lexer_skip_whitespace(Lexer *this);
static String *Lexer_get_exp(Lexer *this);

static Token *Lexer_get_id(Lexer *this, int ch);
static Token *Lexer_get_number(Lexer *this, int ch);

Token *Lexer_next(Lexer *this)
{
	Lexer_skip_whitespace(this);
	int ch = Lexer_get_char(this);
	if (is_id_start(ch))
	{
		return Lexer_get_id(this, ch);
	}
	else if (is_number_start(ch))
	{
		return Lexer_get_number(this, ch);
	}
	return NULL;
}

Token *Lexer_peek(Lexer *this)
{
	Lexer prevState = *this;
	Token *token = Lexer_next(this);
	*this = prevState;
	return token;
}

static int is_newline(int ch)
{
	return ch == '\r' || ch == '\n';
}

static int is_id_start(int ch)
{
	return (
	    ('a' <= ch && ch <= 'z') ||
	    ('A' <= ch && ch <= 'Z') ||
	    ch == '$' ||
	    ch == '_');
}

static int is_id_content(int ch)
{
	return is_id_start(ch) || ('0' <= ch && ch <= '9');
}

static int is_number_start(int ch)
{
	return isdigit(ch) || ch == '.';
}

static int is_keyword(String *string)
{
	int size = sizeof(keywords) / sizeof(char *);
	for (int i = 0; i < size; i++)
	{
		if (strcmp(string->buf, keywords[i]) == 0)
			return 1;
	}
	return 0;
}

static Pos Lexer_get_pos(Lexer *this)
{
	return (Pos){.line = this->line, .col = this->col};
}

static int Lexer_get_char(Lexer *this)
{
	int ch = this->code[this->pos];
	this->pos++;
	if (is_newline(ch))
	{
		this->line++;
		this->col = 0;
	}
	else
	{
		this->col++;
	}
	return ch;
}

static int Lexer_peek_char(Lexer *this)
{
	return this->code[this->pos];
}

static void Lexer_skip_whitespace(Lexer *this)
{
	while (isspace(Lexer_peek_char(this)))
		Lexer_get_char(this);
}

static String *Lexer_get_exp(Lexer *this)
{
	String *exp = String_new();
	String_push(exp, 'e');
	int ch = Lexer_peek_char(this);
	if (ch == '+' || ch == '-')
	{
		String_push(exp, Lexer_get_char(this));
		ch = Lexer_peek_char(this);
	}
	while (isdigit(ch))
	{
		String_push(exp, Lexer_get_char(this));
		ch = Lexer_peek_char(this);
	}
	return exp;
}

static Token *Lexer_get_id(Lexer *this, int ch)
{
	MAKE_TOKEN(id)
	String_push(id, ch);
	while (is_id_content(Lexer_peek_char(this)))
	{
		String_push(id, Lexer_get_char(this));
	}
	end = Lexer_get_pos(this);
	loc = Loc_make(start, end);
	token_data.id = id;
	if (is_keyword(id))
	{
		return Token_new(TOK_KEYWORD, loc, token_data);
	}
	return Token_new(TOK_ID, loc, token_data);
}

static Token *Lexer_get_number(Lexer *this, int ch)
{
	MAKE_TOKEN(number)
	int next_ch;
	int is_exp = 0;
	TokenKind kind = ch == '.' ? TOK_DOUBLE : TOK_INTEGER;
	String_push(number, ch);
	for (;;)
	{
		next_ch = Lexer_peek_char(this);
		if (toupper(next_ch) == 'E')
		{
			Lexer_get_char(this);
			String *exp = Lexer_get_exp(this);
			String_append(number, exp);
			String_delete(exp);
			is_exp = 1;
			goto make_token;
		}
		else if (kind == TOK_INTEGER && next_ch == '.')
		{
			Lexer_get_char(this);
			String_push(number, next_ch);
			kind = TOK_DOUBLE;
		}
		else if (isdigit(next_ch))
		{
			Lexer_get_char(this);
			String_push(number, next_ch);
		}
		else
		{
			break;
		}
	}
make_token:
	end = Lexer_get_pos(this);
	loc = Loc_make(start, end);
	if (kind == TOK_DOUBLE || is_exp)
	{
		kind = TOK_DOUBLE;
		token_data.doubl = strtod(number->buf, NULL);
	}
	else
	{
		token_data.integer = strtol(number->buf, NULL, 10);
	}
	return Token_new(kind, loc, token_data);
}
