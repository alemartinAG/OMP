import numpy as np
import scipy.misc as smp
import base64

# Create a 1024x1024x3 array of 8 bit unsigned integers
dataRGB = np.zeros((5424,21696,3), dtype=np.float)

#data[512,512] = [254,0,0]       # Makes the middle pixel red
#data[512,513] = [0,0,255]       # Makes the next pixel blue

#for i in range(0, 1024):
#	for j in range(0, 1024):
#		data[i,j] = [(255/1024)*i,(255/1024)*j,(255/2048)*(i+j)]

#dtype = np.dtype([("bright", np.float32)])
#dt = np.dtype([("bright", [("a", float), ("b", float)])])
y = 21696
x = 21696/4

dt = np.dtype(np.float)

file = open("parte02", "rb")

data = np.fromfile(file, dt)
data.shape = (data.size//-1, 21696)

#for i in range(0, 5424):
	#for j in range(0, 21696):
		#dataRGB[i,j] = [data[j+(5424*i)], 0, 0]
		#print(data[i,j])

img = smp.toimage(data)       	# Create a PIL image
#img.show()                      # View in default viewer
smp.imsave('magenjajen222.png', img)