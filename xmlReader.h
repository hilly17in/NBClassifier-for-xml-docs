#define pragma once

/* can later see for class */

#define CHKHR(stmt)             do { hr = (stmt); if (FAILED(hr)) goto CleanUp; } while(0) 
#define HR(stmt)                do { hr = (stmt); goto CleanUp; } while(0) 
#define SAFE_RELEASE(I)         do { if (I){ I->Release(); } I = NULL; } while(0) 

unsigned long
xmlRead(string xmlFile, string &data);