src = $(wildcard *.cpp)
obj = $(src:.c=.o)

LDFLAGS = -L../lib/cryptopp -L../lib/poco/lib -lpthread -lcryptopp -lPocoFoundation -lPocoUtil -I../lib/cryptopp/include -I../lib/poco/include -I../lib/mtl -I../lib/pugixml/src

crypt: $(obj)
	$(CXX) -c $^ $(LDFLAGS) -std=c++14 -O2
	ar rcs ../delivery/libcrypt.a *.o

.PHONY: clean
clean:
	rm -f $(obj) crypt
