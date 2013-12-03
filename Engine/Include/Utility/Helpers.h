#ifndef HELPERS_H
#define HELPERS_H

#define SAFE_DELETE(a) delete(a); a = NULL;
#define SAFE_DELETE_ARRAY(a) delete [] (a); a = NULL;
#define RELEASE(x) if (x) { x->Release(); x = NULL; }
#define V_RETURN(a) {hr = a; if (hr != S_OK) { LOG_ERROR("%i", hr); }; return hr; }
#define ARRAY_SIZE(x) sizeof(x) / sizeof(*x)

#endif // HELPERS_H