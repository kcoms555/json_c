# json_c
Eazy and concise C JSON parser
# Installation
You can use it by simply importing 'json_c.c' into your source code.
```
#include "json_c.c"
```
If you need, you can easily split "json_c.c" into "json_c.h" and "json_c.c". I inserted a partition between the declation(header) and the definition(body) in "json_c.c"
# usage
### Sample of a JSON Object
```
{
        "string": "It's a string",
        "number small integer": 1027,
        "number big integer": 1027102710271027,
        "number double1": .1027,
        "number double2": .1027e+4,
        "object": {
                "name": "some object",
                "object id": 1027
        },
        "array": [
	{
                "name": "object 1 in array",
                "number": 1027
        	}, 
	{
                "name": "object 2 in array",
                "number": 1027
        	},
	1027, 0.102700, 1027.000000, true, false, null, ?nonejsontype?,
        ]
        "boolean true": true,
        "boolean false": false,
        "null": null
        "none-json-type": somethingwrong
}
```

### Assign a JSON Object
```
        //Creation and assignment of a json_value
        //Dynamic allocation used
        json_value json = json_create(str);
```

### Print a JSON variable
json_print(json_value) can print every type
```
        //JSON Print Demonstration
        json_print(json); putchar('\n');
	//json_fprint(stdout, json); putchar('\n'); //You can use this if you need to print into FILE*
```
##### output
```
BOOLEAN or NULL error
BOOLEAN or NULL error	<- They are printed because of wrong inputs('?nonejsontype?', 'somethingwrong'). It needs parseable JSON strings
{
        "string": "It's a string",
        "number small integer": 1027,
        "number big integer": 1027102710271027,
        "number double1": 0.102700,
        "number double2": 1027.000000,
        "object": {
                "name": "some object",
                "object id": 1027
        },
        "array": [{
                "name": "object 1 in array",
                "number": 1027
        }, {
                "name": "object 2 in array",
                "number": 1027
        }, 1027, 0.102700, 1027.000000, true, false, null, undefined],
        "boolean true": true,
        "boolean false": false,
        "null": null,
        "none-json-type": undefined
}
```
### Get anything
You can get a anything you need with 'json_get(json_value, key1, ...)' from a JSON Object or a JSON Array
If you want to get 'name' of the object which is the second value of 'array', You can write 'json_get(json, "array", 1, "name)'. It is a similar to JS way 'json.array[1].name'

### Get a JSON string
You can get a JSON string with json_get_string(json_value, key1, key2, ...).
'key' can be a type of char*(as a key) or int(as a index)
```
        //JSON String Demonstration
        printf("string : %s\n", json_get_string(json, "string"));
```
##### output
```
string : It's a string
```
### Get a JSON number
You can get a JSON number with json_get_int(json_value, key1, key2, ...) or json_get_double or json_get_longlongint, json_get_float
```
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
```
##### output
```
number small integer : 1027
number big integer(int) : -63861709
number big integer(long long int) : 1027102710271027
number double1 0.102700
number double2 1027.000000
number small integer(with json_get_double) : 1027.000000
number double2(with json_get_int) : 1027
```
### Get a JSON value from a JSON Object
You can get a JSON Object with json_get(json_value, key1, key2, ...).
Or you can get a JSON value in a JSON Object you need (see the comment 'by key search2')
You can use the array indexing way.
```
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
```
##### output
```
object : {
        "name": "some object",
        "object id": 1027
}
object->name: some object
object->object id: 1027
object->name: some object
object->object id: 1027
object[0] : "some object"
object[1] : 1027
object[0] : "some object"
object[1] : 1027
```
### Get a JSON value from a JSON Array
You can get a JSON Array with json_get(json_value, index1, index2, ...).
```
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
```
##### output
```
array: [{
        "name": "object 1 in array",
        "number": 1027
}, {
        "name": "object 2 in array",
        "number": 1027
}, 1027, 0.102700, 1027.000000, true, false, null, undefined]
array[0] : {
        "name": "object 1 in array",
        "number": 1027
}
array[1] : {
        "name": "object 2 in array",
        "number": 1027
}
array[2] : 1027
array[3] : 0.102700
array[4] : 1027.000000
array[5] : true
array[6] : false
array[7] : null
error tracing : (array)(undefined)[8]	<-- It is printed when it is a type of undefined or not readable
array[8] : undefined
array[0] : {
        "name": "object 1 in array",
        "number": 1027
}
array[1] : {
        "name": "object 2 in array",
        "number": 1027
}
array[2] : 1027
array[3] : 0.102700
array[4] : 1027.000000
array[5] : true
array[6] : false
array[7] : null
error tracing : (object)->(array)array(undefined)[8]
array[8] : undefined
```
### Get a JSON boolean
You can get a JSON boolean with json_get_bool(json_value, key1, key2, ...). json_get_int will not be not working and will print a error message.
```
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
```
##### output
```
boolean true(with json_print): true
boolean false(with json_print): false
json_to_longlongint error: the type of the json_value is not the type of JSON_NUMBERboolean true(with json_get_int): 0
json_to_longlongint error: the type of the json_value is not the type of JSON_NUMBERboolean false(with json_get_int): 0
boolean true(with json_get_bool): 1
boolean false(with json_get_bool): 0
```
### Check whether it is JSON null or not
You cannot get anything from the type of JSON null. Use json_is_null(json_value).
```
//JSON Null Demonstration
printf("null: "); json_print(json, "null"); putchar('\n');
printf("null(with json_is_null): %d\n", json_is_null(json_get(json, "null")));
//See how it will be shown
printf("null(with json_get_int): %d\n", json_get_int(json, "null"));
```
##### output
```
null: null
null(with json_is_null): 1
json_to_longlongint error: the type of the json_value is not the type of JSON_NUMBERnull(with json_get_int): 0
```
### Get something with parsed errer
When json_create() reads a something unparseable, It sets its value as a undefined. Any functions printing or getting it will print an errer.
```
//Demonstraion of undefined type (It is created when a json parser got error)
printf("none-json-type: "); json_print(json, "none-json-type"); putchar('\n');
//See how it will be shown
printf("none-json-type(with json_get_int): %d\n", json_get_int(json, "none-json-type"));
```
##### output
```
error tracing : (object)->(undefined)none-json-type
none-json-type: undefined
error tracing : (object)->(undefined)none-json-type
json_to_longlongint error: the type of the json_value is not the type of JSON_NUMBERnone-json-type(with json_get_int): 0
```
### Free what you have assigned
```
//Free a json_value
//When you have parsed a json string and created a json_value, you have to call a function json_free with json_value you have created before end of block(or return)
json_free(json);

//The 'json_value's asigned by json_get, obj and arr, are not allocated in a dynamic allocation way. Thus you are not allowed to call the function json_free with them. The only thing you have to know is that you have to call a json_free with json_value you have assigned with json_create
// ( don't do that ) json_free(obj or arr)
```
# Merit of this JSON parser
1. It does not care of comma (',').
2. It is much simple to access a value in a JSON Object or a JSON Array deeply, thanks to variadic macros in C.
3. When you access a JSON value which is not exist, it will print a error tracker. It helps you to debug it.

# Known Problems
1. It supports upto 100 items in a single Object or an Array. (I will modify the source code to use dynamic allocation, if possible)
2. It does not support unicodes yet. (somebody tell me how to implement it, if you know)
