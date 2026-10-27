#include <stdio.h>
#include <string.h>
#include <ctype.h>

const char *keywords[] =
{
    "auto",
    "break",
    "case",
    "char",
    "const",
    "continue",
    "default",
    "do",
    "double",
    "else",
    "enum",
    "extern",
    "float",
    "for",
    "goto",
    "if",
    "int",
    "long",
    "register",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "struct",
    "switch",
    "typedef",
    "union",
    "unsigned",
    "void",
    "volatile",
    "while"
};

#define KEYWORD_COUNT (sizeof(keywords) / sizeof(keywords[0]))

int is_keyword(const char *word)
{
    int i;

    for (i = 0; i < KEYWORD_COUNT; i++)
    {
        if (strcmp(word, keywords[i]) == 0)
        {
            return 1;
        }
    }

    return 0;
}

void word_scan(FILE *fp, int first)
{
    char word[100];
    int i = 0;
    int ch;

    word[i++] = first;

    while ((ch = fgetc(fp)) != EOF)
    {
        if (isalnum(ch) || ch == '_')
        {
            word[i++] = ch;
        }
        else
        {
            ungetc(ch, fp);
            break;
        }
    }

    word[i] = '\0';

    if (is_keyword(word))
    {
        printf("%-20s : Keyword\n", word);
    }
    else
    {
        printf("%-20s : Identifier\n", word);
    }
}

void scan_number(FILE *fp, int first)
{
    char number[100];
    int i = 0;
    int ch;
    int dot = 0;
    int invalid = 0;

    number[i++] = first;

    while ((ch = fgetc(fp)) != EOF)
    {
        if (isdigit(ch))
        {
            number[i++] = ch;
        }
        else if (ch == '.' && dot == 0)
        {
            dot = 1;
            number[i++] = ch;
        }
        else if (ch == '.')
        {
            number[i++] = ch;
            invalid = 1;
        }
        else if (isalpha(ch) || ch == '_')
        {
            number[i++] = ch;
            invalid = 1;
        }
        else
        {
            ungetc(ch, fp);
            break;
        }
    }

    number[i] = '\0';

    if (invalid)
    {
        printf("%-20s : ERROR - Invalid number\n", number);
    }
    else if (dot == 1)
    {
        printf("%-20s : Floating constant\n", number);
    }
    else
    {
        printf("%-20s : Integer\n", number);
    }
}

void scan_string(FILE *fp)
{
    char str[200];
    int i = 0;
    int ch;

    str[i++] = '"';

    while ((ch = fgetc(fp)) != EOF)
    {
        str[i++] = ch;

        if (ch == '"')
        {
            break;
        }
    }

    str[i] = '\0';

    if (str[i - 1] == '"')
    {
        printf("%-20s : String constant\n", str);
    }
    else
    {
        printf("%-20s : ERROR - Unterminated string\n", str);
    }
}

void scan_character(FILE *fp)
{
    char value[20];
    int i = 0;
    int ch;
    int count = 0;
    int closed = 0;

    value[i++] = '\'';

    while ((ch = fgetc(fp)) != EOF)
    {
        value[i++] = ch;

        if (ch == '\'')
        {
            closed = 1;
            break;
        }

        count++;
    }

    value[i] = '\0';

    if (!closed)
    {
        printf("%-20s : ERROR - Invalid character constant\n",
               value);
    }
    else if (count != 1)
    {
        printf("%-20s : ERROR - Invalid character constant\n",
               value);
    }
    else
    {
        printf("%-20s : Character constant\n", value);
    }
}

void scan_preprocessor(FILE *fp)
{
    char directive[200];
    int i = 0;
    int ch;

    directive[i++] = '#';

    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '\n')
        {
            break;
        }

        directive[i++] = ch;
    }

    directive[i] = '\0';

    printf("%-20s : Preprocessor directive\n", directive);
}

void scan_operator(FILE *fp, int first)
{
    char op[4];
    int ch;

    op[0] = first;
    op[1] = '\0';

    ch = fgetc(fp);


      // Checking for two char operators

    if ((first == '+' && (ch == '+' || ch == '=')) ||
        (first == '-' && (ch == '-' || ch == '=')) ||
        (first == '*' && ch == '=') ||
        (first == '/' && ch == '=') ||
        (first == '%' && ch == '=') ||
        (first == '=' && ch == '=') ||
        (first == '!' && ch == '=') ||
        (first == '<' && (ch == '=' || ch == '<')) ||
        (first == '>' && (ch == '=' || ch == '>')) ||
        (first == '&' && (ch == '&' || ch == '=')) ||
        (first == '|' && (ch == '|' || ch == '=')) ||
        (first == '^' && ch == '='))
    {
        op[1] = ch;
        op[2] = '\0';

           //Checking  <<= and >>=
        

        if (strcmp(op, "<<") == 0 ||
            strcmp(op, ">>") == 0)
        {
            ch = fgetc(fp);

            if (ch == '=')
            {
                op[2] = '=';
                op[3] = '\0';
            }
            else
            {
                ungetc(ch, fp);
            }
        }
    }
    else
    {
        if (ch != EOF)
        {
            ungetc(ch, fp);
        }
    }

    // classification of operator

    if (strcmp(op, "=") == 0 ||
        strcmp(op, "+=") == 0 ||
        strcmp(op, "-=") == 0 ||
        strcmp(op, "*=") == 0 ||
        strcmp(op, "/=") == 0 ||
        strcmp(op, "%=") == 0 ||
        strcmp(op, "&=") == 0 ||
        strcmp(op, "|=") == 0 ||
        strcmp(op, "^=") == 0 ||
        strcmp(op, "<<=") == 0 ||
        strcmp(op, ">>=") == 0)
    {
        printf("%-20s : Assignment operator\n", op);
    }

    else if (strcmp(op, "==") == 0 ||
            strcmp(op, "!=") == 0 ||
            strcmp(op, "<") == 0 ||
            strcmp(op, ">") == 0 ||
            strcmp(op, "<=") == 0 ||
            strcmp(op, ">=") == 0)
    {
        printf("%-20s : Relational operator\n", op);
    }

    else if (strcmp(op, "++") == 0)
    {
        printf("%-20s : Increment operator\n", op);
    }

    else if (strcmp(op, "--") == 0){
        printf("%-20s : Decrement operator\n", op);
    }

    else if (strcmp(op, "+") == 0 ||
            strcmp(op, "-") == 0 ||
            strcmp(op, "*") == 0 ||
            strcmp(op, "/") == 0 ||
            strcmp(op, "%") == 0)
    {
        printf("%-20s : Arithmetic operator\n", op);
    }
    else if (strcmp(op, "&&") == 0 ||strcmp(op, "||") == 0 || strcmp(op, "!") == 0)
    {
        printf("%-20s : Logical operator\n", op);
    }
    else if (strcmp(op, "&") == 0 ||
            strcmp(op, "|") == 0 ||
            strcmp(op, "^") == 0 ||
            strcmp(op, "~") == 0 ||
            strcmp(op, "<<") == 0 ||
            strcmp(op, ">>") == 0)
    {
        printf("%-20s : Bitwise operator\n", op);
    }
    else
    {
        printf("%-20s : Operator\n", op);
    }
}

void scan_special_character(int ch)
{
    printf("%-20c : Special char\n", ch);
}

void skip_single_line_comment(FILE *fp)
{
    int ch;

    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '\n')
        {
            break;
        }
    }
}

void skip_multi_line_comment(FILE *fp)
{
    int ch;
    int prev = 0;

    while ((ch = fgetc(fp)) != EOF)
    {
        if (prev == '*' && ch == '/')
        {
            break;
        }

        prev = ch;
    }
}

int main(int argc, char *argv[])
{
    FILE *fp;
    int ch;

    if (argc != 2)
    {
        printf("Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    fp = fopen(argv[1], "r");

    if (fp == NULL)
    {
        printf("Error: Unable to open file\n");
        return 1;
    }

    while ((ch = fgetc(fp)) != EOF)
    {
        if (isspace(ch))
        {
            continue;
        }
//Preprocessor directive
        
        if (ch == '#')
        {
            scan_preprocessor(fp);
            continue;
        }

        
          // Keyword / Identifier
        
        if (isalpha(ch) || ch == '_')
        {
            word_scan(fp, ch);
            continue;
        }

        
           //Integer / Floating constant
       
        if (isdigit(ch))
        {
            scan_number(fp, ch);
            continue;
        }

        
          // String constant
        
        if (ch == '"')
        {
            scan_string(fp);
            continue;
        }

        
        //   Char constant
        
        if (ch == '\'')
        {
            scan_character(fp);
            continue;
        }

        
        //Comment or div oper
       
        if (ch == '/')
        {
            int next = fgetc(fp);

            if (next == '/')
            {
                skip_single_line_comment(fp);
            }
            else if (next == '*')
            {
                skip_multi_line_comment(fp);
            }
            else
            {
                if (next != EOF)
                {
                    ungetc(next, fp);
                }

                scan_operator(fp, ch);
            }

            continue;
        }

        
        //oper
        if (ch == '+' ||
            ch == '-' ||
            ch == '*' ||
            ch == '%' ||
            ch == '=' ||
            ch == '<' ||
            ch == '>' ||
            ch == '!' ||
            ch == '&' ||
            ch == '|' ||
            ch == '^' ||
            ch == '~')
        {
            scan_operator(fp, ch);
            continue;
        }

        
        //   Special char
        if (ch == '(' ||
            ch == ')' ||
            ch == '{' ||
            ch == '}' ||
            ch == '[' ||
            ch == ']' ||
            ch == ';' ||
            ch == ',' ||
            ch == ':')
        {
            scan_special_character(ch);
            continue;
        }

        
        //Invalid character
        
        printf("%-20c : ERROR - Invalid character\n", ch);
    }

    fclose(fp);

    return 0;
}