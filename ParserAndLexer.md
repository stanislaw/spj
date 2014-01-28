# Parser and lexer

## Lexer

```cpp
enum TokenType
{
    Number,
    String,
    Comma,
    OpeningBracket,
    ClosingBracket,
    OpeningCurly,
    ClosingCurly,
    EndOfFile
};
 
struct Token
{
    TokenType type;
    id value;
};
 
// function which provides next character
typedef int Reader();
 
class Tokenizer
{
    Reader* inputReader;
    LexicalProcessor* lexicalProcessor;
 
    enum { Normal, InString, InNumber } state;
    bool nextTokenReady;
    char nextTokenBuf[1024];
    int bufferIdx;
    Token nextToken;
    char characterForReparse;
    bool mustReparse;
 
public:
 
    Tokenizer(Reader reader, LexicalProcessor* processor) :
    {
        if (reader == nullptr) throw something();
        if (processor == nullptr) throw something();
 
        inputReader = reader;
        lexicalProcessor = processor;
        mustReparse = false;
    }
 
    Token GetNextToken()
    {
        while (true)
        {
            int ch = mustReparse ? characterForReparse : (*inputReader)();
            mustReparse = ProcessNextChar(ch);
            if (mustReparse)
                characterForReparse = ch;
            if (nextTokenReady)
                return nextToken;
        };
 
        // if we are here, EOF is encountered
        // cleanup: check if state is normal etc.
    }
 
private:
    bool GetNextToken(char ch)
    {
        nextTokenReady = false;
        switch (state)
        {
        case Normal:
            return GetNextTokenNormal(ch);
        case InNumber:
            return GetNextTokenNumber(ch);
        case InString:
            return GetNextTokenString(ch);
        default:
            throw wtf;
        }
    }
 
    bool GetNextTokenNormal(char ch)
    {
        switch (ch)
        {
        case EOF:
            nextToken.type = EndOfFile;
            nextTokenReady = true;
            return false;
        case ' ':
            // space in normal mode ignored
            return false;
        case '[':
            nextToken.type = OpeningBracket;
            nextTokenReady = true;
            return false;
        case ']':
            nextToken.type = ClosingBracket;
            nextTokenReady = true;
            return false;
        case '{':
            nextToken.type = OpeningCurly;
            nextTokenReady = true;
            return false;
        case '}':
            nextToken.type = ClosingCurly;
            nextTokenReady = true;
            return false;
        case '"':
            state = InString;
            bufferIdx = 0;
            return false; // we can swallow the opening quote
        }
        if (isdigit(ch))
        {
            state = InNumber;
            bufferIdx = 0;
            return true;
        }
    }
 
    bool GetNextTokenNumber(char ch)
    {
        if (isdigit(ch))
        {
            nextTokenBuf[bufferIdx++] = ch;
            if (bufferIdx >= 1024) // TODO: make it a named constant
                throw OUT_OF_BUFFER_SPACE;
            // TODO: make the buffer dynamically resizable
            return false;
        }
        // else the number has ended, ship out and reparse
        nextToken.type = Number;
        nextToken.value = [NSNumber fromString: nextTokenBuf]; // whatever the correct syntax is
        nextTokenReady = true;
        state = Normal;
        return true;
    }
 
    bool GetNextTokenString(char ch)
    {
        if (ch == '"') // end of string
        {
            nextToken.type = String;
            nextToken.value = [NSString fromCString: nextTokenBuf]; // whatever the correct syntax is
            nextTokenReady = true;
            state = Normal;
            return false; // no need to reparse, the quote belongs to the string
        }
 
        if (ch == EOF)
            throw EndOfFileInsideString();
           
        nextTokenBuf[bufferIdx++] = ch;
        if (bufferIdx >= 1024) // TODO: make it a named constant
            throw OUT_OF_BUFFER_SPACE;
        // TODO: make the buffer dynamically resizable
        return false;
    }
}
```

## Parser

```cpp
class Parser
{
    Tokenizer lexer;
    Token curr;
 
public:
    Parser(Tokenizer* lexer)
    {
        this.lexer = lexer;
        Skip(); // look ahead
    }
 
    JSONObject* Parse()
    {
        return ParseObject();
    }
 
private:
    void Skip()
    {
        curr = lexer.GetNextToken();
    }
 
    void EnsureSkip(TokenType type)
    {
        if (curr.type != type)
            throw SyntaxError;
        Skip();
    }
 
    bool TrySkip((TokenType type)
    {
        if (curr.type != type)
            return false;
        Skip();
        return true;
    }
 
    // using diagarams from json.org
    JSONObject* TryParseObject()
    {
        if (!TrySkip(OpeningCurly))
            return nullptr;
        JSONObject* o = new JSONObject();
        while (curr.type != ClosingCurly)
        {
            JSONString* key = ParseString();
            EnsureSkip(Comma);
            id value = ParseValue();
            o.AddKeyValue(key, value);
        }
        EnsureSkip(ClosingCurly);
    }
 
    JSONObject* ParseObject()
    {
        JSONObject* o = TryParseObject();
        if (o == nullptr)
            throw SyntaxError("Object expected");
    }
 
    JSONString* TryParseString()
    {
        if (curr.type != String)
            return nullptr;
        return new JSONString((NSString*)curr.value);
    }
 
    JSONString* ParseString()
    {
        JSONString* s = TryParseString();
        if (s == nullptr)
            throw SyntaxError("String expected");
    }
 
    id TryParseValue()
    {
        // value is either a string, or a number, or an object, or an array, or true, or false, or null
        id result;
        if ((result = TryParseString()) != nullptr)
            return result;
        if ((result = TryParseNumber()) != nullptr)
            return result;
        if ((result = TryParseObject()) != nullptr)
            return result;
        if ((result = TryParseArray()) != nullptr)
            return result;
        //  etc.
        // if not found:
        return nullptr;
    }
 
    id ParseValue()
    {
        id result = TryParseValue();
        if (result == nullptr)
            throw SyntaxError("Cannot parse value");
    }
 
    JSONArray* TryParseArray()
    {
        if (!TrySkip(OpeningBracket))
            return nullptr;
        JSONArray* a = new JSONArray();
        do
        {
            id value = ParseValue();
            a.addValue(value);
        } while (TrySkip(Comma));
        EnsureSkip(ClosingBracket);
        return a;
    }
 
 
    JSONArray* ParseArray()
    {
        JSONArray* a = TryParseArray();
        if (a == nullptr)
            throw SyntaxError("Array expected");
        return a;
    }
}
```


