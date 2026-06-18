#include "systems/input/InputManager.h"

#if defined(OIS_APPLE_PLATFORM)

#include "OgreRenderWindow.h"

#import <AppKit/AppKit.h>
#include <math.h>

namespace
{
	int GetMouseButtonMask(OIS::MouseButtonID button)
	{
		return 1 << static_cast<int>(button);
	}

	bool IsMouseButtonUpEvent(NSEventType type)
	{
		return type == NSEventTypeLeftMouseUp ||
			type == NSEventTypeRightMouseUp ||
			type == NSEventTypeOtherMouseUp;
	}

	OIS::MouseButtonID ToOisMouseButton(NSEvent* event)
	{
		NSInteger button = [event buttonNumber];
		if (button == 0)
			return OIS::MB_Left;
		if (button == 1)
			return OIS::MB_Right;
		if (button == 2)
			return OIS::MB_Middle;
		if (button < OIS::MB_Left)
			button = OIS::MB_Left;
		if (button > OIS::MB_Button7)
			button = OIS::MB_Button7;
		return static_cast<OIS::MouseButtonID>(button);
	}
}

@interface HelloOgreNativeMouseBridge : NSObject
{
@private
	InputManager* _manager;
	NSView* _view;
	id _monitor;
	int _buttons;
	int _lastRawX;
	int _lastRawY;
	BOOL _hasLastPosition;
}

- (id)initWithInputManager:(InputManager*)manager view:(NSView*)view;
- (void)invalidate;
- (void)handleEvent:(NSEvent*)event;

@end

@implementation HelloOgreNativeMouseBridge

- (id)initWithInputManager:(InputManager*)manager view:(NSView*)view
{
	self = [super init];
	if (self)
	{
		_manager = manager;
		_view = [view retain];
		_buttons = 0;
		_lastRawX = 0;
		_lastRawY = 0;
		_hasLastPosition = NO;

		NSWindow* window = [_view window];
		[window setAcceptsMouseMovedEvents:YES];

		NSEventMask mask =
			NSEventMaskMouseMoved |
			NSEventMaskLeftMouseDown |
			NSEventMaskLeftMouseUp |
			NSEventMaskRightMouseDown |
			NSEventMaskRightMouseUp |
			NSEventMaskOtherMouseDown |
			NSEventMaskOtherMouseUp |
			NSEventMaskLeftMouseDragged |
			NSEventMaskRightMouseDragged |
			NSEventMaskOtherMouseDragged |
			NSEventMaskScrollWheel;

		HelloOgreNativeMouseBridge* bridge = self;
		_monitor = [[NSEvent addLocalMonitorForEventsMatchingMask:mask handler:^NSEvent* (NSEvent* event) {
			[bridge handleEvent:event];
			return event;
		}] retain];
	}
	return self;
}

- (void)dealloc
{
	[self invalidate];
	[_view release];
	[super dealloc];
}

- (void)invalidate
{
	if (_monitor != nil)
	{
		[NSEvent removeMonitor:_monitor];
		[_monitor release];
		_monitor = nil;
	}
	_manager = nullptr;
}

- (BOOL)getMousePositionFromEvent:(NSEvent*)event rawX:(int*)rawX rawY:(int*)rawY relX:(int*)relX relY:(int*)relY inside:(BOOL*)inside
{
	if (_view == nil || [_view window] == nil || [event window] != [_view window])
		return NO;

	NSRect bounds = [_view bounds];
	NSPoint point = [_view convertPoint:[event locationInWindow] fromView:nil];
	const BOOL pointInside = NSPointInRect(point, bounds);

	CGFloat scaleX = 1.0f;
	CGFloat scaleY = 1.0f;
	if ([_view respondsToSelector:@selector(convertSizeToBacking:)])
	{
		NSSize scale = [_view convertSizeToBacking:NSMakeSize(1.0f, 1.0f)];
		scaleX = scale.width;
		scaleY = scale.height;
	}
	if (scaleX <= 0.0f || scaleY <= 0.0f)
	{
		CGFloat backingScale = [[_view window] backingScaleFactor];
		if (backingScale <= 0.0f)
			backingScale = 1.0f;
		scaleX = backingScale;
		scaleY = backingScale;
	}

	const int x = static_cast<int>(lround((point.x - NSMinX(bounds)) * scaleX));
	const int y = static_cast<int>(lround((NSMaxY(bounds) - point.y) * scaleY));

	*rawX = x;
	*rawY = y;
	*relX = _hasLastPosition ? x - _lastRawX : 0;
	*relY = _hasLastPosition ? y - _lastRawY : 0;
	*inside = pointInside;

	_lastRawX = x;
	_lastRawY = y;
	_hasLastPosition = YES;
	return YES;
}

- (int)wheelDeltaFromEvent:(NSEvent*)event
{
	CGFloat delta = [event scrollingDeltaY];
	if (delta == 0.0f)
		delta = [event deltaY];
	if (delta == 0.0f)
		return 0;

	if ([event hasPreciseScrollingDeltas])
		return static_cast<int>(lround(delta * 10.0f));

	return static_cast<int>(lround(delta * 120.0f));
}

- (void)handleEvent:(NSEvent*)event
{
	if (_manager == nullptr)
		return;

	int rawX = 0;
	int rawY = 0;
	int relX = 0;
	int relY = 0;
	BOOL inside = NO;
	if (![self getMousePositionFromEvent:event rawX:&rawX rawY:&rawY relX:&relX relY:&relY inside:&inside])
		return;

	const NSEventType type = [event type];
	if (!inside && _buttons == 0 && !IsMouseButtonUpEvent(type))
		return;

	switch (type)
	{
	case NSEventTypeLeftMouseDown:
	case NSEventTypeRightMouseDown:
	case NSEventTypeOtherMouseDown:
	{
		const OIS::MouseButtonID button = ToOisMouseButton(event);
		_buttons |= GetMouseButtonMask(button);
		_manager->HandleNativeMouseButton(rawX, rawY, relX, relY, button, true, _buttons);
		break;
	}
	case NSEventTypeLeftMouseUp:
	case NSEventTypeRightMouseUp:
	case NSEventTypeOtherMouseUp:
	{
		const OIS::MouseButtonID button = ToOisMouseButton(event);
		_buttons &= ~GetMouseButtonMask(button);
		_manager->HandleNativeMouseButton(rawX, rawY, relX, relY, button, false, _buttons);
		break;
	}
	case NSEventTypeScrollWheel:
	{
		const int wheelDelta = [self wheelDeltaFromEvent:event];
		if (wheelDelta != 0)
			_manager->HandleNativeMouseMove(rawX, rawY, relX, relY, wheelDelta, _buttons);
		break;
	}
	default:
		_manager->HandleNativeMouseMove(rawX, rawY, relX, relY, 0, _buttons);
		break;
	}
}

@end

void InputManager::InstallNativeMouseBridge()
{
	if (m_nativeMouseBridge != nullptr)
		return;

	if (m_renderWindow == nullptr)
		return;

	void* viewPtr = nullptr;
	m_renderWindow->getCustomAttribute("VIEW", &viewPtr);
	NSView* view = static_cast<NSView*>(viewPtr);
	if (view == nil)
		return;

	m_nativeMouseBridge = [[HelloOgreNativeMouseBridge alloc] initWithInputManager:this view:view];
}

void InputManager::UninstallNativeMouseBridge()
{
	if (m_nativeMouseBridge == nullptr)
		return;

	HelloOgreNativeMouseBridge* bridge = static_cast<HelloOgreNativeMouseBridge*>(m_nativeMouseBridge);
	[bridge invalidate];
	[bridge release];
	m_nativeMouseBridge = nullptr;
}

#endif
