import os
import shutil
import glob

for e in glob.glob(r'delivery/include/*.h'):
	os.remove(e)
	
if os.path.exists(r'delivery/cryptd.lib'):	
	os.remove(r'delivery/cryptd.lib')

for h in glob.glob(r'crypt/*.h'):
	shutil.copy(h, r'delivery/include')
	
shutil.copy(r'x64/Debug/cryptd.lib', r'delivery')
