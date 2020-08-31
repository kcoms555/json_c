#include <stdio.h>
#include "json_c.c"
int main(void){
	const char * str = "\
{\
	\"string\":\"It's a string\"\
	\"number small integer\":1027,\
	\"number big integer\":1027102710271027\
	\"number double1\":.1027,\
	\"number double2\":.1027e+4,\
	\"object\":{\
		\"name\":\"some object\",\
		\"object id\":1027\
	}\
	\"array\":[\
		{\
			\"name\":\"object 1 in array\",\
			\"number\":1027\
		},\
		{\
			\"name\":\"object 2 in array\",\
			\"number\":1027\
		},\
		,1027, .1027, .1027e+4, true, false, null, ?nonejsontype?\
	]\
	\"boolean true\":true,\
	\"boolean false\":false,\
	\"null\":null,\
	\"none-json-type\":somethingwrong,\
}\
";
	//Creation and assignment of a json_value
	//Dynamic allocation used
	json_value json = json_create(str);

	//JSON Print Demonstration
	json_print(json); putchar('\n');
	//It supports to print a JSON object with FILE*
	//json_fprint(stdout, json); putchar('\n');
	putchar('\n');

	//JSON String Demonstration
	printf("string : %s\n", json_get_string(json, "string"));
	putchar('\n');

	//JSON Number Demonstration
	printf("number small integer : %d\n", json_get_int(json, "number small integer"));
	printf("number big integer(int) : %d\n", json_get_int(json, "number big integer"));
	printf("number big integer(long long int) : %lld\n", json_get_longlongint(json, "number big integer"));
	printf("number double1 %f\n", json_get_double(json, "number double1"));
	printf("number double2 %f\n", json_get_double(json, "number double2"));
	//print int type number as double
	printf("number small integer(with json_get_double) : %f\n", json_get_double(json, "number small integer"));
	//print double type number as int
	printf("number double2(with json_get_int) : %d\n", json_get_int(json, "number double2"));
	putchar('\n');

	//JSON Object Demonstration
	printf("object : "); json_print(json, "object"); putchar('\n');
	json_value obj = json_get(json, "object");
	//by key search1
	printf("object->name: %s\n", json_get_string(obj, "name"));
	printf("object->object id: %d\n", json_get_int(obj, "object id"));
	//by key search2 (not recommended)
	printf("object->name: %s\n", json_get_string(json, "object", "name"));
	printf("object->object id: %d\n", json_get_int(json, "object", "object id"));
	//indexing is much faster then key search
	//by indexing1
	for(int i=0; i<json_len(obj); i++){
		printf("object[%d] : ", i); json_print(obj, i); putchar('\n');
	}
	//by indexing2 (not recommended)
	for(int i=0; i<json_len(json_get(json, "object")); i++){
		printf("object[%d] : ", i); json_print(json, "object", i); putchar('\n');
	}
	putchar('\n');

	//JSON Array Demonstration
	printf("array: "); json_print(json, "array"); putchar('\n');
	json_value arr = json_get(json, "array");
	//by indexing1
	for(int i=0; i<json_len(arr); i++){
		printf("array[%d] : ", i); json_print(arr, i); putchar('\n');
	}
	//by indexing2 (not recommended)
	for(int i=0; i<json_len(arr); i++){
		printf("array[%d] : ", i); json_print(json, "array", i); putchar('\n');
	}
	putchar('\n');

	//JSON Boolean Demonstration
	//case of json_print
	printf("boolean true(with json_print): "); json_print(json, "boolean true"); putchar('\n');
	printf("boolean false(with json_print): "); json_print(json, "boolean false"); putchar('\n');
	//case of json_get_int
	printf("boolean true(with json_get_int): %d\n", json_get_int(json, "boolean true"));
	printf("boolean false(with json_get_int): %d\n", json_get_int(json, "boolean false"));
	//case of json_get_bool
	printf("boolean true(with json_get_bool): %d\n", json_get_bool(json, "boolean true"));
	printf("boolean false(with json_get_bool): %d\n", json_get_bool(json, "boolean false"));
	putchar('\n');

	//JSON Null Demonstration
	printf("null: "); json_print(json, "null"); putchar('\n');
	printf("null(with json_is_null): %d\n", json_is_null(json_get(json, "null")));
	//See how it will be shown
	printf("null(with json_get_int): %d\n", json_get_int(json, "null"));
	putchar('\n');

	//Demonstraion of undefined type (It is created when a json parser got error)
	printf("none-json-type: "); json_print(json, "none-json-type"); putchar('\n');
	//See how it will be shown
	printf("none-json-type(with json_get_int): %d\n", json_get_int(json, "none-json-type"));
	putchar('\n');

	//Free a json_value
	//When you have parsed a json string and created a json_value, you have to call a function json_free with json_value you have created before end of block(or return)
	json_free(json);

	//The 'json_value's asigned by json_get, obj and arr, are not allocated in a dynamic allocation way. Thus you are not allowed to call the function json_free with them. The only thing you have to know is that you have to call a json_free with json_value you have assigned with json_create
	// ( don't do that ) json_free(obj or arr)

	return 0;
}
