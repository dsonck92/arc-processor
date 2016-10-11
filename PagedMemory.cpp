#include "PagedMemory.h"

#include <QDebug>









void PagedMemory::writeHelper(quint32 loc, quint8 *data, quint8 size)
{
    // Then find the alignment, 0 = perfectly word aligned
    quint8 align = loc & 0x3;

    if(align == 0) {
        // Perfectly aligned means it can directly write
        memcpy(getWrMemPtr(loc), data, size);
        return; // Shortcut
    }

    // If the alignment isn't perfect, we *might* cross a page
    // border. The maximum size we can write is 4 bytes so we
    // calculate a safe area which is guaranteed to stay
    // within the boundaries of the page
    quint8 safe = 0x4 - align;

    // If our safe area is smaller than the size we want to write
    if(safe<size) {
        // We first store the safe part
        memcpy(getWrMemPtr(loc), data, safe);
        // And then recalculate to possibly get the next page
        memcpy(getWrMemPtr(loc+safe), data+safe, size-safe);
    } else {
        // Otherwise we can simply store the complete data
        memcpy(getWrMemPtr(loc), data, size);
    }
}

void PagedMemory::readHelper(quint32 loc, quint8 *data, quint8 size) const
{
    // Then find the alignment, 0 = perfectly word aligned
    quint8 align = loc & 0x3;

    if(align == 0) {
        // Perfectly aligned means it can directly read
        const quint8 *p = getRdMemPtr(loc);
        if(p)
            memcpy(data, p, size);
        else
            memset(data, 0, size);
        return; // Shortcut
    }

    // If the alignment isn't perfect, we *might* cross a page
    // border. The maximum size we can read is 4 bytes so we
    // calculate a safe area which is guaranteed to stay
    // within the boundaries of the page
    quint8 safe = 0x4 - align;

    // If our safe area is smaller than the size we want to read
    if(safe<size) {
        // We first read the safe part
        const quint8 *p = getRdMemPtr(loc);
        if(p)
            memcpy(data, p, safe);
        else
            memset(data, 0, safe);
        // And then recalculate to possibly get the next page
        p = getRdMemPtr(loc+safe);
        if(p)
            memcpy(data+safe, p, size-safe);
        else
            memset(data+safe, 0, size-safe);
    } else {
        // Otherwise we can simply read the complete data
        const quint8 *p = getRdMemPtr(loc);
        if(p)
            memcpy(data, p, size);
        else
            memset(data, 0, size);
    }
}

PagedMemory::PagedMemory()
{
    qDebug() << this << "Created, 0 pages allocated";
}

PagedMemory::~PagedMemory()
{
    for(quint16 key : m_pages.keys()) {
        freePage(key);
    }
    qDebug() << this << "Destroyed, 0 pages allocated";
}

/*quint32 PagedMemory::readDWord(quint32 loc) const
{
    quint32 data;
    readHelper(m_addr, &data, sizeof(data));

    return data;
}

*/
