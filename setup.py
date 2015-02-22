import urllib.request, shutil, zipfile, os.path, os

wxwidget = "http://kaist.bplaced.net/tempest/libs/wxwidget.zip"
cryptopp = "http://kaist.bplaced.net/tempest/libs/cryptopp.zip"
pugixml = "http://kaist.bplaced.net/tempest/libs/pugixml.zip"

def downloadfile(url):
	filename = url.split('/')[-1]
	print("downloading " + filename + "...")
	with urllib.request.urlopen(url) as response, open('lib\\'+filename, 'wb') as out_file:
		shutil.copyfileobj(response, out_file)
	print("unzip " + filename + "...")
	unzip('lib\\' + filename, 'lib\\')
	
def unzip(source_filename, dest_dir):
	with zipfile.ZipFile(source_filename) as zf:
		for member in zf.infolist():
			words = member.filename.split('/')
			path = dest_dir
			for word in words[:-1]:
				drive, word = os.path.splitdrive(word)
				head, word = os.path.split(word)
				if word in (os.curdir, os.pardir, ''): continue
				path = os.path.join(path, word)
			zf.extract(member, path)
			
downloadfile(pugixml)
downloadfile(wxwidget)
downloadfile(cryptopp)
