DST = ..\bin\Debug

%.dll:
	cl /EHsc /I"..\src" /I"E:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include" /MDd /DWIN32 /DVIOLET_SCRIPT $*.cpp /link /out:$*.dll /DLL /Debug "$(DST)\Engine.lib" "$(DST)\Editor.lib" "$(DST)\Game.lib" /LIBPATH:"E:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\lib;C:\Program Files (x86)\Windows Kits\8.1\Lib\winv6.3\um\x86;$(DST)"

TARGETS = button contextMenu entityList

all: $(foreach TARGET, $(TARGETS), $(TARGET).dll)

install: all
	copy *.json $(DST)
	copy *.ttf $(DST)
	copy *.vert $(DST)
	copy *.frag $(DST)
	copy *.dll $(DST)
	copy *.pdb $(DST)
	copy *.lua $(DST)
	copy *.png $(DST)

install-live: all
	copy *.json $(DST)
	copy *.ttf $(DST)
	copy *.dll $(DST)
	copy *.pdb $(DST)/*.pdb.swp
	copy *.lua $(DST)
	copy *.png $(DST)

clean:
	del *.dll *.obj *.lib *.ilk *.pdb *.exp *~ *.swp

uninstall:
	del $(foreach TARGET, $(TARGETS), $(DST)\$(TARGET).dll)
	del $(foreach TARGET, $(TARGETS), $(DST)\$(TARGET).pdb)
	del $(DST)\*.lua
	del $(DST)\*.json
	del $(DST)\*.ttf
	del $(DST)\*.vert
	del $(DST)\*.frag
	del $(DST)\*.png
