#include "php_zhangzy.h"

ZEND_FUNCTION(zhangzy_hello) {
	php_printf("hello world!\n");
}

//带返回值的函数
ZEND_FUNCTION(zhangzy_return) {
	ZVAL_LONG(return_value, 100);
	return;
}
//引用传递的参数
ZEND_FUNCTION(zhangzy_ref) {
	zval *a;	//声明一个变量结构体
	//将参数传递给a
	//第一个参数前面的宏表示参数个数，后面的是线程安全资源管理器
	//第二个参数是变量类型，具体类型查看相关文档
	//第三个表示需要赋值的变量结构体
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &a) == FAILURE) {
		RETURN_NULL();
	} 
	if(!a->is_ref__gc) {
		return;	//不是引用传值，直接return
	}
	//将a转换成字符串
	convert_to_string(a);

	//更改a的数据
	ZVAL_STRING(a, "new string", 1);
	return;

}
//接收1个参数的函数
ZEND_FUNCTION(zhangzy_param) {
	char *name;
	int name_len;
	//获取任意类型的
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		RETURN_NULL();
	} 
	PHPWRITE(name, name_len);

}
//接收2个参数
ZEND_FUNCTION(zhangzy_param2) {
	char *name1, *name2;
	int name1_len, name2_len;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name1, &name1_len, &name2, &name2_len) == FAILURE) {
		RETURN_NULL();
	}
	PHPWRITE(name1, name1_len);
	PHPWRITE(name2, name2_len);
}
//带默认值的函数
ZEND_FUNCTION(zhangzy_param3) {
	char *name1;
	int name1_len;
	char *name2 = "default param";
	int name2_len = sizeof(name2) - 1;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &name1, &name1_len, &name2, &name2_len) == FAILURE) {
		RETURN_NULL();
	}

	PHPWRITE(name1, name1_len);
	PHPWRITE(name2, name2_len);

}
//实现php的count函数
ZEND_FUNCTION(zhangzy_count) {
	zval *arr;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &arr) == FAILURE){
	RETURN_NULL();
	}
	RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(arr)));
}
//数组
ZEND_FUNCTION(zhangzy_array) {
	zval *array;
	array_init(return_value); //初始化，参数必须是zval类型

	add_assoc_long(return_value, "lefe", 42);
	add_index_bool(return_value, 123, 1);
	add_next_index_double(return_value, 3.1415926);
	add_next_index_string(return_value, "foo", 1);
	add_next_index_string(return_value, estrdup("bar"), 0);	//手动申请空间
	MAKE_STD_ZVAL(array);
	array_init(array);
	add_next_index_long(array, 1);
	add_next_index_long(array, 10);
	add_next_index_long(array, 100);
	add_index_zval(return_value, 567, array);
}

//指定的value做key
ZEND_FUNCTION(zhangzy_vtok) {
	zval *arr;
	char *val;
	int val_len;
	//获取参数，必须为2个
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "as", &arr, &val, &val_len) == FAILURE) {
		RETURN_NULL();
	}
	//初始化数组，参数必须zval类型
	array_init(return_value);
	//获取数组长度
	int len = zend_hash_num_elements(Z_ARRVAL_P(arr)), len2;
	int i, j;
	char *k, *v;
	zval **item;
	zval **item2; //存储数组每个元素
	zval **item3;
	//RETURN_LONG(len);
	//先将数组的指针移至第一个
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(arr));
	for(i = 0; i < len; i++) {
		//获取当前数据为数组
		zend_hash_get_current_data(Z_ARRVAL_P(arr), (void**)&item);
		//获取子数组的长度
		len2 = zend_hash_num_elements(Z_ARRVAL_PP(item));
		for(j = 0; j < len2; j++) {
			zend_hash_get_current_key(Z_ARRVAL_PP(item), &k, sizeof(k), 0);
			//查找数组中与指定key相等的值
			if(strcmp(k, val) == 0) {
				zend_hash_get_current_data(Z_ARRVAL_PP(item), (void**)&item3);
				//转换成string类型，记住**即指向指针的指针加_ex就行了
				convert_to_string_ex(item3);
				//转换成数组
				convert_to_array_ex(item);
				add_assoc_zval(return_value, Z_STRVAL_PP(item3), *item);
				break;
			}
			//循环获取下一个key
			zend_hash_move_forward(Z_ARRVAL_PP(item));

		}
		//循环到下一个元素
		zend_hash_move_forward(Z_ARRVAL_P(arr));
	}
}

//资源
ZEND_FUNCTION(zhangzy_resource) {
	
}

//fopen，指定打开文件与模式
ZEND_FUNCTION(zhangzy_fopen) {
	char *file, *mode;
	int file_len, mode_len;
	php_stream *stream;
	int options = ENFORCE_SAFE_MODE | REPORT_ERRORS;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &file, &file_len, &mode, &mode_len) == FAILURE) {
		RETURN_NULL();
	}

	stream = php_stream_open_wrapper(file, mode, options, NULL);
	if(!stream) {
		RETURN_FALSE;
	}
	
	php_stream_to_zval(stream, return_value);
}

//类,接口
zend_class_entry *zzyclass_ce, *zzyinterface_ce, *zzyclass_chird_ce;

//注册类方法
//构造方法
ZEND_METHOD(zzyclass, __construct) {
	php_printf("构造方法\n");
}
//public 方法
ZEND_METHOD(zzyclass, zzy_m_public) {
	php_printf("public 方法\n");
}

//修改属性
ZEND_METHOD(zzyclass, set_zzy_p_str) {
	zval *zzy_p_str;
	zend_class_entry *ce;
	ce = Z_OBJCE_P(getThis());	//获取当前对象
	//获取方法参数
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zzy_p_str) == FAILURE) {
		RETURN_NULL();
	}
	//更新属性值
	zend_update_property(ce,getThis(), "zzy_p_str", strlen("zzy_p_str"), zzy_p_str TSRMLS_CC);
}

//读属性
ZEND_METHOD(zzyclass, get_zzy_p_str) {
	zval *zzy_p_str;
	zend_class_entry *ce;
	ce = Z_OBJCE_P(getThis());
	zzy_p_str = zend_read_property(ce, getThis(), "zzy_p_str", strlen("zzy_p_str"), 0 TSRMLS_CC);
	RETURN_ZVAL(zzy_p_str, 0, 1);
}


//类方法表
static zend_function_entry zzyclass_method[] = {
	ZEND_ME(zzyclass, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	ZEND_ME(zzyclass, zzy_m_public, NULL, ZEND_ACC_PUBLIC)
	ZEND_ME(zzyclass, set_zzy_p_str, NULL, ZEND_ACC_PUBLIC)
	ZEND_ME(zzyclass, get_zzy_p_str, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

//接口方法表
static zend_function_entry zzyinterface_method[] = {
	ZEND_ABSTRACT_ME(zzyinterface, zzy_i_func1, NULL)
	{NULL, NULL, NULL}
};

ZEND_METHOD(zzyclass_chird, zzy_m_chird_func1) {
	php_printf("子类的方法，不能再父类调用\n");
}

ZEND_METHOD(zzyclass_chird, zzy_i_func1) {
	php_printf("实现接口的方法\n");
}

//子类方法
static zend_function_entry zzyclass_chird_method[] = {
	ZEND_ME(zzyclass_chird, zzy_m_chird_func1, NULL, ZEND_ACC_PUBLIC)
	ZEND_ME(zzyclass_chird, zzy_i_func1, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

//注册常量
zzy_init_constant() {
	zend_register_string_constant("ZZY_CONST", sizeof("ZZY_CONST"), "test", CONST_CS | CONST_PERSISTENT, NULL TSRMLS_CC);

}


PHP_MINIT_FUNCTION(zhangzy) {
	zzy_init_constant();	//初始化常量
	REGISTER_STRING_CONSTANT("ZZY_EXTVER", "0.1", CONST_CS | CONST_PERSISTENT);
	zend_class_entry ce, chird_ce, interface_ce;
	//初始化，类名为“"zzyclass"
	INIT_CLASS_ENTRY(ce, "zzyclass", zzyclass_method);
	zzyclass_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);

	//定义属性
	//默认为null
	zend_declare_property_null(zzyclass_ce, "zzy_p_null", strlen("zzy_p_null"), ZEND_ACC_PUBLIC TSRMLS_CC);
	//默认为false
	zend_declare_property_bool(zzyclass_ce, "zzy_p_bool", strlen("zzy_p_bool"), 0, ZEND_ACC_PUBLIC TSRMLS_CC);
	//默认为字符串
	zend_declare_property_stringl(zzyclass_ce, "zzy_p_str", strlen("zzy_p_str"), "default string", strlen("default string"), ZEND_ACC_PUBLIC TSRMLS_CC);
	//设置任意类型的属性
	zval *any;
	//创建数组
	MAKE_STD_ZVAL(any);
	//初始化数组
	array_init(any);
	//add_assoc_string(any, "a", "a", 0);

	//zend_declare_property(zzyclass_ce, "zzy_p_any", strlen("zzy_p_any"), Z_ARRVAL_P(any), ZEND_ACC_PUBLIC TSRMLS_CC);
	
	//接口
	INIT_CLASS_ENTRY(interface_ce, "zzyinterface", zzyinterface_method);
	zzyinterface_ce = zend_register_internal_interface(&interface_ce TSRMLS_CC);
	//定义一个子类
	INIT_CLASS_ENTRY(chird_ce, "zzyclass_chird", zzyclass_chird_method);
	//继承zzyclass类
	zzyclass_chird_ce = zend_register_internal_class_ex(&chird_ce, zzyclass_ce, "zzyclass" TSRMLS_CC);
	
	//实现zzyinterface接口
	zend_class_implements(zzyclass_chird_ce TSRMLS_CC, 1, zzyinterface_ce);

	return SUCCESS;
}

//声明一个zzyclass_chird对象并调用相关方法
ZEND_FUNCTION(zhangzy_obj) {
	zval *obj, *retval, *method;	//声明一个对象
	MAKE_STD_ZVAL(obj);	//申请内存
	MAKE_STD_ZVAL(retval);
	MAKE_STD_ZVAL(method);
	ZVAL_STRING(method, "zzy_m_chird_func1", 1);
	object_init_ex(obj, zzyclass_chird_ce);
	call_user_function(NULL, &obj, method, retval, 0, NULL TSRMLS_CC);
	zval_ptr_dtor(&obj);
	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&method);
	return;
}

//注册函数
static zend_function_entry zhangzy_functions[] = {
	ZEND_FE(zhangzy_hello, NULL)
	ZEND_FE(zhangzy_return, NULL)
	ZEND_FE(zhangzy_ref, NULL)
	ZEND_FE(zhangzy_param, NULL)
	ZEND_FE(zhangzy_param2, NULL)
	ZEND_FE(zhangzy_param3, NULL)
	ZEND_FE(zhangzy_count, NULL)
	ZEND_FE(zhangzy_array, NULL)
	ZEND_FE(zhangzy_vtok, NULL)
	ZEND_FE(zhangzy_obj, NULL)
	ZEND_FE(zhangzy_fopen, NULL)
	ZEND_NAMED_FE(zhangzy_hi, ZEND_FN(zhangzy_hello), NULL)	//zhangzy_hello 的别名
	{NULL, NULL, NULL}
};

//module_entry
zend_module_entry zhangzy_module_entry = {
	//#if ZEND_MODULE_API_NO >= 20010901
		STANDARD_MODULE_HEADER,
	//#endif
		"zhangzy",	//这是扩展名称，往往会使用一个宏
		zhangzy_functions,	//functions
		PHP_MINIT(zhangzy),	//MINIT	模块启动初始化
		NULL,	//MSHUTDOWN	模块结束
		NULL,	//RINIT	收到请求执行
		NULL,	//RSHUTDOWN	请求结束
		NULL,	//MINFO
	//#if ZEND_MODULE_API_NO >= 20110901
		"0.1",	//我们扩展版本
	//#endif
		STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_ZHANGZY
ZEND_GET_MODULE(zhangzy)
#endif

