#pragma once
#include <WString.h>

void saveBoolean(const char *key, const bool value);
void saveString(const char *key, const String value);
bool readBool(const char *key);
String readString(const char *key);
