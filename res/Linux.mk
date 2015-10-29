DST = ../bin

%.so: %.cpp
	g++ -std=c++1y -shared -fPIC -o $*.so $*.cpp -I../src -L$(DST) -lViolet -lEditor -lGame

all: $(foreach TARGET, $(TARGETS), $(TARGET).so)

install: all
	cp *.json $(DST) 2>/dev/null || :
	sed -i 's/\.dll/\.so/g' $(DST)/*.json
	cp *.ttf $(DST) 2>/dev/null || :
	cp *.vert $(DST) 2>/dev/null || :
	cp *.frag $(DST) 2>/dev/null || :
	cp *.so $(DST) 2>/dev/null || :
	cp *.lua $(DST) 2>/dev/null || :
	cp *.png $(DST) 2>/dev/null || :

install-live: all
	cp *.json $(DST) 2>/dev/null || :
	cp *.ttf $(DST) 2>/dev/null || :
	cp *.so $(DST) 2>/dev/null || :
	cp *.lua $(DST) 2>/dev/null || :
	cp *.png $(DST) 2>/dev/null || :

clean:
	rm -f *.so *~ *.swp

uninstall:
	rm -f $(foreach TARGET, $(TARGETS), $(DST)/$(TARGET).so)
	rm -f $(DST)/*.lua
	rm -f $(DST)/*.json
	rm -f $(DST)/*.ttf
	rm -f $(DST)/*.vert
	rm -f $(DST)/*.frag
	rm -f $(DST)/*.png
