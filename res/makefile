DST = ..\bin\Debug
TARGETS = click paddle paddle2 reload quit

%.dll: %.cpp
	cl /EHsc /I"..\src" /I"E:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include" /MDd /DWIN32 $< /link /out:$@ /DLL /Debug "$(DST)\VioletPhysics.lib" "$(DST)\VioletCore.lib" /LIBPATH:"E:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\lib;C:\Program Files (x86)\Windows Kits\8.1\Lib\winv6.3\um\x86;$(DST)"

all: $(foreach TARGET, $(TARGETS), $(TARGET).dll )

install: all
	copy *.json $(DST)\*.json
	copy *.vert $(DST)\*.vert
	copy *.frag $(DST)\*.frag
	copy *.dll $(DST)\*.dll
	copy *.pdb $(DST)\*.pdb

install-live: all
	copy *.json $(DST)\*.json
	copy *.dll $(DST)\*.dll.swp
	copy *.pdb $(DST)\*.pdb.swp

clean:
	del *.dll *.obj *.lib *.ilk *.pdb *.exp *~ *.swp

uninstall:
	del $(foreach TARGET, $(TARGETS), $(DST)\$(TARGET).dll)
	del $(foreach TARGET, $(TARGETS), $(DST)\$(TARGET).pdb)
	del $(DST)\*.json
	del $(DST)\*.vert
	del $(DST)\*.frag
