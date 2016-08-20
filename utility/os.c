#include "violet/utility/os.h"

#ifdef _WIN32

#include <windows.h>
#include <shobjidl.h>

static
b8 _vlt_file_dialog(char *filename, u32 n, const char *ext, CLSID clsid, IID iid)
{
	b8 retval = false;

	if (!SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
		goto out;

	IFileDialog *dialog;
	if (!SUCCEEDED(CoCreateInstance(&clsid, NULL, CLSCTX_ALL, &iid, &dialog)))
		goto err_init;

	PWSTR psz_ext = malloc(sizeof(wchar_t)*n);
	mbstowcs(psz_ext, ext, n);
	if (!SUCCEEDED(dialog->lpVtbl->SetDefaultExtension(dialog, psz_ext)))
		goto err_ext;

	COMDLG_FILTERSPEC filters[1] = { { L"", psz_ext } };
	if (!SUCCEEDED(dialog->lpVtbl->SetFileTypes(dialog, 1, filters)))
		goto err_ext;

	if (!SUCCEEDED(dialog->lpVtbl->Show(dialog, NULL)))
		goto err_dlg;

	IShellItem *item;
	if (!SUCCEEDED(dialog->lpVtbl->GetResult(dialog, &item)))
		goto err_dlg;

	PWSTR psz_file_path;
	if (!SUCCEEDED(item->lpVtbl->GetDisplayName(item, SIGDN_FILESYSPATH, &psz_file_path)))
		goto err_itm;

	CoTaskMemFree(psz_file_path);
	wcstombs(filename, psz_file_path, n);
	retval = true;

err_itm:
	item->lpVtbl->Release(item);
err_dlg:
	dialog->lpVtbl->Release(dialog);
err_ext:
	free(psz_ext);
err_init:
	CoUninitialize();
out:
	return retval;
}

b8 vlt_file_open_dialog(char *filename, u32 n, const char *ext)
{
	return _vlt_file_dialog(filename, n, ext, CLSID_FileOpenDialog,
		IID_IFileOpenDialog);
}

b8 vlt_file_save_dialog(char *filename, u32 n, const char *ext)
{
	return _vlt_file_dialog(filename, n, ext, CLSID_FileSaveDialog,
		IID_IFileSaveDialog);
}

#endif
