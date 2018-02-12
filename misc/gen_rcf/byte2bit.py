#!/usr/bin/python
import sys, re, os
f=open(sys.argv[1], 'r')
file_str = f.readlines()
f.close()
str_32=''
str_bit=''
str0=''
str1=''
str2=''
str3=''
cnt=0
str_bit_t = ''
def charactertobit(a):
	if a=='0':
		bit_str = '0000'	
	elif a=='1':
		bit_str = '0001'	
	elif a=='2':
		bit_str = '0010'	
	elif a=='3':
		bit_str = '0011'	
	elif a=='4':
		bit_str = '0100'	
	elif a=='5':
		bit_str = '0101'	
	elif a=='6':
		bit_str = '0110'	
	elif a=='7':
		bit_str = '0111'	
	elif a=='8':
		bit_str = '1000'	
	elif a=='9':
		bit_str = '1001'	
	elif a=='a':
		bit_str = '1010'	
	elif a=='b':
		bit_str = '1011'	
	elif a=='c':
		bit_str = '1100'	
	elif a=='d':
		bit_str = '1101'	
	elif a=='e':
		bit_str = '1110'	
	else:
		bit_str = '1111'
	return bit_str
	
for line in file_str:
	if cnt ==0: str0=line.strip('\n')
	elif cnt ==1: str1=line.strip('\n')
	elif cnt ==2: str2=line.strip('\n')
	else:
		str3=line.strip('\n')
		str_32 = str_32+str3 + str2 + str1 + str0+'\n'
		str_temp = str3 + str2 + str1 + str0
		for str_a in str_temp:
			str_bit_t = str_bit_t + charactertobit(str_a)
		str_bit = str_bit + str_bit_t + '\n'
		str_bit_t = ''
	if cnt<3:
		cnt=cnt + 1
	else:
		cnt =0
#f=open('bootRom32.hex', 'w')
#f.write(str_32)
#f.close()

#f=open('bootRombit.hex', 'w')
f=open(sys.argv[2], 'w')
f.write(str_bit)
f.close()

