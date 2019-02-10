 # -*-coding:utf-8 -*
bytes =[0x5c,0x54,0x50,0x4c,0x48,0x44,0x40,0x3c,0x38,0x34,0x30,0x2c,0x28,0x24,0x20,0x1c]
key = 0xAC70
password=[]
for i in bytes:
    password.append(hex(i^key))
shellcode = ''.join(password)
print(shellcode.upper().replace("0X","\\x"))
#n=2
#shellcode=',\\x'.join([password[i:i+n] for i in range(0,len(password),n)])
#shellcode = '\\x'+shellcode
#print(shellcode)
#bis='x'.join([password[i:i+n] for i in range(0,len(password),n)])
#bis= 'x'+bis
#print(bis)
