#include "prefsMod.h"
#include <Preferences.h>

Preferences preferences;

const char *prefsName = "orzel7";
const size_t MAX_STR_SIZE = 128;

void saveBoolean(const char *key, const bool value)
{
    preferences.begin(prefsName, false);
    preferences.putBool(key, value);
    preferences.end();
}

void saveString(const char *key, const String value)
{
    preferences.begin(prefsName, false);
    preferences.putString(key, value);
    preferences.end();
}

bool readBool(const char *key)
{
    bool value = NULL;
    preferences.begin(prefsName, true);
    value = preferences.getBool(key, true);
    preferences.end();
    return value;
}

String readString(const char *key)
{
    String value;
    preferences.begin(prefsName, true);
    value = preferences.getString(key, "");
    preferences.end();
    return value;
}
