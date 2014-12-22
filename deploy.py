import os
import shutil
import glob

for e in glob.glob(r'delivery/include/*.h'):
	os.remove(e)
	
if os.path.exists(r'delivery/crypt.lib'):	
	os.remove(r'delivery/crypt.lib')

for h in glob.glob(r'crypt/*.h'):
	shutil.copy(h, r'delivery/include')
	
shutil.copy(r'x64/Release/crypt.lib', r'delivery')
