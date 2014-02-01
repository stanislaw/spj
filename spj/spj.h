#include <stdio.h>


struct SpjJSONData;


#ifdef USE_OBJ_DICT
struct htab; // will be implemented sometime
#endif


typedef enum SpjJSONValueType {
  SpjJSONValueObject,
  SpjJSONValueArray,
  SpjJSONValueString,
  SpjJSONValueNumber,
  SpjJSONValueBool,
  SpjJSONValueNull
} SpjJSONValueType;


typedef struct SpjObject {
  struct SpjJSONData *data; // array
  size_t size;
#ifdef USE_OBJ_DICT
  struct htab *hash;
#endif
} SpjObject;


typedef struct SpjArray {
    struct SpjJSONData *data; // array
    size_t size;
} SpjArray;


typedef struct SpjString {
    char *data; // nil terminating
    size_t size;
} SpjString;


union SpjJSONValue {
    struct SpjObject object;
    struct SpjArray  array;
    struct SpjString string;
    double number;   // Bool, Null, integer and real (all in one)
};


typedef struct SpjJSONData {
    SpjJSONValueType type; // try to do it without flags
    // struct jval *parent; // calculated AFTER the filling of the PARENT's container
    char *name;            // name pairs (valid only for members of the object)
    union SpjJSONValue value;
} SpjJSONData;


typedef enum SpjJSONTokenType {
    SpjJSONTokenNumber,
    SpjJSONTokenString,
    SpjJSONTokenBool,     // true false
    SpjJSONTokenNull,     // null
    SpjJSONTokenObjectStart, // {
    SpjJSONTokenArrayStart, // [
    SpjJSONTokenObjectEnd,   // }
    SpjJSONTokenArrayEnd,   // ]
    SpjJSONTokenColon,    // : =
    SpjJSONTokenComma,    // ,
    SpjJSONTokenError,
    SpjJSONTokenEOS //
} SpjJSONTokenType;


typedef struct spj_iterator_t {
    const char *data;
    size_t currentposition;
    size_t datasize;
} spj_iterator_t;


typedef struct {
    //char in;
    union SpjJSONValue value;
} spj_lexer_t;


// Для всех лексем, кроме Object и Array лексер возвращает (в аргументе lex) значение, которое парсер просто копирует в элемент контейнера.
SpjJSONTokenType spj_getoken (spj_iterator_t *iterator, spj_lexer_t *lexer);


// Jerror устанавливаем код возврата в curpos + 1 и начинаем просто возвращаться из рекурсии с ним.
int spj_parse (const char *json_str, SpjJSONData *root);
int spj_delete (SpjJSONData *object);




