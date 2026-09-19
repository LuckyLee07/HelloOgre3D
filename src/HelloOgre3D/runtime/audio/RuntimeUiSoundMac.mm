#if defined(__APPLE__)
#import <AppKit/AppKit.h>

// One retained owner per voice. The delegate never touches C++/Lua state;
// completed NSSounds release immediately, and Stop clears the delegate first.
@interface HelloRuntimeSoundVoice : NSObject <NSSoundDelegate>
{
	NSSound* m_sound;
}
- (BOOL)startWithBytes:(const unsigned char*)bytes length:(NSUInteger)length volume:(float)volume;
- (BOOL)isPlaying;
- (void)setVolume:(float)volume;
- (void)stop;
@end

@implementation HelloRuntimeSoundVoice
- (BOOL)startWithBytes:(const unsigned char*)bytes length:(NSUInteger)length volume:(float)volume
{
	@synchronized(self)
	{
		NSData* data = [NSData dataWithBytes:bytes length:length];
		m_sound = [[NSSound alloc] initWithData:data];
		if (m_sound == nil) return NO;
		[m_sound setVolume:volume];
		[m_sound setDelegate:self];
		if (![m_sound play])
		{
			[self stop];
			return NO;
		}
		return YES;
	}
}
- (BOOL)isPlaying
{
	@synchronized(self)
	{
		return m_sound != nil && [m_sound isPlaying];
	}
}
- (void)setVolume:(float)volume
{
	@synchronized(self)
	{
		[m_sound setVolume:volume];
	}
}
- (void)stop
{
	@synchronized(self)
	{
		if (m_sound != nil)
		{
			[m_sound setDelegate:nil];
			[m_sound stop];
			[m_sound release];
			m_sound = nil;
		}
	}
}
- (void)sound:(NSSound*)sound didFinishPlaying:(BOOL)finished
{
	(void)finished;
	@synchronized(self)
	{
		if (sound == m_sound)
		{
			[m_sound setDelegate:nil];
			[m_sound autorelease];
			m_sound = nil;
		}
	}
}
- (void)dealloc
{
	[self stop];
	[super dealloc];
}
@end

bool RuntimeUiSoundMacPlay(void*& handle, const unsigned char* bytes, unsigned long size, float volume)
{
	@autoreleasepool
	{
		HelloRuntimeSoundVoice* voice = [[HelloRuntimeSoundVoice alloc] init];
		if (![voice startWithBytes:bytes length:static_cast<NSUInteger>(size) volume:volume])
		{
			[voice release];
			return false;
		}
		handle = voice;
		return true;
	}
}

bool RuntimeUiSoundMacIsPlaying(void* handle)
{
	@autoreleasepool
	{
		return [static_cast<HelloRuntimeSoundVoice*>(handle) isPlaying] == YES;
	}
}

void RuntimeUiSoundMacSetVolume(void* handle, float volume)
{
	@autoreleasepool
	{
		[static_cast<HelloRuntimeSoundVoice*>(handle) setVolume:volume];
	}
}

void RuntimeUiSoundMacStop(void*& handle)
{
	@autoreleasepool
	{
		HelloRuntimeSoundVoice* voice = static_cast<HelloRuntimeSoundVoice*>(handle);
		[voice stop];
		[voice release];
		handle = nullptr;
	}
}
#endif
