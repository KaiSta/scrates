src = $(wildcard *.cpp)
obj = $(src:.c=.o)

LDFLAGS = -L../lib/poco/lib -L../delivery -lpthread -lcryptopp -lPocoFoundation -lPocoUtil -lcrypt -I../lib/poco/include -I../lib/mtl -I../delivery/include

test: $(obj)
	$(CXX) -o $@ $^ $(LDFLAGS) -std=c++14 -O2

.PHONY: clean
clean:
	rm -f $(obj) test
	rm *.o
