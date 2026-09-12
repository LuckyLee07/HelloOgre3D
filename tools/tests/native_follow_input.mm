// Compile from the repository root (macOS):
// xcrun clang++ -std=c++17 -fno-objc-arc -Isrc/HelloOgre3D/sandbox
// -Isrc/external/ois/includes -Isrc/Engine/ogre3d/include
// tools/tests/native_follow_input.mm -framework AppKit -o /tmp/native_follow_input
// Exercises the production Cocoa bridge with local events; no OS input is posted.
#include "../../src/HelloOgre3D/sandbox/systems/input/InputManagerMac.mm"
#include <cassert>
#include <cstdio>

namespace
{
	struct Motion { int x, y, dx, dy, wheel, buttons; };
	std::vector<Motion> motions;
	int keyResets = 0, releases = 0;
	bool follow = true;
}

// Listener test doubles isolate native conversion from the game/renderer.
InputManager::InputManager(Ogre::RenderWindow*, OgreCameraController*, const WindowStateSetter&,
	const WindowStateSetter&) : m_renderWindow(nullptr), m_nativeMouseBridge(nullptr) {}
InputManager::~InputManager() {}
bool InputManager::IsFollowCamera() const { return follow; }
void InputManager::ResetHeldKeys() { ++keyResets; }
void InputManager::HandleNativeMouseMove(int x, int y, int dx, int dy, int wheel, int buttons)
{ motions.push_back({x, y, dx, dy, wheel, buttons}); }
void InputManager::HandleNativeMouseButton(int, int, int, int, OIS::MouseButtonID, bool down, int)
{ if (!down) ++releases; }
bool InputManager::keyPressed(const OIS::KeyEvent&) { return true; }
bool InputManager::keyReleased(const OIS::KeyEvent&) { return true; }
bool InputManager::mouseMoved(const OIS::MouseEvent&) { return true; }
bool InputManager::mousePressed(const OIS::MouseEvent&, OIS::MouseButtonID) { return true; }
bool InputManager::mouseReleased(const OIS::MouseEvent&, OIS::MouseButtonID) { return true; }

@interface ProbeView : NSView { @public CGFloat backing; }
@end
@implementation ProbeView
- (NSSize)convertSizeToBacking:(NSSize)size
{ return NSMakeSize(size.width * backing, size.height * backing); }
@end

// Deliberately hold the absolute pointer still: relative look must keep working
// at a screen edge, and its totals must not depend on absolute UI coordinates.
@interface ProbeEvent : NSObject
{
@public
	NSWindow* win;
	NSEventType kind;
	CGFloat dx, dy;
}
@end
@implementation ProbeEvent
- (NSWindow*)window { return win; }
- (NSPoint)locationInWindow { return NSMakePoint(100, 100); }
- (NSEventType)type { return kind; }
- (NSInteger)buttonNumber { return 2; }
- (CGFloat)deltaX { return dx; }
- (CGFloat)deltaY { return dy; }
- (CGFloat)scrollingDeltaY { return dy; }
- (BOOL)hasPreciseScrollingDeltas { return YES; }
@end

static void Send(HelloOgreNativeMouseBridge* bridge, ProbeEvent* event,
	NSEventType type, CGFloat x = 0, CGFloat y = 0)
{
	event->kind = type; event->dx = x; event->dy = y;
	[bridge handleEvent:(NSEvent*)event];
}

int main()
{
	@autoreleasepool
	{
		[NSApplication sharedApplication];
		InputManager manager(nullptr, nullptr, {}, {});
		for (int scale : {1, 2}) for (int width : {960, 1280, 1920})
		{
			NSWindow* window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, width, 720)
				styleMask:NSWindowStyleMaskBorderless backing:NSBackingStoreBuffered defer:NO];
			[window setReleasedWhenClosed:NO];
			ProbeView* view = [[ProbeView alloc] initWithFrame:NSMakeRect(0, 0, width, 720)];
			view->backing = scale;
			[window setContentView:view];
			HelloOgreNativeMouseBridge* bridge = [[HelloOgreNativeMouseBridge alloc] initWithInputManager:&manager view:view];
			ProbeEvent* event = [ProbeEvent new]; event->win = window;
			motions.clear(); follow = true;
			Send(bridge, event, NSEventTypeOtherMouseDown);
			for (int i = 0; i < 400; ++i) Send(bridge, event, NSEventTypeOtherMouseDragged, .25, .125);
			int sumX = 0, sumY = 0;
			for (const Motion& m : motions) { sumX += m.dx; sumY += m.dy; }
			assert(sumX == 100 && sumY == 50);
			assert(motions.back().x == 100 * scale && motions.back().y == 620 * scale);
			for (int i = 0; i < 400; ++i) Send(bridge, event, NSEventTypeOtherMouseDragged, -.25, -.125);
			sumX = sumY = 0;
			for (const Motion& m : motions) { sumX += m.dx; sumY += m.dy; }
			assert(sumX == 0 && sumY == 0);
			Send(bridge, event, NSEventTypeScrollWheel, 100, 1);
			assert(motions.back().dx == 0 && motions.back().dy == 0 && motions.back().wheel == 10);

			Send(bridge, event, NSEventTypeOtherMouseDragged, .75, .75);
			Send(bridge, event, NSEventTypeOtherMouseUp);
			Send(bridge, event, NSEventTypeOtherMouseDown);
			Send(bridge, event, NSEventTypeOtherMouseDragged, .25, .25);
			assert(motions.back().dx == 0 && motions.back().dy == 0);
			const int beforeKeys = keyResets, beforeReleases = releases;
			[[NSNotificationCenter defaultCenter] postNotificationName:NSWindowDidResignKeyNotification object:window];
			assert(keyResets == beforeKeys + 1 && releases == beforeReleases + 1);
			Send(bridge, event, NSEventTypeOtherMouseDragged, 20, 20);
			assert(motions.back().buttons == 0 && motions.back().dx == 0);
			Send(bridge, event, NSEventTypeOtherMouseDown);
			Send(bridge, event, NSEventTypeOtherMouseDragged, .75, .75);
			assert(motions.back().dx == 0 && motions.back().dy == 0);
			follow = false;
			Send(bridge, event, NSEventTypeOtherMouseDragged, 20, 20);
			assert(motions.back().dx == 0 && motions.back().dy == 0);
			[bridge invalidate];
			const int afterKeys = keyResets;
			[[NSNotificationCenter defaultCenter] postNotificationName:NSWindowDidResignKeyNotification object:window];
			assert(keyResets == afterKeys);
			[event release]; [bridge release]; [view release]; [window close]; [window release];
			std::printf("PASS native-follow scale=%d width=%d fractional=100,50 reverse=0,0 edge=ok reset=ok focus=ok cleanup=ok\n", scale, width);
		}
	}
}
