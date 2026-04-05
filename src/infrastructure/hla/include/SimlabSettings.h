#pragma once
#include <string>
#include <DevStudio/HlaSettings.h>

namespace SDL_RTI
{
	std::wstring towstr(const std::string& s);
	std::string tostr(const std::wstring& s);
}

namespace SDL_RTI {

	class SimlabSettings : public DevStudio::HlaSettings
	{


	public:
		/// <summary>
		/// Default constructor.
		/// </summary>
		SimlabSettings();
		SimlabSettings(const std::wstring& crcHost, int crcPort);

		~SimlabSettings() {

		}

		/// <summary>
		/// Sets the CRC host.
		/// </summary>
		/// <param name="crcPort"></param>
		void setCrcHost(const std::string& crcHost);

		// <summary>
		/// Sets the CRC host.
		/// </summary>
		/// <param name="crcPort"></param>
		void setCrcPort(int crcPort);

		/// <summary>
		/// Set the federation name.
		/// </summary>
		/// <param name="name">Path of the LSD..</param>
		void setLocalSettingsDesignator(const std::string& lsd);

		/// <summary>
		/// Set the federation name.
		/// </summary>
		/// <param name="name">name of the federation.</param>
		void setFederationName(const std::string& name);

		/// <summary>
		/// Set the federation name.
		/// </summary>
		/// <param name="name">name of the federate.</param>
		void setFederateName(const std::string& name);

		/// <summary>
		/// Enables/Disables the capability to create the federation.
		/// </summary>
		/// <param name="createFederation">True to enable the creation of a federation.</param>
		void setCreateFederation(bool createFederation);

		/// <summary>
		/// Enables/Disables the capability to destroy the federation.
		/// </summary>
		/// <param name="createFederation">True to enable the destruction of a federation.</param>
		void setDestroyFederation(bool destroyFederation);
	};
}


