#ifndef EGLUT_X11_H
#define EGLUT_X11_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EGLUT_NO_X11_INCLUDE
#include <X11/Xlib.h>

Display* eglutGetDisplay();
Window eglutGetWindowHandle();
#endif

void eglutInitX11ClassInstanceName(const char *value);
void eglutInitX11ClassName(const char *value);

#ifdef __cplusplus
}
#endif

#endif /* EGLUT_H */