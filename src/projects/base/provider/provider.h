//==============================================================================
//
//  Provider Base Class
//
//  Created by Kwon Keuk Han
//  Copyright (c) 2018 AirenSoft. All rights reserved.
//
//==============================================================================
#pragma once

#include <base/common_types.h>
#include <base/info/host.h>
#include <base/media_route/media_route_interface.h>
#include <orchestrator/data_structure.h>

namespace pvd
{
	class Application;
	class Stream;

	// RTMP Server와 같은 모든 Provider는 다음 Interface를 구현하여 MediaRouterInterface에 자신을 등록한다.
	class Provider : public OrchestratorProviderModuleInterface
	{
	public:
		virtual ProviderType GetProviderType() = 0;

		virtual bool Start();
		virtual bool Stop();

		// app_name으로 Application을 찾아서 반환한다.
		std::shared_ptr<Application> GetApplicationByName(ov::String app_name);
		std::shared_ptr<Stream> GetStreamByName(ov::String app_name, ov::String stream_name);

		std::shared_ptr<Application> GetApplicationById(info::application_id_t app_id);
		std::shared_ptr<Stream> GetStreamById(info::application_id_t app_id, uint32_t stream_id);

	protected:
		Provider(const cfg::Server &server_config, const info::Host &host_info, const std::shared_ptr<MediaRouteInterface> &router);
		virtual ~Provider();

		const cfg::Server &GetServerConfig() const;
		const info::Host &GetHostInfo() const;

		// For child class
		virtual std::shared_ptr<Application> OnCreateProviderApplication(const info::Application &app_info) = 0;
		virtual bool OnDeleteProviderApplication(const info::Application &app_info) = 0;

		//--------------------------------------------------------------------
		// Implementation of OrchestratorModuleInterface
		//--------------------------------------------------------------------
		bool OnCreateApplication(const info::Application &app_info) override;
		bool OnDeleteApplication(const info::Application &app_info) override;

		//--------------------------------------------------------------------
		// Implementation of OrchestratorProviderModuleInterface
		//--------------------------------------------------------------------
		bool CheckOriginAvailability(const std::vector<ov::String> &url_list) override
		{
			return false;
		}

		bool PullStream(const info::Application &app_info, const ov::String &stream_name, const std::vector<ov::String> &url_list)
		{
			return false;
		}

	private:
		const cfg::Server _server_config;
		const info::Host _host_info;
		std::map<info::application_id_t, std::shared_ptr<Application>> _applications;
		std::shared_ptr<MediaRouteInterface> _router;
	};

}  // namespace pvd