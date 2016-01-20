DST = ..\bin\Debug

include Targets.mk

%.dll: %.cpp
	cl /EHsc /I"..\src" /I"E:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\include" /MDd /ZI /DWIN32 /DVIOLET_SCRIPT $*.cpp /link /out:$*.dll /DLL /Debug "$(DST)\Violet.lib" "$(DST)\Editor.lib" /LIBPATH:"E:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\lib;C:\Program Files (x86)\Windows Kits\8.1\Lib\winv6.3\um\x86;$(DST)"

all: $(foreach TARGET, $(TARGETS), $(TARGET).dll)

install: all
	xcopy /c /y *.json $(DST)
	xcopy /c /y *.ttf $(DST)
	xcopy /c /y *.vert $(DST)
	xcopy /c /y *.frag $(DST)
	xcopy /c /y *.dll $(DST)
	xcopy /c /y *.pdb $(DST)
	xcopy /c /y *.lua $(DST)
	xcopy /c /y *.png $(DST)

install-live: all
	xcopy /c /y *.json $(DST)
	xcopy /c /y *.ttf $(DST)
	xcopy /c /y *.dll $(DST)
	xcopy /c /y *.pdb $(DST)/*.pdb.swp
	xcopy /c /y *.lua $(DST)
	xcopy /c /y *.png $(DST)

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
