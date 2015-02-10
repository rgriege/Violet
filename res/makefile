click.dll:
	cl /EHsc /I"..\src" /I"E:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include" /MDd /DWIN32 click.cpp /link /out:click.dll /DLL /Debug "E:\Documents\alottabit\violet\bin\Debug\VioletPhysics.lib" "E:\Documents\alottabit\violet\bin\Debug\VioletCore.lib" /LIBPATH:"E:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\lib;C:\Program Files (x86)\Windows Kits\8.1\Lib\winv6.3\um\x86;E:\Documents\alottabit\violet\bin\Debug"

paddle.dll:
	cl /EHsc /I"..\src" /I"E:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include" /MDd /DWIN32 paddle.cpp /link /out:paddle.dll /DLL /Debug "E:\Documents\alottabit\violet\bin\Debug\VioletPhysics.lib" "E:\Documents\alottabit\violet\bin\Debug\VioletCore.lib" /LIBPATH:"E:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\lib;C:\Program Files (x86)\Windows Kits\8.1\Lib\winv6.3\um\x86;E:\Documents\alottabit\violet\bin\Debug"

paddle2.dll:
	cl /EHsc /I"..\src" /I"E:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include" /MDd /DWIN32 paddle2.cpp /link /out:paddle2.dll /DLL "E:\Documents\alottabit\violet\bin\Debug\VioletPhysics.lib" "E:\Documents\alottabit\violet\bin\Debug\VioletCore.lib" /LIBPATH:"E:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\lib;C:\Program Files (x86)\Windows Kits\8.1\Lib\winv6.3\um\x86;E:\Documents\alottabit\violet\bin\Debug"

reload.dll:
	cl /EHsc /I"..\src" /I"E:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\include" /MDd /DWIN32 reload.cpp /link /out:reload.dll /DLL /Debug "E:\Documents\alottabit\violet\bin\Debug\VioletPhysics.lib" "E:\Documents\alottabit\violet\bin\Debug\VioletCore.lib" /LIBPATH:"E:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\lib;C:\Program Files (x86)\Windows Kits\8.1\Lib\winv6.3\um\x86;E:\Documents\alottabit\violet\bin\Debug"

install: click.dll paddle.dll paddle2.dll reload.dll
	copy *.json ..\bin\Debug\*.json
	copy *.dll ..\bin\Debug\*.dll
	copy *.pdb ..\bin\Debug\*.pdb

install-live: click.dll paddle.dll paddle2.dll reload.dll
	copy *.json ..\bin\Debug\*.json
	copy *.dll ..\bin\Debug\*.dll.swp
	copy *.pdb ..\bin\Debug\*.pdb.swp

clean:
	del *.dll *.obj *.lib *.ilk *.pdb *.exp *~

uninstall:
	del ..\bin\Debug\*.json
	del ..\bin\Debug\click.dll
	del ..\bin\Debug\click.pdb
	del ..\bin\Debug\paddle.dll
	del ..\bin\Debug\paddle.pdb
	del ..\bin\Debug\paddle2.dll
	del ..\bin\Debug\paddle2.pdb
	del ..\bin\Debug\reload.dll
	del ..\bin\Debug\reload.pdb
	del ..\bin\Debug\*.dll.swp
	del ..\bin\Debug\*.pdb.swp
