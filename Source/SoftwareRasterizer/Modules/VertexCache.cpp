
#include "VertexCache.h"

namespace SoftwareRasterizer
{
	static VertexCacheEntry s_vertexCacheEntry[16];
    static VertexCacheEntry* s_vertexCacheEntryHead;
    static VertexCacheEntry* s_vertexCacheEntryTail;

    static void AppendTail(VertexCacheEntry* entry)
    {
        if (nullptr == s_vertexCacheEntryHead)
        {
            s_vertexCacheEntryHead = entry;
            s_vertexCacheEntryTail = entry;
        }
        else
        {
            entry->prev = s_vertexCacheEntryTail;
            s_vertexCacheEntryTail->next = entry;
            s_vertexCacheEntryTail = entry;
        }
    }

    static void Remove(VertexCacheEntry* entry)
    {
        if (nullptr == entry->prev)
        {
            s_vertexCacheEntryHead = entry->next;
        }
        else
        {
            entry->prev->next = entry->next;
        }

        if (nullptr == entry->next)
        {
            s_vertexCacheEntryTail = entry->prev;
        }
        else
        {
            entry->next->prev = entry->prev;
        }

        entry->prev = nullptr;
        entry->next = nullptr;
    }


	void VertexCache::InitializeCache()
	{
        s_vertexCacheEntryHead = nullptr;
        s_vertexCacheEntryTail = nullptr;
        for (int i = 0; i < 16; i++)
        {
            VertexCacheEntry* entry = &(s_vertexCacheEntry[i]);
            entry->prev = nullptr;
            entry->next = nullptr;
            entry->vertexIndex = -1;
            AppendTail(entry);
        }
	}

    VertexCacheEntry* VertexCache::LookupVertexCache(int vertexIndex)
    {
        for (VertexCacheEntry* entry = s_vertexCacheEntryTail; nullptr != entry; entry = entry->prev)
        {
            if (entry->vertexIndex == vertexIndex)
            {
                Remove(entry);
                AppendTail(entry);
                return entry;
            }
        }
        return nullptr;
    }

    VertexCacheEntry* VertexCache::WriteVertexCache(int vertexIndex)
    {
        VertexCacheEntry* entry = s_vertexCacheEntryHead;
        Remove(entry);
        AppendTail(entry);
        entry->vertexIndex = vertexIndex;
        return entry;
    }

}
