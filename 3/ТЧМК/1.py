import time

with open('input.txt') as Input_File:
    Num1 =  int(Input_File.readline())
    Num2 = int(Input_File.readline())
    g=1
    if(Num1<0 and Num2<0):
        Num1, Num2 = -Num1, -Num2
        g=-1
    if(Num1<0):
        Num1=-Num1
        g=-1
    if(Num2<0):
        Num2=-Num2
        g=-1
    if(Num2>=Num1 and Num2>0 and Num1>0):
        Num1,Num2=Num2,Num1
flag=1
def Euclid():
    if(flag):print("Р Р°СЃС€РёСЂРµРЅРЅС‹Р№ Р°Р»РіРѕСЂРёС‚Рј Р•РІРєР»РёРґР°")
    r1, r2, x1, x2 = Num1, Num2, 1, 0
    y1, y2, i, q2 =  0, 1, 0, Num1//Num2
    
    if(flag):print(f"РЁР°Рі: 0; r: {r1}; x: {x1}; y: {y1};")
    if(flag):print(f"РЁР°Рі: 1; r: {r2}; x: {x2}; y: {y2}; q: {q2};")
    while(1):
        if(r2==0): break
        r1, r2=r2, r1%r2
        x1, x2=x2, x1-q2*x2
        y1, y2=y2, y1-q2*y2
        if(r2!=0): q2=r1//r2
        if(flag):print(f"РЁР°Рі: {i+2}; r: {r2}; x: {x2}; y: {y2}; q: {q2};")
        i+=1
    if((x1*Num1+y1*Num2)==r1): 
        if(flag):print(f"РќРћР”({Num1}; {Num2}) = {r1*g}; x = {x1}; y = {y1};")
    else: 
        if(flag):print("РћС€РёР±РєР° РІ Р°Р»РіРѕСЂРёС‚РјРµ.")

def Binary_Euclid():
    if(flag):print("Р Р°СЃС€РёСЂРµРЅРЅС‹Р№ Р±РёРЅР°СЂРЅС‹Р№ Р°Р»РіРѕСЂРёС‚Рј Р•РІРєР»РёРґР°")
    a, b = Num1, Num2
    d = 1
    while a & 1 == 0 and b & 1 == 0:
        a=a>>1
        b=b>>1
        d=d*2
    r1, r2, x1, x2 = a, b, 1, 0
    y1, y2 = 0, 1
    i=2
    if(flag):print(f"РЁР°Рі: 0; r: {r1}; x: {x1}; y: {y1}")
    if(flag):print(f"РЁР°Рі: 1; r: {r2}; x: {x2}; y: {y2}")
    while(r1!=0):
        while r1 & 1 == 0:
            r1 = r1 >> 1
            if x1 & 1 == 0 and y1& 1 == 0:
                x1, y1 = x1 >> 1, y1 >> 1
            else:
                x1, y1 = (x1 + b) >> 1, (y1 - a) >> 1
        while r2 & 1 == 0:
            r2 = r2 >> 1
            if x2 & 1 == 0 and y2 & 1 == 0:
                x2, y2 = x2 >> 1, y2 >> 1
            else:
                x2, y2 = (x2 + b) >> 1, (y2 - a) >> 1
        if abs(r1) >= abs(r2):
            r1, x1, y1 = r1 - r2, x1 - x2, y1 - y2
        else:
            r2, x2, y2 = r2 - r1, x2 - x1, y2 - y1
        if(flag):print(f"РЁР°Рі: {i}; r: {r2*d}; x: {x2}; y: {y2}")
        i+=1
    if((x2*Num1+y2*Num2)==r2*d): 
        if(flag):print(f"РќРћР”({Num1}; {Num2}) = {r2*d*g}; x = {x2}; y = {y2};")
    else:  
        if(flag):print("РћС€РёР±РєР° РІ Р°Р»РіРѕСЂРёС‚РјРµ.")

def Remains():
    if(flag):print("Р Р°СЃС€РёСЂРµРЅРЅС‹Р№ Р°Р»РіРѕСЂРёС‚Рј Р•РІРєР»РёРґР° СЃ СѓСЃРµС‡РµРЅРЅС‹РјРё РѕСЃС‚Р°С‚РєР°РјРё")
    i=2
    r1, r2, x1, x2 = Num1, Num2, 1, 0
    y1, y2, q1, q2 =  0, 1, 0, Num1//Num2
    if(flag):print(f"РЁР°Рі: {0}; r: {r1}; x: {x1}; y: {y1};")
    if(flag):print(f"РЁР°Рі: {1}; r: {r2}; x: {x2}; y: {y2}; q: {q2};")
    while(r2!=0):
        q1=r1//r2
        r1, r2=r2, r1%r2
        if(r2!=0): q2=r1//r2
        x1, x2=x2, x1-q1*x2
        y1, y2=y2, y1-q1*y2
        if (r2 > r1 / 2): 
            r2 = r1 - r2
            x2, y2 = x1 - x2, y1 - y2
        if(flag):print(f"РЁР°Рі: {i}; r: {r2}; x: {x2}; y: {y2}; q: {q2};")
        i+=1
    if((x1*Num1+y1*Num2)==r1): 
        if(flag):print(f"РќРћР”({Num1}; {Num2}) = {r1*g}; x = {x1}; y = {y1};")
    else: 
        if(flag):print("РћС€РёР±РєР° РІ Р°Р»РіРѕСЂРёС‚РјРµ.")


Time = time.time()

for i in range(100):
    Euclid()
    flag=0
Time = time.time() - Time
Time=Time/100
print(f"Р’СЂРµРјСЏ СЂР°Р±РѕС‚С‹ Р°Р»РіРѕСЂРёС‚РјР°: {Time}")
flag=1
Time = time.time()
for i in range(100):
    Binary_Euclid()
    flag=0
Time = time.time() - Time
Time=Time/100
print(f"Р’СЂРµРјСЏ СЂР°Р±РѕС‚С‹ Р°Р»РіРѕСЂРёС‚РјР°: {Time}")
flag=1
Time = time.time()
for i in range(100):
    Remains()
    flag=0
Time = time.time() - Time
Time=Time/100
print(f"Р’СЂРµРјСЏ СЂР°Р±РѕС‚С‹ Р°Р»РіРѕСЂРёС‚РјР°: {Time}")