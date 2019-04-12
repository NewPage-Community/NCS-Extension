#ifndef NCS_ROUTER_H
#define NCS_ROUTER_H

class RouterHandler
{
public:
	RouterHandler(IPluginContext *pContext, IChangeableForward *forward)
		: m_pContext(pContext)
		, m_forward(forward)
	{}

	~RouterHandler()
	{
		forwards->ReleaseForward(m_forward);
	}

	IChangeableForward *GetForward()
	{
		return m_forward;
	}

	IPluginContext *GetPluginContext()
	{
		return m_pContext;
	}

	void OnTerminate(IThreadHandle* pHandle, bool cancel)
	{}

	void CallHandler(Handle_t hdl)
	{

	}

private:
	IChangeableForward *m_forward;
	IPluginContext *m_pContext;
};

class Router
{
public:
	typedef std::map<std::string, RouterHandler *> mapRouter;

	Router()
	{}

	~Router()
	{
		m_mapRouter.clear();
	}

	void AddRouter(std::string router, IPluginContext *pContext, IChangeableForward *hdl)
	{
		m_mapRouter.insert(std::pair<std::string, RouterHandler *>(router, new RouterHandler(pContext, hdl)));
	}

	void RemoveRouter(std::string router)
	{
		mapRouter::iterator hdl = m_mapRouter.find(router);
		if (hdl != m_mapRouter.end())
			delete hdl->second;
		m_mapRouter.erase(router);
	}

	RouterHandler* Handler(std::string router)
	{
		mapRouter::iterator hdl = m_mapRouter.find(router);
		if (hdl != m_mapRouter.end())
			return static_cast<RouterHandler*>(hdl->second);
	}

private:
	mapRouter m_mapRouter;
};

#endif