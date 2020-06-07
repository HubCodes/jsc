#include "js.h"

/*
 * Token: Distinguished source code elements
 */

Token *Token_new(TokenKind kind, Loc loc, TokenData data)
{
	Token *self = calloc(sizeof(Token), 1);
	self->kind = kind;
	self->loc = loc;
	self->data = data;
	return self;
}

/*
 * Lexer: Tokenize raw source code into Token
 */

#define MAKE_TOKEN(name)                 \
	String *name = String_new();     \
	Pos start = Lexer_get_pos(self); \
	Pos end;                         \
	Loc loc;                         \
	TokenData token_data;

Lexer *Lexer_new(const char *code, int code_size)
{
	Lexer *self = calloc(sizeof(Lexer), 1);
	self->code = calloc(sizeof(char), code_size);
	self->code_size = code_size;
	self->pos = 0;
	self->line = 0;
	self->col = 0;
	strncpy(self->code, code, code_size);
	return self;
}

static int is_newline(int ch);
static int is_id_start(int ch);
static int is_id_content(int ch);
static int is_number_start(int ch);
static int is_keyword(String *string);
static int is_punct(int ch);

static Pos Lexer_get_pos(Lexer *self);
static int Lexer_get_char(Lexer *self);
static int Lexer_peek_char(Lexer *self);
static void Lexer_skip_whitespace(Lexer *self);
static String *Lexer_get_exp(Lexer *self);

static Token *Lexer_get_id(Lexer *self, int ch);
static Token *Lexer_get_number(Lexer *self, int ch);

Token *Lexer_next(Lexer *self)
{
	Lexer_skip_whitespace(self);
	int ch = Lexer_get_char(self);
	if (is_id_start(ch))
	{
		return Lexer_get_id(self, ch);
	}
	else if (is_number_start(ch))
	{
		return Lexer_get_number(self, ch);
	}
	else if (is_punct(ch))
	{
		return Lexer_get_punct(self, ch);
	}
	return NULL;
}

Token *Lexer_peek(Lexer *self)
{
	Lexer prevState = *self;
	Token *token = Lexer_next(self);
	*self = prevState;
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

static int is_punct(int ch)
{
	static const char *puncts = "{}[]();:.";
	return strchr(puncts, ch) != NULL;
}

static Pos Lexer_get_pos(Lexer *self)
{
	return (Pos){.line = self->line, .col = self->col};
}

static int Lexer_get_char(Lexer *self)
{
	int ch = self->code[self->pos];
	self->pos++;
	if (is_newline(ch))
	{
		self->line++;
		self->col = 0;
	}
	else
	{
		self->col++;
	}
	return ch;
}

static int Lexer_peek_char(Lexer *self)
{
	return self->code[self->pos];
}

static void Lexer_skip_whitespace(Lexer *self)
{
	while (isspace(Lexer_peek_char(self)))
		Lexer_get_char(self);
}

static String *Lexer_get_exp(Lexer *self)
{
	String *exp = String_new();
	String_push(exp, 'e');
	int ch = Lexer_peek_char(self);
	if (ch == '+' || ch == '-')
	{
		String_push(exp, Lexer_get_char(self));
		ch = Lexer_peek_char(self);
	}
	while (isdigit(ch))
	{
		String_push(exp, Lexer_get_char(self));
		ch = Lexer_peek_char(self);
	}
	return exp;
}

static Token *Lexer_get_id(Lexer *self, int ch)
{
	MAKE_TOKEN(id)
	String_push(id, ch);
	while (is_id_content(Lexer_peek_char(self)))
	{
		String_push(id, Lexer_get_char(self));
	}
	end = Lexer_get_pos(self);
	loc = Loc_make(start, end);
	token_data.id = id;
	if (is_keyword(id))
	{
		return Token_new(TOK_KEYWORD, loc, token_data);
	}
	return Token_new(TOK_ID, loc, token_data);
}

static Token *Lexer_get_number(Lexer *self, int ch)
{
	MAKE_TOKEN(number)
	int next_ch;
	int is_exp = 0;
	TokenKind kind = ch == '.' ? TOK_DOUBLE : TOK_INTEGER;
	String_push(number, ch);
	for (;;)
	{
		next_ch = Lexer_peek_char(self);
		if (toupper(next_ch) == 'E')
		{
			Lexer_get_char(self);
			String *exp = Lexer_get_exp(self);
			String_append(number, exp);
			String_delete(exp);
			is_exp = 1;
			goto make_token;
		}
		else if (kind == TOK_INTEGER && next_ch == '.')
		{
			Lexer_get_char(self);
			String_push(number, next_ch);
			kind = TOK_DOUBLE;
		}
		else if (isdigit(next_ch))
		{
			Lexer_get_char(self);
			String_push(number, next_ch);
		}
		else
		{
			break;
		}
	}
make_token:
	end = Lexer_get_pos(self);
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

static Token *Lexer_get_punct(Lexer *self, int ch)
{
	MAKE_TOKEN(punct)
	end = Lexer_get_pos(self);
	loc = Loc_make(start, end);
	token_data.punct = ch;
	return Token_new(TOK_PUNCT, loc, token_data);
}
