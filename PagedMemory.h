#ifndef PAGEDMEMORY_H
#define PAGEDMEMORY_H

#include <ARCMemory.h>
#include <QMap>
#include <QDebug>

class PagedMemory : public QObject, public ARCMemory
{
    Q_OBJECT

    QMap<quint16, quint8 *> m_pages;

    void freePage(quint16 page)
    {
        qDebug() << this << "Free page  :" << page;
        delete m_pages.take(page);
        qDebug() << this << "Total pages:" << m_pages.size();
    }
    const quint8* getPage(quint16 page) const
    {
        if(m_pages.contains(page)) {
            return m_pages.value(page);
        } else {
            return 0;
        }
    }
    quint8* getPage(quint16 page)
    {
        if(m_pages.contains(page)) {
            return m_pages.value(page);
        } else {
            qDebug() << this << "Alloc page :" << page;
            quint8 * p = new quint8 [0x3FFFFF];
            m_pages.insert(page, p);
            qDebug() << this << "Total pages:" << m_pages.size();
            return p;
        }
    }

    const quint8* getRdMemPtr(quint32 loc) const
    {
        const quint8 *p = getPage(loc >> 22);
        if(p == 0) {
            return 0;
        }
        p += loc & 0x3FFFFF;

        return p;
    }
    quint8* getWrMemPtr(quint32 loc)
    {
        quint8 *p = getPage(loc >> 22);
        p += loc & 0x3FFFFF;

        return p;
    }

private:
    void writeHelper(quint32 loc, quint8* data, quint8 size);
    void readHelper(quint32 loc, quint8* data, quint8 size) const;

public:
    PagedMemory();
    virtual ~PagedMemory();

    //quint32 readDWord ( quint32 loc ) const;

public:
  //quint8 read(quint32 addr) const {return m_vu32Memory[addr];}
  //const quint8 *data() const {return m_vu32Memory;}


signals:
  void memoryChange ( quint32 begin , quint32 end );
};

#endif // PAGEDMEMORY_H
