#if defined(__APPLE__)
#import <AppKit/AppKit.h>

// The runtime owns one retained NSSound; no Lua callback or native pointer escapes.
bool RuntimeUiSoundMacPlay(void*& handle, const unsigned char* bytes, unsigned long size)
{
	@autoreleasepool
	{
		NSData* data = [NSData dataWithBytes:bytes length:static_cast<NSUInteger>(size)];
		NSSound* sound = [[NSSound alloc] initWithData:data];
		if (sound == nil) return false;
		if (![sound play])
		{
			[sound release];
			return false;
		}
		handle = sound;
		return true;
	}
}

void RuntimeUiSoundMacStop(void*& handle)
{
	@autoreleasepool
	{
		NSSound* sound = static_cast<NSSound*>(handle);
		if (sound != nil)
		{
			[sound stop];
			[sound release];
		}
		handle = nullptr;
	}
}
#endif
