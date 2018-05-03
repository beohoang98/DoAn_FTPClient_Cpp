from time import sleep

for i in range(0, 10):
    for j in range(0, 10):
        sleep(0.01)
        print ("%3d"% (i*10 + j), sep=" ", end=" ", flush=True)
    print ("\r\n")