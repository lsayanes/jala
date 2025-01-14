


#include <memory>
#include <vector>
#include <stdint.h>
#include <algorithm>

#include "Config.h"
#include "Types.h"

#include "../Components/Physics.h"
#include "../Components/Properties.h"

#include "Entity.h"
#include "CharSet.h"

#include <debug.h>

#include "FrameBuffer.h"

namespace draw
{


	FrameBuffer::FrameBuffer(draw_t w, draw_t h) :
        m_Properties{ components::Properties{w, h, components::ATC_MALLOC_BUFFER, "FrameBuffer - malloc Buffer"} }

    {

        m_Line = std::make_unique<uint8_t[]>(m_Properties.lineSize);
        m_pbyBuffer = static_cast<uint8_t*>(malloc(w * h * components::Properties::bpp));

        if(nullptr == m_pbyBuffer)
            throw ("FrameBuffer::FrameBuffer m_pbyBuffer == nullptr");

    }

    FrameBuffer::FrameBuffer(uint8_t* pBuffer, draw_t w, draw_t h) :
        m_Properties{ components::Properties{w, h, components::ATC_BUFFER_DEV, "FrameBuffer - Dev Buffer"} }
    {

        m_Line = std::make_unique<uint8_t[]>(m_Properties.lineSize);
        m_pbyBuffer = pBuffer;

        dbg("%s", m_Properties.infoDebug());
    }

	FrameBuffer::~FrameBuffer()
	{
        if (m_pbyBuffer && components::ATC_MALLOC_BUFFER == m_Properties.allocType())
        {
            free(m_pbyBuffer);
            m_pbyBuffer = nullptr;
        }

	}



    void FrameBuffer::put(Entity& e) const
    {
        auto &physicsRef = e.physics();
        auto &propRef = e.properties();
        auto data = e.data();

        size_t line = 0;
        size_t linSizeEnt = propRef.lineSize;
        size_t offset;
        size_t components = m_Properties.components();

        unsigned char* pbyPix = m_pbyBuffer;
        const int x = static_cast<int>((physicsRef.rc.left * components));
        int y = physicsRef.rc.top;

        if (propRef.alpha && components > 3)
        {
            size_t i;
            unsigned char* psrc, *pdest;
            while (line < static_cast<size_t>(physicsRef.rc.height) && y < m_Properties.width - 1 && y >= 0)
            {
                y = static_cast<int>((line + physicsRef.rc.top));
                pdest = m_pbyBuffer + ((y * m_Properties.lineSize) + x);
                psrc = data + (line * linSizeEnt);


                for (i = 0; i < linSizeEnt; i+=components)
                {
                    if (0 != *(psrc + i + 3))
                    {
                        *(pdest + i + 0) = *(psrc + i + 0);
                        *(pdest + i + 1) = *(psrc + i + 1);
                        *(pdest + i + 2) = *(psrc + i + 2);
                    }
                }

                line++;
            }
        }
        else
        {
            while (line < static_cast<size_t>(physicsRef.rc.height) && y < (m_Properties.height - 1) && y >= 0)
            {
                y = static_cast<int>((line + physicsRef.rc.top));
                offset = (y * m_Properties.lineSize) + x;
                std::memcpy(pbyPix + offset, data + (line * linSizeEnt), linSizeEnt);
                line++;
            }
        }
    }

    void FrameBuffer::put(std::vector<Entity*> &v) const
    {
        std::for_each(v.begin(), v.end(), [&](Entity* it) { put(*it); });
    }

    void FrameBuffer::put(CharSet& c) const
    {
        std::vector<Entity*>* v = c.get();
        std::for_each(v->begin(), v->end(), [&](Entity* it) { put(*it); });
    }

    void FrameBuffer::fill(components::Properties& Prop, draw_t w, draw_t h, unsigned char* pbySurface, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        size_t y = 0;
        size_t offset;
        size_t lineSize = Prop.lineSize;
        unsigned char comp = Prop.components();

        std::unique_ptr<unsigned char[]> line = std::make_unique<unsigned char[]>(lineSize);
        unsigned char* pbyLine = line.get();

        uint32_t pixel = FrameBuffer::buildPixel(r, g, b, a);
//        std::fill(pbyLine, pbyLine + lineSize, pixel);

        for (uint32_t ud = 0; ud < lineSize; ud += comp)
            std::memcpy(&pbyLine[ud], &pixel, comp);

        while (y < static_cast<size_t>(h))
        {
            offset = (y * lineSize);
            std::memcpy(pbySurface + offset, pbyLine, lineSize);
            y++;
        }
    }

    void FrameBuffer::fill(Entity& e, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {

        size_t y = 0;
        size_t offset;
        components::Properties& prop = e.properties();
        components::Physics& phy = e.physics();
        size_t lineSize = prop.lineSize;
        unsigned char comp = prop.components();
        unsigned char* pbyPix = e.data();


        std::unique_ptr<unsigned char[]> line = std::make_unique<unsigned char[]>(prop.lineSize);
        unsigned char* pbyLine = line.get();

        uint32_t pixel = buildPixel(r, g, b, a);

        for (uint32_t ud = 0; ud < lineSize; ud += comp)
            std::memcpy(&pbyLine[ud], &pixel, comp);

        while (y < static_cast<size_t>(phy.rc.height))
        {
            offset = (y * lineSize);
            std::memcpy(pbyPix + offset, pbyLine, lineSize);
            y++;
        }

        //fill(e.properties(), e.data().get(), r, g, b, a);
    }


    void FrameBuffer::fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {

        size_t y = 0;
        size_t offset;
        size_t lineSize = m_Properties.lineSize;
        uint8_t comp = m_Properties.components();

        unsigned char* pbyPix = m_pbyBuffer;
        unsigned char* pbyLine = m_Line.get();

        uint32_t pixel = buildPixel(r, g, b, a);


        for (uint32_t ud = 0; ud < lineSize; ud += comp)
            std::memcpy(&pbyLine[ud], &pixel, comp);

        while (y < static_cast<size_t>(m_Properties.height))
        {
            offset = (y * lineSize);
            std::memcpy(pbyPix + offset, pbyLine, lineSize);
            y++;
        }


        //fill(m_Properties, m_pbyBuffer, r, g, b, a);
    }
}//draw