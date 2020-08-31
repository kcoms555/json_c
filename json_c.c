#ifndef __JSONC_HEADER__
#define __JSONC_HEADER__

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#define JSON_MAX_INDEX 100
#define JSON_LAST_ARG_MAGIC_NUMBER -1027
#define JSON_STRBUFSIZE 256

typedef enum json_type_enum { JSON_UNDEFINED = 0x0, JSON_NUMBER = 0x1, JSON_STRING=0x2, JSON_BOOLEAN=0x4, JSON_ARRAY=0x8, JSON_OBJECT=0x10, JSON_NULL=0x20, JSON_INTEGER=0x40, JSON_DOUBLE=0x80 } json_type;
typedef enum json_keyorvalue_enum { JSON_KEY, JSON_VALUE } json_keyorvalue;
typedef struct json_small_stack_s{
	int top;
	int type[20];
	const void * stacktrace[20];
} json_small_stack;
typedef struct json_value_s {
    json_type type;
    void* value;
} json_value;
typedef struct json_object_s {
    int last_index;
    char* keys[JSON_MAX_INDEX];
    json_value values[JSON_MAX_INDEX];
} json_object;
typedef struct json_array_s {
    int last_index;
    json_value values[JSON_MAX_INDEX];
} json_array;

json_value json_string_to_value(const char** json_message);
json_value json_create(const char* json_message);
json_array * json_create_array(const char** json_message);
json_object * json_create_object(const char** json_message);

#define json_get_int(...) ((int)json_to_longlongint(json_get(__VA_ARGS__)))
#define json_get_longlongint(...) json_to_longlongint(json_get(__VA_ARGS__))
#define json_get_float(...) ((float)json_to_double(json_get(__VA_ARGS__)))
#define json_get_double(...) json_to_double(json_get(__VA_ARGS__))
#define json_get_bool(...) json_to_bool(json_get(__VA_ARGS__))
#define json_get_string(...) json_to_string(json_get(__VA_ARGS__))

#define json_to_int(v) ((int)json_to_longlongint(v))
long long int json_to_longlongint(json_value v);
#define json_to_float(v) ((float)json_to_float(v))
double json_to_double(json_value v);
bool json_to_bool(json_value v);
char * json_to_string(json_value v);
bool json_is_null(json_value v);
json_type json_get_type(json_value v);
const char * const json_type_to_string(int type);

//TODO read json file
json_value json_read(const char * const path);

#define json_get(...) (json_get_value(__VA_ARGS__, (void*)JSON_LAST_ARG_MAGIC_NUMBER))
json_value json_get_value(json_value v, ...);
json_value json_get_from_json_value(json_value v, const void* k);
json_value json_get_from_object(json_object* json, const char* key);
json_value json_get_from_array(json_array* json, const int index);
int json_len(json_value v);
int json_get_last_index(json_value v);

void json_fprint_value(FILE * outfp, const json_value v, int tab);
void json_fprint_array(FILE * outfp, const json_array* json, int tab);
void json_fprint_object(FILE * outfp, const json_object* json, int tab);
#define json_fprint(outfp, ...) (json_fprint_value(outfp, json_get(__VA_ARGS__), 0))
#define json_print(...) json_fprint(stdout, __VA_ARGS__)

json_small_stack json_stacktrace_get_stack(void);
void json_stacktrace_push(json_small_stack * jss, int type, const void * key);
void json_stacktrace_print(FILE * fp, const json_small_stack * const jss);

void json_free(json_value jsonv);
void json_free_array(json_array* jsona);
void json_free_object(json_object* jsono);

//int strcasecmp(const char* a, const char* b);
#ifdef __cplusplus
}
#endif
#endif

#ifndef __JSONC_BODY__
#define __JSONC_BODY__
#ifdef __cplusplus
extern "C"{
#endif
static const int MAX_INDEX = JSON_MAX_INDEX;
static const json_value undefined_json = {JSON_UNDEFINED, NULL};

json_value json_get_value(json_value v, ...) {
	void * key = NULL;
	void * vakey = NULL;
	va_list ap;
	va_start(ap, v);
	key = va_arg(ap, void *);
	if((int)key == JSON_LAST_ARG_MAGIC_NUMBER){ 
		return v;
		//fprintf(stderr, "json_get error : json_get needs two arguments at least and each of arguments must be a index(integer) or string(search key) except the first argument\n");
		//return undefined_json;
	}
	if( ! (v.type == JSON_ARRAY || v.type == JSON_OBJECT)){
		fprintf(stderr, "json_get error : the first argument of json_get should be an array or an object (type : %s)\n", json_type_to_string(v.type));
		return undefined_json;
	}
	json_small_stack jss = json_stacktrace_get_stack();

	if(v.type == JSON_OBJECT) {
		if((int)key>=0 && (int)key <= ((json_object *)(v.value))->last_index) json_stacktrace_push(&jss, v.type, ((json_object *)(v.value))->keys[(int)key]);
		else json_stacktrace_push(&jss, v.type, key);
	}
	else json_stacktrace_push(&jss, v.type, key);

	json_value ret = json_get_from_json_value(v, key);
	if(ret.type == JSON_UNDEFINED){
		fprintf(stderr, "error tracing : ");
		json_stacktrace_print(stderr, &jss);
		fprintf(stderr, "\n");
		return ret;
	}

	while(1){
		vakey = va_arg(ap, void *);
		if((int)vakey == JSON_LAST_ARG_MAGIC_NUMBER) break; 

		if(ret.type == JSON_OBJECT) {
			if((int)key>=0 && (int)vakey <= ((json_object *)(ret.value))->last_index) json_stacktrace_push(&jss, ret.type, ((json_object *)(ret.value))->keys[(int)vakey]);
			else json_stacktrace_push(&jss, ret.type, vakey);
		}
		else json_stacktrace_push(&jss, ret.type, vakey);

		ret = json_get_from_json_value(ret, vakey);

		if(ret.type == JSON_UNDEFINED){
			fprintf(stderr, "error tracing : ");
			json_stacktrace_print(stderr, &jss);
			fprintf(stderr, "\n");
			return ret;
		}
	}
    return ret;
}
json_value json_get_from_json_value(json_value v, const void* key) {
    if (v.type == JSON_OBJECT) return json_get_from_object((json_object *)(v.value), (char *)key);
    if (v.type == JSON_ARRAY) return json_get_from_array((json_array *)(v.value), (int)key);
	fprintf(stderr, "json_get_from_json_value error : cannot get a json value with key from json_value that is not an object nor an array(value type : %s)\n", json_type_to_string(v.type));
    return undefined_json;
}
json_value json_get_from_object(json_object* json, const char* key) {
	//when the key is assummed as an index
	if((int)key >=0 && (int)key <= json->last_index)
		return json->values[(int)key];
	if((int)key <= MAX_INDEX && (int)key>= 0){
		fprintf(stderr, "json_get_from_object error : out of index\n");
		return undefined_json;
	}
		
	//when the key is assummed as a string
    if (json == NULL || key == NULL || *key == '\0') return undefined_json;
    for (int i = 0; i <= json->last_index; i++) {
        if (strcmp(json->keys[i], key) == 0) {
            return json->values[i];
        }
    }
	fprintf(stderr, "json_get_from_object error : no value corresponding to the key(%s)\n", key);
    return undefined_json;
}
json_value json_get_from_array(json_array* json, const int index) {
    if (json == NULL || index < 0 || json->last_index < index){
		fprintf(stderr, "json_get_from_array error : out of index\n");
		return undefined_json;
	}
    return json->values[index];
}
int json_len(json_value v){
	return json_get_last_index(v)+1;
}
int json_get_last_index(json_value v){
	if(v.type == JSON_OBJECT) return ((json_object *)(v.value))->last_index;
	if(v.type == JSON_ARRAY) return ((json_array *)(v.value))->last_index;

	fprintf(stderr, "json_get_last_index : the type of json_value is not a JSON_ARRAY nor a JSON_OBJECT");
	return -1;
}

json_value json_string_to_value(const char** json_message) {
    char c;
    char temp[64] = "";
    json_value jsonv;
    jsonv.type = JSON_UNDEFINED;
    jsonv.value = NULL;

    while (c = *((*json_message)++)) {
        switch (c) {
        //in : {something}
        //   : c   		//c and *json_message are same position
        //out: c          p
        //return : JSON OBJECT {something}
        case '{':
            (*json_message)--;
            jsonv.type = JSON_OBJECT;
            jsonv.value = json_create_object(json_message);
            return jsonv;
        case '}':
            printf("parse error : unexpected token '}'\n");
            return jsonv;
        //in : [something]
        //   : cp    //c==c, p= *json_message는 동일한 위치
        //out: c          p
        //return : JSON JSON_ARRAY [something]
        case '[':
            (*json_message)--;
            jsonv.type = JSON_ARRAY;
            jsonv.value = json_create_array(json_message);
            return jsonv;
        case ']':
            printf("parse error : unexpected token ']'\n");
            return jsonv;
        //in : "test"
        //   : cp     //c ==c, p == *json_message 현재 위치
        //out: c     p
        //return: char[5] 'test\0'
        case '\"':
        {
            jsonv.type = JSON_STRING;
			char* str = (char*)malloc(sizeof(char) * JSON_STRBUFSIZE);
			int size = 0;
			if (str == NULL) {
				printf("string malloc error;\n");
				return jsonv;
			}

			//TODO : string process
			while (true) {
				char ch = *((*json_message)++);
				switch(ch){
					case '\\':
					{
						char escape = *((*json_message)++);
						switch(escape){
							case '\"': str[size] = '\"'; break;
							case '\\': str[size] = '\\'; break;
							case '/': str[size] = '/'; break;
							case 'b': str[size] = '\b'; break;
							case 'f': str[size] = '\f'; break;
							case 'n': str[size] = '\n'; break;
							case 'r': str[size] = '\r'; break;
							case 't': str[size] = '\t'; break;
							//Parsing unicodes are not implemented
							case 'u':{
								str[size++] = '\\';
								str[size] = 'u';
								break;
							}
							/*
							//TODO : Implement parsing unicodes;
							case 'u':{
								char chs[4] = {0,};
								for(int i=0; i<4; i++){
									chs[i] = tolower(*((*json_message)++));
									printf("chs[%d] : %c(ascii:%d)\n", i, chs[i], chs[i]);
									if(chs[i] >= 'a' && chs[i] <= 'f') chs[i] = chs[i] - 'a';
									else if(chs[i] >= '0' && chs[i] <= '9') chs[i] = chs[i] - '0';
									else{
										fprintf(stderr, "json_string_to_value error: parse errer at escape string '\\u'\n");
										break;
									}
								}
								str[size++] = chs[0]*16+chs[1];
								str[size] = chs[2]*16+chs[3]; //size will be increased at the end of while
								break;
							}
							*/
							default:
								fprintf(stderr, "json_string_to_value error: parse errer at escape string '\\%c'\n", escape);
						}
						break;
					}
					case '\"':
						str[size] = '\0';
						goto JSON_STRBREAK;
					default:
						str[size] = ch;
				}
				size++;
				if((size+1) % JSON_STRBUFSIZE == 0){
					str = (char *)realloc(str, sizeof(char) * JSON_STRBUFSIZE * (1 + (size+1)/JSON_STRBUFSIZE));
					printf("realloced : %s\n", str);
					if(str == NULL){
						printf("string malloc error;\n");
						return jsonv;
					}
				}
			}
JSON_STRBREAK:
			jsonv.value = str;
            return jsonv;
        }
        default:
        {
			//printf("c : %c\n", c);
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
                    jsonv.type = JSON_NULL;
                    return jsonv;
                }
                if (strcasecmp(temp, "false") == 0 || strcasecmp(temp, "true") == 0) {
                    jsonv.type = JSON_BOOLEAN;
                    jsonv.value = malloc(sizeof(bool));
                    if (strcasecmp(temp, "false") == 0) *((bool *)jsonv.value) = false;
                    else *((bool *)jsonv.value) = true;
                    return jsonv;
                }
                printf("BOOLEAN or NULL error\n");
                return jsonv;
            }
            //in : number
            //return : number(integer or double)
            if (isdigit(c) || c == '-' || c == '+' || c == '.') {
                const char* startptr = (*json_message) - 1;
                while (true) {
                    char ch = *((*json_message)++);
                    if ((isdigit(ch) || ch == '.' || ch=='e' || ch=='E' || ch=='+' || ch == '-') == false)
                        break;
                }
                (*json_message)--;
                int size = (*json_message - startptr);
                memcpy(temp, startptr, sizeof(char) * size);
                temp[size] = '\0';
				
				if(strchr(temp, '.') || strchr(temp, 'e') || strchr(temp, 'E')){
					jsonv.type = (json_type) (JSON_NUMBER|JSON_DOUBLE);
					jsonv.value = malloc(sizeof(double));
					if (jsonv.value == NULL) {
						printf("malloc error!\n");
						return jsonv;
					}
					*((double*)jsonv.value) = atof(temp);
					//printf("temp : %s\n type:%x read : double %f\n", temp, jsonv.type,  *((double *)jsonv.value));
				} else{
					jsonv.type = (json_type) (JSON_NUMBER|JSON_INTEGER);
					jsonv.value = malloc(sizeof(long long int));
					if (jsonv.value == NULL) {
						printf("malloc error!\n");
						return jsonv;
					}
					*((long long int*)jsonv.value) = atoll(temp);
					//printf("temp : %s\n type:%x read : integer %lld\n", temp, jsonv.type,  *((long long int *)jsonv.value));
				}
                return jsonv;
            }
        }
        }
    }
	fprintf(stderr, "json_string_to_value error: json parser meets NULL");
	return jsonv;
}

json_value json_create(const char* json_message) {
    return json_string_to_value(&json_message);
}
json_array* json_create_array(const char** json_message) {
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
                jsona->values[jsona->last_index] = json_string_to_value(json_message);
                jsona->last_index++;
            }
            break;
        case ']':
            jsona->last_index--;
            return jsona;
        default:
            if (isalpha(c) || isdigit(c) || c == '[' || c == '{' || c == '\"' || c == '-' || c=='+' || c=='.') {
                (*json_message)--; 
                jsona->values[jsona->last_index] = json_string_to_value(json_message);
                jsona->last_index++;
            }
        }
    }
	fprintf(stderr, "json_create_array error: json parser meets NULL");
	return jsona;
}
json_object* json_create_object(const char** json_message) {
    json_object* jsono = (json_object*)malloc(sizeof(json_object));
    memset(jsono, 0x00, sizeof(json_object));
    jsono->last_index = 0;
    int stack = 0;
    int keyorvalue = JSON_KEY;
    char c;
    while (c = *((*json_message)++)) {
        switch (c) {
        case '{':
            if (stack == 0) stack++;
            else {
                if (keyorvalue == JSON_KEY) {
                    printf("key cannot be an Object\n");
                    return jsono;
                }
                (*json_message)--;
                jsono->values[jsono->last_index] = json_string_to_value(json_message);
                jsono->last_index++;
                keyorvalue = JSON_KEY;
            }
            break;
        case '}':
            jsono->last_index--;
            return jsono;
        default:
            if (isalpha(c) || isdigit(c) || c == '[' || c == '{' || c == '\"' || c == '-' || c=='+' || c=='.') {
                (*json_message)--;
                if (keyorvalue == JSON_KEY) {
                    json_value v = json_string_to_value(json_message);
                    if (v.type != JSON_STRING) {
                        printf("Key MUST be a string");
                        return jsono;
                    }
                    jsono->keys[jsono->last_index] = (char *)(v.value);
                    keyorvalue = JSON_VALUE;
                }
                else {
                    jsono->values[jsono->last_index] = json_string_to_value(json_message);
                    jsono->last_index++;
                    keyorvalue = JSON_KEY;
                }
            }
        }
    }
	fprintf(stderr, "json_create_object error: json parser meets NULL");
	return jsono;
}

long long int json_to_longlongint(json_value v){
	if( ! (v.type & JSON_NUMBER)){
		fprintf(stderr, "json_to_longlongint error: the type of the json_value is not the type of JSON_NUMBER");
		return 0;
	}
	if( v.type & JSON_INTEGER ) return *((long long int *)(v.value));
	if( v.type & JSON_DOUBLE ) return (long long int)*((double *)(v.value));
	fprintf(stderr, "json_to_longlongint error: unknown numeric type");
	return 0;
}
double json_to_double(json_value v){
	if( ! (v.type & JSON_NUMBER)){
		fprintf(stderr, "json_to_double error: the type of the json_value is not the type of JSON_NUMBER");
		return 0;
	}
	if( v.type & JSON_INTEGER ) return (double)*((long long int *)(v.value));
	if( v.type & JSON_DOUBLE ) return *((double *)(v.value));
	fprintf(stderr, "json_to_double error: unknown numeric type");
	return 0;
}
bool json_to_bool(json_value v){
	if( ! (v.type & JSON_BOOLEAN) ){
		fprintf(stderr, "json_to_bool error: the type of the json_value is not the type of JSON_BOOLEAN");
		return false;
	}
	return *((bool *)(v.value));
}
char * json_to_string(json_value v){
	if( ! (v.type & JSON_STRING) ){
		fprintf(stderr, "json_to_string error: the type of the json_value is not the type of JSON_STRING");
		return NULL;
	}
	return (char *)(v.value);
}
bool json_is_null(json_value v){
	return v.type == JSON_NULL;
}
json_type json_get_type(json_value v){
	return v.type;
}

const char * const json_type_to_string(int type){
	switch(type){
		case JSON_UNDEFINED: return "undefined";
		case JSON_NUMBER: return "number";
		case JSON_NUMBER|JSON_INTEGER: return "number(integer)";
		case JSON_NUMBER|JSON_DOUBLE: return "number(double)";
		case JSON_STRING: return "string";
		case JSON_BOOLEAN: return "boolean";
		case JSON_ARRAY: return "array";
		case JSON_OBJECT: return "object";
		case JSON_NULL: return "null";
		default: return "undefined";
	}
}

void json_fprint_value(FILE * outfp, const json_value v, int tab) {
    if (v.type == JSON_UNDEFINED) fprintf(outfp, "undefined");
    if (v.type == (JSON_NUMBER|JSON_INTEGER)) fprintf(outfp, "%lld", *((long long int *)(v.value)));
    if (v.type == (JSON_NUMBER|JSON_DOUBLE)) fprintf(outfp, "%f", *((double *)(v.value)));
    if (v.type == JSON_ARRAY) json_fprint_array(outfp, (json_array *)(v.value), tab);
    if (v.type == JSON_STRING) fprintf(outfp, "\"%s\"", ((char *)(v.value)));
    if (v.type == JSON_BOOLEAN) fprintf(outfp, *((bool *)(v.value))?"true":"false");
    if (v.type == JSON_OBJECT) {json_fprint_object(outfp, ((json_object*)(v.value)), tab); }
    if (v.type == JSON_NULL) fprintf(outfp, "null");
}
void json_fprint_array(FILE * outfp, const json_array* json, int tab) {
    fprintf(outfp, "[");
    for (int i = 0; i <= json->last_index; i++) {
        json_fprint_value(outfp, json->values[i], tab);
        if(json->last_index != i)
            fprintf(outfp, ", ");
    }
    fprintf(outfp, "]");
}
void json_fprint_object(FILE * outfp, const json_object* json, int tab) {
    fprintf(outfp, "{\n");
    tab++;
    for (int i = 0; i <= json->last_index; i++) {
        for (int i = 0; i < tab; i++) fprintf(outfp, "\t");
        fprintf(outfp, "\"%s\": ", json->keys[i]);
        json_fprint_value(outfp, json->values[i], tab);
        if(json->last_index != i)
            fprintf(outfp, ",\n");
    }
    fprintf(outfp, "\n");
    tab--;
    for (int i = 0; i < tab; i++) fprintf(outfp, "\t");
    fprintf(outfp, "}");
}
/*
void json_fprint(FILE * outfp, const json_value json) {
    json_fprint_value(outfp, json, 0);
	fprintf(outfp, "\n");
}
*/

json_small_stack json_stacktrace_get_stack(void){
	json_small_stack jss = {-1, {JSON_UNDEFINED, }, {NULL, }};
	return jss;
}
void json_stacktrace_push(json_small_stack * jss, int type, const void * key){
	if(jss->top > 19){
		//the stack depth is 20. stop to record a stack trace
		return;
	}
	jss->top++;
	jss->type[jss->top] = type;
	jss->stacktrace[jss->top] = key;
	//printf("push : top:%d, type:%s, key:%d\n", jss->top, json_type_to_string(type), (int)key);
}
void json_stacktrace_print(FILE * fp, const json_small_stack * const jss){
	if(jss->top<0) return;

	fprintf(fp, "(%s)", json_type_to_string(jss->type[0]));

	for(int i=1; i<=jss->top; i++){
		if(jss->type[i-1] == JSON_ARRAY) fprintf(fp, "(%s)[%d]", json_type_to_string(jss->type[i]), (int)jss->stacktrace[i-1]);
		else if(jss->type[i-1] == JSON_OBJECT){ 
			if((int)jss->stacktrace[i-1] <= MAX_INDEX && (int)jss->stacktrace[i-1] >= 0) fprintf(fp, "(%s)[%d]", json_type_to_string(jss->type[i]), (int)jss->stacktrace[i-1]);
			else fprintf(fp, "->(%s)%s", json_type_to_string(jss->type[i]), jss->stacktrace[i-1]);
		}
		else fprintf(fp, "->(%s)", json_type_to_string(jss->type[i]));
	}

	if(jss->type[jss->top] == JSON_ARRAY) fprintf(fp, "(%s)[%d]", json_type_to_string(JSON_UNDEFINED), (int)jss->stacktrace[jss->top]);
	else if(jss->type[jss->top] == JSON_OBJECT){ 
		if((int)jss->stacktrace[jss->top] <= MAX_INDEX && (int)jss->stacktrace[jss->top] >= 0) fprintf(fp, "(%s)[%d]", json_type_to_string(JSON_UNDEFINED), (int)jss->stacktrace[jss->top]);
		else fprintf(fp, "->(%s)%s", json_type_to_string(JSON_UNDEFINED), jss->stacktrace[jss->top]);
	}
	else fprintf(fp, "->(%s)", json_type_to_string(JSON_UNDEFINED));
	
}

void json_free(json_value jsonv) {
    int t = jsonv.type;
	if (t == JSON_NUMBER || t == JSON_STRING || t == JSON_BOOLEAN) {
		free(jsonv.value);
    }
    else if (t == JSON_ARRAY) {
        json_free_array((json_array *)(jsonv.value));
    }
    else if (t == JSON_OBJECT) {
        json_free_object((json_object *)(jsonv.value));
    }
    else {
        //JSON_UNDEFINED or JSON_NULL. It don't have to call free.
    }
}
void json_free_array(json_array* jsona) {
    if (jsona == NULL) return;
    for (int i = 0; i <= jsona->last_index; i++)
        json_free(jsona->values[i]);
    free(jsona);
}
void json_free_object(json_object* jsono) {
    if (jsono == NULL) return;
    for (int i = 0; i <= jsono->last_index; i++) {
        free(jsono->keys[i]);
        json_free(jsono->values[i]);
    }
    free(jsono);
}

/*
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
*/
#ifdef __cplusplus
}
#endif
#endif
