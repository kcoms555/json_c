#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
enum JSON_TYPE { UNDEFINED, NUMBER, STRING, BOOLEAN, ARRAY, OBJECT, JNULL };
enum KEYORVALUE{ KEY, VALUE};
typedef struct json_value_s {
    int type;
	int bytesize;
    void* value;
} json_value;
typedef struct json_object_s {
    int last_index;
    char** keys;
    json_value ** values;
} json_object;
typedef struct json_array_s {
    int last_index;
    json_value ** values;
} json_array;

static json_value * json_create(const char* json_message);
static json_value * json_create_value(const char** json_message);
static json_array * json_create_array(const char** json_message);
static json_object * json_create_object(const char** json_message);

//#define jg json_get
static json_value * jg(json_value * v, const void* key);
static json_value * json_get(json_value * v, const void* key);
static json_value * json_get_from_object(json_object* json, const char* key);
static json_value * json_get_from_array(json_array* json, const int index);

static void json_print_value(const json_value * v, int tab);
static void json_print_array(const json_array * json, int tab);
static void json_print_object(const json_object * json, int tab);
static void json_print(const json_value * json);

static void json_free(json_value * jsonv);
static void json_free_array(json_array * jsona);
static void json_free_object(json_object * jsono);

//static int strcasecmp(const char* a, const char* b);

static json_value * jg(json_value * v, const void* key) {
    return json_get(v, key);
}
static json_value * json_get(json_value * v, const void* key) {
    if (v.type == OBJECT) return json_get_from_object((json_object *)(v->value), (char *)key);
    if (v.type == ARRAY) return json_get_from_array((json_array *)(v->value), (int)key);
    printf("json_get input is not OBJECT nor ARRAY!\n");
    return NULL;
}
static json_value * json_get_from_object(json_object* json, const char* key) {
    if (json == NULL || key == NULL || *key == '\0') return NULL;
    for (int i = 0; i <= json->last_index; i++) {
        if (strcmp(json->keys[i], key) == 0) {
            return &(json->values[i]); //found
        }
    }
	return NULL; //not found
}
static json_value * json_get_from_array(json_array* json, const int index) {
    if (json == NULL || index < 0 || json->last_index < index) return NULL; // out of index
    return &(json->values[index]);
}

json_value * json_get_value(const char** json_message) {
    char c;
    char temp[64] = "";
    json_value jsonv;
    jsonv.type = UNDEFINED;
    jsonv.value = NULL;

    while (c = *((*json_message)++)) {
        switch (c) {
			//in : {something}
			//   : c   		//c and *json_message are same position
			//out: c          p
			//return : JSON OBJECT {something}
			case '{':
				(*json_message)--;
				jsonv.type = OBJECT;
				jsonv.value = json_create_object(json_message);
				return jsonv;
			case '}':
				printf("UNDEFINED }\n");
				return jsonv;
			//in : [something]
			//   : cp    //c==c, p= *json_message는 동일한 위치
			//out: c          p
			//return : JSON ARRAY [something]
			case '[':
				(*json_message)--;
				jsonv.type = ARRAY;
				jsonv.value = json_create_array(json_message);
				return jsonv;
			case ']':
				printf("UNDEFINED ]\n");
				return jsonv;
			//in : "test"
			//   : cp     //c ==c, p == *json_message 현재 위치
			//out: c     p
			//return: char[5] 'test\0'
			case '\"':
			{
				jsonv.type = STRING;
				const char* startptr = (*json_message);
				while (*((*json_message)++) != '\"');
				int size = (int)(*json_message - startptr -1);
				if (size < 0) size = 0;
				char* str = (char*)malloc(sizeof(char) * (size + 1));
				if (str == NULL) {
					printf("string malloc error;\n");
					return jsonv;
				}
				memcpy(str, startptr, sizeof(char) * size);
				str[size] = '\0';
				jsonv.value = str;
				return jsonv;
			}
			default:
			{
				//in : null | false | true
				//   : cp  //c==c, p = *json_message 현재 위치
				//out: c   p
				//return : null | false | true
				if (isalpha(c)) {
					const char* startptr = (*json_message) - 1;
					while (isalpha(*((*json_message)++)));
					(*json_message)--;
					int size = (*json_message - startptr);
					memcpy(temp, startptr, sizeof(char) * size);
					temp[size] = '\0';
					if (strcasecmp(temp, "null") == 0) {
						jsonv.type = JNULL;
						return jsonv;
					}
					if (strcasecmp(temp, "false") == 0 || strcasecmp(temp, "true") == 0) {
						jsonv.type = BOOLEAN;
						jsonv.value = malloc(sizeof(int));
						if (strcasecmp(temp, "false") == 0) *((int *)jsonv.value) = false;
						else *((int *)jsonv.value) = true;//TODO
						return jsonv;
					}
					printf("BOOLEAN or NULL error\n");
					return jsonv;
				}
				//in : -8054
				//   : cp  //c==c, p = *json_message 현재 위치
				//out: c    p
				//return : int -8054
				if (isdigit(c) || c == '-' || c == '+') {
					const char* startptr = (*json_message) - 1;
					while (true) {
						char ch = *((*json_message)++);
						if ((isdigit(ch) || ch == '.') == false)
							break;
					}
					(*json_message)--;
					int size = (*json_message - startptr);
					memcpy(temp, startptr, sizeof(char) * size);
					temp[size] = '\0';
					jsonv.type = NUMBER;
					//TODO append process of longlong and super big integer(as string)
					jsonv.bytesize = 4;
					jsonv.value = malloc(sizeof(int));
					if (jsonv.value == NULL) {
						fprintf(stderr, "malloc error!\n");
						return jsonv;
					}
					*((int*)jsonv.value) = atoi(temp);
					return jsonv;
				}
			}
        }
    }
}

static json_value json_create(const char* json_message) {
    return json_get_value(&json_message);
}
json_array * json_create_array(const char** json_message) {
    json_array* jsona = (json_array *)malloc(sizeof(json_array));
    memset(jsona, 0x00, sizeof(json_array));
    jsona->last_index = 0;
    char c;
    int stack = 0;
    while (c = *((*json_message)++)) {
        switch (c) {
        case '[':
            if (stack == 0) stack++;
            else {
                (*json_message)--;
                jsona->values[jsona->last_index] = json_get_value(json_message);
                jsona->last_index++;
            }
            break;
        case ']':
            jsona->last_index--;
            return jsona;
        default:
            if (isalpha(c) || isdigit(c) || c == '[' || c == '{' || c == '\"' ||
                c == '-' || c=='+') {
                (*json_message)--; 
                jsona->values[jsona->last_index] = json_get_value(json_message);
                jsona->last_index++;
            }
        }
    }
}
json_object * json_create_object(const char** json_message) {
    json_object* jsono = (json_object*)malloc(sizeof(json_object));
    memset(jsono, 0x00, sizeof(json_object));
    jsono->last_index = 0;
    int stack = 0;
    int keyorvalue = KEY;
    char c;
    while (c = *((*json_message)++)) {
        switch (c) {
        case '{':
            if (stack == 0) stack++;
            else {
                if (keyorvalue == KEY) {
                    printf("key cannot be an Object\n");
                    return jsono;
                }
                (*json_message)--;
                jsono->values[jsono->last_index] = json_get_value(json_message);
                jsono->last_index++;
                keyorvalue = KEY;
            }
            break;
        case '}':
            jsono->last_index--;
            return jsono;
        default:
            if (isalpha(c) || isdigit(c) || c == '[' || c == '{' || c == '\"' ||
                c == '-' || c=='+') {
                (*json_message)--;
                if (keyorvalue == KEY) {
                    json_value v = json_get_value(json_message);
                    if (v.type != STRING) {
                        printf("Key MUST be a string");
                        return jsono;
                    }
                    jsono->keys[jsono->last_index] = (char *)(v.value);
                    keyorvalue = VALUE;
                }
                else {
                    jsono->values[jsono->last_index] = json_get_value(json_message);
                    jsono->last_index++;
                    keyorvalue = KEY;
                }
            }
        }
    }
}
void json_print_value(const json_value v, int tab) {
    if (v.type == UNDEFINED) printf("UNDEFINED");
    if (v.type == NUMBER) printf("%d", *((int *)(v.value)));
    if (v.type == ARRAY) json_print_array((json_array *)(v.value), tab);
    if (v.type == STRING) printf("\"%s\"", ((char *)(v.value)));
    if (v.type == BOOLEAN) printf(*((int *)(v.value))?"true":"false");
    if (v.type == OBJECT) {json_print_object(((json_object*)(v.value)), tab); }
    if (v.type == JNULL) printf("null");
}
void json_print_array(const json_array* json, int tab) {
    printf("[");
    for (int i = 0; i <= json->last_index; i++) {
        json_print_value(json->values[i], tab);
        if(json->last_index != i)
            printf(",");
    }
    printf("]");
}
void json_print_object(const json_object* json, int tab) {
    printf("{\n");
    tab++;
    for (int i = 0; i <= json->last_index; i++) {
        for (int i = 0; i < tab; i++) printf("\t");
        printf("\"%s\": ", json->keys[i]);
        json_print_value(json->values[i], tab);
        if(json->last_index != i)
            printf(",\n");
    }
    printf("\n");
    tab--;
    for (int i = 0; i < tab; i++) printf("\t");
    printf("}");
}
void json_print(const json_value json) {
    json_print_value(json, 0);
}
static void json_free(json_value jsonv) {
    int t = jsonv.type;
	if (t == NUMBER || t == STRING || t == BOOLEAN) {
		free(jsonv.value);
    }
    else if (t == ARRAY) {
        json_free_array((json_array *)(jsonv.value));
    }
    else if (t == OBJECT) {
        json_free_object((json_object *)(jsonv.value));
    }
    else {
        //UNDEFINED or JNULL. It don't have to call free.
    }
}
static void json_free_array(json_array* jsona) {
    if (jsona == NULL) return;
    for (int i = 0; i <= jsona->last_index; i++)
        json_free(jsona->values[i]);
    free(jsona);
}
static void json_free_object(json_object* jsono) {
    if (jsono == NULL) return;
    for (int i = 0; i <= jsono->last_index; i++) {
        free(jsono->keys[i]);
        json_free(jsono->values[i]);
    }
    free(jsono);
}

int strcasecmp(const char* a, const char* b) {
    while (true) {
        if (*a == '\0' || *b == '\0') {
            if (*a == *b) return 0;
            else return -1;
        }
        if (tolower(*a) != tolower(*b))
            return -1;
        a++;
        b++;
    }
}
