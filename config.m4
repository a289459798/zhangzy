PHP_ARG_ENABLE(zhangzy, 
	[Whether to enable the "zhangzy" extension],
	[enable-zhangzy		Enable "zhangzy" extension support])

if test $PHP_ZHANGZY != "no"; then
	PHP_SUBST(ZHANGZY_SHARED_LIBADD)
	PHP_NEW_EXTENSION(zhangzy, zhangzy.c, $ext_shared)
fi
