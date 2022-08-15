namespace draw
{
	class EntityMngr 
	{
	private:
		std::unordered_map<std::string, Entity*>						m_Entities{};
		std::unordered_map<std::string, std::vector<Entity*>*>			m_Text{};
		
		FrameBuffer															&m_FrameBufferRef;
	public:
		static constexpr size_t INI_MAX_ENTITIES {1024};

		explicit EntityMngr(FrameBuffer &fbuffer);
		virtual ~EntityMngr();

		void renderAll() noexcept;

		inline void addText(std::string szName, std::vector<Entity*>* pVct) noexcept
		{
			m_Text[szName] = pVct;
		}

		const prop_type create(std::string szName, prop_type w, prop_type h) noexcept;
		const prop_type create(std::string szName, prop_type w, prop_type h, std::string szPath) noexcept;
		
		Entity& operator()(std::string szName) const;

		inline Entity* get(std::string szName) const noexcept
		{
			auto it = m_Entities.find(szName);
			if (it != m_Entities.end())
				return it->second;

			return nullptr;
		}

		inline FrameBuffer& frameBuffer() const noexcept  { return m_FrameBufferRef; };
		inline unsigned char bpp() const noexcept { return m_FrameBufferRef.properties().bpp(); }
		inline void fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept { m_FrameBufferRef.fill(r, g, b, a); };
		inline void flip() const noexcept { m_FrameBufferRef.flip(); }
	};
};// draw