#ifndef HELPERS_H
#define HELPERS_H

#define SAFE_DELETE(a) delete(a); a = NULL;
#define V_RETURN(a) {hr = a; if (hr != S_OK) { LOG_ERROR("%i", hr); }; return hr; }

#endif // HELPERS_H