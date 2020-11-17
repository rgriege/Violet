#include "violet/core.h"
#include "violet/os.h"
#include "violet/array.h"
#include "violet/string.h"

#import <Foundation/Foundation.h>
#import <AppKit/NSOpenPanel.h>
#import <AppKit/NSSavePanel.h>
#import <AppKit/NSPopUpButton.h>

/* Unfortunately, we can't just call setAllowedFileTypes on the creation of the panel
 * like we can in other saner OS file panels. MacOS makes us handle it ourselves,
 * which results in all this callback stuff. */
@interface FileButtonHandler : NSObject {
	id panel;
	const file_dialog_filter_t *filters;
}
- (id)initWithPanel:(id)aPanel andFilters:(const file_dialog_filter_t*)filterList;
- (void)trigger:(id)sender;
@end

@implementation FileButtonHandler
- (id)initWithPanel:(id)aPanel andFilters:(const file_dialog_filter_t*)filterList
{
	self = [super init];
	panel = aPanel;
	filters = filterList;
	return self;
}

- (void)trigger:(id)sender
{
	const int idx = max([sender indexOfSelectedItem], 0);
	const file_dialog_filter_t *filter = &filters[idx];
	const char *pattern = filter->pattern;

	/* For some reason, the title doesn't update unless we call this. */
	[sender synchronizeTitleAndSelectedItem];

	/* NOTE(rgriege): this probably leaks, but it shouldn't happen often */
	NSMutableArray<NSString*> *file_types = [NSMutableArray arrayWithCapacity:1];
	/* MacOS also doesn't allow ';' delimeters, so we have to split them manually. */
	do {
		/* Don't include the "*." from the pattern - keep just the extension. */
		const char *extension = &pattern[2];
		const char *delim = strstr(extension, ";");
		const char *end = delim ? delim : extension + strlen(extension);
		const char *next = delim ? delim + 1 : extension + strlen(extension);
		const u32 len = (u32)(end - extension);
		log_error("adding filter %.*s", len, extension);
		[file_types addObject:[[NSString alloc] initWithBytes:extension length:len encoding:NSUTF8StringEncoding]];
		pattern = next;
	} while (*pattern != '\0');
	[panel setAllowedFileTypes:file_types];
}
@end

typedef struct file__ctx
{
	NSPopUpButton *button;
	FileButtonHandler *handler;
	NSView *accessoryView;
} file__ctx_t;

static
file__ctx_t file__setup_panel(id panel, const file_dialog_filter_t filters[], u32 num_filters)
{
	file__ctx_t ctx = {0};

	NSRect frame = NSMakeRect(0, 10, 300, 22);
	ctx.button = [[NSPopUpButton alloc] initWithFrame:frame pullsDown:NO];
	ctx.handler = [[FileButtonHandler alloc] initWithPanel:panel andFilters:filters];
	ctx.button.target = ctx.handler;
	ctx.button.action = @selector(trigger:);
	for (u32 i = 0; i < num_filters; ++i)
		[ctx.button addItemWithTitle:[NSString stringWithUTF8String:filters[i].name]];
	[ctx.handler trigger:ctx.button];

	NSRect viewFrame = NSMakeRect(0, 0, 300, 42);
	ctx.accessoryView = [[NSView alloc] initWithFrame:viewFrame];
	[ctx.accessoryView addSubview:ctx.button];
	[panel setAccessoryView:ctx.accessoryView];

	[panel setAllowsOtherFileTypes: NO];
	[panel setExtensionHidden: YES];

	return ctx;
}

static
void file__destroy_panel(file__ctx_t ctx)
{
	[ctx.handler release];
	[ctx.button release];
	[ctx.accessoryView release];
}

b32 file_open_dialog(char *fname, u32 fname_sz, const file_dialog_filter_t filters[], u32 num_filters)
{
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	file__ctx_t ctx = file__setup_panel(panel, filters, num_filters);

	b32 success = [panel runModal] == NSModalResponseOK
	           && [[[panel URLs] objectAtIndex:0] getFileSystemRepresentation:fname maxLength:fname_sz] == YES;

	file__destroy_panel(ctx);

	return success;
}

b32 file_save_dialog(char *fname, u32 fname_sz, const file_dialog_filter_t filters[], u32 num_filters)
{
	NSSavePanel *panel = [NSSavePanel savePanel];
	file__ctx_t ctx = file__setup_panel(panel, filters, num_filters);

	b32 success = [panel runModal] == NSModalResponseOK
	          && [[panel URL] getFileSystemRepresentation:fname maxLength:fname_sz] == YES;

	file__destroy_panel(ctx);

	return success;
}

char *imhomedir(void)
{
	return imstrcpy([NSHomeDirectory() UTF8String]);
}
