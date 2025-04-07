#pragma once
#include <string.h>

// Custom key type with hash
struct DataKey
{
    const uint8_t *data;
    size_t len;

    bool operator==(const DataKey &other) const
    {
        return len == other.len && memcmp(data, other.data, len) == 0;
    }
};

struct DataKeyHash
{
    size_t operator()(const DataKey &key) const
    {
        size_t hash = 0;
        for (size_t i = 0; i < key.len; i++)
        {
            hash = hash * 31 + key.data[i];
        }
        return hash;
    }
};

bool runEvery(unsigned long &lastTime, const unsigned long interval, void (*action)());
bool isTimeAfter(unsigned long currentTime, unsigned long lastTime, const unsigned long interval);
