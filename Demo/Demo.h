#pragma once

#include <Config.h>
//#include <Device.h>

class Demo : public draw::Device
{
private:

	draw::CharSet						*m_pDbgFont{nullptr};
	draw::FrameBuffer					m_frmBuffer;
	const draw::components::Properties	&m_frmbuffProp;
	draw::EntityMngr					m_EnMan;

	int 								m_fps;

	const std::string TXT_ID_DBG{ "dbg" };
	const std::string DBGFONTPATH  { "./WhiteOnBlack.ttf" };
	
	static constexpr size_t BCKGRND_W{ 1024 };
	static constexpr size_t BCKGRND_H{ 600 };
	const std::string BACKGROUND{ "background" };
	const std::string BCKGRNDTPATH{ "./bgd_test.png" };


	/*
			Error msg
	*/

	const std::string ERR_CREATING{ "Error try to create " };

public:

	const int DGB_X{ width / 2};
	const int DGB_Y{ height / 2};
public:
	
	//explicit Demo(draw::FrameBuffer& fbuffer);
	explicit Demo(draw::draw_t w, draw::draw_t h);
	virtual ~Demo();

	void render();
	bool create();

	void updateDbg(std::string sz);

	void onClose(); 
	void onKeyDown(unsigned long  nKey);

};

