#include "violet/core.h"
#include "violet/os.h"
#include "violet/array.h"
#include "violet/string.h"

#import <Foundation/Foundation.h>
#import <AppKit/NSOpenPanel.h>
#import <AppKit/NSSavePanel.h>

b32 file_open_dialog(char *fname, u32 fname_sz, const file_dialog_filter_t filters[], u32 num_filters)
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];

	NSMutableArray<NSString*> *file_types = [NSMutableArray arrayWithCapacity:num_filters];
	for (u32 i = 0; i < num_filters; ++i)
		[file_types addObject:[NSString stringWithUTF8String:&filters[i].pattern[2]]];
	[panel setAllowedFileTypes:file_types];

	return [panel runModal] == NSModalResponseOK
	    && [[[panel URLs] objectAtIndex:0] getFileSystemRepresentation:fname maxLength:fname_sz] == YES;
}

b32 file_save_dialog(char *fname, u32 fname_sz, const file_dialog_filter_t filters[], u32 num_filters)
{
	NSSavePanel *panel = [NSSavePanel savePanel];

	NSMutableArray<NSString*> *file_types = [NSMutableArray arrayWithCapacity:num_filters];
	for (u32 i = 0; i < num_filters; ++i)
		[file_types addObject:[NSString stringWithUTF8String:&filters[i].pattern[2]]];
	[panel setAllowedFileTypes:file_types];

	return [panel runModal] == NSModalResponseOK
	    && [[panel URL] getFileSystemRepresentation:fname maxLength:fname_sz] == YES;
}

char *imhomedir(void)
{
	return imstrcpy([NSHomeDirectory() UTF8String]);
}
