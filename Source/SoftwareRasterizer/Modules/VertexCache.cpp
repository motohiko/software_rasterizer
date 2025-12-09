
#include "VertexCache.h"

namespace SoftwareRasterizer
{
	static VertexCacheEntry s_vertexCacheEntry[16];
    static VertexCacheEntry* s_vertexCacheEntryHead;
    static VertexCacheEntry* s_vertexCacheEntryTail;

    static void AppendEntryHead(VertexCacheEntry* entry)
    {
        if (nullptr == s_vertexCacheEntryHead)
        {
            s_vertexCacheEntryHead = entry;
            s_vertexCacheEntryTail = entry;
        }
        else
        {
            entry->next = s_vertexCacheEntryHead;
            s_vertexCacheEntryHead->prev = entry;
            s_vertexCacheEntryHead = entry;
        }
    }

    static void AppendEntryTail(VertexCacheEntry* entry)
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

    static void RemoveEntry(VertexCacheEntry* entry)
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
            entry->enabled = false;
            entry->vertexId = 0;
            AppendEntryTail(entry);
        }
	}

    VertexCacheEntry* VertexCache::LookupVertexCache(int vertexId)
    {
        for (VertexCacheEntry* entry = s_vertexCacheEntryHead; nullptr != entry; entry = entry->next)
        {
            if (entry->enabled && entry->vertexId == vertexId)
            {
                RemoveEntry(entry);
                AppendEntryHead(entry);
                return entry;
            }
        }
        return nullptr;
    }

    VertexCacheEntry* VertexCache::GetVertexCache(int vertexId)
    {
        VertexCacheEntry* entry = s_vertexCacheEntryTail;
        RemoveEntry(entry);
        AppendEntryHead(entry);
        entry->enabled = true;
        entry->vertexId = vertexId;
        return entry;
    }

}
