#include "cocoa_bridge.h"
#include <AppKit/AppKit.h>
#include <QuartzCore/CAMetalLayer.h>

namespace CocoaBridge {

void AddLayerToWindow(void* native_window_handle, CA::MetalLayer* layer) {
    NSWindow* cocoa_window = (__bridge NSWindow*)native_window_handle;
    CAMetalLayer* native_layer = (__bridge CAMetalLayer*)layer;

    [[cocoa_window contentView] setLayer:native_layer];
    [native_layer setMaximumDrawableCount:2];
    [[cocoa_window contentView] setWantsLayer:YES];
    [[cocoa_window contentView] setNeedsLayout:YES];
}

} // namespace CocoaBridge
