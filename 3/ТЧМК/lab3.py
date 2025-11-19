import time
a = 249
b = 36
zero = {'Q': 0, 'R': a, 'X': 1, 'Y': 0, 'i': 1}
first = {'Q': 0, 'R': b, 'X': 0, 'Y': 1, 'i': 2}

nod = []

nod.append(zero)
nod.append(first)

i = 0
start_time = time.time()
R = nod[i-1]['R'] % nod[i]['R']


while(R!=0):
    i+=1
    current = {'Q': nod[i-1]['R']//nod[i]['R'] , 'R': nod[i-1]['R'] % nod[i]['R'], 'X': nod[i-1]['X'] - nod[i]['Q']*nod[i]['X'], 'Y': nod[i-1]['Y'] - nod[i]['Q']*nod[i]['Y'], 'i': nod[i]['i']+1}
    nod.append(current)
    if(abs(nod[i]['R']) > abs(nod[i-1]['R']//2) and current['R']!=0):
        i+=1
        current = {'Q': nod[i-1]['R']//nod[i]['R'] , 'R': abs(nod[i]['R'] - nod[i-1]['R']), 'X': nod[i-1]['X'] - nod[i]['X'], 'Y': nod[i-1]['Y'] - nod[i]['Y'], 'i': nod[i]['i']+1}
        nod.append(current)
        R = abs(nod[i]['R'] - nod[i-1]['R'])
    else:
        R = nod[i-1]['R'] % nod[i]['R']
end_time = time.time()
nod.pop(len(nod)-1)
   

d = nod[i]['R']
x = nod[i]['X']
y = nod[i]['Y']
for i in range(len(nod)):
    print(nod[i])
print('\n'+'\n')
print('D = ', d)
print('X = ', x)
print('Y = ', y)
print('Time: ', 1000*(end_time-start_time), 'ms')