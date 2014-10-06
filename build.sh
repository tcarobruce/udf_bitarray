gcc -Wall $(mysql_config --include) -I /usr/local/include -c udf_bitarray.c -o udf_bitarray.o && 
gcc -shared -o udf_bitarray.so udf_bitarray.o && 
cp udf_bitarray.so $(mysql_config --plugindir) &&
mysql -u root test < udf_bitarray.sql
