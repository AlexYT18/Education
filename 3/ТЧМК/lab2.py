import time
a = 510310626446768116296726339876905441011
b = 1019730023910830418627227565271959261797
zero = {'U': a, 'V': b, 'A': 1, 'B': 0, 'C': 0, 'D': 1, 'i': 0}
nod = []
nod.append(zero)
g = 1
i = 0

start_time = time.time()

while(a % 2 == 0 and b % 2 == 0):
    i+=1
    a = a // 2
    b = b // 2
    g*=2
    current = {'U': a, 'V': b, 'A': 1, 'B': 0, 'C': 0, 'D': 1, 'i': i}
    nod.append(current)  

while(nod[i]['U'] != 0):
    while(nod[i]['U'] % 2 == 0):
        i+=1
        current = {'U': nod[i-1]['U']//2, 'V': nod[i-1]['V'], 'A': nod[i-1]['A'], 'B': nod[i-1]['B'], 'C': nod[i-1]['C'], 'D': nod[i-1]['D'], 'i': nod[i-1]['i'] + 1}
        nod.append(current)
        if(nod[i]['A'] % 2 == 0 and nod[i]['B'] % 2 == 0):
            i+=1
            current = {'U': nod[i-1]['U'], 'V': nod[i-1]['V'], 'A': nod[i-1]['A']//2, 'B': nod[i-1]['B']//2, 'C': nod[i-1]['C'], 'D': nod[i-1]['D'], 'i': nod[i-1]['i'] + 1}
            nod.append(current)
        else:
            i+=1
            current = {'U': nod[i-1]['U'], 'V': nod[i-1]['V'], 'A': (nod[i-1]['A'] + b)//2, 'B': (nod[i-1]['B'] - a)//2, 'C': nod[i-1]['C'], 'D': nod[i-1]['D'], 'i': nod[i-1]['i'] + 1}
            nod.append(current)
    while(nod[i]['V'] % 2 == 0):
        i+=1
        current = {'U': nod[i-1]['U'], 'V': nod[i-1]['V']//2, 'A': nod[i-1]['A'], 'B': nod[i-1]['B'], 'C': nod[i-1]['C'], 'D': nod[i-1]['D'], 'i': nod[i-1]['i'] + 1}
        nod.append(current)
        if(nod[i]['C'] % 2 == 0 and nod[i]['D'] % 2 == 0):
            i+=1
            current = {'U': nod[i-1]['U'], 'V': nod[i-1]['V'], 'A': nod[i-1]['A'], 'B': nod[i-1]['B'], 'C': nod[i-1]['C']//2, 'D': nod[i-1]['D']//2, 'i': nod[i-1]['i'] + 1}
            nod.append(current)
        else:
            i+=1
            current = {'U': nod[i-1]['U'], 'V': nod[i-1]['V'], 'A': nod[i-1]['A'], 'B': nod[i-1]['B'], 'C': (nod[i-1]['C'] + b)//2, 'D': (nod[i-1]['D'] - a)//2, 'i': nod[i-1]['i'] + 1}
            nod.append(current)
    if(nod[i]['U']>=nod[i]['V']):
        i+=1
        current = {'U': nod[i-1]['U'] - nod[i-1]['V'], 'V': nod[i-1]['V'], 'A': nod[i-1]['A'] - nod[i-1]['C'], 'B': nod[i-1]['B'] - nod[i-1]['D'], 'C': nod[i-1]['C'], 'D': nod[i-1]['D'], 'i': nod[i-1]['i'] + 1}
        nod.append(current)
    else:
        i+=1
        current = {'U': nod[i-1]['U'], 'V': nod[i-1]['V'] - nod[i-1]['U'], 'A': nod[i-1]['A'], 'B': nod[i-1]['B'], 'C': nod[i-1]['C'] - nod[i-1]['A'], 'D': nod[i-1]['D'] - nod[i-1]['B'], 'i': nod[i-1]['i'] + 1}
        nod.append(current)
end_time = time.time()  

d = g*nod[i]['V']
x = nod[i]['C']
y = nod[i]['D']

for i in range(len(nod)):
    print(nod[i])
print('\n'+'\n')
print('D = ', d)
print('X = ', x)
print('Y = ', y)
print('Time: ', 1000*(end_time-start_time), 'ms')