// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include <ole2.h> 
#include <xmllite.h> 
#include <shlwapi.h> 
 
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <set>
#include <map>
#include <algorithm>
#include <cmath>
using namespace std;

#include "stemming/english_stem.h"
// TODO: reference additional headers your program requires here
#include "listDir.h"
#include "xmlReader.h"

#pragma comment(lib, "xmllite.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma warning(disable : 4127)  // conditional expression is constant 
