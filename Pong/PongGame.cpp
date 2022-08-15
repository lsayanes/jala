
#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <chrono>

#include <Config.h>

#include <Types.h>
#include <Component.h>
#include <Physics.h>
#include <Properties.h>

#include <Device.h>
#include <Entity.h>
#include <Sprite.h>
#include <FontLib.h>
#include <CharSet.h>
#include <FrameBuffer.h>

#include <EntityMngr.h>


#include "PongGame.h"


PongGame::PongGame(void* pDevHandle, draw::FrameBuffer& fbuffer):
	m_phDevHandle{pDevHandle},
	m_EnMan{fbuffer},
	m_bRun{false},
	m_mtxRender{}
{

}

PongGame::~PongGame()
{
	if (m_pDbgFont)
		delete m_pDbgFont;
}


bool PongGame::create()
{
	bool bRet = false;

	if (nullptr != (m_pDbgFont = new draw::CharSet{ draw::FontLib::instance()->newFont(DBGFONTPATH.c_str(), 16), m_EnMan.bpp() }))
	{
		size_t t = m_EnMan.create(BACKGROUND, BCKGRND_W, BCKGRND_H, BCKGRNDTPATH);
		t > 0 && (t = m_EnMan.create(BALL, BALL_W, BALL_H));
		if (bRet = t > 0)
		{
			//initial positions
			auto& bckphy = m_EnMan(BACKGROUND).physics();
			bckphy.x = static_cast<draw::phy_type>((SCREEN_W / 2) - (BCKGRND_W / 2));
			bckphy.y = 80;

			auto& ballphy = m_EnMan(BALL).physics();
			auto& ballprp = m_EnMan(BALL).properties();
			ballphy.x = bckphy.x;
			ballphy.y = static_cast<draw::phy_type>( bckphy.y + (BCKGRND_H - ballprp.h) );
			m_EnMan.frameBuffer().fill(m_EnMan(BALL), 255, 255, 255);
		}
	}
	else
		printf(std::string{ERR_CREATING + DBGFONTPATH }.c_str());

	return bRet;
}

void PongGame::dbg(std::string sz)
{
	std::string str = PongGame::TXT_ID_DBG + ": " + sz;
	auto vc = m_pDbgFont->flatText(str.c_str(), DGB_X, DGB_Y);
	m_EnMan.addText(PongGame::TXT_ID_DBG, vc);
}

void PongGame::render()
{
	while (m_bRun)
	{
		m_mtxRender.lock();

		m_EnMan.fill(255, 255, 255);
		
		m_EnMan.renderAll();
		
		m_EnMan.flip();
		
		m_mtxRender.unlock();

	}
}

void PongGame::run()
{
	m_bRun = true;

	int i = 0;
	char str[1024];

	//auto& ballphy = m_EnMan(BALL).physics();

	auto lasttime = std::chrono::steady_clock::now();

	while (m_bRun)
	{
		m_mtxRender.lock();
		

		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lasttime).count() > 1000)
		{
			sprintf(str, "fps :%d", i);
			dbg(str);
			i = 0;

			lasttime = std::chrono::steady_clock::now();
		}
		else
			i++;
		
		m_mtxRender.unlock();
	}
}

void PongGame::start()
{
	std::thread tRun(&PongGame::run, this);
	tRun.detach();

	std::thread tRender(&PongGame::render, this);
	tRender.detach();
}

void PongGame::stop()
{
	m_bRun = false;
}


