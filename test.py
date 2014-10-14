import bitarray
import MySQLdb

conn = MySQLdb.connect(user='root', db='test')
cursor = conn.cursor()
cursor.execute("truncate ids")
for i in range(0, 1000, 2):
    cursor.execute("insert into ids values (%s)", (i,))
cursor.execute("select bitarray(id, 1000) from ids where id % 3 = 0 group by 'a'")
r = cursor.fetchone()
bitarray_bytes = r[0]

bits = bitarray.bitarray()
bits.frombytes(bitarray_bytes)

print len(bits)
print bits.any()
print bits.count()

