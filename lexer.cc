//Joe Dunagan (Tue-Thur) and Sabien Najim (Mon-Wed)
//Worked on this project together 
//
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "IF", "WHILE", "DO", "THEN", "PRINT",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN",
    "NOTEQUAL", "GREATER", "LESS", "LTEQ", "GTEQ",
    "DOT", "NUM", "ID", "ERROR", "REALNUM", "BASE08NUM", "BASE16NUM"
        // added REALNUM, BASE08NUM & BASE16NUM
};

#define KEYWORDS_COUNT 5
string keyword[] = { "IF", "WHILE", "DO", "THEN", "PRINT" };

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber()
{

    char c;
    bool thruIf = false;
    bool base16 = false, base8 = false, real = false;
    input.GetChar(c);
    if (isdigit(c) || (c >= 'A' && c <= 'F') || c == 'x') {
        if (c == '0') {
            while (!input.EndOfInput() && c == '0') {
                //tmp.lexeme += c;
                input.GetChar(c);
            }

            if(c == '.' || c == 'x'){//eg cases, 00.12 or 00000x16 or 00x08
                tmp.lexeme = "0";
            }else{//eg cases such as 0000012, 00000012x16
               
                while (!input.EndOfInput() && isdigit(c)) {
                    tmp.lexeme += c;
                    input.GetChar(c);
                }

            }
        }else{
            tmp.lexeme = "";
            while (!input.EndOfInput() && isdigit(c)) {
                tmp.lexeme += c;
                input.GetChar(c);
            }
        }
       
        //get next input after the cirst char
        if(c == '.'){//realNum case
            thruIf = true;
            input.GetChar(c);
            if(isdigit(c)){
                tmp.lexeme += '.';
                while (!input.EndOfInput() && isdigit(c)) {
                    tmp.lexeme += c;
                    input.GetChar(c);
                }
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = REALNUM;
                tmp.line_no = line_no;
                return tmp;
            }else{
                //error has happened
                input.UngetChar(c);
                input.UngetChar('.');
            }
        }
       
        if (c >= 'A' && c <= 'F'){//base 16 case
            thruIf = true;
            base16 = true;
            string holder = "";
            //tmp.lexeme += c;
            holder += c;
            input.GetChar(c);
            if((c >= 'A' && c <= 'F') || isdigit(c)){
                while (!input.EndOfInput() && (isdigit(c) || (c >= 'A' && c <= 'F'))){
                    //tmp.lexeme += c;
                    holder += c;
                    input.GetChar(c);
                }
            }

            if(c == 'x'){
                thruIf = true;
                holder += c;
                input.GetChar(c);
                if(c == '1'){
                    holder += c;
                    input.GetChar(c);
                    if(c == '6'){
                        base16 == true;
                        holder += c;
                        tmp.lexeme += holder;
                        tmp.token_type = BASE16NUM;
                        tmp.line_no = line_no;
                        return tmp;
                    }
                    //Error eg, 1Ax0, 1AFx06
                }
               
            }
            //move onto getting the x in there
            holder += c;
            for(int x = holder.length();x > 0; x--){
                input.UngetChar(holder[x-1]);
            }
            //input.UngetChar(tmp.lexeme[tmp.lexeme.length()-1]);
        }

        if(c == 'x'){
            thruIf = true;
            string holder = "";
            holder += c;
            input.GetChar(c);
            if(c == '0' && base16 == false){
                holder += c;
                input.GetChar(c);
                if(c == '8'){
                    tmp.lexeme += 'x';
                    tmp.lexeme += '0';
                    tmp.lexeme += '8';
                    tmp.token_type = BASE08NUM;
                    tmp.line_no = line_no;
                    return tmp;
                }
            }
           
            if(c == '1'){
                holder += c;
                input.GetChar(c);
                if(c == '6'){
                    base16 == true;
                    tmp.lexeme += 'x';
                    tmp.lexeme += '1';
                    tmp.lexeme += '6';
                    tmp.token_type = BASE16NUM;
                    tmp.line_no = line_no;
                    return tmp;
                }
                //Error eg, 1AFx0, 1AFx06
            }
            //error if reaching here add code add
            holder += c;
            for(int x = holder.length();x > 0; x--){
                input.UngetChar(holder[x-1]);
            }
        }
        //1Ax0
        //cases eg, 1st mixed case,1ZZ
        if(!input.EndOfInput() && thruIf == false){
            input.UngetChar(c);
        }
        //no special base 16 or 08 or real # cases
        tmp.token_type = NUM;
        tmp.line_no = line_no;
        return tmp;

    }
    else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
        case '.':
            tmp.token_type = DOT;
            return tmp;
        case '+':
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.token_type = MINUS;
            return tmp;
        case '/':
            tmp.token_type = DIV;
            return tmp;
        case '*':
            tmp.token_type = MULT;
            return tmp;
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '[':
            tmp.token_type = LBRAC;
            return tmp;
        case ']':
            tmp.token_type = RBRAC;
            return tmp;
        case '(':
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.token_type = RPAREN;
            return tmp;
        case '<':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = LTEQ;
            } else if (c == '>') {
                tmp.token_type = NOTEQUAL;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = LESS;
            }
            return tmp;
        case '>':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = GTEQ;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = GREATER;
            }
            return tmp;
        default:
            if (isdigit(c)) {
                input.UngetChar(c);
                return ScanNumber();
            } else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}

int main()
{
    LexicalAnalyzer lexer;
    Token token;

    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }
}