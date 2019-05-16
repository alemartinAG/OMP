from netCDF4 import Dataset
import matplotlib
import matplotlib.pyplot as plt
import numpy as np

g16nc = Dataset('OR_ABI-L2-CMIPF-M6C02_G16_s20191011800206_e20191011809514_c20191011809591.nc','r')
#g16nc = Dataset('out.nc', 'r')
datos = g16nc.variables['CMI'][:]
g16nc.close()
g16nc = None

fig = plt.figure(figsize=(10,10), dpi=5)
im = plt.imshow(datos, cmap='Greys_r')
plt.savefig('imagen.png',bbox_inches='tight',pad_inches = 0)