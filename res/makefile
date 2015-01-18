click.dll:
	cl /EHsc /I"..\src" /I"E:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include" /MD click.cpp /link /DLL /LIBPATH:"E:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\lib;C:\Program Files (x86)\Windows Kits\8.1\Lib\winv6.3\um\x86" /out:click.dll

publish: click.dll
	copy *.json ..\bin\Debug\*.json
	copy *.dll ..\bin\Debug\*.dll
	copy *.lua ..\bin\Debug\*.lua

clean:
	del *.dll *.obj *~
	del ..\bin\Debug\*.json
# del ..\bin\Debug\*.dll
	del ..\bin\Debug\*.lua
