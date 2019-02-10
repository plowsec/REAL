passwd="1234"
key = 0xfc
password=[]
for i in passwd:
    password.append(str(ord(i)^key))
print(",".join(password))
